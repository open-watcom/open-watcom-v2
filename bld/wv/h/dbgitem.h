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
    unsigned_8      ub;
      signed_8      sb;
    unsigned_16     uw;
      signed_16     sw;
    unsigned_32     ud;
      signed_32     sd;
    unsigned_64     uq;
      signed_64     sq;
    sreal           sf;
    lreal           lf;
    xreal           xf;
    addr32_off      so;
    addr48_off      lo;
    unsigned_64     qo;
    addr32_ptr      sa;
    addr48_ptr      la;
    address         xa;
    scomplex        sc;
    lcomplex        lc;
    xcomplex        xc;
    scb_nw          nwscb;
    scb_fw          fwscb;
    scb_nd          ndscb;
    scb_fd          fdscb;
    unsigned_8      ar[1];  /* actually variable size */
} item_mach;

typedef unsigned_16 item_type; enum {
        IT_NIL,
        IT_UB,
        IT_SB,
        IT_UW,
        IT_SW,
        IT_UD,
        IT_SD,
        IT_SF,
        IT_LF,
        IT_XF,
        IT_SO,
        IT_LO,
        IT_SA,
        IT_LA,
        IT_SC,
        IT_LC,
        IT_XC,
        IT_NWSCB,
        IT_FWSCB,
        IT_NDSCB,
        IT_FDSCB,
        IT_ERR = 0x0100,
        IT_IO  = 0x0200,
        IT_DEC = 0x0400,
        IT_INC = 0x0800,
        IT_TYPE_MASK = ~(IT_ERR | IT_IO | IT_DEC | IT_INC)
};

//extern bool       ItemGet( address *, item_mach *, item_type );
extern item_type    ItemGetMAD( address *, item_mach *, item_type, mad_type_handle );
//extern bool       ItemPut( address *, item_mach *, item_type );
extern item_type    ItemPutMAD( address *, item_mach *, item_type, mad_type_handle );
extern unsigned     ItemSize( item_type );
//extern item_type    ItemType( unsigned );
//extern item_type    ItemTypeFromMADType( mad_type_handle );

#define DBGITEM_H
#endif
