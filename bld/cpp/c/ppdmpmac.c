/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Macro dump function.
*
****************************************************************************/


#include "_preproc.h"
#include "ppdmpmac.h"


static void dump_func( const MACRO_ENTRY *me, const PREPROC_VALUE *val, void *cookie )
{
    /* unused parameters */ (void)cookie;

    printf( "#define %s %s ", me->name, me->replacement_list );
    if( val->type == PPTYPE_SIGNED ) {
        printf( "(value=%ld)\n", val->val.ivalue );
    } else {
        printf( "(value=%luUL)\n", val->val.uvalue );
    }
}

void PP_Dump_Macros( void )
{
    PP_MacrosWalk( dump_func, NULL );
}
