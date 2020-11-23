/**
 * /src/main.c
 *
 * Copyright (C) 2017 HinsShum
 *
 * main.c is free software: you can redistribute it and/or modify
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
#include "platform.h"
#include "errorno.h"
#include "strategy.h"
#include "firmware.h"
#include "user_serial.h"
#include "cpu_freq.h"

/* prototype */
void systemclock_config(void);
extern struct st_platform_descibe g_platform;

/* function */
int main(void)
{
    int32_t retval = FIRMWARE_UPDATE_OK;

    cpu_freq_config();
    /* Initialize some of the necessary things for bootloader */
    platform_init();
    if(CY_EOK == strategy_check_serial()) {
        strategy_cmd_process();
    }
    if(g_platform.bin.magic_number == PLATFORM_BIN_MAGIC && g_platform.bin.bin_updated == false) {
        /* read image from spi flash */
        retval = firmware_updating(g_platform.bin.bin_md5, strlen((char *)g_platform.bin.bin_md5), g_platform.bin.bin_size);
    }
    if(retval == FIRMWARE_UPDATE_OK) {
        user_serial_write("Start application\r\n", strlen("Start application\r\n"));
        /* start application */
        platform_start_application();
    }
    /* if can not jump to app, than reset */
    if(retval == FIRMWARE_CHECK_ERR) {
        user_serial_write("Firmware verify failed\r\n", strlen("Firmware verify failed\r\n"));
    } else if(retval == FIRMWARE_UPDATE_ERR) {
        user_serial_write("Firmware burn failed\r\n", strlen("Firmware burn failed\r\n"));
    } else {
        user_serial_write("No application exit\r\n", strlen("No application exit\r\n"));
    }
    platform_system_reset();

    while(true);
}
