/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  NetBIOS structures and definitions.
*
****************************************************************************/



enum commands {
    NCBCALL         = 0x10,           /* NCB CALL                           */
    NCBLISTEN       = 0x11,           /* NCB LISTEN                         */
    NCBHANGUP       = 0x12,           /* NCB HANG UP                        */
    NCBSEND         = 0x14,           /* NCB SEND                           */
    NCBRECV         = 0x15,           /* NCB RECEIVE                        */
    NCBRECVANY      = 0x16,           /* NCB RECEIVE ANY                    */
    NCBCHAINSEND    = 0x17,           /* NCB CHAIN SEND                     */
    NCBDGSEND       = 0x20,           /* NCB SEND DATAGRAM                  */
    NCBDGRECV       = 0x21,           /* NCB RECEIVE DATAGRAM               */
    NCBDGSENDBC     = 0x22,           /* NCB SEND BROADCAST DATAGRAM        */
    NCBDGRECVBC     = 0x23,           /* NCB RECEIVE BROADCAST DATAGRAM     */
    NCBADDNAME      = 0x30,           /* NCB ADD NAME                       */
    NCBDELNAME      = 0x31,           /* NCB DELETE NAME                    */
    NCBRESET        = 0x32,           /* NCB RESET                          */
    NCBASTAT        = 0x33,           /* NCB ADAPTER STATUS                 */
    NCBSSTAT        = 0x34,           /* NCB SESSION STATUS                 */
    NCBCANCEL       = 0x35,           /* NCB CANCEL                         */
    NCBADDGRNAME    = 0x36,           /* NCB ADD GROUP NAME                 */
    NCBENUM         = 0x37,           /* NCB ENUMERATE LANA NUMBERS         */
    NCBUNLINK       = 0x70,           /* NCB UNLINK                         */
    NCBSENDNA       = 0x71,           /* NCB SEND NO ACK                    */
    NCBCHAINSENDNA  = 0x72,           /* NCB CHAIN SEND NO ACK              */
    NCBLANSTALERT   = 0x73,           /* NCB LAN STATUS ALERT               */
    NCBACTION       = 0x77,           /* NCB ACTION                         */
    NCBFINDNAME     = 0x78,           /* NCB FIND NAME                      */
    NCBTRACE        = 0x79,           /* NCB TRACE                          */

    ASYNCH          = 0x80,           /* high bit set == asynchronous       */
};

#define NET_INVALID_CMD     0x7f

enum nbrc {
    NRC_GOODRET     = 0x00,   /* good return                                */
                              /* also returned when ASYNCH request accepted */
    NRC_BUFLEN      = 0x01,   /* illegal buffer length                      */
    NRC_ILLCMD      = 0x03,   /* illegal command                            */
    NRC_CMDTMO      = 0x05,   /* command timed out                          */
    NRC_INCOMP      = 0x06,   /* message incomplete, issue another command  */
    NRC_BADDR       = 0x07,   /* illegal buffer address                     */
    NRC_SNUMOUT     = 0x08,   /* session number out of range                */
    NRC_NORES       = 0x09,   /* no resource available                      */
    NRC_SCLOSED     = 0x0a,   /* session closed                             */
    NRC_CMDCAN      = 0x0b,   /* command cancelled                          */
    NRC_DUPNAME     = 0x0d,   /* duplicate name                             */
    NRC_NAMTFUL     = 0x0e,   /* name table full                            */
    NRC_ACTSES      = 0x0f,   /* no deletions, name has active sessions     */
    NRC_LOCTFUL     = 0x11,   /* local session table full                   */
    NRC_REMTFUL     = 0x12,   /* remote session table full                  */
    NRC_ILLNN       = 0x13,   /* illegal name number                        */
    NRC_NOCALL      = 0x14,   /* no callname                                */
    NRC_NOWILD      = 0x15,   /* cannot put * in NCB_NAME                   */
    NRC_INUSE       = 0x16,   /* name in use on remote adapter              */
    NRC_NAMERR      = 0x17,   /* name deleted                               */
    NRC_SABORT      = 0x18,   /* session ended abnormally                   */
    NRC_NAMCONF     = 0x19,   /* name conflict detected                     */
    NRC_IFBUSY      = 0x21,   /* interface busy, IRET before retrying       */
    NRC_TOOMANY     = 0x22,   /* too many commands outstanding, retry later */
    NRC_BRIDGE      = 0x23,   /* ncb_lana_num field invalid                 */
    NRC_CANOCCR     = 0x24,   /* command completed while cancel occurring   */
    NRC_CANCEL      = 0x26,   /* command not valid to cancel                */
    NRC_DUPENV      = 0x30,   /* name defined by anther local process       */
    NRC_ENVNOTDEF   = 0x34,   /* environment undefined. RESET required      */
    NRC_OSRESNOTAV  = 0x35,   /* required OS resources exhausted            */
    NRC_MAXAPPS     = 0x36,   /* max number of applications exceeded        */
    NRC_NOSAPS      = 0x37,   /* no saps available for netbios              */
    NRC_NORESOURCES = 0x38,   /* requested resources are not available      */
    NRC_INVADDRESS  = 0x39,   /* invalid ncb address or length > segment    */
    NRC_INVDDID     = 0x3B,   /* invalid NCB DDID                           */
    NRC_LOCKFAIL    = 0x3C,   /* lock of user area failed                   */
    NRC_OPENERR     = 0x3f,   /* NETBIOS not loaded                         */
    NRC_SYSTEM      = 0x40,   /* system error                               */

    NRC_PENDING     = 0xff,   /* asynchronous command is not yet finished   */

};

#define ILLEGAL_NAME_NUM    0
#define ILLEGAL_LSN         0
#define NCBNAMSZ            16
#define MAX_LANA            254

typedef struct _NCB {
    unsigned char   ncb_command;            /* command code                   */
    unsigned char   ncb_retcode;            /* return code                    */
    unsigned char   ncb_lsn;                /* local session number           */
    unsigned char   ncb_num;                /* number of our network name     */
    unsigned char   far *ncb_buffer;        /* address of message buffer      */
    unsigned short  ncb_length;             /* size of message buffer         */
    unsigned char   ncb_callname[NCBNAMSZ]; /* blank-padded name of remote    */
    unsigned char   ncb_name[NCBNAMSZ];     /* our blank-padded netname       */
    unsigned char   ncb_rto;                /* rcv timeout/retry count        */
    unsigned char   ncb_sto;                /* send timeout/sys timeout       */
    void (far *ncb_post)( struct _NCB * );  /* POST routine address           */
    unsigned char   ncb_lana_num;           /* lana (adapter) number          */
    unsigned char   ncb_cmd_cplt;           /* 0xff => commmand pending       */
    unsigned char   ncb_reserve[14];        /* reserved, used by BIOS         */
} NCB, *PNCB;

#if 0
typedef struct {
    unsigned char           cmd;
    unsigned char           ret_code;
    unsigned char           lsn;
    unsigned char           name_num;
    void                    far *buff;
    unsigned short          length;
    char                    call_name[NCBNAMSZ];
    char                    name[NCBNAMSZ];
    unsigned char           r_timeout;
    unsigned char           s_timeout;
    void                    (far *post)();
    unsigned char           lana_num;
    volatile unsigned char  cmd_cmplt;
    char                    reserved[14];
} NCB;
#endif

typedef struct {
    unsigned char           length;
    unsigned char           lana[ MAX_LANA + 1 ];
} LANA_ENUM;
