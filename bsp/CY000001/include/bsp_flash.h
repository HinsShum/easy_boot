/**
 * @file /bsp/RSTF103ZET6/bsp_flash.h
 *
 * Copyright (C) 2020
 *
 * bsp_flash.h is free software: you can redistribute it and/or modify
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
#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

/*---------- includes ----------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
extern bool bsp_flash_write(uint32_t pageaddr, void *data, uint32_t length);
extern bool bsp_flash_erase(uint32_t pageaddr, uint32_t erase_pages);
extern bool bsp_flash_erase_block(uint16_t blocknum);
extern bool bsp_flash_init(void);

#endif /* __BSP_FLASH_H */
