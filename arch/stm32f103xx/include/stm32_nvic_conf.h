/**
 * @file stm32f1xx_nvic_conf.h
 *
 * Copyright (C) 2020
 *
 * stm32f2xx_nvic_conf.h is free software: you can redistribute it and/or modify
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
#ifndef __STM32F1XX_NVIC_CONF_H
#define __STM32F1XX_NVIC_CONF_H

/*---------- includes ----------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "stm32f1xx.h"

/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/**
 * @brief get the pre-emption priority
 * @note if use this function, NVIC all bits must for pre-emption.
 * @param priority: interrput server handler prioriy
 *
 * @retval real priority
 */
__STATIC_INLINE uint32_t NVIC_ConfigPreEmptionPriority(uint32_t priority)
{
    uint32_t tmp = 0;

    assert_param(priority < (1UL << __NVIC_PRIO_BITS));
    tmp = NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                              ((1UL << __NVIC_PRIO_BITS) - 1UL - priority), 0);

    return tmp;
}

#endif /* __STM32F1XX_NVIC_CONF_H */
