/**
* @file ymodem.h
*  
* @brief this is the system hook file ,used to initial the user 
*        task and user hardware
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/12/3 15:49:04
*/
#ifndef __YMODEM_H__
#define __YMODEM_H__

#ifdef __cpluscplus
extern "c" 
{
#endif /* __cpluscplus */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef YMODEM_SAVE_FILE_NAME
#define YMODEM_SAVE_FILE_NAME  (0)
#endif /* YMODEM_SAVE_FILE_NAME */

#ifndef YMODEM_REC_TIMEOUT
#define YMODEM_REC_TIMEOUT  (100)
#endif /* YMODEM_REC_TIMEOUT */

#ifndef YMODEM_ENABLE_CRC   
#define YMODEM_ENABLE_CRC   (0)
#endif /* YMODEM_ENABLE_CRC */

#ifndef YMODEM_ERROR_MAX
#define YMODEM_ERROR_MAX    (50)
#endif /* YMODEM_ERROR_MAX */

#ifndef YMODEM_FILENAME_LENGTH
#define YMODEM_FILENAME_LENGTH  (30)
#endif /* YMODEM_FILENAME_LENGTH */

struct st_ymodem_describe{
    uint32_t filesize;
    #if YMODEM_SAVE_FILE_NAME > 0
    uint32_t filename[YMODEM_FILENAME_LENGTH+2];
    #endif
    uint32_t frame_count;
    uint32_t err_count;
    uint8_t *pbuf;
    uint8_t  err_out;

    bool  (*get_char)(uint8_t *out_char,uint16_t timeout);
    void  (*put_char)(uint8_t pchar);
    bool  (*save_frame)(uint32_t offset,uint8_t *pframe,uint32_t framesize);
    bool  (*file_info_cb)(char *pfilename,uint32_t filesize);      
};


#define YMODEM_ERR_OK                 (00)
#define YMODEM_ERR_TIMEOUT            (-1) 
#define YMODEM_ERR_WRONG_CRC          (-2)
#define YMODEM_ERR_SAVE               (-3)
#define YMODEM_ERR_FILE_INFO          (-4)
#define YMODEM_ERR_UNKOWN             (-5)

extern int32_t ymodem_receive_file(struct st_ymodem_describe *pymodem);

#ifdef __cpluscplus
}
#endif /* __cpluscplus */
#endif /* __YMODEM_H__ */
