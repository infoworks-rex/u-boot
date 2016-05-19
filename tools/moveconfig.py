#!/usr/bin/env python2
#
# Author: Masahiro Yamada <yamada.masahiro@socionext.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

"""
Move config options from headers to defconfig files.

Since Kconfig was introduced to U-Boot, we have worked on moving
config options from headers to Kconfig (defconfig).

This tool intends to help this tremendous work.


Usage
-----

First, you must edit the Kconfig to add the menu entries for the configs
you are moving.

And then run this tool giving CONFIG names you want to move.
For example, if you want to move CONFIG_CMD_USB and CONFIG_SYS_TEXT_BASE,
simply type as follows:

  $ tools/moveconfig.py CONFIG_CMD_USB CONFIG_SYS_TEXT_BASE

The tool walks through all the defconfig files and move the given CONFIGs.

The log is also displayed on the terminal.

Each line is printed in the format
<defconfig_name>   :  <action>

<defconfig_name> is the name of the defconfig
(without the suffix _defconfig).

<action> shows what the tool did for that defconfig.
It looks like one of the followings:

 - Move 'CONFIG_... '
   This config option was moved to the defconfig

 - CONFIG_... is not defined in Kconfig.  Do nothing.
   The entry for this CONFIG was not found in Kconfig.
   There are two common cases:
     - You forgot to create an entry for the CONFIG before running
       this tool, or made a typo in a CONFIG passed to this tool.
     - The entry was hidden due to unmet 'depends on'.
       This is correct behavior.

 - 'CONFIG_...' is the same as the define in Kconfig.  Do nothing.
   The define in the config header matched the one in Kconfig.
   We do not need to touch it.

 - Undefined.  Do nothing.
   This config option was not found in the config header.
   Nothing to do.

 - Compiler is missing.  Do nothing.
   The compiler specified for this architecture was not found
   in your PATH environment.
   (If -e option is passed, the tool exits immediately.)

 - Failed to process.
   An error occurred during processing this defconfig.  Skipped.
   (If -e option is passed, the tool exits immediately on error.)

Finally, you will be asked, Clean up headers? [y/n]:

If you say 'y' here, the unnecessary config defines are removed
from the config headers (include/configs/*.h).
It just uses the regex method, so you should not rely on it.
Just in case, please do 'git diff' to see what happened.


How does it work?
-----------------

This tool runs configuration and builds include/autoconf.mk for every
defconfig.  The config options defined in Kconfig appear in the .config
file (unless they are hidden because of unmet dependency.)
On the other hand, the config options defined by board headers are seen
in include/autoconf.mk.  The tool looks for the specified options in both
of them to decide the appropriate action for the options.  If the given
config option is found in the .config, but its value does not match the
one from the board header, the config option in the .config is replaced
with the define in the board header.  Then, the .config is synced by
"make savedefconfig" and the defconfig is updated with it.

For faster processing, this tool handles multi-threading.  It creates
separate build directories where the out-of-tree build is run.  The
temporary build directories are automatically created and deleted as
needed.  The number of threads are chosen based on the number of the CPU
cores of your system although you can change it via -j (--jobs) option.


Toolchains
----------

Appropriate toolchain are necessary to generate include/autoconf.mk
for all the architectures supported by U-Boot.  Most of them are available
at the kernel.org site, some are not provided by kernel.org.

The default per-arch CROSS_COMPILE used by this tool is specified by
the list below, CROSS_COMPILE.  You may wish to update the list to
use your own.  Instead of modifying the list directly, you can give
them via environments.


Available options
-----------------

 -c, --color
   Surround each portion of the log with escape sequences to display it
   in color on the terminal.

 -d, --defconfigs
  Specify a file containing a list of defconfigs to move

 -n, --dry-run
   Perform a trial run that does not make any changes.  It is useful to
   see what is going to happen before one actually runs it.

 -e, --exit-on-error
   Exit immediately if Make exits with a non-zero status while processing
   a defconfig file.

 -H, --headers-only
   Only cleanup the headers; skip the defconfig processing

 -j, --jobs
   Specify the number of threads to run simultaneously.  If not specified,
   the number of threads is the same as the number of CPU cores.

 -v, --verbose
   Show any build errors as boards are built

To see the complete list of supported options, run

  $ tools/moveconfig.py -h

"""

import fnmatch
import multiprocessing
import optparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

SHOW_GNU_MAKE = 'scripts/show-gnu-make'
SLEEP_TIME=0.03

# Here is the list of cross-tools I use.
# Most of them are available at kernel.org
# (https://www.kernel.org/pub/tools/crosstool/files/bin/), except the followings:
# arc: https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases
# blackfin: http://sourceforge.net/projects/adi-toolchain/files/
# nds32: http://osdk.andestech.com/packages/nds32le-linux-glibc-v1.tgz
# nios2: https://sourcery.mentor.com/GNUToolchain/subscription42545
# sh: http://sourcery.mentor.com/public/gnu_toolchain/sh-linux-gnu
#
# openrisc kernel.org toolchain is out of date, download latest one from
# http://opencores.org/or1k/OpenRISC_GNU_tool_chain#Prebuilt_versions
CROSS_COMPILE = {
    'arc': 'arc-linux-',
    'aarch64': 'aarch64-linux-',
    'arm': 'arm-unknown-linux-gnueabi-',
    'avr32': 'avr32-linux-',
    'blackfin': 'bfin-elf-',
    'm68k': 'm68k-linux-',
    'microblaze': 'microblaze-linux-',
    'mips': 'mips-linux-',
    'nds32': 'nds32le-linux-',
    'nios2': 'nios2-linux-gnu-',
    'openrisc': 'or1k-elf-',
    'powerpc': 'powerpc-linux-',
    'sh': 'sh-linux-gnu-',
    'sparc': 'sparc-linux-',
    'x86': 'i386-linux-'
}

STATE_IDLE = 0
STATE_DEFCONFIG = 1
STATE_AUTOCONF = 2
STATE_SAVEDEFCONFIG = 3

ACTION_MOVE = 0
ACTION_NO_ENTRY = 1
ACTION_NO_CHANGE = 2

COLOR_BLACK        = '0;30'
COLOR_RED          = '0;31'
COLOR_GREEN        = '0;32'
COLOR_BROWN        = '0;33'
COLOR_BLUE         = '0;34'
COLOR_PURPLE       = '0;35'
COLOR_CYAN         = '0;36'
COLOR_LIGHT_GRAY   = '0;37'
COLOR_DARK_GRAY    = '1;30'
COLOR_LIGHT_RED    = '1;31'
COLOR_LIGHT_GREEN  = '1;32'
COLOR_YELLOW       = '1;33'
COLOR_LIGHT_BLUE   = '1;34'
COLOR_LIGHT_PURPLE = '1;35'
COLOR_LIGHT_CYAN   = '1;36'
COLOR_WHITE        = '1;37'

### helper functions ###
def get_devnull():
    """Get the file object of '/dev/null' device."""
    try:
        devnull = subprocess.DEVNULL # py3k
    except AttributeError:
        devnull = open(os.devnull, 'wb')
    return devnull

def check_top_directory():
    """Exit if we are not at the top of source directory."""
    for f in ('README', 'Licenses'):
        if not os.path.exists(f):
            sys.exit('Please run at the top of source directory.')

def check_clean_directory():
    """Exit if the source tree is not clean."""
    for f in ('.config', 'include/config'):
        if os.path.exists(f):
            sys.exit("source tree is not clean, please run 'make mrproper'")

def get_make_cmd():
    """Get the command name of GNU Make.

    U-Boot needs GNU Make for building, but the command name is not
    necessarily "make". (for example, "gmake" on FreeBSD).
    Returns the most appropriate command name on your system.
    """
    process = subprocess.Popen([SHOW_GNU_MAKE], stdout=subprocess.PIPE)
    ret = process.communicate()
    if process.returncode:
        sys.exit('GNU Make not found')
    return ret[0].rstrip()

def color_text(color_enabled, color, string):
    """Return colored string."""
    if color_enabled:
        return '\033[' + color + 'm' + string + '\033[0m'
    else:
        return string

def log_msg(color_enabled, color, defconfig, msg):
    """Return the formated line for the log."""
    return defconfig[:-len('_defconfig')].ljust(37) + ': ' + \
        color_text(color_enabled, color, msg) + '\n'

def update_cross_compile(color_enabled):
    """Update per-arch CROSS_COMPILE via environment variables

    The default CROSS_COMPILE values are available
    in the CROSS_COMPILE list above.

    You can override them via environment variables
    CROSS_COMPILE_{ARCH}.

    For example, if you want to override toolchain prefixes
    for ARM and PowerPC, you can do as follows in your shell:

    export CROSS_COMPILE_ARM=...
    export CROSS_COMPILE_POWERPC=...

    Then, this function checks if specified compilers really exist in your
    PATH environment.
    """
    archs = []

    for arch in os.listdir('arch'):
        if os.path.exists(os.path.join('arch', arch, 'Makefile')):
            archs.append(arch)

    # arm64 is a special case
    archs.append('aarch64')

    for arch in archs:
        env = 'CROSS_COMPILE_' + arch.upper()
        cross_compile = os.environ.get(env)
        if not cross_compile:
            cross_compile = CROSS_COMPILE.get(arch, '')

        for path in os.environ["PATH"].split(os.pathsep):
            gcc_path = os.path.join(path, cross_compile + 'gcc')
            if os.path.isfile(gcc_path) and os.access(gcc_path, os.X_OK):
                break
        else:
            print >> sys.stderr, color_text(color_enabled, COLOR_YELLOW,
                 'warning: %sgcc: not found in PATH.  %s architecture boards will be skipped'
                                            % (cross_compile, arch))
            cross_compile = None

        CROSS_COMPILE[arch] = cross_compile

def cleanup_one_header(header_path, patterns, dry_run):
    """Clean regex-matched lines away from a file.

    Arguments:
      header_path: path to the cleaned file.
      patterns: list of regex patterns.  Any lines matching to these
                patterns are deleted.
      dry_run: make no changes, but still display log.
    """
    with open(header_path) as f:
        lines = f.readlines()

    matched = []
    for i, line in enumerate(lines):
        for pattern in patterns:
            m = pattern.search(line)
            if m:
                print '%s: %s: %s' % (header_path, i + 1, line),
                matched.append(i)
                break

    if dry_run or not matched:
        return

    with open(header_path, 'w') as f:
        for i, line in enumerate(lines):
            if not i in matched:
                f.write(line)

def cleanup_headers(configs, dry_run):
    """Delete config defines from board headers.

    Arguments:
      configs: A list of CONFIGs to remove.
      dry_run: make no changes, but still display log.
    """
    while True:
        choice = raw_input('Clean up headers? [y/n]: ').lower()
        print choice
        if choice == 'y' or choice == 'n':
            break

    if choice == 'n':
        return

    patterns = []
    for config in configs:
        patterns.append(re.compile(r'#\s*define\s+%s\W' % config))
        patterns.append(re.compile(r'#\s*undef\s+%s\W' % config))

    for dir in 'include', 'arch', 'board':
        for (dirpath, dirnames, filenames) in os.walk(dir):
            for filename in filenames:
                if not fnmatch.fnmatch(filename, '*~'):
                    cleanup_one_header(os.path.join(dirpath, filename),
                                       patterns, dry_run)

### classes ###
class Progress:

    """Progress Indicator"""

    def __init__(self, total):
        """Create a new progress indicator.

        Arguments:
          total: A number of defconfig files to process.
        """
        self.current = 0
        self.total = total

    def inc(self):
        """Increment the number of processed defconfig files."""

        self.current += 1

    def show(self):
        """Display the progress."""
        print ' %d defconfigs out of %d\r' % (self.current, self.total),
        sys.stdout.flush()

class KconfigParser:

    """A parser of .config and include/autoconf.mk."""

    re_arch = re.compile(r'CONFIG_SYS_ARCH="(.*)"')
    re_cpu = re.compile(r'CONFIG_SYS_CPU="(.*)"')

    def __init__(self, configs, options, build_dir):
        """Create a new parser.

        Arguments:
          configs: A list of CONFIGs to move.
          options: option flags.
          build_dir: Build directory.
        """
        self.configs = configs
        self.options = options
        self.dotconfig = os.path.join(build_dir, '.config')
        self.autoconf = os.path.join(build_dir, 'include', 'autoconf.mk')
        self.config_autoconf = os.path.join(build_dir, 'include', 'config',
                                            'auto.conf')

    def get_cross_compile(self):
        """Parse .config file and return CROSS_COMPILE.

        Returns:
          A string storing the compiler prefix for the architecture.
          Return a NULL string for architectures that do not require
          compiler prefix (Sandbox and native build is the case).
          Return None if the specified compiler is missing in your PATH.
          Caller should distinguish '' and None.
        """
        arch = ''
        cpu = ''
        for line in open(self.dotconfig):
            m = self.re_arch.match(line)
            if m:
                arch = m.group(1)
                continue
            m = self.re_cpu.match(line)
            if m:
                cpu = m.group(1)

        if not arch:
            return None

        # fix-up for aarch64
        if arch == 'arm' and cpu == 'armv8':
            arch = 'aarch64'

        return CROSS_COMPILE.get(arch, None)

    def parse_one_config(self, config, dotconfig_lines, autoconf_lines):
        """Parse .config, defconfig, include/autoconf.mk for one config.

        This function looks for the config options in the lines from
        defconfig, .config, and include/autoconf.mk in order to decide
        which action should be taken for this defconfig.

        Arguments:
          config: CONFIG name to parse.
          dotconfig_lines: lines from the .config file.
          autoconf_lines: lines from the include/autoconf.mk file.

        Returns:
          A tupple of the action for this defconfig and the line
          matched for the config.
        """
        not_set = '# %s is not set' % config

        for line in dotconfig_lines:
            line = line.rstrip()
            if line.startswith(config + '=') or line == not_set:
                old_val = line
                break
        else:
            return (ACTION_NO_ENTRY, config)

        for line in autoconf_lines:
            line = line.rstrip()
            if line.startswith(config + '='):
                new_val = line
                break
        else:
            new_val = not_set

        if old_val == new_val:
            return (ACTION_NO_CHANGE, new_val)

        # If this CONFIG is neither bool nor trisate
        if old_val[-2:] != '=y' and old_val[-2:] != '=m' and old_val != not_set:
            # tools/scripts/define2mk.sed changes '1' to 'y'.
            # This is a problem if the CONFIG is int type.
            # Check the type in Kconfig and handle it correctly.
            if new_val[-2:] == '=y':
                new_val = new_val[:-1] + '1'

        return (ACTION_MOVE, new_val)

    def update_dotconfig(self, defconfig):
        """Parse files for the config options and update the .config.

        This function parses the generated .config and include/autoconf.mk
        searching the target options.
        Move the config option(s) to the .config as needed.

        Arguments:
          defconfig: defconfig name.

        Returns:
          Return log string
        """

        results = []

        with open(self.dotconfig) as f:
            dotconfig_lines = f.readlines()

        with open(self.autoconf) as f:
            autoconf_lines = f.readlines()

        for config in self.configs:
            result = self.parse_one_config(config, dotconfig_lines,
                                           autoconf_lines)
            results.append(result)

        log = ''

        for (action, value) in results:
            if action == ACTION_MOVE:
                actlog = "Move '%s'" % value
                log_color = COLOR_LIGHT_GREEN
            elif action == ACTION_NO_ENTRY:
                actlog = "%s is not defined in Kconfig.  Do nothing." % value
                log_color = COLOR_LIGHT_BLUE
            elif action == ACTION_NO_CHANGE:
                actlog = "'%s' is the same as the define in Kconfig.  Do nothing." \
                         % value
                log_color = COLOR_LIGHT_PURPLE
            else:
                sys.exit("Internal Error. This should not happen.")

            log += log_msg(self.options.color, log_color, defconfig, actlog)

        with open(self.dotconfig, 'a') as f:
            for (action, value) in results:
                if action == ACTION_MOVE:
                    f.write(value + '\n')

        os.remove(self.config_autoconf)
        os.remove(self.autoconf)

        return log

class Slot:

    """A slot to store a subprocess.

    Each instance of this class handles one subprocess.
    This class is useful to control multiple threads
    for faster processing.
    """

    def __init__(self, configs, options, progress, devnull, make_cmd):
        """Create a new process slot.

        Arguments:
          configs: A list of CONFIGs to move.
          options: option flags.
          progress: A progress indicator.
          devnull: A file object of '/dev/null'.
          make_cmd: command name of GNU Make.
        """
        self.options = options
        self.progress = progress
        self.build_dir = tempfile.mkdtemp()
        self.devnull = devnull
        self.make_cmd = (make_cmd, 'O=' + self.build_dir)
        self.parser = KconfigParser(configs, options, self.build_dir)
        self.state = STATE_IDLE
        self.failed_boards = []

    def __del__(self):
        """Delete the working directory

        This function makes sure the temporary directory is cleaned away
        even if Python suddenly dies due to error.  It should be done in here
        because it is guranteed the destructor is always invoked when the
        instance of the class gets unreferenced.

        If the subprocess is still running, wait until it finishes.
        """
        if self.state != STATE_IDLE:
            while self.ps.poll() == None:
                pass
        shutil.rmtree(self.build_dir)

    def add(self, defconfig):
        """Assign a new subprocess for defconfig and add it to the slot.

        If the slot is vacant, create a new subprocess for processing the
        given defconfig and add it to the slot.  Just returns False if
        the slot is occupied (i.e. the current subprocess is still running).

        Arguments:
          defconfig: defconfig name.

        Returns:
          Return True on success or False on failure
        """
        if self.state != STATE_IDLE:
            return False
        cmd = list(self.make_cmd)
        cmd.append(defconfig)
        self.ps = subprocess.Popen(cmd, stdout=self.devnull,
                                   stderr=subprocess.PIPE)
        self.defconfig = defconfig
        self.state = STATE_DEFCONFIG
        return True

    def poll(self):
        """Check the status of the subprocess and handle it as needed.

        Returns True if the slot is vacant (i.e. in idle state).
        If the configuration is successfully finished, assign a new
        subprocess to build include/autoconf.mk.
        If include/autoconf.mk is generated, invoke the parser to
        parse the .config and the include/autoconf.mk, and then set the
        slot back to the idle state.

        Returns:
          Return True if the subprocess is terminated, False otherwise
        """
        if self.state == STATE_IDLE:
            return True

        if self.ps.poll() == None:
            return False

        if self.ps.poll() != 0:
            print >> sys.stderr, log_msg(self.options.color, COLOR_LIGHT_RED,
                                         self.defconfig, "Failed to process."),
            if self.options.verbose:
                print >> sys.stderr, color_text(self.options.color,
                                                COLOR_LIGHT_CYAN,
                                                self.ps.stderr.read())
            self.progress.inc()
            self.progress.show()
            if self.options.exit_on_error:
                sys.exit("Exit on error.")
            # If --exit-on-error flag is not set, skip this board and continue.
            # Record the failed board.
            self.failed_boards.append(self.defconfig)
            self.state = STATE_IDLE
            return True

        if self.state == STATE_AUTOCONF:
            self.log = self.parser.update_dotconfig(self.defconfig)

            """Save off the defconfig in a consistent way"""
            cmd = list(self.make_cmd)
            cmd.append('savedefconfig')
            self.ps = subprocess.Popen(cmd, stdout=self.devnull,
                                       stderr=subprocess.PIPE)
            self.state = STATE_SAVEDEFCONFIG
            return False

        if self.state == STATE_SAVEDEFCONFIG:
            if not self.options.dry_run:
                shutil.move(os.path.join(self.build_dir, 'defconfig'),
                            os.path.join('configs', self.defconfig))
            # Some threads are running in parallel.
            # Print log in one shot to not mix up logs from different threads.
            print self.log,
            self.progress.inc()
            self.progress.show()
            self.state = STATE_IDLE
            return True

        self.cross_compile = self.parser.get_cross_compile()
        if self.cross_compile is None:
            print >> sys.stderr, log_msg(self.options.color, COLOR_YELLOW,
                                         self.defconfig,
                                         "Compiler is missing.  Do nothing."),
            self.progress.inc()
            self.progress.show()
            if self.options.exit_on_error:
                sys.exit("Exit on error.")
            # If --exit-on-error flag is not set, skip this board and continue.
            # Record the failed board.
            self.failed_boards.append(self.defconfig)
            self.state = STATE_IDLE
            return True

        cmd = list(self.make_cmd)
        if self.cross_compile:
            cmd.append('CROSS_COMPILE=%s' % self.cross_compile)
        cmd.append('KCONFIG_IGNORE_DUPLICATES=1')
        cmd.append('include/config/auto.conf')
        self.ps = subprocess.Popen(cmd, stdout=self.devnull,
                                   stderr=subprocess.PIPE)
        self.state = STATE_AUTOCONF
        return False

    def get_failed_boards(self):
        """Returns a list of failed boards (defconfigs) in this slot.
        """
        return self.failed_boards

class Slots:

    """Controller of the array of subprocess slots."""

    def __init__(self, configs, options, progress):
        """Create a new slots controller.

        Arguments:
          configs: A list of CONFIGs to move.
          options: option flags.
          progress: A progress indicator.
        """
        self.options = options
        self.slots = []
        devnull = get_devnull()
        make_cmd = get_make_cmd()
        for i in range(options.jobs):
            self.slots.append(Slot(configs, options, progress, devnull,
                                   make_cmd))

    def add(self, defconfig):
        """Add a new subprocess if a vacant slot is found.

        Arguments:
          defconfig: defconfig name to be put into.

        Returns:
          Return True on success or False on failure
        """
        for slot in self.slots:
            if slot.add(defconfig):
                return True
        return False

    def available(self):
        """Check if there is a vacant slot.

        Returns:
          Return True if at lease one vacant slot is found, False otherwise.
        """
        for slot in self.slots:
            if slot.poll():
                return True
        return False

    def empty(self):
        """Check if all slots are vacant.

        Returns:
          Return True if all the slots are vacant, False otherwise.
        """
        ret = True
        for slot in self.slots:
            if not slot.poll():
                ret = False
        return ret

    def show_failed_boards(self):
        """Display all of the failed boards (defconfigs)."""
        failed_boards = []

        for slot in self.slots:
            failed_boards += slot.get_failed_boards()

        if len(failed_boards) > 0:
            msg = [ "The following boards were not processed due to error:" ]
            msg += failed_boards
            for line in msg:
                print >> sys.stderr, color_text(self.options.color,
                                                COLOR_LIGHT_RED, line)

            with open('moveconfig.failed', 'w') as f:
                for board in failed_boards:
                    f.write(board + '\n')

def move_config(configs, options):
    """Move config options to defconfig files.

    Arguments:
      configs: A list of CONFIGs to move.
      options: option flags
    """
    if len(configs) == 0:
        print 'Nothing to do. exit.'
        sys.exit(0)

    print 'Move %s (jobs: %d)' % (', '.join(configs), options.jobs)

    if options.defconfigs:
        defconfigs = [line.strip() for line in open(options.defconfigs)]
        for i, defconfig in enumerate(defconfigs):
            if not defconfig.endswith('_defconfig'):
                defconfigs[i] = defconfig + '_defconfig'
            if not os.path.exists(os.path.join('configs', defconfigs[i])):
                sys.exit('%s - defconfig does not exist. Stopping.' %
                         defconfigs[i])
    else:
        # All the defconfig files to be processed
        defconfigs = []
        for (dirpath, dirnames, filenames) in os.walk('configs'):
            dirpath = dirpath[len('configs') + 1:]
            for filename in fnmatch.filter(filenames, '*_defconfig'):
                defconfigs.append(os.path.join(dirpath, filename))

    progress = Progress(len(defconfigs))
    slots = Slots(configs, options, progress)

    # Main loop to process defconfig files:
    #  Add a new subprocess into a vacant slot.
    #  Sleep if there is no available slot.
    for defconfig in defconfigs:
        while not slots.add(defconfig):
            while not slots.available():
                # No available slot: sleep for a while
                time.sleep(SLEEP_TIME)

    # wait until all the subprocesses finish
    while not slots.empty():
        time.sleep(SLEEP_TIME)

    print ''
    slots.show_failed_boards()

def main():
    try:
        cpu_count = multiprocessing.cpu_count()
    except NotImplementedError:
        cpu_count = 1

    parser = optparse.OptionParser()
    # Add options here
    parser.add_option('-c', '--color', action='store_true', default=False,
                      help='display the log in color')
    parser.add_option('-d', '--defconfigs', type='string',
                      help='a file containing a list of defconfigs to move')
    parser.add_option('-n', '--dry-run', action='store_true', default=False,
                      help='perform a trial run (show log with no changes)')
    parser.add_option('-e', '--exit-on-error', action='store_true',
                      default=False,
                      help='exit immediately on any error')
    parser.add_option('-H', '--headers-only', dest='cleanup_headers_only',
                      action='store_true', default=False,
                      help='only cleanup the headers')
    parser.add_option('-j', '--jobs', type='int', default=cpu_count,
                      help='the number of jobs to run simultaneously')
    parser.add_option('-v', '--verbose', action='store_true', default=False,
                      help='show any build errors as boards are built')
    parser.usage += ' CONFIG ...'

    (options, configs) = parser.parse_args()

    if len(configs) == 0:
        parser.print_usage()
        sys.exit(1)

    # prefix the option name with CONFIG_ if missing
    configs = [ config if config.startswith('CONFIG_') else 'CONFIG_' + config
                for config in configs ]

    check_top_directory()

    check_clean_directory()

    update_cross_compile(options.color)

    if not options.cleanup_headers_only:
        move_config(configs, options)

    cleanup_headers(configs, options.dry_run)

if __name__ == '__main__':
    main()
