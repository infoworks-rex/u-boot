#ifndef _CONFIG_M5272C3_H
#define _CONFIG_M5272C3_H

#define CONFIG_COMMANDS  ( CONFIG_CMD_DFL & ~(CFG_CMD_LOADS | CFG_CMD_LOADB) )
#include <cmd_confdefs.h>
#define CONFIG_BOOTDELAY 5

#define CFG_MAX_FLASH_SECT 11
#define CFG_CACHELINE_SIZE 16
#define CFG_MALLOC_LEN (256 << 10)
#define CFG_INIT_RAM_ADDR 0x20000000
#define CFG_INIT_RAM_SIZE 0x1000
#define CFG_INIT_DATA_OFFSET 0
#define CONFIG_BAUDRATE 19200
#define CFG_MONITOR_BASE 0x3e0000
#define CFG_MONITOR_LEN 0x20000
#define CFG_SDRAM_BASE 0
#define CFG_FLASH_BASE 0xffe00000
#define CFG_PROMPT "MCF5272C3> "
#define CFG_CBSIZE 1024
#define CFG_MAXARGS 64
#define CFG_LOAD_ADDR 0x20000
#define CFG_BOOTMAPSZ 0
#define CFG_BARGSIZE CFG_CBSIZE
#define CFG_BAUDRATE_TABLE { 9600 , 19200 , 38400 , 57600, 115200 }
#define CFG_ENV_ADDR 0xffe04000
#define CFG_ENV_SIZE 0x2000
#define CFG_ENV_IS_IN_FLASH 1
#define CFG_PBSIZE 1024
#define CFG_MAX_FLASH_BANKS 1
#define CFG_MEMTEST_START 0x400
#define CFG_MEMTEST_END   0x380000
#define CFG_HZ 1000000
#define CFG_FLASH_ERASE_TOUT 10000000

#define FEC_ENET

#define CONFIG_M5272

#endif	/* _CONFIG_M5272C3_H */
