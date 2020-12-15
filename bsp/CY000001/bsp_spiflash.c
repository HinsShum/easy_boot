/**
 * @file /bsp/RSTF103ZEY6/bsp_spiflash.c
 *
 * Copyright (C) 2020
 *
 * bsp_spiflash.c is free software: you can redistribute it and/or modify
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
#include "bsp_spiflash.h"
#include "stm32f1xx_ll_conf.h"
#include "w25qxx.h"

/*---------- macro ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
static struct st_w25qxx_describe m_bsp_spiflash;
struct st_device dev_w25qxx;

/*---------- function ----------*/
static bool bsp_spiflash_cs_ctl(bool on)
{
    if(on) {
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
    } else {
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
    }

    return true;
}

static uint8_t bsp_spiflash_xfer(uint8_t ch)
{
    while(true != LL_SPI_IsActiveFlag_TXE(SPI2));
    LL_SPI_TransmitData8(SPI2, ch);
    while(true != LL_SPI_IsActiveFlag_RXNE(SPI2));
    return LL_SPI_ReceiveData8(SPI2);
}

static void bsp_spiflash_io_init(void)
{
    LL_GPIO_InitTypeDef gpio_init_structure;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    while(true != LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB));
    /* GPIO configure
     * CS   ---> PB12
     * CLK  ---> PB13
     * MOSI ---> PB15
     * MISO ---> PB14
     */
    LL_GPIO_StructInit(&gpio_init_structure);
    gpio_init_structure.Pin = LL_GPIO_PIN_14;
    LL_GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.Pin = LL_GPIO_PIN_12;
    gpio_init_structure.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_init_structure.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &gpio_init_structure);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);

    gpio_init_structure.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
    gpio_init_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init_structure.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &gpio_init_structure);
}

static void bsp_spiflash_reg_init(void)
{
    LL_SPI_InitTypeDef spi_init_structure;

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    while(true != LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPI2));
    /* spi clock = 72M / 64 = 1.125M */
    LL_SPI_StructInit(&spi_init_structure);
    spi_init_structure.Mode = LL_SPI_MODE_MASTER;
    spi_init_structure.ClockPolarity = LL_SPI_POLARITY_LOW;
    spi_init_structure.ClockPhase = LL_SPI_PHASE_1EDGE;
    spi_init_structure.NSS = LL_SPI_NSS_SOFT;
    spi_init_structure.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
    LL_SPI_Init(SPI2, &spi_init_structure);
    LL_SPI_Enable(SPI2);
}

bool bsp_spiflash_init(void)
{
    bsp_spiflash_io_init();
    bsp_spiflash_reg_init();

    m_bsp_spiflash.cs_ctl = bsp_spiflash_cs_ctl;
    m_bsp_spiflash.xfer = bsp_spiflash_xfer;
    w25qxx_register(&dev_w25qxx, &m_bsp_spiflash);

    return true;
}

void bsp_spiflash_deinit(void)
{
    LL_SPI_Disable(SPI2);
    LL_SPI_DeInit(SPI2);
}

