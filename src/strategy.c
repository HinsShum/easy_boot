/**
* @strategy.c
*
* @copyright This file creat by rensuiyi ,all right reserve!
*
* @author rensuyi
*
* @date 2016/12/6 16:34:51
*/
#include <string.h>
#include "options.h"
#include "strategy.h"
#include "user_serial.h"
#include "platform.h"
#include "download.h"
#include "bsp_flash.h"
#include "firmware.h"
#include "sl_crc.h"
#include "md5.h"
#include "errorno.h"
#include "utils.h"

extern struct st_platform_descibe g_platform;

int32_t strategy_check_serial(void) {
    char ch = 0x00;
    /*Send BOOTME*/
    user_serial_write("\r\nBOOTME\r\n", 10);

    if(true == user_serial_get_char(&ch, 1500)) {
        if(ch == ' ') {
            return CY_EOK;
        }
    }
    if(true == user_serial_get_char(&ch, 1500)) {
        if(ch == ' ') {
            return CY_EOK;
        }
    }
    return CY_E_BUSY;
}

static int32_t strategy_erase_flash(void) {
    for(uint32_t erase_block_addr = FIRMWARE_INFORMATION_ADDRESS; erase_block_addr < FIRMWARE_ADDRESS_END;
        erase_block_addr += SPI_FLASH_BLOCK_SIZE) {
        firmware_eraseflashblock(erase_block_addr);
        user_serial_write("#", 1);
    }
    user_serial_write("\r\nOK\r\n", strlen("\r\nOK\r\n"));

    return CY_EOK;
}

static int32_t strategy_download_file(void) {
    int32_t res;

    for(uint32_t erase_block_addr = FIRMWARE_INFORMATION_ADDRESS; erase_block_addr < FIRMWARE_ADDRESS_END;
        erase_block_addr += SPI_FLASH_BLOCK_SIZE) {
        firmware_eraseflashblock(erase_block_addr);
    }

    res = download_file();

    if(CY_EOK == res) {
        user_serial_write("\r\nOK\r\n", strlen("\r\nOK\r\n"));
    } else {
        user_serial_write("\r\nERROR\r\n", strlen("\r\nERROR\r\n"));
    }
    return res;
}

static int32_t strategy_get_info(void) {
    user_serial_write("\r\n", 2);

    user_serial_write(SYS_BOOT_VERSION, strlen(SYS_BOOT_VERSION));
    user_serial_write("\r\n", 2);

    user_serial_write((char *)g_platform.app.model_hw, strlen((char *)g_platform.app.model_hw));
    user_serial_write("\r\n", 2);

    user_serial_write((char *)g_platform.app.model_fun, strlen((char *)g_platform.app.model_fun));

    user_serial_write("\r\nBUILDNO:", strlen("\r\nBUILDNO:"));
    user_serial_write((char *)g_platform.app.buildno, strlen((char *)g_platform.app.buildno));

    user_serial_write("\r\nSIZE:", strlen("\r\nSIZE:"));
    user_serial_write((char *)g_platform.app.image_size, strlen((char *)g_platform.app.image_size));

    user_serial_write("\r\nMD5:", strlen("\r\nMD5:"));
    user_serial_write((char *)g_platform.app.image_md5, strlen((char *)g_platform.app.image_md5));

    user_serial_write("\r\nUPDATED:", strlen("\r\nUPDATED:"));
    user_serial_write((char *)g_platform.app.image_updated, strlen((char *)g_platform.app.image_updated));

    {
        uint32_t base = FIRMWARE_ADDRESS_BASE, end = FIRMWARE_ADDRESS_END;
        uint8_t buf[18] = { 0 };

        utils_strcathex((char *)buf, base);
        strcat((char *)buf, "\\");
        utils_strcathex((char *)buf, end);
        user_serial_write("\r\nSF:", strlen("\r\nSF:"));
        user_serial_write((char *)buf, strlen((char *)buf));

        buf[0] = '\0';
        base = SYS_ERASE_ADDR_START;
        end = SYS_ERASE_ADDR_END + 1;
        utils_strcathex((char *)buf, base);
        strcat((char *)buf, "\\");
        utils_strcathex((char *)buf, end);
        user_serial_write("\r\nCF:", strlen("\r\nCF:"));
        user_serial_write((char *)buf, strlen((char *)buf));
    }
    user_serial_write("\r\n", 2);

    return CY_EOK;
}

int32_t strategy_cmd_process(void) {
    char ch = 0x00;
    user_serial_drop();
    user_serial_write("\r\nCMD\r\n", 7);
    while(true) {
        // wait for 10S
        platform_feed_watchdog();
        if(true == user_serial_get_char(&ch, 10000)) {
            switch(ch) {
                case 'E':
                case 'e':
                    /* Erese the flash */
                    strategy_erase_flash();
                    break;
                case 'I':
                case 'i':
                    /* get the system info */
                    strategy_get_info();
                    break;
                case 'Y':
                case 'y':
                    strategy_download_file();
                    /* ymodem get files */
                    break;
                case 'g':
                case 'G':
                    user_serial_write("\r\nGO\r\n", 6);
                    platform_start_application();
                case 'r':
                case 'R':
                    user_serial_write("\r\nOK\r\n", 6);
                    platform_system_reset();
                default:
                    break;
            }
            user_serial_drop();
        } else {
            /* time out */
            break;
        }
    }
    
    return CY_EOK;
}

