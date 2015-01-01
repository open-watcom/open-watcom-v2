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


#ifndef DBGITEM_H
#define DBGITEM_H

#include "madtypes.h"

typedef struct {
    addr32_off  str;
    unsigned_16 len;
} scb_nw;

typedef struct {
    addr32_ptr  str;
    unsigned_16 len;
} scb_fw;

typedef struct {
    addr48_off  str;
    unsigned_32 len;
} scb_nd;

typedef struct {
    addr48_ptr  str;
    unsigned_32 len;
} scb_fd;

typedef union {
    #define pick(e,s,t,n) t n;
    #include "_dbgitem.h"
    #undef pick
    unsigned_8      ar[1];  /* actually variable size */
} item_mach;

typedef enum {
    IT_NIL,
    #define pick(e,s,t,n) e,
    #include "_dbgitem.h"
    #undef pick
    IT_MAX,

    IT_ERR = 0x0100,
    IT_IO  = 0x0200,
    IT_DEC = 0x0400,
    IT_INC = 0x0800,
} item_type;
#define IT_TYPE_MASK    ~(IT_ERR | IT_IO | IT_DEC | IT_INC)


extern item_type    ItemGetMAD( address *, item_mach *, item_type, mad_type_handle );
extern item_type    ItemPutMAD( address *, const item_mach *, item_type, mad_type_handle );
extern unsigned     ItemSize( item_type );

#endif
