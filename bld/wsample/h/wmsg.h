/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Execution Sampler message ordinals and functions.
*
****************************************************************************/


#include "wmsg.rh"

#define MSG_USE_E_BASE  (MSG_USAGE_LN_1 + RLE_ENGLISH * MSG_LANG_SPACING)
#define MSG_USE_J_BASE  (MSG_USAGE_LN_1 + RLE_JAPANESE * MSG_LANG_SPACING)

#define GET_MESSAGE(m)  MsgArray[(m) - ERR_FIRST_MESSAGE]

extern char FAR_PTR     *MsgArray[ERR_LAST_MESSAGE - ERR_FIRST_MESSAGE + 1];

#ifdef __WINDOWS__
extern bool     MsgInit( HINSTANCE inst );
#else
extern bool     MsgInit( void );
#endif
extern void     MsgFini( void );
extern void     MsgPrintfUsage( int first_ln, int last_ln );
