/**
 * @file /driver/device.c
 *
 * Copyright (C) 2020
 *
 * device.c is free software: you can redistribute it and/or modify
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

/*---------- includes ----------*/
#include "device.h"
#include "config/errorno.h"

/*---------- macro ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- type define ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/
/**
 * @brief call the device initial function
 * @param device: device abstract variable
 *
 * @retval errorno
 */
int32_t device_init(struct st_device *device)
{
    int32_t retval = CY_E_POINT_NONE;

    if(NULL != device && NULL != device->init) {
        retval = device->init(device);
    }

    return retval;
}

/**
 * @brief call the device read functions
 * @param device: device abstract variable
 * @param buf: store the data read from the device
 * @param addition: additional variable
 * @param len: the buffer length
 *
 * @retval errorno
 */
int32_t device_read(struct st_device *device, void *buf,
                    uint32_t addition, uint32_t len)
{
    int32_t retval = CY_E_POINT_NONE;

    if(NULL != device && NULL != device->read) {
        retval = device->read(device, buf, addition, len);
    }

    return retval;
}

/**
 * @brief call the device write functions
 * @param device: device abstract variable
 * @param buf: store the data to be written to the device
 * @param addition: additional vaiables
 * @param len: the buffer length
 *
 * @retval errorno
 */
int32_t device_write(struct st_device *device, void *buf,
                     uint32_t addition, uint32_t len)
{
    int32_t retval = CY_E_POINT_NONE;

    if(NULL != device && NULL != device->write) {
        retval = device->write(device, buf, addition, len);
    }

    return retval;
}

/**
 * @brief call the device ioctl functions
 * @param device: device abstract variable
 * @param cmd: ioctl command type
 * @param arg: ioctl command argument
 *
 * @retval errorno
 */
int32_t device_ioctl(struct st_device *device, uint32_t cmd, void *arg)
{
    int32_t retval = CY_E_POINT_NONE;

    if(NULL != device && NULL != device->ioctl) {
        retval = device->ioctl(device, cmd, arg);
    }

    return retval;
}
