/**
 * @file sl_crc.h
 *
 * @copyright This file creat by rensuiyi ,all right reserve!
 * 
 * @author rensuyi
 * 
 * @date 2013/12/30 9:59:58
 */
#ifndef __SL_CRC_H__
#define __SL_CRC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /*__cplusplus */

#include <stdint.h>
#include <stdbool.h>

extern uint8_t sl_revert_sum8(void *data,uint16_t len);
extern uint8_t sl_crc8(void *data,uint16_t len);
extern uint16_t sl_crc16(void *data, uint16_t len);  
extern uint8_t sl_sum8(void *data,uint16_t len);

#ifdef __cplusplus
}
#endif /*__cplusplus */
#endif /* __SL_CRC_H__ */
