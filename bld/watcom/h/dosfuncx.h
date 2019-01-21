/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  <dosfunc.h> header file extensions
*
****************************************************************************/


#include <dosfunc.h>

#define DOS_GET_CHAR_ECHO_CHECK     0x01
#define DOS_OUTPUT_CHAR             0x06
#define DOS_GET_CHAR_NO_ECHO_CHECK  0x08
#define DOS_BUFF_INPUT              0x0A
#define DOS_INPUT_STATUS            0x0B
#define DOS_SET_DRIVE               0x0E
#define DOS_DELETE_FCB              0x13
#define DOS_CREATE_PSP              0x26
#define DOS_PARSE_FCB               0x29
#define DOS_GET_DTA                 0x2F
#define DOS_TERM_STAY_RESID         0x31
#define DOS_FREE_SPACE              0x36
#define DOS_COUNTRY_INFO            0x38
#define DOS_SET_PSP                 0x50
#define DOS_GET_PSP                 0x51
#define DOS_GET_LIST_OF_LIST        0x52
#define DOS_EXT_ERR                 0x59
#define DOS_CREATE_TMP              0x5A
#define DOS_CREATE_NEW              0x5B
#define DOS_RECORD_LOCK             0x5C
#define DOS_TRUENAME                0x60
#define DOS_SET_HCOUNT              0x67
#define DOS_EXT_CREATE              0x6C
