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
* Description:  Intel x86 descriptor prototype.
*
****************************************************************************/


#ifndef _DESCRIPT_H_INCLUDED
#define _DESCRIPT_H_INCLUDED

typedef struct {
    unsigned short  limit_15_0;
    unsigned short  base_15_0;
    unsigned char   base_23_16;
    unsigned char   available                : 1;
    unsigned char   writeable_or_readable    : 1;
    unsigned char   expanddown_or_conforming : 1;
    unsigned char   type                     : 2;
    unsigned char   dpl                      : 2;
    unsigned char   present                  : 1;
    unsigned char   limit_19_16              : 4;
    unsigned char   avl                      : 1;
    unsigned char   reserved                 : 1;
    unsigned char   big_or_default           : 1;
    unsigned char   granularity              : 1;
    unsigned char   base_31_24;
} descriptor;

#define GET_DESC_BASE( desc ) \
    ((DWORD)(desc).base_15_0 + ((DWORD)(desc).base_23_16 << 16L) + \
    ((DWORD)(desc).base_31_24 << 24L))

#define GET_DESC_LIMIT( desc ) \
    ((desc).granularity ? \
    ((((DWORD)(desc).limit_15_0 + ((DWORD)(desc).limit_19_16 << 16L)) << 12L) + 0xfffL) : \
    ((DWORD)(desc).limit_15_0 + ((DWORD)(desc).limit_19_16 << 16L)))

#endif /* _DESCRIPT_H_INCLUDED */
