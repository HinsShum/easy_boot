/**
 * @file /bsp/RSTF103ZET6/bsp_uart.c
 *
 * Copyright (C) 2020
 *
 * bsp_uart.c is free software: you can redistribute it and/or modify
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
#include "bsp_uart.h"
#include "stm32f1xx_ll_conf.h"
#include "user_serial.h"

/*---------- macro ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/
static void bsp_uart_io_init(void)
{
    LL_GPIO_InitTypeDef gpio_init_structure;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    while(true != LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOA));
    /* GPIO configure
     * PA9  ---> TX
     * PA10 ---> RX
     */
    LL_GPIO_StructInit(&gpio_init_structure);
    gpio_init_structure.Pin = LL_GPIO_PIN_10;
    LL_GPIO_Init(GPIOA, &gpio_init_structure);

    gpio_init_structure.Pin = LL_GPIO_PIN_9;
    gpio_init_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &gpio_init_structure);
}

static void bsp_uart_reg_init(void)
{
    LL_USART_InitTypeDef uart_init_structure;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    while(true != LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_USART1));

    LL_USART_StructInit(&uart_init_structure);
    uart_init_structure.BaudRate = 115200;
    LL_USART_Init(USART1, &uart_init_structure);
    LL_USART_ConfigAsyncMode(USART1);

    LL_USART_ClearFlag_RXNE(USART1);
    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_Enable(USART1);
}

static void bsp_uart_nvic_init(void)
{
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 14, 0));
    NVIC_EnableIRQ(USART1_IRQn);
}

static uint16_t bsp_uart_write(uint8_t *pbuf, uint16_t len)
{
    for(uint16_t i = 0; i < len; ++i) {
        LL_USART_TransmitData8(USART1, pbuf[i]);
        while(SET != LL_USART_IsActiveFlag_TC(USART1));
    }

    return len;
}

bool bsp_uart_init(void)
{
    bsp_uart_io_init();
    bsp_uart_reg_init();
    bsp_uart_nvic_init();
    user_serial_register(bsp_uart_write);

    return true;
}

void bsp_uart_deinit(void)
{
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_USART_DeInit(USART1);
    LL_USART_Disable(USART1);
    NVIC_DisableIRQ(USART1_IRQn);
    user_serial_unregister();
}

