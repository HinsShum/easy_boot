/**
 * /src/platform.c
 *
 * Copyright (C) 2017 HinsShum
 *
 * platform.c is free software: you can redistribute it and/or modify
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
#include <string.h>
#include "bsp.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "bsp_watchdog.h"
#include "bsp_flash.h"
#include "bsp_spiflash.h"
#include "platform.h"
#include "simplefifo.h"
#include "config/options.h"
#include "device.h"
#include "config/errorno.h"
#include "sl_crc.h"

/* marco */
#define UART_FIFO_BUF_LENGTH    (1024)

/* type define */
typedef bool (*init_fnc_t)(void);
typedef void (*deinit_fnc_t)(void);

/* variable */
static uint8_t m_uart_fifo_buf[1048];
static struct st_platform_bl_info __attribute__((at(SYS_BL_INFO_RAM_ADDRESS))) bl_info = {
    .magic_number = 0x5A5A,
    .reserver = {0},
    .bl_version = SYS_BOOT_VERSION
};

/**
 * initlialize seuqence
 */
static init_fnc_t init_fnc_sequence[] = {
    bsp_init,
    bsp_systick1ms_init,
    bsp_flash_init,
    bsp_led_init,
    bsp_watchdog_init,
    bsp_uart_init,
    bsp_spiflash_init,
    NULL
};

/**
 * deinitlialize seuqence
 */
static deinit_fnc_t deinit_fnc_sequence[] = {
    bsp_deinit,
    bsp_systick_deinit,
    bsp_uart_deinit,
    bsp_spiflash_deinit,
    NULL
};

/*---------- variable ----------*/
struct st_platform_descibe g_platform;
extern struct st_device dev_w25qxx;

/* function */
static char *int2str(long digit, char *str, long str_len)
{
    char *p = NULL;
    char number = 0;
    long digit_tmp = digit;

    str_len--;
    *(str + str_len) = '\0';
    if(digit < 0) {
        digit = -digit;
    }

    do {
        number = digit % 10;
        *(str + --str_len) = '0' + number;
        digit /= 10;
    } while(digit != 0 && str_len > 1);

    if(str_len > 0) {
        if(digit_tmp < 0) {
            str_len--;
            *(str + str_len) = '-';
        }
        p = str + str_len;
    }

    return p;
}

static void platform_load_app_info(void)
{
    struct st_platform_bin_header bin_info;
    uint8_t tmp_buf[10] = { 0 };
    uint8_t *p = NULL;

    memset(&bin_info, 0, sizeof(struct st_platform_bin_header));
    /* Get application information */
    device_read(&dev_w25qxx, (void *)&bin_info, FIRMWARE_INFORMATION_ADDRESS, sizeof(struct st_platform_bin_header));
    
    if(bin_info.magic_number == PLATFORM_BIN_MAGIC) {
        /**
         * calc crc
         */
        if(0 != sl_revert_sum8((void *)&bin_info, sizeof(struct st_platform_bin_header))) {
            goto crc_err;
        }
        p = (uint8_t *)int2str(bin_info.bin_size, (char *)tmp_buf, sizeof(tmp_buf));
        strlcpy((char *)g_platform.app.image_size, (char *)p, sizeof(g_platform.app.image_size));
        p = (uint8_t *)int2str(bin_info.bin_buildno, (char *)tmp_buf, sizeof(tmp_buf));
        strlcpy((char *)g_platform.app.buildno, (char *)p, sizeof(g_platform.app.buildno));
        strlcpy((char *)g_platform.app.image_md5, (char *)bin_info.bin_md5, sizeof(g_platform.app.image_md5));
        if(bin_info.bin_updated == true) {
            strlcpy((char *)g_platform.app.image_updated, "true", sizeof(g_platform.app.image_updated));
        } else {
            strlcpy((char *)g_platform.app.image_updated, "false", sizeof(g_platform.app.image_updated));
        }
        strlcpy((char *)g_platform.app.model_hw, (char *)bin_info.model_hw, sizeof(g_platform.app.model_hw));
        strlcpy((char *)g_platform.app.model_fun, (char *)bin_info.model_fun, sizeof(g_platform.app.model_fun));

        g_platform.bin = bin_info;
    } else {
crc_err:
        strlcpy((char *)g_platform.app.image_md5, "UNKNOW", sizeof(g_platform.app.image_md5));
        strlcpy((char *)g_platform.app.image_size, "UNKNOW", sizeof(g_platform.app.image_size));
        strlcpy((char *)g_platform.app.buildno, "?", sizeof(g_platform.app.buildno));
        strlcpy((char *)g_platform.app.image_updated, "UNKNOW", sizeof(g_platform.app.image_updated));
        strlcpy((char *)g_platform.app.model_hw, SYS_HARD_VERSION, sizeof(g_platform.app.model_hw));
        strlcpy((char *)g_platform.app.model_fun, SYS_HARD_VERSION, sizeof(g_platform.app.model_fun));
    }
}

/**
 * platform_peripherals_init() - Initlialize the peripherals that will be used.
 * @note: intrinsic void __disable_irq();
 *        intrinsic void __enable_irq();
 *
 * retval: true
 */
static bool platform_peripherals_init(void)
{
    init_fnc_t *init_fnc_ptr = NULL;

    /* disable interrupt */
    __disable_irq();
    /* init function */
    for(init_fnc_ptr = init_fnc_sequence; *init_fnc_ptr != NULL; ++init_fnc_ptr) {
        if(true != (*init_fnc_ptr)()) {
            __error_handler(__FILE__, __LINE__);
        }
    }
    /* enable interrupt */
    __enable_irq();
    return true;
}

/**
 * platform_peripherals_deinit() - Uninitialize the peripherals.
 *
 * retval: None
 */
static void platform_peripherals_deinit(void)
{
    deinit_fnc_t *deinit_fnc_ptr = NULL;

    for(deinit_fnc_ptr = deinit_fnc_sequence; *deinit_fnc_ptr != NULL; ++deinit_fnc_ptr) {
        (*deinit_fnc_ptr)();
    }
}

/**
 * platform_misc_init() - other initlialize
 *
 * retval: true
 */
static bool platform_misc_init(void)
{
    platform_load_app_info();
    /* fifo for ymodem initlization */
    simplefifo_init(&g_platform.uart_fifo, m_uart_fifo_buf, UART_FIFO_BUF_LENGTH);

    return true;
}

/**
 * platform_driver_init() - driver initlialize
 * @note: all driver init function put here
 *
 * retval: Once the driver failed to initlialize, it returns
 * false.
 */
static bool platform_driver_init(void)
{
    /* if w25qxx init failed, it also starts normally
     */
    device_init(&dev_w25qxx);

    return true;
}

/**
 * platform_init() - Initlialize
 *
 * retval: None
 */
static inline void platform_hw_init(void)
{
    platform_peripherals_init();
}

/**
 * platform_deinit() - Uninitialize
 *
 * retval: None
 */
static inline void platform_hw_deinit(void)
{
    platform_peripherals_deinit();
}

bool platform_init(void)
{
    platform_hw_init();
    platform_driver_init();
    platform_misc_init();

    return true;
}

void platform_deinit(void)
{
    platform_hw_deinit();
}

/**
 * platform_feed_watchdog() - feed watch dog
 * @note: this function must be achieved in bsp
 *
 * retval: None
 */
void platform_feed_watchdog(void)
{
    extern void bsp_feeddog(void);
    bsp_feeddog();
}

/**
 * platform_write_chipin_flash() - write data to flash
 * @pageaddr: Specifie the address to be programmed.
 * @data: Specifie the data to be programmed
 * @length: Specifie the length to be programmed
 * @note: this function must be achieved in bsp
 *
 * retval: true/false
 */
bool platform_write_chipin_flash(uint32_t pageaddr, void *data, uint32_t length)
{
    extern bool bsp_flash_write(uint32_t pageaddr, void *data, uint32_t length);
    return bsp_flash_write(pageaddr, data, length);
}

/**
 * platform_chipin_flash_erase_block() - erase the specified
 * FLASH
 * @blocknum: Initial FLASH page number to erase when mass erase
 *          is disabled
 * @note: this function must be achieved in bsp
 *
 * retval: true/false
 */
bool platform_chipin_flash_erase_block(uint16_t blocknum)
{
    extern bool bsp_flash_erase_block(uint16_t blocknum);
    return bsp_flash_erase_block(blocknum);
}

void platform_start_application(void)
{
    void (*jump)(void);
    uint32_t jump_address;
    uint32_t sp_address = *(__IO uint32_t *)APPLICATION_ADDRESS;

    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
    if(sp_address > SYS_SRAM_ADDR_START && sp_address <= SYS_SRAM_END) {
        platform_deinit();
        /* Disable almost all interrupts */
        __disable_irq();
        /* Jump to user application */
        jump_address = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
        jump = (void (*)(void))jump_address;
        jump();
    }
}

/**
 * __error_handle() - User can add his own implementation report
 * the error return state.
 * @file: error file
 * @line: error line
 *
 * retval: None
 */
void __error_handler(char *file, int line)
{
    while(true) {
    }
}










