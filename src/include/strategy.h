/**
* @file strategy.h
*  
* @brief this is the system hook file ,used to initial the user 
*        task and user hardware
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/12/6 16:35:28
*/
#ifndef __STRATEGY_H__
#define __STRATEGY_H__

#ifdef __cpluscplus
extern "c" 
{
#endif /* __cpluscplus */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
//#include "stm32l0xx_ll_conf.h"

extern int32_t strategy_check_serial(void);
extern int32_t strategy_cmd_process(void);
extern int32_t strategy_check_spiflash(void);
extern int32_t strategy_copy_spiflash(int32_t size) ;


#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __STRATEGY_H__ */
