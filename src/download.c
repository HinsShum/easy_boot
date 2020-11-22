/**
* @download.c
*
* @copyright This file creat by rensuiyi ,all right reserve!
*
* @author rensuyi
*
* @date 2016/12/7 18:51:05
*/
#include <string.h>
#include "options.h"
#include "download.h"
#include "components/ymodem/ymodem.h"
#include "driver/user_serial.h"
#include "platform.h"
#include "utils/md5.h"
#include "bsp_flash.h"
#include "errorno.h"
#include "firmware.h"

struct st_ymodem_describe m_ymodem;
struct st_md5_ctx m_md5_ctx;
static char m_firmware_md5[32 + 1];

extern struct st_platform_descibe g_platform;

static bool download_file_get_char(uint8_t *out_char, uint16_t timeout) {
    if(true == user_serial_get_char((char *)out_char, timeout)) {
        return true;
    }
    return false;
}

static void download_file_put_char(uint8_t pchar) {
    user_serial_write((char *)&pchar, 1);
}

static bool download_file_save_frame(uint32_t offset, uint8_t *pframe, uint32_t framesize) {
    md5_update(&m_md5_ctx, pframe, framesize);
    if(framesize & 0x03) {
        //align 4
        framesize = (framesize + 3) & 0xFFFFFFFC;
    }
    firmware_writetoflash(pframe, offset + FIRMWARE_ADDRESS_BASE, framesize);
    platform_feed_watchdog();

    return true;
}

static bool download_file_file_info_cb(char *pfilename, uint32_t filesize) {
    char *ptmp1 = NULL, *ptmp2 = NULL;

    if(filesize > SYS_ERASE_ADDR_END - SYS_ERASE_ADDR_START) {
        return false;
    }

    if(NULL == strstr(pfilename, SYS_HARD_VERSION)) {
        return false;
    }
    /* set model hw */
    if(true != firmware_setinfo(FIRMWARE_SETINFO_CMD_MODEL_HW, SYS_HARD_VERSION, strlen(SYS_HARD_VERSION))) {
        return false;
    }

    /* set model fun */
    ptmp1 = strstr(pfilename, "_");
    if(NULL == ptmp1) {
        return false;
    }
    ptmp2 = strstr(++ptmp1, "_");
    if(NULL == ptmp2) {
        return false;
    }
    if(true != firmware_setinfo(FIRMWARE_SETINFO_CMD_MODEL_FUN, (uint8_t *)ptmp1, ptmp2 - ptmp1)) {
        return false;
    }
    /* set build number */
    ptmp1 = strstr(++ptmp2, "_");
    if(NULL == ptmp1) {
        return false;
    }
    if(true != firmware_setinfo(FIRMWARE_SETINFO_CMD_BUILDNO, (uint8_t *)ptmp2, ptmp1 - ptmp2)) {
        return false;
    }
    /* set md5 */
    ptmp2 = strstr(++ptmp1, ".");
    if(NULL == ptmp2) {
        return false;
    }
    if(true != firmware_setinfo(FIRMWARE_SETINFO_CMD_MD5, (uint8_t *)ptmp1, ptmp2 - ptmp1)) {
        return false;
    }

    strncpy(m_firmware_md5, ptmp1, ptmp2 - ptmp1);
    m_firmware_md5[ptmp2 - ptmp1] = '\0';

    /* set firmware size */
    if(true != firmware_setinfo(FIRMWARE_SETINFO_CMD_SIZE, &filesize, 1)) {
        return false;
    }

    return true;
}

static uint8_t download_char_to_hex(uint8_t ch) {
    if((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    }
    if((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    }
    if((ch >= '0') && (ch <= '9')) {
        return ch - '0';
    }
    return 0;
}

static void download_str_to_hex(char *str, uint8_t *phex) {
    uint8_t i = 0;
    while((str != NULL) && (i < 32)) {
        if(i & 0x01) {
            phex[i >> 1] += download_char_to_hex(*str++);
        } else {
            phex[i >> 1] = download_char_to_hex(*str++) << 4;
        }
        i++;
    }
}

int32_t download_file(void) {
    uint8_t md5_value[16];
    uint8_t buf[32];
    int32_t res;
    uint8_t i = 0;

    memset(&m_ymodem, 0, sizeof(struct st_ymodem_describe));
    md5_init(&m_md5_ctx);

    m_ymodem.file_info_cb = download_file_file_info_cb;
    m_ymodem.save_frame = download_file_save_frame;
    m_ymodem.get_char = download_file_get_char;
    m_ymodem.put_char = download_file_put_char;
    m_ymodem.pbuf = (uint8_t *)g_platform.gbuf;

    res = ymodem_receive_file(&m_ymodem);
    if(res == YMODEM_ERR_OK) {
        /* check the md5 */
        md5_final(&m_md5_ctx, md5_value);
        download_str_to_hex(m_firmware_md5, buf);
        for(i = 0; i < 16; ++i) {
            if(buf[i] != md5_value[i]) {
                return SL_ERROR;
            }
        }
        firmware_setinfo(FIRMWARE_SETINFO_CMD_UPDATED_FALSE, NULL, 0);
        firmware_saveinfo();

        return SL_EOK;
    }
    return SL_ERROR;
}
