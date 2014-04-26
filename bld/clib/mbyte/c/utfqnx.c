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


#include "variety.h"
#include "mbqnx.h"

const utf_table
#if defined( _M_I86HM )
    __far
#else
    __near
#endif
__utf_table[7] = {
    { 0x80, 0x00, 0 * 6, 0x0000007f, 0x00000000 },  /* 1 byte sequence */
    { 0xe0, 0xc0, 1 * 6, 0x000007ff, 0x00000080 },  /* 2 byte sequence */
    { 0xf0, 0xe0, 2 * 6, 0x0000ffff, 0x00000800 },  /* 3 byte sequence */
    { 0xf8, 0xf0, 3 * 6, 0x001fffff, 0x00010000 },  /* 4 byte sequence */
    { 0xfc, 0xf8, 4 * 6, 0x03ffffff, 0x00200000 },  /* 5 byte sequence */
    { 0xfe, 0xfc, 5 * 6, 0x7fffffff, 0x04000000 },  /* 6 byte sequence */
    0
};
