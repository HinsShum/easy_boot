/* include */
#include "simplefifo.h"

/* function */
int16_t simplefifo_init(struct st_simplefifo_describe *pfifo, uint8_t *buf, uint16_t size) {
    pfifo->head = 0;
    pfifo->tail = 0;
    pfifo->size = 0;
    pfifo->maxsize = size;
    pfifo->pdata = buf;
    return 0;
}
int16_t simplefifo_getsize(struct st_simplefifo_describe *pfifo) {
    return pfifo->size;
}

int16_t simplefifo_reset(struct st_simplefifo_describe *pfifo){
    pfifo->head = 0;
    pfifo->tail = 0;
    pfifo->size = 0;
    return 0;
}

int16_t simplefifo_write(struct st_simplefifo_describe *pfifo, uint8_t *buf, uint16_t length) {
    uint16_t len = 0;
    uint16_t full_tail;

    full_tail = (pfifo->tail + pfifo->maxsize - 1) % pfifo->maxsize;

    //have a space 0-head 1-tail ,full
    while (pfifo->head != full_tail) {
        //copy the data
        if (len >= length) {
            break;
        }
        pfifo->pdata[pfifo->head] = buf[len];
        pfifo->head = (pfifo->head + 1) % pfifo->maxsize;
        len++;
    }
    return len;
}

int16_t simplefifo_read(struct st_simplefifo_describe *pfifo, uint8_t *buf, uint16_t length) {
    uint16_t len = 0;

    while (pfifo->head != pfifo->tail) {
        if (len >= length) {
            break;
        }
        buf[len] = pfifo->pdata[pfifo->tail];
        pfifo->tail = (pfifo->tail + 1) % pfifo->maxsize;
        len++;
    }
    return len;
}

int16_t simplefifo_get_available(struct st_simplefifo_describe *pfifo) {
    uint16_t full_tail;
    uint16_t available;

    full_tail = (pfifo->tail + pfifo->maxsize - 1) % pfifo->maxsize;
    available =  (full_tail + pfifo->maxsize - pfifo->head) % pfifo->maxsize;

    return available;
}


