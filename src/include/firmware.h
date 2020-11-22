/**
 * /src/firmware.h
 *
 * Copyright (C) 2017 HinsShum
 *
 * firmware.h is free software: you can redistribute it and/or modify
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
 */
#ifndef __FIRMWARE_H
#define __FIRMWARE_H

/* include */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* marco */
#define FIRMWARE_UPDATE_OK                      (0)
#define FIRMWARE_UPDATE_ERR                     (-1)
#define FIRMWARE_CHECK_ERR                      (-2)

#ifndef SPI_FLASH_PAGE_SIZE
#define SPI_FLASH_PAGE_SIZE                     (256)
#endif
#ifndef SPI_FLASH_BLOCK_SIZE
#define SPI_FLASH_BLOCK_SIZE                    (1024 * 4)
#endif
#ifndef FIRMWARE_INFORMATION_ADDRESS
#define FIRMWARE_INFORMATION_ADDRESS            (0x0)
#endif
#ifndef FIRMWARE_ADDRESS_BASE
#define FIRMWARE_ADDRESS_BASE                   (FIRMWARE_INFORMATION_ADDRESS + SPI_FLASH_BLOCK_SIZE)
#endif
#ifndef FIRMWARE_ADDRESS_END
#define FIRMWARE_ADDRESS_END                    (1024L * 64)
#endif
/* chipin flash */
#ifndef FLASH_PROGRAM_START_PHYSICAL_ADDRESS
#define FLASH_PROGRAM_START_PHYSICAL_ADDRESS    (0x08000000)
#endif
#ifndef FLASH_BLOCK_SIZE
#define FLASH_BLOCK_SIZE                        (128U)
#endif

/**
 * Set info cmd
 */
#define FIRMWARE_SETINFO_CMD_BUILDNO            (0x01)
#define FIRMWARE_SETINFO_CMD_SIZE               (0x02)
#define FIRMWARE_SETINFO_CMD_MD5                (0x03)
#define FIRMWARE_SETINFO_CMD_MODEL_FUN          (0x04)
#define FIRMWARE_SETINFO_CMD_MODEL_HW           (0x05)
#define FIRMWARE_SETINFO_CMD_UPDATED_TRUE       (0x06)
#define FIRMWARE_SETINFO_CMD_UPDATED_FALSE      (0x07)

/* type define */
struct st_firmware_describe {
    uint8_t *firmware_buf;
    uint32_t chipin_addr_base;
    uint32_t chipin_addr_end;
    uint32_t chipout_addr_base;
    uint32_t chipout_addr_end;
    uint32_t firmware_information_addr;

    int32_t (*write_chipin)(void *buf, uint32_t addr, uint32_t len);
    int32_t (*read_chipin)(void *buf, uint32_t addr, uint32_t len);
    int32_t (*write_chipout)(void *buf, uint32_t addr, uint32_t len);
    int32_t (*read_chipout)(void *buf, uint32_t addr, uint32_t len);
};

/* prototype */
extern int32_t firmware_updating(uint8_t *checkval_buf, uint32_t checkval_len, uint32_t firmware_size);
extern int32_t firmware_writetoflash(void *buf, uint32_t addr, uint32_t len);
extern int32_t firmware_eraseflashblock(uint32_t block_addr);
extern bool firmware_setinfo(uint8_t cmd, void *buf, uint8_t len);
extern bool firmware_saveinfo(void);

#endif /* __FIRMWARE_H */
