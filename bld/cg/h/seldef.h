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


#ifndef __SELDEF__H__
#define __SELDEF__H__

#include "useinfo.h"

typedef struct select_list {
        struct select_list      *next;
        signed_32               low;
        signed_32               high;
        unsigned_32             count;
        label_handle            label;
} select_list;

typedef struct select_node {
#ifndef NDEBUG
        use_info        useinfo;
#endif
        struct select_list      *list;
        label_handle            other_wise;
        signed_32               lower;
        signed_32               upper;
        uint                    num_cases;
} select_node;

typedef struct tbl_control {
        label_handle            lbl;            /*  for the table */
        label_handle            value_lbl;      /*  for the value table */
        uint                    size;
        label_handle            cases[1];
} tbl_control;
#endif // __SELDEF__H__
