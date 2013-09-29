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
* Description:  upscan operator index into constant tables
*
****************************************************************************/


#ifndef _OPR_H_INCLUDED
#define _OPR_H_INCLUDED

typedef enum {
    #define pick(id,opr_index,proc_index) id,
    #include "oprdefn.h"
    #undef pick
} OPR;

//  operator group ranges

#define OPR_FIRST_LOGOP     OPR_EQV
#define OPR_LAST_LOGOP      OPR_NOT
#define OPR_FIRST_RELOP     OPR_EQ
#define OPR_LAST_RELOP      OPR_GT
#define OPR_FIRST_ARITHOP   OPR_PLS
#define OPR_LAST_ARITHOP    OPR_CAT

#endif
