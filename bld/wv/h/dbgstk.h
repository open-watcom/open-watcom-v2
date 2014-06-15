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


#include "dbginfo.h"

typedef enum {
    SF_LOCATION         = 0x0001, /* v.loc is valid */
    SF_NAME             = 0x0002, /* v.name is valid as a symbol name */
    SF_SYM              = 0x0004, /* v.sh is valid */
    SF_END_PURGE        = 0x0008, /* end point of stack purge operation */
    SF_IMP_ADDR         = 0x0010, /* implied address op for addr exprs */
    SF_SCOPE            = 0x0020, /* is a failed scope name? */
    SF_CONST            = 0x0040, /* object was a constant */
    SF_FORM_MASK        = (SF_LOCATION|SF_NAME|SF_SYM)
} stack_flags;

typedef struct {
    location_list       loc;
    unsigned long       ss_offset;
    char                *allocated;
} string;

typedef union {
    lookup_item         li;
    sym_handle          *sh;
    location_list       loc;
    address             addr;
    unsigned_64         uint;
    signed_64           sint;
    long                sint32;
    long unsigned       uint32;
    xreal               real;
    xcomplex            cmplx;
    string              string;
    struct stack_entry  *save_sp;
} stack_value;


typedef struct stack_entry {
    struct stack_entry  *up, *dn;
    type_handle         *th;
    dip_type_info       info;
    stack_flags         flags;
    stack_value         v;
    location_context    *lc;
} stack_entry;

#define SET_TH( e )     (e)->th = (void *)((e)+1)
#define SET_SH( e )     (e)->v.sh = (void *)((unsigned_8 *)((e)+1)+type_SIZE)
