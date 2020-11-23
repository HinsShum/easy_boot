/**
* @file w25qxx.h
*  
* @brief this is the system hook file ,used to initial the user 
*        task and user hardware
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2015/4/20 10:42:55
*/
#ifndef __W25QXX_H__
#define __W25QXX_H__

#ifdef __cpluscplus
extern "c"
{
#endif /* __cpluscplus */

#include "device.h"

#define  W25Q80_IDENTIFIED_CODE          (0x13)
#define  W25Q16_IDENTIFIED_CODE          (0x14)
#define  W25Q64_IDENTIFIED_CODE          (0x16)


#define  W25QXX_IOCTL_ERAES_SEGMENT       (IOCTL_USER_START + 0x01)
#define  W25QXX_IOCTL_ERAES_CHIP          (IOCTL_USER_START + 0x02)
#define  W25QXX_IOCTL_GET_ID              (IOCTL_USER_START + 0x03)


/**
 * LOCK SUPPORT 
 */
#define  W25QXX_LOCK_ENABLE   (0)

struct st_w25qxx_describe {
    uint8_t(*xfer)(uint8_t data);
    bool(*power_ctl)(bool on);
    bool(*cs_ctl)(bool on);
    uint16_t page_size;
    uint32_t pages;
    uint16_t sector_size;
    uint32_t sectors;
    uint32_t addr_max; 
    uint16_t icode;

#if W25QXX_LOCK_ENABLE  > 0
    SemaphoreHandle_t lock;
#endif /* W25QXX_LOCK_ENABLE */
};

extern int32_t w25qxx_register(struct st_device *pdev, struct st_w25qxx_describe *pw25qxx);

#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __W25QXX_H__ */
