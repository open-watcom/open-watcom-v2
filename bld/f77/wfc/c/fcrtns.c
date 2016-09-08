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


//
// FCRTNS   : F-Code processing runtime routines
//

#include "ftnstd.h"
#include "global.h"
#include "wf77defs.h"
#include "cg.h"
#include "wf77auxd.h"
#include "wf77aux.h"
#include "fcgbls.h"
#include "cgflags.h"
#include "cpopt.h"
#include "types.h"
#include "fctypes.h"
#include "rtconst.h"
#include "rstutils.h"
#include "fcrtns.h"
#include "wf77info.h"
#include "cgswitch.h"
#include "cgprotos.h"

#include <string.h>


extern  cgflags_t       CGFlags;

typedef struct rt_rtn {
    char        __FAR *name;
    sym_id      sym_ptr;
    aux_info    *aux;
    byte        typ;
} rt_rtn;

static rt_rtn  __FAR RtnTab[] = {
    #define pick(id,name,sym,aux,typ) {name,sym,aux,typ},
    #include "rtdefn.h"
    #undef pick
};

#define MAX_RT_INDEX    ((sizeof( RtnTab ) / sizeof( RtnTab[0] ))-1)


call_handle     InitCall( RTCODE rtn_id ) {
//======================================

// Initialize a call to a runtime routine.

    sym_id      sym;
    rt_rtn      __FAR *rt_entry;
    byte        typ;
    int         name_len;
    char        __FAR *ptr;

    rt_entry = &RtnTab[ rtn_id ];
    sym = rt_entry->sym_ptr;
    if( sym == NULL ) {
        name_len = 0;
        ptr = rt_entry->name;
        while( *ptr != NULLCHAR ) {
            SymBuff[ name_len ] = *ptr;
            ++name_len;
            ++ptr;
        }
        sym = STAdd( SymBuff, name_len );
        sym->u.ns.flags = SY_USAGE | SY_TYPE | SY_SUBPROGRAM | SY_FUNCTION |
                        SY_RT_ROUTINE;
        if( rt_entry->typ == FT_NO_TYPE ) {
            sym->u.ns.u1.s.typ = FT_INTEGER_TARG;
        } else {
            sym->u.ns.u1.s.typ = rt_entry->typ;
        }
        sym->u.ns.xt.size = TypeSize( sym->u.ns.u1.s.typ );
        sym->u.ns.u3.address = NULL;
        sym->u.ns.si.sp.u.imp_segid = AllocImpSegId();
        rt_entry->sym_ptr = sym;
    }
    typ = F77ToCGType( sym );
    return( CGInitCall( CGFEName( sym, typ ), typ, rt_entry->sym_ptr ) );
}


void    InitRtRtns( void ) {
//====================

// Initialize symbol table entries for run-time routines.

    int         rt_index;

    rt_index = 0;
    while( rt_index <= MAX_RT_INDEX ) {
        RtnTab[ rt_index ].sym_ptr = NULL;
        rt_index++;
    }
}


void    FreeRtRtns( void ) {
//====================

// Free symbol table entries for run-time routines.

    int         rt_index;
    sym_id      sym;

    rt_index = 0;
    while( rt_index <= MAX_RT_INDEX ) {
        sym = RtnTab[ rt_index ].sym_ptr;
        if( sym != NULL ) {
            if( ( CGFlags & CG_FATAL ) == 0 ) {
                if( sym->u.ns.u3.address != NULL ) {
                    BEFreeBack( sym->u.ns.u3.address );
                }
            }
            STFree( sym );
            RtnTab[ rt_index ].sym_ptr = NULL;
        }
        rt_index++;
    }
}


aux_info    *RTAuxInfo( sym_id rtn ) {
//====================================

// Return aux information for run-time routine.

    rt_rtn      __FAR *rt_entry;

    rt_entry = RtnTab;
    while( rt_entry->sym_ptr != rtn ) {
        rt_entry++;
    }
    return( rt_entry->aux );
}
