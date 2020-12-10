/**
 * @file /arch/stm32f103xx/board/CY000001/cpu_freq.c
 *
 * Copyright (C) 2020
 *
 * cpu_freq.c is free software: you can redistribute it and/or modify
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
#include "cpu_freq.h"
#include "stm32f1xx_ll_conf.h"

/*---------- macro ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/
void cpu_freq_config(void)
{
    LL_UTILS_PLLInitTypeDef pll_init_structure = {0};
    LL_UTILS_ClkInitTypeDef clk_init_structure = {0};

    /* Restore RCC */
    LL_RCC_DeInit();
    /* Enable Prefetch Buffer */
    LL_FLASH_EnablePrefetch();
    /* After a system reset, the HSI oscollator will be selected as system clock.
     * If config System clock from HSE failed, automatic switch to HSI(MAX: 64MHz)
     * HSI will keep enable, it will be used when operate embedded flash memory
     * AHB_CLK = PLL_CLK/1
     * APB1_CLK = AHB_CLK/2
     * APB2_CLK = AHB_CLK/1
     * APB1_PERIPHERAL = APB1_CLK
     * APB1_TIMER = APB1_CLK * 2
     * APB2_PERIPHERAL = APB2_CLK
     * APB2_TIMER = APB2_CLK
     */
#if HSE_VALUE > 8000000
    pll_init_structure.Prediv =LL_RCC_PREDIV_DIV_2;
#else
    pll_init_structure.Prediv = LL_RCC_PREDIV_DIV_1;
#endif
    pll_init_structure.PLLMul = LL_RCC_PLL_MUL_9;
    clk_init_structure.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
    clk_init_structure.APB1CLKDivider = LL_RCC_APB1_DIV_2;
    clk_init_structure.APB2CLKDivider = LL_RCC_APB2_DIV_1;
    if(SUCCESS != LL_PLL_ConfigSystemClock_HSE(HSE_VALUE, LL_UTILS_HSEBYPASS_OFF,
                                               &pll_init_structure, &clk_init_structure)) {
        pll_init_structure.PLLMul = LL_RCC_PLL_MUL_16;
        if(SUCCESS != LL_PLL_ConfigSystemClock_HSI(&pll_init_structure, &clk_init_structure)) {
            while(true);
        }
    }
    /* close other no use oscollator */
    LL_RCC_LSI_Disable();
    LL_RCC_LSE_Disable();
}

