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


#ifndef DWABBREV_H_INCLUDED
#define DWABBREV_H_INCLUDED

typedef uint_32 abbrev_code;

/* AB_ENUM_MASK is an index into a table of partial abbreviations we use */
#define AB_ENUM_MASK            0x0000003f
#define AB_ENUM_SHIFT           0

/* anything in the AB_ALWAYS mask will always have the attr/form pair
    emitted */
#define AB_ALWAYS               0xf0000000

/*
    attr/form pairs are emitted in the following order:

    Tag for abbrev.
    attr/forms for all the following AB_ definitions in the order that
        the definitions appear below.
    AB_LOWHIGH_PC
    AB_SUBR_DECLARATION
    Any extra attr/forms specified in dwmakeab.c for this abbrev.

    It is important that AB_LOCATION and AB_DATA_LOCATION be emitted
    adjacently - there is code which assumes that this.

    AB_SUBR_DECLARATION is a magic flag designed to reduce the number of
    possible AB_SUBROUTINE abbreviations.  This takes into account that there
    will be two possibilities for subroutines:
    it is a DECLARATION: then we will have the AT_declaration,
    or it's a DEFINITION: we have AT_return_addr, AT_low/high_pc

    AB_START_REF doesn't actually correspond to an attr-form pair, it is
    just used as a flag inside the code.
*/
#define AB_START_REF            0x10000000
#define AB_SIBLING              0x20000000
#define AB_ACCESSIBILITY        0x40000000
#define AB_START_SCOPE          0x80000000

#define AB_DECL                 0x00000040
#define AB_DECLARATION          0x00000080
#define AB_TYPE                 0x00000100
#define AB_NAME                 0x00000200

#define AB_BYTE_SIZE            0x00000400
#define AB_OPACCESS             0x00000800
#define AB_DATA_LOCATION        0x00001000
#define AB_RETURN_ADDR          0x00002000
#define AB_ARTIFICIAL           0x00004000
#define AB_MEMBER               0x00008000
#define AB_SEGMENT              0x00010000
#define AB_ADDRESS_CLASS        0x00020000
#define AB_VTABLE_LOC           0x00040000
#define AB_SUBR_DECLARATION     0x00080000
#define AB_LOWHIGH_PC           0x00100000


#ifndef NO_ENUM
#include "dwabenum.i"
#endif

#define InitDebugAbbrev         DW_InitDebugAbbrev
#define FiniDebugAbbrev         DW_FiniDebugAbbrev
#define MarkAbbrevAsUsed        DW_MarkAbbrevAsUsed

void            InitDebugAbbrev( dw_client );
void            FiniDebugAbbrev( dw_client );
unsigned        MarkAbbrevAsUsed( dw_client, abbrev_code * );
extern void     GenAllAbbrev( dw_client  cli );

#endif
