/**
 * @file /bsp/RSTF103ZET6/bsp_led.c
 *
 * Copyright (C) 2020
 *
 * bsp_led.c is free software: you can redistribute it and/or modify
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

/*---------- includes ----------*/
#include "stm32f1xx_ll_conf.h"
#include "bsp_led.h"

/*---------- macro ----------*/
#define BSP_LED_SYS_PORT            (GPIOB)
#define BSP_LED_SYS_PIN             (LL_GPIO_PIN_5)

/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/
bool bsp_led_init(void)
{
    LL_GPIO_InitTypeDef gpio_init_structure;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    while(true != LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB));

    LL_GPIO_SetOutputPin(BSP_LED_SYS_PORT, BSP_LED_SYS_PIN);
    LL_GPIO_StructInit(&gpio_init_structure);
    gpio_init_structure.Pin = BSP_LED_SYS_PIN;
    gpio_init_structure.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_init_structure.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_init_structure.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(BSP_LED_SYS_PORT, &gpio_init_structure);

    return true;
}

void bsp_led_deinit(void)
{
    LL_GPIO_SetOutputPin(BSP_LED_SYS_PORT, BSP_LED_SYS_PIN);
}

void bsp_led_on(uint8_t led_id)
{
    switch(led_id) {
        case BSP_LED_SYS: LL_GPIO_ResetOutputPin(BSP_LED_SYS_PORT, BSP_LED_SYS_PIN); break;
        default: break;
    }
}

void bsp_led_off(uint8_t led_id)
{
    switch(led_id) {
        case BSP_LED_SYS: LL_GPIO_SetOutputPin(BSP_LED_SYS_PORT, BSP_LED_SYS_PIN); break;
        default: break;
    }
}

void bsp_led_revert(uint8_t led_id)
{
    switch(led_id) {
        case BSP_LED_SYS: LL_GPIO_TogglePin(BSP_LED_SYS_PORT, BSP_LED_SYS_PIN); break;
        default: break;
    }
}

