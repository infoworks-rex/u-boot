# Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
#
# SPDX-License-Identifier: GPL-2.0

# Test U-Boot's "dfu" command. The test starts DFU in U-Boot, waits for USB
# device enumeration on the host, executes dfu-util multiple times to test
# various transfer sizes, many of which trigger USB driver edge cases, and
# finally aborts the "dfu" command in U-Boot.

import os
import os.path
import pytest
import u_boot_utils

'''
Note: This test relies on:

a) boardenv_* to contain configuration values to define which USB ports are
available for testing. Without this, this test will be automatically skipped.
For example:

env__usb_dev_ports = (
    {
        "tgt_usb_ctlr": "0",
        "host_usb_dev_node": "/dev/usbdev-p2371-2180",
        # This parameter is optional /if/ you only have a single board
        # attached to your host at a time.
        "host_usb_port_path": "3-13",
    },
)

env__dfu_configs = (
    # eMMC, partition 1
    {
        "alt_info": "/dfu_test.bin ext4 0 1;/dfu_dummy.bin ext4 0 1",
        "cmd_params": "mmc 0",
    },
)
b) udev rules to set permissions on devices nodes, so that sudo is not
required. For example:

ACTION=="add", SUBSYSTEM=="block", SUBSYSTEMS=="usb", KERNELS=="3-13", MODE:="666"

(You may wish to change the group ID instead of setting the permissions wide
open. All that matters is that the user ID running the test can access the
device.)
'''

# The set of file sizes to test. These values trigger various edge-cases such
# as one less than, equal to, and one greater than typical USB max packet
# sizes, and similar boundary conditions.
test_sizes = (
    64 - 1,
    64,
    64 + 1,
    128 - 1,
    128,
    128 + 1,
    960 - 1,
    960,
    960 + 1,
    4096 - 1,
    4096,
    4096 + 1,
    1024 * 1024 - 1,
    1024 * 1024,
    8 * 1024 * 1024,
)

first_usb_dev_port = None

@pytest.mark.buildconfigspec('cmd_dfu')
def test_dfu(u_boot_console, env__usb_dev_port, env__dfu_config):
    '''Test the "dfu" command; the host system must be able to enumerate a USB
    device when "dfu" is running, various DFU transfers are tested, and the
    USB device must disappear when "dfu" is aborted.

    Args:
        u_boot_console: A U-Boot console connection.
        env__usb_dev_port: The single USB device-mode port specification on
            which to run the test. See the file-level comment above for
            details of the format.
        env__dfu_config: The single DFU (memory region) configuration on which
            to run the test. See the file-level comment above for details
            of the format.

    Returns:
        Nothing.
    '''

    def start_dfu():
        '''Start U-Boot's dfu shell command.

        This also waits for the host-side USB enumeration process to complete.

        Args:
            None.

        Returns:
            Nothing.
        '''

        fh = u_boot_utils.attempt_to_open_file(
            env__usb_dev_port['host_usb_dev_node'])
        if fh:
            fh.close()
            raise Exception('USB device present before dfu command invoked')

        u_boot_console.log.action(
            'Starting long-running U-Boot dfu shell command')

        cmd = 'setenv dfu_alt_info "%s"' % env__dfu_config['alt_info']
        u_boot_console.run_command(cmd)

        cmd = 'dfu 0 ' + env__dfu_config['cmd_params']
        u_boot_console.run_command(cmd, wait_for_prompt=False)
        u_boot_console.log.action('Waiting for DFU USB device to appear')
        fh = u_boot_utils.wait_until_open_succeeds(
            env__usb_dev_port['host_usb_dev_node'])
        fh.close()

    def stop_dfu(ignore_errors):
        '''Stop U-Boot's dfu shell command from executing.

        This also waits for the host-side USB de-enumeration process to
        complete.

        Args:
            ignore_errors: Ignore any errors. This is useful if an error has
                already been detected, and the code is performing best-effort
                cleanup. In this case, we do not want to mask the original
                error by "honoring" any new errors.

        Returns:
            Nothing.
        '''

        try:
            u_boot_console.log.action(
                'Stopping long-running U-Boot dfu shell command')
            u_boot_console.ctrlc()
            u_boot_console.log.action(
                'Waiting for DFU USB device to disappear')
            u_boot_utils.wait_until_file_open_fails(
                env__usb_dev_port['host_usb_dev_node'], ignore_errors)
        except:
            if not ignore_errors:
                raise

    def run_dfu_util(alt_setting, fn, up_dn_load_arg):
        '''Invoke dfu-util on the host.

        Args:
            alt_setting: The DFU "alternate setting" identifier to interact
                with.
            fn: The host-side file name to transfer.
            up_dn_load_arg: '-U' or '-D' depending on whether a DFU upload or
                download operation should be performed.

        Returns:
            Nothing.
        '''

        cmd = ['dfu-util', '-a', str(alt_setting), up_dn_load_arg, fn]
        if 'host_usb_port_path' in env__usb_dev_port:
            cmd += ['-p', env__usb_dev_port['host_usb_port_path']]
        u_boot_utils.run_and_log(u_boot_console, cmd)
        u_boot_console.wait_for('Ctrl+C to exit ...')

    def dfu_write(alt_setting, fn):
        '''Write a file to the target board using DFU.

        Args:
            alt_setting: The DFU "alternate setting" identifier to interact
                with.
            fn: The host-side file name to transfer.

        Returns:
            Nothing.
        '''

        run_dfu_util(alt_setting, fn, '-D')

    def dfu_read(alt_setting, fn):
        '''Read a file from the target board using DFU.

        Args:
            alt_setting: The DFU "alternate setting" identifier to interact
                with.
            fn: The host-side file name to transfer.

        Returns:
            Nothing.
        '''

        # dfu-util fails reads/uploads if the host file already exists
        if os.path.exists(fn):
            os.remove(fn)
        run_dfu_util(alt_setting, fn, '-U')

    def dfu_write_read_check(size):
        '''Test DFU transfers of a specific size of data

        This function first writes data to the board then reads it back and
        compares the written and read back data. Measures are taken to avoid
        certain types of false positives.

        Args:
            size: The data size to test.

        Returns:
            Nothing.
        '''

        test_f = u_boot_utils.PersistentRandomFile(u_boot_console,
            'dfu_%d.bin' % size, size)
        readback_fn = u_boot_console.config.result_dir + '/dfu_readback.bin'

        u_boot_console.log.action('Writing test data to DFU primary ' +
            'altsetting')
        dfu_write(0, test_f.abs_fn)

        u_boot_console.log.action('Writing dummy data to DFU secondary ' +
            'altsetting to clear DFU buffers')
        dfu_write(1, dummy_f.abs_fn)

        u_boot_console.log.action('Reading DFU primary altsetting for ' +
            'comparison')
        dfu_read(0, readback_fn)

        u_boot_console.log.action('Comparing written and read data')
        written_hash = test_f.content_hash
        read_back_hash = u_boot_utils.md5sum_file(readback_fn, size)
        assert(written_hash == read_back_hash)

    # This test may be executed against multiple USB ports. The test takes a
    # long time, so we don't want to do the whole thing each time. Instead,
    # execute the full test on the first USB port, and perform a very limited
    # test on other ports. In the limited case, we solely validate that the
    # host PC can enumerate the U-Boot USB device.
    global first_usb_dev_port
    if not first_usb_dev_port:
        first_usb_dev_port = env__usb_dev_port
    if env__usb_dev_port == first_usb_dev_port:
        sizes = test_sizes
    else:
        sizes = []

    dummy_f = u_boot_utils.PersistentRandomFile(u_boot_console,
        'dfu_dummy.bin', 1024)

    ignore_cleanup_errors = True
    try:
        start_dfu()

        u_boot_console.log.action(
            'Overwriting DFU primary altsetting with dummy data')
        dfu_write(0, dummy_f.abs_fn)

        for size in sizes:
            with u_boot_console.log.section("Data size %d" % size):
                dfu_write_read_check(size)
                # Make the status of each sub-test obvious. If the test didn't
                # pass, an exception was thrown so this code isn't executed.
                u_boot_console.log.status_pass('OK')
        ignore_cleanup_errors = False
    finally:
        stop_dfu(ignore_cleanup_errors)
