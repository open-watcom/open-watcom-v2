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


#include "standard.h"
#include "coderep.h"
#include "procdef.h"
#include "model.h"
#include "cgaux.h"
#include "sysmacro.h"
#include "typedef.h"
#include "feprotos.h"

extern  sym_handle      AskForLblSym(label_handle);
extern  bool            AskIfRTLabel(label_handle);
extern  pointer         BEAuxInfo(pointer,aux_class);

extern  pointer FindAuxInfo( name *name, aux_class request ) {

    bck_info            *bck;
    aux_handle          info;

    if( name == NULL ) {
        info = FEAuxInfo( NULL, AUX_LOOKUP );  /* return default*/
        return( FEAuxInfo( info, request ) );
    } else if( name->n.class != N_MEMORY ) {
        info = FEAuxInfo( NULL, AUX_LOOKUP );  /* return default*/
        return( FEAuxInfo( info, request ) );
    } else if( name->m.memory_type == CG_FE ) {
        info = FEAuxInfo( name->v.symbol, AUX_LOOKUP );
        return( FEAuxInfo( info, request ) );
    } else if( name->m.memory_type == CG_BACK ) {
        bck = name->v.symbol;
        info = FEAuxInfo( AskForLblSym( bck->lbl ), AUX_LOOKUP );
        return( FEAuxInfo( info, request ) );
    }
#if _TARGET & _TARG_INTEL
    else if( name->m.memory_type == CG_LBL
         && AskIfRTLabel( name->v.symbol ) ) {
        info = BEAuxInfo( name->v.symbol, AUX_LOOKUP );
        if( info == NULL ) {
            info = FEAuxInfo( NULL, AUX_LOOKUP );  /* return default*/
            return( FEAuxInfo( info, request ) );
        } else {
            return( BEAuxInfo( info, request ) );
        }
    }
#endif
    else {
        info = FEAuxInfo( NULL, AUX_LOOKUP );  /* return default*/
        return( FEAuxInfo( info, request ) );
    }
}

