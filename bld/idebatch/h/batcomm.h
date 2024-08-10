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


#define TRANS_MAXLEN        1024
#define DEFAULT_LINK_NAME   "BatLink"

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
        char            buff[TRANS_MAXLEN];
        struct {
#ifdef __NT__
            unsigned    len;
#endif
            char        cmd;
            union {
                char            data[1];
                batch_stat      status;
            } u;
        } s;
    } u;
} batch_data;
#include "poppck.h"
