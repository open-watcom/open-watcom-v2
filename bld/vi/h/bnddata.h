/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  bind data structures and definition
*
****************************************************************************/


/*
 * MAGIC_COOKIE must be aligned to 2-bytes
 * including hidden null termination character
 */
#define MAGIC_COOKIE        "CGEXXX\0"
#define MAGIC_COOKIE_SIZE   sizeof( MAGIC_COOKIE )

#define SEEK_POSBACK(p)     (-(long)(p))

#define TYPE_BIND           unsigned short
#define SIZE_BIND           sizeof( TYPE_BIND )
#define TRAILER_SIZE        (MAGIC_COOKIE_SIZE + SIZE_BIND)

#define GET_SIZE(p)         *((TYPE_BIND *)(p))
#define SET_SIZE(p,v)       (*((TYPE_BIND *)(p))=(TYPE_BIND)(v))
#define GET_MAGIC_SIZE(p)   GET_SIZE((char*)(p)+MAGIC_COOKIE_SIZE)
#define SET_MAGIC_SIZE(p,v) SET_SIZE((char*)(p)+MAGIC_COOKIE_SIZE,(v))

