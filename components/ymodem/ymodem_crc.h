/**
* @file ymodem_crc.h
*  
* @brief this is the system hook file ,used to initial the user 
*        task and user hardware
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/12/3 15:51:21
*/
#ifndef __YMODEM_CRC_H__
#define __YMODEM_CRC_H__

#ifdef __cpluscplus
extern "c" 
{
#endif /* __cpluscplus */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern uint16_t ymodem_crc_cal(uint8_t *pbuf,uint16_t len);


#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __YMODEM_CRC_H__ */
