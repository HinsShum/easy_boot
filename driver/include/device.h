/**
 * @file /driver/inc/device.h
 *
 * Copyright (C) 2020
 *
 * device.h is free software: you can redistribute it and/or modify
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
#ifndef __DEVICE_H
#define __DEVICE_H

/*---------- includes ----------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*---------- macro ----------*/
//! define the device type
//! @option tiny, no mutex, no name
//! @option min, have mutex, name
//! @option full, have device list and register and find functions
#define DEVICE_TYPE_TINY                (0x01)
#define DEVICE_TYPE_MIN                 (0x02)
#define DEVICE_TYPE_FULL                (0x03)

//! @option device type
#ifndef DEVICE_TYPE
#define DEVICE_TYPE                     DEVICE_TYPE_TINY
#endif

//! @option define of the command type
#define IOCTL_USER_START                (0X80000000)
#define IOCTL_DEVICE_POWER_ON           (0x00001000)
#define IOCTL_DEVICE_POWER_OFF          (0x00001001)

//! @option define the device attribute
#define DEVICE_ATTRIB_IDLE              (0x00)
#define DEVICE_ATTRIB_START             (0x01)
#define DEVICE_ATTRIB_POWER_OFF         (0x0 << 4U)
#define DEVICE_ATTRIB_POWER_ON          (0x1 << 4U)

//! @option define the method to operate the attribute
#define __device_attrib_ispower(attrib)         ((attrib & 0xF0) == DEVICE_ATTRIB_POWER_ON)
#define __device_attrib_isstart(attrib)         ((attrib & 0x0F) == DEVICE_ATTRIB_START)
#define __device_attrib_setpower(attrib, power) (attrib = (attrib & (~0xF0)) | power)
#define __device_attrib_setstart(attrib, start) (attrib = (attrib & (~0x0F)) | start)

/*---------- type define ----------*/
struct st_device {
#if DEVICE_TYPE == DEVICE_TYPE_FULL
    // add list struct here
#endif
#if DEVICE_TYPE > DEVICE_TYPE_TINY
    // name
    char name[8];
    // mutex lock
#endif
    uint16_t attrib;
    int32_t (*init)(struct st_device *device);
    int32_t (*read)(struct st_device *device, void *buf, uint32_t addition, uint32_t len);
    int32_t (*write)(struct st_device *device, void *buf, uint32_t addition, uint32_t len);
    int32_t (*ioctl)(struct st_device *device, uint32_t cmd, void *arg);
    int32_t (*irq_handler)(struct st_device *device, uint32_t irq_handler, void *arg);
    void *user;
};

/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
extern int32_t device_init(struct st_device *device);
extern int32_t device_read(struct st_device *device, void *buf, uint32_t addition, uint32_t len);
extern int32_t device_write(struct st_device *device, void *buf, uint32_t addition, uint32_t len);
extern int32_t device_ioctl(struct st_device *device, uint32_t cmd, void *arg);

#endif /* __DEVICE_H */
