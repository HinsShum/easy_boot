/**
 * /src/platform.h
 *
 * Copyright (C) 2017 HinsShum
 *
 * platform.h is free software: you can redistribute it and/or modify
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
#ifndef __PLATFORM_H
#define __PLATFORM_H

/* include */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "simplefifo.h"
#include "options.h"

/* marco */
#define PLATFORM_BIN_MAGIC  (0x5A5A)

/*---------- type define ----------*/
struct st_platform_soft_info {
    int8_t image_size[8];
    int8_t buildno[4];
    int8_t image_md5[32 + 1];
    int8_t image_updated[7];
    uint8_t model_hw[8 + 1];
    uint8_t model_fun[8 + 1];
};

struct st_platform_bl_info {
    uint16_t magic_number;
    uint8_t reserver[6];
    char *bl_version;
};

struct st_platform_bin_header {
    uint8_t crc_sum;
    uint8_t reserve[3];
    uint32_t bin_buildno;
    uint32_t bin_size;
    uint8_t bin_md5[32 + 1];
    bool bin_updated;
    uint16_t magic_number;
    uint8_t model_hw[8 + 1];
    uint8_t model_fun[8 + 1];
};

struct st_platform_descibe {
    struct st_platform_bin_header bin;
    struct st_platform_soft_info app;
    struct st_simplefifo_describe uart_fifo;
    char gbuf[1200];
    uint8_t flash_ok;
};

/* variable prototype */
/* function prototype */
extern bool platform_init(void);
extern void platform_deinit(void);
extern void platform_feed_watchdog(void);
extern bool platform_write_chipin_flash(uint32_t pageaddr, void *data, uint32_t length);
extern bool platform_chipin_flash_erase_block(uint16_t blocknum);
extern void platform_start_application(void);
extern void __error_handler(char *file, int line);

/**
 * platform_system_reset() - Reset System
 *
 * retval: None
 */
static inline void platform_system_reset(void)
{
    NVIC_SystemReset();
}

#endif /* __PLATFORM_H */
