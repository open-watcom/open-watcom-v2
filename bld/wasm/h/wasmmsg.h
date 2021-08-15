/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Assembler message definitions
*
****************************************************************************/


#define MAX_MESSAGE_SIZE    128

#if defined( _STANDALONE_ ) && defined( INCL_MSGTEXT )
    // WASM, must be assigned dynamicaly
    #define MSG_SHARE_BASE      0

enum {
    MSG_WASM_BASE = MSG_SHARE_BASE
    #define pick(c,e,j) + 1
    #include "../h/wasmc.msg"
    #undef pick
};

enum {
    MSG_USAGE_BASE = MSG_WASM_BASE
    #define pick(c,e,j) + 1
    #include "../h/wasms.msg"
    #undef pick
};
#endif

#include "wasmmsg.rh"

#if !defined( _STANDALONE_ ) && defined( INCL_MSGTEXT )
    // WCC or WPP, must be assigned dynamicaly
enum {
    MSG_LANG_SPACING = MSG_SHARE_BASE
    #define pick(c,e,j) + 1
    #include "../h/wasmc.msg"
    #undef pick
};
#endif
