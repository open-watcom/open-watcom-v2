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


#ifndef _EXE16M_H

#pragma pack(push,1);

typedef struct gdt_info {
    unsigned_16     gdtlen;
    unsigned_16     gdtaddr;
    unsigned_8      gdtaddr_hi;
    unsigned_8      gdtaccess;
    unsigned_16     gdtreserved;
} gdt_info;

// the values for the options fiels.

enum {
    OPT_KEYBOARD        = 0x0004,
    OPT_OVERLOAD        = 0x0008,
    OPT_INT10           = 0x0010,
    OPT_INIT00          = 0x0020,
    OPT_INITFF          = 0x0040,
    OPT_ROTATE          = 0x0080,
    OPT_AUTO            = 0x1000,
};

#define TRANSPARENT 0x8000

#define NUM_RESERVED_SELS    16         // number of reserved selectors.

// memory strategy constants
#define MPreferExt 0    /* prefer, but not force, extended */
#define MPreferLow 1    /* prefer, but not force, conventional memory */
#define MForceExt 2             /* force extended */
#define MForceLow 3             /* force conventional */
#define MNoStrategy 0       // no strategy specified.

#define acc_code 0x9A   /* present, priv 0, code, read, accessed */
#define acc_data 0x92   /* present, priv 0, data, write, accessed */

#define user_sel 0x80   /* first user selecter (user_gdt << 3) */

#pragma pack(push,1);

#define _EXE16M_H
#endif
