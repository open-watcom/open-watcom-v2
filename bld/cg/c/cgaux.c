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


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "procdef.h"
#include "model.h"
#include "cgauxinf.h"
#include "typedef.h"
#include "feprotos.h"

extern  pointer FindAuxInfo( name *name, aux_class request )
{
    if( name == NULL ) {
        /* return default aux info */
        return( FindAuxInfoSym( NULL, request ) );
    } else if( name->n.class != N_MEMORY ) {
        /* return default aux info */
        return( FindAuxInfoSym( NULL, request ) );
    } else if( name->m.memory_type == CG_FE ) {
        return( FindAuxInfoSym( name->v.symbol, request ) );
    } else if( name->m.memory_type == CG_BACK ) {
        return( FindAuxInfoSym( AskForLblSym( ((back_handle)name->v.symbol)->lbl ), request ) );
#if _TARGET & _TARG_INTEL
    } else if( name->m.memory_type == CG_LBL && AskIfRTLabel( name->v.symbol ) ) {
        aux_handle  info;

        info = BEAuxInfo( name->v.symbol, AUX_LOOKUP );
        if( info == NULL ) {
            /* return default aux info */
            return( FindAuxInfoSym( NULL, request ) );
        } else {
            return( BEAuxInfo( info, request ) );
        }
#endif
    } else {
        /* return default aux info */
        return( FindAuxInfoSym( NULL, request ) );
    }
}

