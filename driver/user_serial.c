/**
* @user_serial.c
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/12/7 11:08:29
*/

/* include */
#include "bsp.h"
#include "bsp_uart.h"
#include "user_serial.h"
#include "platform.h"
#include "simplefifo.h"

#define __delay_ms(ms)      bsp_mdelay(ms)

/* define struct */
typedef struct {
    uint16_t (*serial_write)(uint8_t *, uint16_t);
} user_serial_t;

/* private variable */
static user_serial_t m_user_serial;

/* global variable */
extern struct st_platform_descibe g_platform;

uint16_t user_serial_write(char *pdata, uint16_t len) {
    return m_user_serial.serial_write((uint8_t *)pdata, len);
}


bool user_serial_get_char(char *pout, uint16_t timeout_ms) {
    int16_t res;
    uint16_t timeout = 0;

    timeout = timeout_ms;

    do {
        res = simplefifo_read(&g_platform.uart_fifo, (uint8_t *)pout, 1);
        if (res == 1) {
            return true;
        } else {
            __delay_ms(1);
        }
    }while (--timeout);

    return false;
}

bool user_serial_drop(void) {
    simplefifo_reset(&g_platform.uart_fifo);
    return true;
}

bool user_serial_isr_service(uint8_t ch)
{
    simplefifo_write(&g_platform.uart_fifo, &ch, 1);
    
    return true;
}

bool user_serial_register(user_serial_write_t write)
{
    bool retval = (write == NULL) ? false : true;

    m_user_serial.serial_write = write;

    return retval;
}

void user_serial_unregister(void)
{
    m_user_serial.serial_write = NULL;
}

