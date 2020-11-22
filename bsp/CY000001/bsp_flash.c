/**
 * @file /bsp/RSTF103ZET6/bsp_flash.c
 *
 * Copyright (C) 2020
 *
 * bsp_flash.c is free software: you can redistribute it and/or modify
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
#include "bsp_flash.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_conf.h"
#include "options.h"

/*---------- macro ----------*/
#define FLASH_LOCK_ENABLE               (0)

#if FLASH_LOCK_ENABLE > 0
#define __bsp_flash_write_enter()       __disable_irq()
#define __bsp_flash_write_exit()        __enable_irq()
#else
#define __bsp_flash_write_enter()
#define __bsp_flash_write_exit()
#endif

#ifndef FLASH_BLOCK_SIZE
#define FLASH_BLOCK_SIZE                (0x800)
#endif

/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/
/**
 * @brief For write and erase operations on the flash memory,
 *        the internal RC oscillator(HSI) must be on
 *
 * @retval true: HSI just be opened
 *         false: HSI has been opened before
 */
static bool _bsp_flash_clock_check(void)
{
    bool retval = false;

    if(!LL_RCC_HSI_IsReady()) {
        LL_RCC_HSI_Enable();
        while(!LL_RCC_HSI_IsReady());
        retval = true;
    }

    return retval;
}

/**
 * @brief restore flash clock
 * @param clock_close: true: close HSI
 *                     false: do nothing
 *
 * @retval None
 */
static void _bsp_flash_clock_restore(bool clock_close)
{
    if(clock_close) {
        LL_RCC_HSI_Disable();
    }
}

/**
 * @brief do nothing
 *
 * @retval true
 */
bool bsp_flash_init(void)
{
    return true;
}

/**
 * @brief write data to chipin flash
 * @param pageaddr: Specife the address to be programmed
 * @param data: Specife the data to be programmed
 * @param length: Specife the length to be programmed
 *
 * @retval true/false
 */
bool bsp_flash_write(uint32_t pageaddr, void *data, uint32_t length)
{
    uint32_t *pdata = (uint32_t *)data;
    HAL_StatusTypeDef flash_status = HAL_OK;
    bool retval = true, clock = false;

    /* aligned with word(4-Byte) */
    if(length % 4) {
        length += 4;
    }
    length >>= 2;
    clock = _bsp_flash_clock_check();
    __bsp_flash_write_enter();
    HAL_FLASH_Unlock();
    while((length > 0) && (flash_status == HAL_OK)) {
        flash_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, pageaddr, *pdata);
        pageaddr += 4;
        pdata++;
        length--;
    }
    HAL_FLASH_Lock();
    __bsp_flash_write_exit();
    _bsp_flash_clock_restore(clock);
    if(length) {
        retval = false;
    }

    return retval;
}

/**
 * @brief erase the specified FLASH memory pages
 * @param pageaddr: Initialize flash page address to erase when
 *                mass erase is disabled
 * @param erase_pages: Number of pages to be erase
 *
 * @retval true/false
 */
bool bsp_flash_erase(uint32_t pageaddr, uint32_t erase_pages)
{
    FLASH_EraseInitTypeDef flash_erase_init_structure;
    uint32_t page_err_addr = 0;
    bool retval = true, clock = false;

    flash_erase_init_structure.TypeErase = FLASH_TYPEERASE_PAGES;
    flash_erase_init_structure.PageAddress = pageaddr;
    flash_erase_init_structure.NbPages = erase_pages;
    clock = _bsp_flash_clock_check();
    __bsp_flash_write_enter();
    HAL_FLASH_Unlock();
    if(HAL_OK != HAL_FLASHEx_Erase(&flash_erase_init_structure, &page_err_addr)) {
        retval = false;
    }
    HAL_FLASH_Lock();
    __bsp_flash_write_exit();
    _bsp_flash_clock_restore(clock);

    return retval;
}

/**
 * @brief erase the specified flash memory pages
 * @param blocknum: Initialize flash page number to erase when
 *                mass erase is disable
 *
 * @retval true/false
 */
bool bsp_flash_erase_block(uint16_t blocknum)
{
    uint32_t pageaddr = SYS_FLASH_CHIPIN_BASE + FLASH_BLOCK_SIZE * blocknum;

    return bsp_flash_erase(pageaddr, 1);
}


