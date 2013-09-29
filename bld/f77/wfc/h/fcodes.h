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
* Description:  Define F-Codes constants
*
****************************************************************************/


#ifndef _FCODES_H_INCLUDED
#define _FCODES_H_INCLUDED

typedef enum {
    #define pick(id,code_proc,data_proc) id,
    #include "fcdefn.h"
    #undef pick
} FCODE;

#define FC_BITOPS           FC_BIT_EQUIV
#define FC_LOGOPS           FC_EQV

#define FC_BINOPS           FC_ADD
#define FC_CC_BINOPS        FC_CCADD
#define FC_CX_BINOPS        FC_CXADD
#define FC_XC_BINOPS        FC_XCADD

#define FC_RELOPS           FC_CMP_EQ
#define FC_CC_RELOPS        FC_CC_CMP_EQ
#define FC_CX_RELOPS        FC_CX_CMP_EQ
#define FC_XC_RELOPS        FC_XC_CMP_EQ
#define FC_CHAR_RELOPS      FC_CHAR_CMP_EQ
#define FC_CHAR_1_RELOPS    FC_CHAR_1_CMP_EQ

#define FC_IO_STMTS         FC_EX_READ

#endif
