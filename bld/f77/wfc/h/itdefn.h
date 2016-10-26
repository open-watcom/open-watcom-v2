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


#ifndef _ITDEFN_H_INCLUDED
#define _ITDEFN_H_INCLUDED

#include "symtypes.h"
#include "opr.h"
#include "opn.h"

// Definition of an INTERNAL TEXT NODE:
// ====================================
typedef struct itnode {
    struct itnode       *link;          // link to next itnode
    struct itnode       *list;          // link to argument list
    unsigned_16         oprpos;         // position of opr in statement
    unsigned_16         opnpos;         // position of opn in statement
    OPR                 opr;            // delimiter field
    union {
        DSOPN           ds;
        USOPN           us;
    }                   opn;            // operand code value
    unsigned_16         flags;          // flags
    uint                size;           // size of operand
    sym_id              sym_ptr;        // addr of data area (common block)
    ftn_type            value;          // value of constants, parameters
    int                 opnd_size;      // size of operand field
    char                *opnd;          // operand field
    TYPE                typ;            // type of symbol
    char                chsize : 4;     // size of character operand (OPN_SS1)
    char                is_unsigned : 1;// integer constant is unsigned
    char                is_catparen : 1;// left paren matches right paren of
} itnode;

#endif
