#
# (C) Copyright 2000
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

sinclude $(TOPDIR)/board/amcc/liveoak/config.tmp

ifndef TEXT_BASE
TEXT_BASE = 0xFFFC0000
endif

ifeq ($(CONFIG_NAND_U_BOOT),y)
LDSCRIPT = $(TOPDIR)/board/$(BOARDDIR)/u-boot-nand.lds
endif

ifeq ($(CONFIG_SPI_U_BOOT),y)
LDSCRIPT = $(TOPDIR)/board/$(BOARDDIR)/u-boot-spi.lds
PAD_TO = 0x00840000
endif

ifeq ($(debug),1)
PLATFORM_CPPFLAGS += -DDEBUG
endif
