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


#ifndef _FMTTYPE_H

#include "vbuf.h"

#define SPECIAL_TYPE_FMTS \
 TFMT_DEF(CLASS_TEMPLATE,       "<class template> "     ) \
 TFMT_DEF(NAMESPACE,            "<namespace> "          ) \
 TFMT_DEF(MAX,                  ""                      )

typedef enum {
    #define TFMT_DEF(a,b) TFMT_##a,
    SPECIAL_TYPE_FMTS
    #undef TFMT_DEF
} tfmt_index;

typedef enum {
    FF_DEFAULT      =0x00,  // default behaviour
    FF_USE_VOID     =0x01,  // put void in for no type
    FF_DROP_RETURN  =0x02,  // leave off function return type
    FF_TYPEDEF_STOP =0x04,  // terminate at typedef names
    FF_ARG_NAMES    =0x08,  // use argument names in arglists
    FF_NULL         =0x00
} FMT_CONTROL;
extern FMT_CONTROL FormatTypeDefault;

void FormatFunctionType( TYPE, VBUF *, VBUF *, int, FMT_CONTROL );
void FormatType( TYPE, VBUF *, VBUF * );
void FormatTypeModFlags( type_flag, VBUF * );
extern char *FormatErrorType( TYPE );

#define _FMTTYPE_H
#endif
