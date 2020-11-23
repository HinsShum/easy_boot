/**
* @w25qxx.c
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2015/4/30 9:45:50
*/
#include "w25qxx.h"
#include "config/errorno.h"

#ifndef  W25QXX_DEBUG_ENABLE
#define  W25QXX_DEBUG_ENABLE   (0)
#endif /* W25QXX_DEBUG_ENABLE */

#ifndef W25QXX_LOCK_ENABLE 
#define  W25QXX_LOCK_ENABLE    (0)
#endif

#if W25QXX_DEBUG_ENABLE > 0
#define __w25qxx_debug(x,y...)
#else
#define __w25qxx_debug(x,y...)
#endif

#if W25QXX_LOCK_ENABLE > 0
#define __w25qxx_lock(x)        xSemaphoreTake(x->lock, portMAX_DELAY)
#define __w25qxx_unlock(x)      xSemaphoreGive(x->lock)
#else
#define __w25qxx_lock(x)
#define __w25qxx_unlock(x)
#endif

#define W25QXX_STATUS_REG_BUSY      (1<<0)
#define W25QXX_STATUS_REG_WEL       (1<<1)

static void w25qxx_delay_ms(uint16_t cnt) {
    extern void bsp_mdelay(uint32_t delay);
    bsp_mdelay(cnt);
};

/*
 * Get the Device ID,WinBond EF (0x13,0x14,0x15,0x16(8Mbytes)) 
 * 
 * @author suiyi.key (2015/5/1)
 * 
 * @param pw25qxx 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_get_id(struct st_w25qxx_describe *pw25qxx) {
    uint8_t tmp = 0x00;

    pw25qxx->cs_ctl(true);

    pw25qxx->xfer(0x90);
    pw25qxx->xfer(0x00);
    pw25qxx->xfer(0x00);
    pw25qxx->xfer(0x00);
    tmp = pw25qxx->xfer(0x00);
    pw25qxx->icode = ((uint16_t)tmp) << 8;
    tmp = pw25qxx->xfer(0x00);
    pw25qxx->icode = pw25qxx->icode + tmp;

    pw25qxx->cs_ctl(false);

    return CY_EOK;
}

/*
 * Get the Status Register 1 
 * 
 * @author suiyi.key (2015/5/1)
 * 
 * @param pw25qxx 
 * @param status 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_get_status(struct st_w25qxx_describe *pw25qxx, uint8_t *status) {
    pw25qxx->cs_ctl(true);

    pw25qxx->xfer(0x05);
    *status = pw25qxx->xfer(0x00);

    pw25qxx->cs_ctl(false);
    return CY_EOK;
}

/*
 * Enable Write,must be call before Write/Erase 
 * 
 * @author suiyi.key (2015/5/1)
 * 
 * @param pw25qxx 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_write_enable(struct st_w25qxx_describe *pw25qxx) {
    uint16_t wait_max_ticks = 100;
    uint8_t status = 0xff;

    pw25qxx->cs_ctl(true);
    pw25qxx->xfer(0x06);
    pw25qxx->cs_ctl(false);

    while (--wait_max_ticks > 0) {
        w25qxx_get_status(pw25qxx, &status);
        if (status & W25QXX_STATUS_REG_WEL) {
            break;
        }
        //delay
        {
            uint16_t tick = 1000;
            while (--tick > 0) {}
        }
    }
    if (wait_max_ticks > 0) {
        return CY_EOK;
    }
    __w25qxx_debug("write enable time out\r\n");
    return CY_E_TIME_OUT;

}

/*
 * erase the seckor  4K 
 * 
 * @author suiyi.key (2015/5/1)
 * 
 * @param pw25qxx 
 * @param addr 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_erase_sector(struct st_w25qxx_describe *pw25qxx, uint32_t addr) {
    uint16_t wait_max_ms = 400;
    uint8_t status = 0xff;

    /*
     * write enable 
     */
    if (CY_EOK != w25qxx_write_enable(pw25qxx)) {
        return CY_ERROR;
    }

    pw25qxx->cs_ctl(true);
    pw25qxx->xfer(0x20);
    pw25qxx->xfer((addr >> 16) & 0xff);
    pw25qxx->xfer((addr >> 8) & 0xff);
    pw25qxx->xfer((addr >> 0) & 0xff);
    pw25qxx->cs_ctl(false);
    w25qxx_delay_ms(1);
    /*
     * wait for ready 
     */
    while (--wait_max_ms > 1) {

        w25qxx_get_status(pw25qxx, &status);
        if ((status & W25QXX_STATUS_REG_BUSY) != 0) {
            break;
        }
        w25qxx_delay_ms(1);
    }
    while (--wait_max_ms) {
        w25qxx_delay_ms(1);
        w25qxx_get_status(pw25qxx, &status);
        if ((status & W25QXX_STATUS_REG_BUSY) == 0) {
            break;
        }

    }
    if (wait_max_ms > 0) {
        while (wait_max_ms-- > 340) {
            w25qxx_delay_ms(1);
        }
        return CY_EOK;
    }
    __w25qxx_debug("erase sector time out\r\n");
    return CY_E_TIME_OUT;
}

/*
 * erase the chip with in about 23s 
 * 
 * @author suiyi.key (2015/5/1)
 * 
 * @param pw25qxx 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_erase_chip(struct st_w25qxx_describe *pw25qxx) {
    uint8_t status;
    uint16_t wait_max_ms = 40000;

    /*
     * write enable 
     */
    if (CY_EOK != w25qxx_write_enable(pw25qxx)) {
        return CY_ERROR;
    }

    pw25qxx->cs_ctl(true);
    pw25qxx->xfer(0xc7);
    pw25qxx->cs_ctl(false);
    w25qxx_delay_ms(1);
    /*
     * wait for ready 
     */
    while (--wait_max_ms > 1) {

        w25qxx_get_status(pw25qxx, &status);
        if ((status & W25QXX_STATUS_REG_BUSY) != 0) {
            break;
        }
        w25qxx_delay_ms(1);
    }
    while (--wait_max_ms) {
        w25qxx_delay_ms(1);
        w25qxx_get_status(pw25qxx, &status);
        if ((status & W25QXX_STATUS_REG_BUSY) == 0) {
            break;
        }
    }

    if (wait_max_ms > 0) {
        //wait for ready
        w25qxx_delay_ms(10);
        return CY_EOK;
    }
    __w25qxx_debug("erase chip time out\r\n");
    return CY_E_TIME_OUT;
}

/*
 * Write data to a single page 
 * 
 * @author suiyi.key (2015/5/1)
 * 
 * @param pw25qxx 
 * @param addr 
 * @param buf 
 * @param len 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_page_write(struct st_w25qxx_describe *pw25qxx, uint32_t addr, uint8_t  *buf, uint16_t len) {
    //page algin
    uint16_t page_left = 0x00;
    uint16_t max_len = len;
    uint16_t write_len = 0;
    uint16_t wait_max_ms = 50;
    uint8_t status;
    int32_t res;
    if (addr > pw25qxx->addr_max) {
        __w25qxx_debug("addr:%08X not support\r\n", addr);
        return CY_ERROR;
    }
    page_left = pw25qxx->page_size - (addr % pw25qxx->page_size);
    if (page_left < max_len) {
        max_len = page_left;
    }

    write_len = max_len;

    wait_max_ms = 50;
    __w25qxx_lock(pw25qxx);
    while (wait_max_ms-- > 1) {
        res = w25qxx_write_enable(pw25qxx);
        if (res == CY_EOK) {
            break;
        }
        w25qxx_delay_ms(1);
    }
    if (CY_EOK != res) {
        __w25qxx_unlock(pw25qxx);
        return CY_ERROR;
    }
    //start write

    pw25qxx->cs_ctl(true);
    pw25qxx->xfer(0x02);
    pw25qxx->xfer((addr >> 16) & 0xff);
    pw25qxx->xfer((addr >> 8) & 0xff);
    pw25qxx->xfer((addr >> 0) & 0xff);

    while (max_len-- > 0) {
        pw25qxx->xfer(*buf++);
    }
    pw25qxx->cs_ctl(false);

    wait_max_ms = 50;
    //wait for ready
    while (--wait_max_ms) {
        w25qxx_delay_ms(1);
        w25qxx_get_status(pw25qxx, &status);
        if ((status & W25QXX_STATUS_REG_BUSY) == 0) {
            break;
        }

    }
    __w25qxx_unlock(pw25qxx);
    if (wait_max_ms > 0) {
        return write_len;
    }
    __w25qxx_debug("write time out\r\n");
    return CY_E_TIME_OUT;
}

/*
 * Get the Chip infomations 
 * 
 * @author suiyi.key (2015/4/30)
 * 
 * @param pdev 
 * 
 * @return int32_t 
 */
static int32_t w25qxx_init(struct st_device *pdev) {

    struct st_w25qxx_describe *pw25qxx = (struct st_w25qxx_describe *)pdev->user;

    if (pw25qxx->xfer == NULL) {
        __w25qxx_debug("w25qxx driver get NULL parameters\r\n");
        return CY_E_POINT_NONE;
    }

    if (__device_attrib_isstart(pdev->attrib)) {
        return CY_EOK;
    }
    //get the id
    w25qxx_get_id(pw25qxx);
    //check if is w25qxx
    if ((pw25qxx->icode >> 8) != 0xef && (pw25qxx->icode >> 8) != 0xc8) {
        __w25qxx_debug("flash type not pw25qxx:%04X\r\n", pw25qxx->icode);
        return CY_ERROR;
    }
    //check the sub type
    if ((pw25qxx->icode & 0xff) == W25Q80_IDENTIFIED_CODE) {
        pw25qxx->page_size = 256;      //256byte
        pw25qxx->sector_size = 4 * 1024; //4K
        pw25qxx->sectors = 256;
        pw25qxx->pages = 4096;
        pw25qxx->addr_max = 1 * 1024 * 1024ul; //1M
    }
    if ((pw25qxx->icode & 0xff) == W25Q16_IDENTIFIED_CODE) {}
    if ((pw25qxx->icode & 0xff) == W25Q64_IDENTIFIED_CODE) {
        pw25qxx->page_size = 256;      //256byte
        pw25qxx->sector_size = 4 * 1024; //4K
        pw25qxx->sectors = 2048;
        pw25qxx->pages = 32768;
        pw25qxx->addr_max = 8 * 1024 * 1024ul; //8M
    }

    if (pw25qxx->page_size == 0) {
        __w25qxx_debug("flash size not support!\r\n");
        return CY_ERROR;
    }
#if W25QXX_LOCK_ENABLE > 0
    pw25qxx->lock = xSemaphoreCreateMutex();
    if (pw25qxx->lock == NULL) {
        return SL_ERROR;
    }
#endif /* W25QXX_LOCK_ENABLE > 0 */
    __device_attrib_setstart(pdev->attrib, DEVICE_ATTRIB_START);
    __w25qxx_debug("already initialized!"); 
    return CY_EOK;
}

static int32_t w25qxx_read(struct st_device *pdev, void *buf, uint32_t addition, uint32_t len) {
    uint32_t  max_len = len;
    uint8_t *pdata = (uint8_t *)buf;
    struct st_w25qxx_describe *pw25qxx = (struct st_w25qxx_describe *)pdev->user;


    if ((buf == NULL) || (pw25qxx->xfer == NULL)) {
        __w25qxx_debug("w25qxx driver get NULL parameters\r\n");
        return CY_E_POINT_NONE;
    }

    //read
    if (addition > pw25qxx->addr_max) {
        return 0;
    }
    max_len = pw25qxx->addr_max - addition;
    if (max_len > len) {
        max_len = len;
    }
    len = max_len;
    __w25qxx_lock(pw25qxx);
    pw25qxx->cs_ctl(true);
    pw25qxx->xfer(0x03);
    pw25qxx->xfer((addition >> 16) & 0xff);
    pw25qxx->xfer((addition >> 8) & 0xff);
    pw25qxx->xfer((addition >> 0) & 0xff);
    while (max_len-- > 0) {
        *pdata++ = pw25qxx->xfer(0x00);
    }
    pw25qxx->cs_ctl(false);
    __w25qxx_unlock(pw25qxx);
    return len;
}

static int32_t w25qxx_write(struct st_device *pdev, void *buf, uint32_t addition, uint32_t len) {
    uint16_t  left_len = 0;
    uint32_t  max_len = 0;
    uint32_t  write_len = 0;
    int32_t  res = 0;
    uint8_t *pdata = (uint8_t *)buf;
    struct st_w25qxx_describe *pw25qxx = (struct st_w25qxx_describe *)pdev->user;

    if ((buf == NULL) || (pw25qxx->xfer == NULL)) {
        __w25qxx_debug("w25qxx driver get NULL parameters\r\n");
        return CY_E_POINT_NONE;
    }
    if (addition > pw25qxx->addr_max) {
        return 0;
    }
    max_len = pw25qxx->addr_max - addition;
    if (max_len > len) {
        max_len = len;
    }

    //write
    left_len = pw25qxx->page_size - addition % pw25qxx->page_size;
    if (left_len > max_len) {
        left_len = max_len;
    }
    if (left_len > 0) {
        res = w25qxx_page_write(pw25qxx, addition, pdata, left_len);
        if (res < 0) {
            return res;
        }
        write_len += res;
        addition += left_len;
        max_len -= left_len;
    }

    while (max_len > 0) {
        if (max_len > pw25qxx->page_size) {
            res = w25qxx_page_write(pw25qxx, addition, pdata + write_len, pw25qxx->page_size);
            max_len -= pw25qxx->page_size;
            addition += pw25qxx->page_size;
        } else {
            res = w25qxx_page_write(pw25qxx, addition, pdata + write_len, max_len);
            max_len = 0x00;
        }
        if (res < 0) {
            return res;
        }
        write_len += res;
    }
    return write_len;
}

static int32_t w25qxx_ioctl(struct st_device *pdev, uint32_t cmd, void *arg) {
    int32_t result;
    struct st_w25qxx_describe *pw25qxx = (struct st_w25qxx_describe *)pdev->user;

    if (pw25qxx->xfer == NULL) {
        __w25qxx_debug("w25qxx driver get NULL parameters\r\n");
        return CY_E_POINT_NONE;
    }

    switch (cmd) {
    case IOCTL_DEVICE_POWER_OFF:
        if (pw25qxx->power_ctl) {
            w25qxx_delay_ms(10);
            pw25qxx->power_ctl(false);
        }
        return CY_EOK;
    case IOCTL_DEVICE_POWER_ON:
        if (pw25qxx->power_ctl) {
            pw25qxx->power_ctl(true);
            w25qxx_delay_ms(10);
        }
        return CY_EOK;
    case W25QXX_IOCTL_ERAES_CHIP:
        __w25qxx_lock(pw25qxx);
        result = w25qxx_erase_chip(pw25qxx);
        __w25qxx_unlock(pw25qxx);
        return result;
    case W25QXX_IOCTL_ERAES_SEGMENT:
        {
            uint32_t *addr;
            if (arg == NULL) {
                return CY_E_POINT_NONE;
            }
            addr = (uint32_t *)arg;
            __w25qxx_lock(pw25qxx);
            result = w25qxx_erase_sector(pw25qxx, *addr);
            __w25qxx_unlock(pw25qxx);
            return result;
        }
    case W25QXX_IOCTL_GET_ID:
        {
            uint16_t *pid;
            if (arg == NULL) {
                return CY_E_POINT_NONE;
            }
            pid = (uint16_t *)arg;
            *pid = pw25qxx->icode;
            return CY_EOK;
        }
    default:
        break;
    }
    return CY_EOK;
}

int32_t w25qxx_register(struct st_device *pdev, struct st_w25qxx_describe *pw25qxx){
    
    pdev->user = pw25qxx;

    pdev->init = w25qxx_init;
    pdev->read = w25qxx_read;
    pdev->write = w25qxx_write;
    pdev->ioctl = w25qxx_ioctl;

    return CY_EOK;
}
