/**
 * @file /config/CY000001/options.h
 *
 * Copyright (C) 2020
 *
 * options.h is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author HinsShum hinsshum@qq.com
 */
#ifndef __RSTF103ZET6_OPTIONS_H
#define __RSTF103ZET6_OPTIONS_H

/*---------- includes ----------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "stm32f1xx_ll_conf.h"
#include "stm32f1xx_hal_flash_ex.h"

/*---------- macro ----------*/
#define SYS_BOOT_VERSION                        ("V1.00")
#ifndef SYS_HARD_VERSION
#define SYS_HARD_VERSION                        ("CY000001")
#endif

/* bootloader information address in SRAM */
#define SYS_BL_INFO_RAM_ADDRESS                 (SRAM_BASE)

/* MCU SRAM information */
#define SYS_SRAM_ADDR_START                     (SRAM_BASE)
#define SYS_SRAM_SIZE                           (0x10000) /* 64K */
#define SYS_SRAM_END                            (SYS_SRAM_ADDR_START + SYS_SRAM_SIZE)

/* MCU Flash information */
#define SYS_FLASH_CHIPIN_BASE                   (FLASH_BASE)
#define SYS_FLASH_CHIPIN_SIZE                   (0x80000) /* 512K */
#define SYS_FLASH_CHIPIN_BLOCK_SIZE             (0x800) /* 2K */
#define SYS_FLASH_CHIPIN_END                    (SYS_FLASH_CHIPIN_BASE + SYS_FLASH_CHIPIN_SIZE)

/* firmware information in chipin flash */
#define BOOTLOAD_MAX_SIZE                       (0x400 * 20) /* 20K boot image size */
#define APPLICATION_ADDRESS                     (SYS_FLASH_CHIPIN_BASE + BOOTLOAD_MAX_SIZE)
#define SYS_ERASE_ADDR_START                    (APPLICATION_ADDRESS)
#define SYS_ERASE_ADDR_END                      (SYS_FLASH_CHIPIN_END - 1)
#define FLASH_PROGRAM_START_PHYSICAL_ADDRESS    (SYS_FLASH_CHIPIN_BASE)
#define FLASH_BLOCK_SIZE                        (SYS_FLASH_CHIPIN_BLOCK_SIZE)

/* firmware information about exteral flash */
#define SPI_FLASH_BLOCK_SIZE                    (0x1000) /* 4K */
#define SPI_FLASH_MAX_SIZE                      (0X800000) /* 8M */
#define SPI_FLASH_PAGE_SIZE                     (SPI_FLASH_MAX_SIZE / SPI_FLASH_BLOCK_SIZE)
#define SYS_APP_MAX_SIZE                        (SYS_FLASH_CHIPIN_SIZE - BOOTLOAD_MAX_SIZE)
#define FIRMWARE_INFORMATION_ADDRESS            (SPI_FLASH_MAX_SIZE - SYS_APP_MAX_SIZE - SPI_FLASH_BLOCK_SIZE)
#define FIRMWARE_ADDRESS_BASE                   (FIRMWARE_INFORMATION_ADDRESS + SPI_FLASH_BLOCK_SIZE)
#define FIRMWARE_ADDRESS_END                    (FIRMWARE_ADDRESS_BASE + SYS_APP_MAX_SIZE)

/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
#endif /* __RSTF103ZET6_OPTIONS_H */
