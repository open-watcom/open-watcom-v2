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
* Description:  Assembler message output interface.
*
****************************************************************************/


#ifndef _ASMRCMSG_H_INCLUDED
#define _ASMRCMSG_H_INCLUDED

    #define MAX_MESSAGE_SIZE    128

    #define MSG_SHARE_BASE      1
    #define MSG_WOMP_BASE       200
    #define MSG_WASM_BASE       500
    #define MSG_USAGE_BASE      900

#if defined( USE_TEXT_MSGS )

enum {
#define pick(c,e,j) c,
    START_BASE1 = MSG_SHARE_BASE - 1,
#include "../h/asmshare.msg"
    MSG_SHARE_LAST,
    START_BASE2 = MSG_WOMP_BASE - 1,
#include "../h/womp.msg"
    MSG_WOMP_LAST,
    START_BASE3 = MSG_WASM_BASE - 1,
#include "../h/wasm.msg"
    MSG_WASM_LAST,
#undef pick
};

#else

    #define MSG_LANG_SPACING    1000

    #include "msg.gh"
    #include "wmpmsg.gh"
    #include "wasmmsg.gh"

#endif

#endif
