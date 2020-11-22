/**
 * @file /bsp/RSTF103ZET6/bsp.c
 *
 * Copyright (C) 2020
 *
 * bsp.c is free software: you can redistribute it and/or modify
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
#include "bsp.h"
#include "bsp_led.h"
#include "stm32f1xx_ll_conf.h"
#include "config/attributes.h"

/*---------- macro ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
static __IO uint32_t tick = 0;
static uint32_t clk_cycle = 0;

/*---------- function ----------*/
/**
 * @brief This function configures the Cortex-M3 Systick source to have 1ms time base
 *
 * @retval None
 */
bool bsp_systick1ms_init(void)
{
    LL_RCC_ClocksTypeDef RCC_ClocksStructure = {0};

    /* Configures the Cortex-M SysTick source to have 1ms time base. */
    LL_RCC_GetSystemClocksFreq(&RCC_ClocksStructure);
    LL_Init1msTick(RCC_ClocksStructure.HCLK_Frequency);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SYSTICK_EnableIT();
    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

    /* calc clock cycle
     * @note: clk_cycle is used for bsp_udelay()
     */
    clk_cycle = RCC_ClocksStructure.HCLK_Frequency / 1000000;

    return true;
}

/**
 * @brief  enable systick
 *
 * @retval None
 */
__STATIC_INLINE void __unused bsp_systick_enable(void)
{
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

/**
 * @brief disable systick
 *
 * @retval None
 */
__STATIC_INLINE void __unused bsp_systick_disable(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

/**
 * @brief Disable systick, close systick exception request
 *
 * @retval None
 */
void bsp_systick_deinit(void)
{
    bsp_systick_disable();
    LL_SYSTICK_DisableIT();
}

/**
 * @brief This function is called to increment a global
 *        variable "tick" used as application time base.
 * @note: In the default implementation, this variable is
 *      incremented each 1ms in Systick ISR.
 *
 * retval: None
 */
void HAL_IncTick(void)
{
    tick++;
}

/**
 * @brief Provides a tick value in millisecond
 *
 * @retval tick value
 */
uint32_t HAL_GetTick(void)
{
    return tick;
}

/**
 * @brief systick interrupt service funtion
 *
 * @retval None
 */
void bsp_systick_isr(void)
{
    uint32_t cnt = HAL_GetTick();

    if(!(cnt % 500)) {
        bsp_led_revert(BSP_LED_SYS);
    }
}

/**
 * @brief initialize some registers first
 *
 * @retval true
 */
bool bsp_init(void)
{
    extern uint32_t Image$$ER_IROM1$$Base[];
    /* Relocate vector table */
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | ((uint32_t)Image$$ER_IROM1$$Base - SRAM_BASE);  /* Vector Table Relocation in Internal SRAM */
#else
    SCB->VTOR = FLASH_BASE | ((uint32_t)Image$$ER_IROM1$$Base - FLASH_BASE); /* Vector Table Relocation in Internal FLASH */
#endif
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    while(true != LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_AFIO));

    return true;
}

void bsp_deinit(void)
{
    /* Disable SYSCFG Clock */
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_AFIO);
}

/**
 * bsp_udelay() - This function provides accurate delay (in microsecond) based on SysTick counter flag
 * @note: Blocking delay
 * @note: Based on the system HCLK.
 * @note: The delay time length can not exceed the SysTick LOAD, otherwise, use LL_mDelay().
 * @us: Delay specifies the delay time length, in microsecond.
 *
 * retval: None
 */
void bsp_udelay(__IO uint32_t us)
{
    uint32_t tmp1, tmp2, delta;

    tmp1 = SysTick->VAL;
    while(true) {
        tmp2 = SysTick->VAL;
        delta = tmp2 < tmp1 ? (tmp1 - tmp2) : (SysTick->LOAD - tmp2 + tmp1);
        if(delta >= (us * clk_cycle)) {
            break;
        }
    }
}

/**
 * bsp_mdelay() - This function provides accurate delay (in
 * milliseconds) based on SysTick counter flag
 * @note: When a RTOS is used, it is recommended to avoid using
 *      blocking delay and use rather osDelay service.
 * @note: To respect 1ms timebase, user should call @ref
 *      LL_Init1msTick function which will configure Systick to
 *      1ms.
 * @delay: Delay specifies the delay time length, in
 *       milliseconds.
 *
 * retval: None
 */
void bsp_mdelay(uint32_t delay)
{
    LL_mDelay(delay);
}
