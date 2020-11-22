#ifndef __SIMPLEFIFO_H__
#define __SIMPLEFIFO_H__

#ifdef __cpluscplus
extern "C"
{
#endif

/* include */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* define struct */
struct st_simplefifo_describe {
    uint16_t size;
    uint16_t maxsize;
    uint16_t head;
    uint16_t tail;
    uint8_t *pdata;
};

/* prototype */
extern int16_t simplefifo_init(struct st_simplefifo_describe *pfifo, uint8_t *buf, uint16_t size);
extern int16_t simplefifo_reset(struct st_simplefifo_describe *pfifo);
extern int16_t simplefifo_getsize(struct st_simplefifo_describe *pfifo);
extern int16_t simplefifo_write(struct st_simplefifo_describe *pfifo, uint8_t *buf, uint16_t length);
extern int16_t simplefifo_read(struct st_simplefifo_describe *pfifo, uint8_t *buf, uint16_t length);

#ifdef __cpluscplus
}
#endif
#endif /* __SIMPLEFIFO_H__ */
