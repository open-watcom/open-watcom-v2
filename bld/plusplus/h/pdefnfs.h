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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// Definitions for fs registration routines
//
// 94/10/23 -- J.W.Welch            -- defined


#define fs_push_ret     HW_D( HW_EMPTY )
#define fs_push_parms   P_EAX
#define fs_push_saves   HW_NotD( HW_EMPTY )
static byte_seq fs_push =
{   15, {
    0x64,0xff,0x35,0x00,0x00,0x00,0x00          // push fs:0
,   0x8f,0x00                                   // pop [eax]
,   0x64,0xa3,0x00,0x00,0x00,0x00               // mov fs:0,eax
} };


#define fs_pop_ret      HW_D( HW_EMPTY )
#define fs_pop_parms    P_EAX
#define fs_pop_saves    HW_NotD( HW_EAX )
static byte_seq fs_pop =
{   6,
    { 0x64,0xa3,0x00,0x00,0x00,0x00 }          // mov fs:0,eax
};


struct  inline_funcs Fs_Functions[] =
{   { ".FsPush", &fs_push, fs_push_parms, fs_push_ret, fs_push_saves }
,   { ".FsPop",  &fs_pop,  fs_pop_parms,  fs_pop_ret,  fs_pop_saves  }
,   { NULL                                                          }
};
