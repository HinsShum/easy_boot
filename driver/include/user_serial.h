/**
* @file user_serial.h
*  
* @brief this is the system hook file ,used to initial the user 
*        task and user hardware
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/7/27 13:36:31
*/
#ifndef __USER_SERIAL_H__
#define __USER_SERIAL_H__

#ifdef __cpluscplus
extern "c" 
{
#endif /* __cpluscplus */

/* include */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* define struct */
typedef uint16_t (*user_serial_write_t)(uint8_t *, uint16_t);

/* prototype */
extern uint16_t user_serial_write(char *pdata, uint16_t len);
extern bool user_serial_get_char(char *pout, uint16_t timeout_ms);
extern bool user_serial_drop(void);
extern bool user_serial_register(user_serial_write_t write);
extern void user_serial_unregister(void);

#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __USER_SERIAL_H__ */
