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
* Description:  Macro dump function.
*
****************************************************************************/


#include "preproc.h"
#include "ppdmpmac.h"


void PP_Dump_Macros( void )
{
    int             hash;
    const char      *endptr;
    MACRO_ENTRY     *me;
    PREPROC_VALUE   val;

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        for( me = PPHashTable[hash]; me != NULL; me = me->next ) {
            if( me->parmcount == 0 && me->replacement_list != NULL ) {
                if( PPEvalExpr( me->replacement_list, &endptr, &val ) ) {
                    if( *endptr == '\0' ) {
                        printf( "#define %s %s ", me->name, me->replacement_list );
                        if( val.type == PPTYPE_SIGNED ) {
                            printf( "(value=%ld)\n", val.val.ivalue );
                        } else {
                            printf( "(value=%luUL)\n", val.val.uvalue );
                        }
                    }
                }
            }
        }
    }
}
