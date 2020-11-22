/**
 * @file /bsp/RSTF103ZET6/bsp.h
 *
 * Copyright (C) 2020
 *
 * bsp.h is free software: you can redistribute it and/or modify
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
#ifndef __BSP_H
#define __BSP_H

/*---------- includes ----------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
extern void bsp_systick_deinit(void);
extern bool bsp_systick1ms_init(void);
extern void HAL_IncTick(void);
extern uint32_t HAL_GetTick(void);
extern void bsp_systick_isr(void);
extern bool bsp_init(void);
extern void bsp_deinit(void);
extern void bsp_udelay(uint32_t us);
extern void bsp_mdelay(uint32_t delay);

#endif /* __BSP_H */
