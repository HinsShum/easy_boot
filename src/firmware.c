/**
 * /src/firmware.c
 *
 * Copyright (C) 2017 HinsShum
 *
 * firmware.c is free software: you can redistribute it and/or modify
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

/*---------- includes ----------*/
#include "md5.h"
#include "bsp_flash.h"
#include "user_serial.h"
#include "w25qxx.h"
#include "options.h"
#include "platform.h"
#include "sl_crc.h"
#include "device.h"
#include "firmware.h"
#include <stdlib.h>

/* marco */
#define firmware_debug(x, y...)     printk(x, ##y)

#define FIRMWARE_OK     (0)
#define FIRMWARE_ERR    (-1)

#define SIZE_1K         (1024)

extern struct st_device dev_w25qxx;
extern struct st_platform_descibe g_platform;

/* function */
static void firmware_memcpy(uint32_t dst, uint32_t src, uint32_t length)
{
    for(; length > 0; --length) {
        *(uint8_t *)dst = *(uint8_t *)src;
        dst++;
        src++;
    }
}

static uint8_t firmware_char2hex(uint8_t ch)
{
    if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    }
    if ((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    }
    if ((ch >= '0') && (ch <= '9')) {
        return ch - '0';
    }

    return 0;
}

static void firmware_str2hex(uint8_t *str, uint32_t str_len, uint8_t *phex)
{
    uint8_t i = 0;

    while(str != NULL && i < str_len) {
        if(i & 0x01) {
            phex[i >> 1] += firmware_char2hex(*str++);
        } else {
            phex[i >> 1] = firmware_char2hex(*str++) << 4;
        }
        i++;
    }
}

static int32_t firmware_verify(uint8_t *checkval_buf, uint32_t val_len, uint32_t firmware_size)
{
    uint8_t md5_val[16] = { 0 }, buf[16] = { 0 };
    struct st_md5_ctx md5_ctx;
    uint32_t read_size = 0;
    uint32_t firmware_addr = FIRMWARE_ADDRESS_BASE;

    md5_init(&md5_ctx);
    while(true) {
        read_size = firmware_size > SIZE_1K ? SIZE_1K : firmware_size;
        device_read(&dev_w25qxx, g_platform.gbuf, firmware_addr, read_size);
        md5_update(&md5_ctx, (unsigned char *)g_platform.gbuf, read_size);
        firmware_size -= read_size;
        firmware_addr += read_size;
        if(firmware_size == 0) {
            break;
        }
    }
    md5_final(&md5_ctx, md5_val);
    firmware_str2hex(checkval_buf, val_len, buf);
    for(uint8_t i = 0; i < 16; ++i) {
        if(buf[i] != md5_val[i]) {
            return FIRMWARE_ERR;
        }
    }

    return FIRMWARE_OK;
}

static int32_t firmware_burn(uint8_t *checkval_buf, uint32_t val_len, uint32_t firmware_size)
{
    uint8_t md5_val[16] = { 0 }, buf[16] = { 0 };
    uint8_t md5_calc_buf[256] = { 0 };
    struct st_md5_ctx md5_ctx;
    uint32_t block_current = (SYS_ERASE_ADDR_START - FLASH_PROGRAM_START_PHYSICAL_ADDRESS) / FLASH_BLOCK_SIZE;
    uint32_t block_end = ((uint32_t)SYS_ERASE_ADDR_START + firmware_size - \
                          (uint32_t)FLASH_PROGRAM_START_PHYSICAL_ADDRESS) / FLASH_BLOCK_SIZE;
    uint32_t firmware_addr = FIRMWARE_ADDRESS_BASE, app_addr = APPLICATION_ADDRESS;
    uint32_t read_size = 0, size = firmware_size;

    block_end = ((uint32_t)SYS_ERASE_ADDR_START + firmware_size) & (FLASH_BLOCK_SIZE - 1) ? block_end + 1 : block_end;
    for(; block_current < block_end; ++block_current) {
        platform_chipin_flash_erase_block((uint16_t)block_current);
        user_serial_write("#", 1);
    }
    user_serial_write("\r\nBurn Firmware", strlen("\r\nBurn Firmware"));
    while(true) {
        platform_feed_watchdog();
        user_serial_write(".", 1);
        read_size = size > SIZE_1K ? SIZE_1K : size;
        device_read(&dev_w25qxx, g_platform.gbuf, firmware_addr, read_size);
        platform_write_chipin_flash(app_addr, (void *)g_platform.gbuf, read_size);
        size -= read_size;
        firmware_addr += read_size;
        app_addr += read_size;
        if(size == 0) {
            break;
        }
    }
    user_serial_write("\r\n", 2);
    app_addr = APPLICATION_ADDRESS;
    size = firmware_size;
    md5_init(&md5_ctx);
    while(true) {
        platform_feed_watchdog();
        read_size = size > sizeof(md5_calc_buf) ? sizeof(md5_calc_buf) : size;
        firmware_memcpy((uint32_t)md5_calc_buf, app_addr, read_size);
        md5_update(&md5_ctx, (unsigned char *)md5_calc_buf, read_size);
        app_addr += read_size;
        size -= read_size;
        if(size == 0) {
            break;
        }
    }
    md5_final(&md5_ctx, md5_val);

    firmware_str2hex(checkval_buf, val_len, buf);
    for(uint8_t i = 0; i < 16; ++i) {
        if(buf[i] != md5_val[i]) {
            return FIRMWARE_ERR;
        }
    }

    return FIRMWARE_OK;
}

int32_t firmware_updating(uint8_t *checkval_buf, uint32_t checkval_len, uint32_t firmware_size)
{
    uint32_t erase_block_addr = FIRMWARE_INFORMATION_ADDRESS;

    if(FIRMWARE_OK != firmware_verify(checkval_buf, checkval_len, firmware_size)) {
        return FIRMWARE_CHECK_ERR;
    }

    if(FIRMWARE_OK != firmware_burn(checkval_buf, checkval_len, firmware_size)) {
        return FIRMWARE_UPDATE_ERR;
    }

    g_platform.bin.bin_updated = true;
    device_ioctl(&dev_w25qxx, W25QXX_IOCTL_ERAES_SEGMENT, &erase_block_addr);
    g_platform.bin.crc_sum = 0;
    g_platform.bin.crc_sum = sl_revert_sum8(&g_platform.bin, sizeof(struct st_platform_bin_header));
    device_write(&dev_w25qxx, &g_platform.bin, FIRMWARE_INFORMATION_ADDRESS, sizeof(struct st_platform_bin_header));
    
    return FIRMWARE_UPDATE_OK;
}

int32_t firmware_writetoflash(void *buf, uint32_t addr, uint32_t len)
{
    return device_write(&dev_w25qxx, buf, addr, len);
}

int32_t firmware_eraseflashblock(uint32_t block_addr)
{
    return device_ioctl(&dev_w25qxx, W25QXX_IOCTL_ERAES_SEGMENT, &block_addr);
}

bool firmware_setinfo(uint8_t cmd, void *buf, uint8_t len)
{
    uint8_t *pbuf = (uint8_t *)buf;

    switch(cmd) {
        case FIRMWARE_SETINFO_CMD_MODEL_HW :
            strncpy((char *)g_platform.bin.model_hw, (char *)pbuf, len);
            g_platform.bin.model_hw[len] = '\0';
            break;
        case FIRMWARE_SETINFO_CMD_MODEL_FUN :
            if(len >= sizeof(g_platform.bin.model_fun)) {
                return false;
            }
            strncpy((char *)g_platform.bin.model_fun, (char *)pbuf, len);
            g_platform.bin.model_fun[len] = '\0';
            break;
        case FIRMWARE_SETINFO_CMD_BUILDNO :
            if(len > 3) {
                return false;
            }
            g_platform.bin.bin_buildno = 0;
            do {
                g_platform.bin.bin_buildno = g_platform.bin.bin_buildno * 10 + (*pbuf - '0');
                len--;
                pbuf++;
            } while(len != 0);
            break;
        case FIRMWARE_SETINFO_CMD_MD5 :
            if(len >= sizeof(g_platform.bin.bin_md5)) {
                return false;
            }
            strncpy((char *)g_platform.bin.bin_md5, (char *)pbuf, len);
            g_platform.bin.bin_md5[len] = '\0';
            break;
        case FIRMWARE_SETINFO_CMD_SIZE :
            g_platform.bin.bin_size = *(uint32_t *)buf;
            break;
        case FIRMWARE_SETINFO_CMD_UPDATED_TRUE :
            g_platform.bin.bin_updated = true;
            break;
        case FIRMWARE_SETINFO_CMD_UPDATED_FALSE :
            g_platform.bin.bin_updated = false;
            break;
        default : return false;
    }

    return true;
}

bool firmware_saveinfo(void)
{
    g_platform.bin.crc_sum = 0;
    g_platform.bin.magic_number = PLATFORM_BIN_MAGIC;
    g_platform.bin.crc_sum = sl_revert_sum8(&g_platform.bin, sizeof(struct st_platform_bin_header));
    device_write(&dev_w25qxx, &g_platform.bin, FIRMWARE_INFORMATION_ADDRESS, sizeof(struct st_platform_bin_header));
        
    return true;
}

