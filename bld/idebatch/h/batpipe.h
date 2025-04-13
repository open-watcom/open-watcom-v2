/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "batcher.h"


#define DEFAULT_LINK_NAME   "BatLink"
#define COMSPEC_MAXLEN      80

#define TRANS_MAXLEN        1024
#ifdef __NT__
#define TRANS_BDATA_MAXLEN  (TRANS_MAXLEN - sizeof( batch_len ))
#else
#define TRANS_BDATA_MAXLEN  TRANS_MAXLEN
#endif
#define TRANS_DATA_MAXLEN   (TRANS_BDATA_MAXLEN - 1)

#ifdef __NT__

#define PREFIX              ""
#define PREFIX_LEN          0
#define NAME_MAXLEN         80

#else

#define PREFIX              "\\PIPE\\"
#define PREFIX_LEN          6
#define NAME_MAXLEN         12

#endif

enum {
    LNK_NOP,
    LNK_CWD,
    LNK_RUN,
    LNK_QUERY,
    LNK_CANCEL,
    LNK_DONE,
    LNK_SHUTDOWN,
    LNK_OUTPUT,
    LNK_STATUS,
    LNK_ABORT
};

#include "pushpck1.h"
typedef struct batch_data {
    union {
        char            buffer[TRANS_BDATA_MAXLEN];
        struct {
            char        cmd;
            union {
                /*
                 * add one byte for additional null terminate character
                 */
                char            data[TRANS_DATA_MAXLEN + 1];
                batch_len       len;
                batch_stat      status;
            } u;
        } s;
    } u;
} batch_data;
#include "poppck.h"

#ifdef __NT__
#include "pushpck1.h"
typedef struct batch_shmem {
    batch_len   len;
    batch_data  data;
} batch_shmem;
#include "poppck.h"
#endif

extern batch_data       bdata;

#ifdef __NT__
extern int              BatservPipeCreate( const char *name );
#endif
extern int              BatservPipeOpen( const char *name );
extern int              BatservReadData( void );
extern int              BatservWriteCmd( char link_cmd );
extern int              BatservWriteData( char link_cmd, const void *buff, unsigned len );
extern void             BatservPipeClose( void );
