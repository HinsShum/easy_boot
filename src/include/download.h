/**
* @file download.h
*  
* @brief this is the system hook file ,used to initial the user 
*        task and user hardware
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/12/7 18:51:34
*/
#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#ifdef __cpluscplus
extern "c" 
{
#endif /* __cpluscplus */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
    
extern int32_t download_file(void);

#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __DOWNLOAD_H__ */
