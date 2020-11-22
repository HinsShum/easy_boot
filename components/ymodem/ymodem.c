/**
* @ymodem.c
*
* @copyright This file creat by rensuiyi ,all right reserve!
* 
* @author rensuyi
* 
* @date 2016/12/3 15:49:15
*/
#include <string.h>
#include "ymodem.h"
#include "ymodem_crc.h"
#define YMODEM_PACKET_SEQNO_INDEX      (1)
#define YMODEM_PACKET_SEQNO_COMP_INDEX (2)

#define YMODEM_PACKET_HEADER           (3)     /* start, block, block-complement */
#define YMODEM_PACKET_TRAILER          (2)     /* CRC bytes */
#define YMODEM_PACKET_OVERHEAD         (YMODEM_PACKET_HEADER + YMODEM_PACKET_TRAILER)
#define YMODEM_PACKET_SIZE             (128)
#define YMODEM_PACKET_1K_SIZE          (1024)


/* ASCII control codes: */
#define YMODEM_SOH (0x01)      /* start of 128-byte data packet */
#define YMODEM_STX (0x02)      /* start of 1024-byte data packet */
#define YMODEM_EOT (0x04)      /* end of transmission */
#define YMODEM_ACK (0x06)      /* receive OK */
#define YMODEM_NAK (0x15)      /* receiver error; retry */
#define YMODEM_CAN (0x18)      /* two of these in succession aborts transfer */
#define YMODEM_CRC (0x43)      /* use in place of first NAK for CRC mode */

/**
 * initial the ymodem struct 
 * 
 * @author rensuiyi (2016/12/5)
 * 
 * @param pymodem 
 */
static void ymodem_reset_values(struct st_ymodem_describe *pymodem) {
    pymodem->err_count = 0;
    pymodem->err_out = 0;
    pymodem->frame_count = 0;
}


/**
 * 
 * 
 * @author rensuiyi (2016/12/5)
 * 
 * @param str 
 * 
 * @return uint32_t 
 */
static uint32_t ymodem_str_to_32(char *str) {
    const char *s = str;
    uint32_t acc;
    uint8_t c;

    /* strip leading spaces if any */
    do {
        c = *s++;
    } while (c == ' ');

    for (acc = 0; (c >= '0') && (c <= '9'); c = *s++) {
        c -= '0';
        acc *= 10;
        acc += c;
    }
    return acc;
}

/**
 * 
 * 
 * @author rensuiyi (2016/12/5)
 * 
 * @return int32_t  0-ok,-1 timeout,-2 corrupt package,-3 
 *         cancel,-4 normal over  end
 *  
 */
static int32_t ymodem_receive_packet(struct st_ymodem_describe *pymodem, char *pout, int32_t *pout_len) {
    uint8_t ch;
    uint16_t i = 0;
    uint16_t package_size = 0;

    *pout_len = 0;
    if (!pymodem->get_char(&ch, YMODEM_REC_TIMEOUT)) {
        return -1;
    }

    switch (ch) {
    case YMODEM_SOH:
        package_size = YMODEM_PACKET_SIZE;
        break;
    case YMODEM_STX:
        package_size = YMODEM_PACKET_1K_SIZE;
        break;
    case YMODEM_EOT:
        return -4;
    case YMODEM_CAN:
        if (!pymodem->get_char(&ch, YMODEM_REC_TIMEOUT)) {
            return -1;
        }
        if (ch == YMODEM_CAN) {
            return -3;
        }
    default:
        return -3;
    }

    /* next should be data */
    *pout = (char)ch;

    for (i = 1; i < (package_size + YMODEM_PACKET_OVERHEAD); i++) {
        if (!pymodem->get_char(&ch, YMODEM_REC_TIMEOUT)) {
            return -1;
        }
        pout[i] = (char)ch;
    }

    /*check the sequence number */
    if (pout[YMODEM_PACKET_SEQNO_INDEX] != ((pout[YMODEM_PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
        return -2;
    }
#if YMODEM_ENABLE_CRC
    
#endif /* YMODEM_ENABLE_CRC */

    *pout_len = package_size;
    return 0;
}

static void ymodem_force_cancel(struct st_ymodem_describe *pymodem){
     pymodem->put_char(YMODEM_CAN);
     pymodem->put_char(YMODEM_CAN);
     pymodem->put_char(YMODEM_CAN);
     pymodem->put_char(YMODEM_CAN);
     pymodem->put_char(YMODEM_CAN);
     pymodem->put_char(YMODEM_CAN);
}
/**
 * 
 * 
 * @author rensuiyi (2016/12/5)
 * 
 * @param pymodem 
 * 
 * @return int32_t  >0 filesize ,0,cancel, -1 error
 */
int32_t ymodem_receive_file(struct st_ymodem_describe *pymodem) {
    char *ppackage_data;
    int32_t  packet_length, crc_nak;
    uint32_t packets_received, errors;
    char  *pfile_name, *pfile_size = NULL;
    int32_t receive_len;
    uint32_t size = 0;
    uint32_t fullsize = 0;
    uint8_t idle;

    ppackage_data = (char*)pymodem->pbuf;
    
    ymodem_reset_values(pymodem);

    if (!pymodem->get_char) {
        return YMODEM_ERR_UNKOWN;
    }
    errors = 0;
    /*do recieve */
    crc_nak = 1;
    pymodem->put_char(YMODEM_CRC);

    packets_received = 0;
    receive_len = 0;

    while (true) {
        switch (ymodem_receive_packet(pymodem, (char*)pymodem->pbuf, &packet_length)) {
        case 0:
            errors = 0;
            /*check the sequence */
            if (((ppackage_data[YMODEM_PACKET_SEQNO_INDEX] & 0xff) < (packets_received & 0xff)) ||
                (((ppackage_data[YMODEM_PACKET_SEQNO_INDEX] & 0xff) == 255) && ((packets_received & 0xff) == 0))) {
                pymodem->put_char(YMODEM_ACK);
                break;
            }
            if ((ppackage_data[YMODEM_PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) {
                pymodem->put_char(YMODEM_NAK);
            } else {
                if (packets_received == 0) {
                    /* prase the file name  */
                    if (ppackage_data[YMODEM_PACKET_HEADER] != 0) {
                        /*have the file name */
                        uint16_t i = 0;
                        pfile_name = ppackage_data + YMODEM_PACKET_HEADER;
                        for (i = YMODEM_PACKET_HEADER; i < 256; i++) {
                            if (ppackage_data[i] == 0) {
                                pfile_size = ppackage_data + i + 1;
                            }
                            if (ppackage_data[i] == ' ') {
                                if (pfile_size) {
                                    ppackage_data[i] = 0x00;
                                    break;
                                }
                            }
                        }
#if YMODEM_SAVE_FILE_NAME
                        strncpy(pymodem->filename, pfile_name, YMODEM_FILENAME_LENGTH);
#endif /* YMODEM_SAVE_FILE_NAME */
                        size = ymodem_str_to_32(pfile_size);
                        pymodem->filesize = size;
                        if(size & (1024-1)){
                            fullsize = -(size&(1024-1)) + 1024 + size;
                        }else{
                            fullsize = size;
                        }
                         
                        if (pymodem->file_info_cb) {
                            if (true != pymodem->file_info_cb(pfile_name, size)) {
                                //shut
                                ymodem_force_cancel(pymodem);
                                return YMODEM_ERR_FILE_INFO;
                            }
                        }
                        //pass
                        pymodem->put_char(YMODEM_ACK);
                        pymodem->put_char(crc_nak ? YMODEM_CRC : YMODEM_NAK);
                        crc_nak = 0;
                    } else {
                        /*have no name */
                        pymodem->put_char(YMODEM_ACK);
                        return 0;
                    }
                    ++packets_received;
                } else {
                    /*copy to the data */
                    /*last package will fillwith 1K or 128 byte*/
                    if (receive_len + packet_length > fullsize) {
                        ymodem_force_cancel(pymodem);
                        return 0;
                    }

                    /* save the frame */
                    if (pymodem->save_frame) {
                        uint32_t reallen = packet_length;
                        if(receive_len + packet_length > size){
                            reallen = size &(packet_length-1);
                        }
                        if (true != pymodem->save_frame(receive_len, (uint8_t*)ppackage_data + YMODEM_PACKET_HEADER,
                                                         reallen)) {
                            ymodem_force_cancel(pymodem);
                            return 0;
                        }
                    }
                    receive_len += packet_length;
                    pymodem->put_char(YMODEM_ACK);
                    ++packets_received;
                }
            }
            break;
        case -1:
            /* timeout */
            if (packets_received != 0) {
                /*error out */
                if (++errors >= YMODEM_ERROR_MAX) {
                    ymodem_force_cancel(pymodem);
                    return 0;
                }
            }
            pymodem->put_char(YMODEM_CRC);
            break;
        case -2:
            /* wrong package */
            return YMODEM_ERR_UNKOWN;
        case -3:
            /* cancel */
            return YMODEM_ERR_UNKOWN;
        case -4:
            /* normal out */
            pymodem->put_char(YMODEM_ACK);
            pymodem->put_char(YMODEM_CRC);
            pymodem->put_char(YMODEM_ACK);
            /*delay for the split of the system */
            pymodem->get_char(&idle,YMODEM_REC_TIMEOUT);
            return YMODEM_ERR_OK;
        }
    }
}
