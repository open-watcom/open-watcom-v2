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
// AUXLOOK      : auxiliary information lookup
//

#include "ftnstd.h"
#include "global.h"
#include "wf77aux.h"
#include "iflookup.h"
#include "cpopt.h"
#include <string.h>
#include "clibext.h"

extern  void            CopyAuxInfo(aux_info *,aux_info *);
extern  aux_info        *NewAuxEntry(char *,int);
extern  aux_info        *RTAuxInfo(sym_id);

extern  aux_info        DefaultInfo;
extern  aux_info        IFVarInfo;
extern  aux_info        IFCharInfo;
extern  aux_info        IFChar2Info;
extern  aux_info        IFXInfo;
extern  aux_info        IFInfo;
extern  aux_info        FortranInfo;
extern  aux_info        ProgramInfo;
extern  aux_info        *AuxInfo;


aux_info    *AuxLookupName( char *name, int name_len ) {
//======================================================

    aux_info    *aux;

    aux = AuxInfo;
    for(;;) {
        if( aux == NULL ) break;
        if( aux->sym_len == name_len ) {
            if( memicmp( name, aux->sym_name, name_len ) == 0 ) break;
        }
        aux = aux->link;
    }
    return( aux );
}


aux_info    *AuxLookupAdd( char *name, int name_len ) {
//=====================================================

    aux_info    *aux;

    aux = AuxLookupName( name, name_len );
    if( aux == NULL ) {
        aux = NewAuxEntry( name, name_len );
        CopyAuxInfo( aux, &FortranInfo );
    }
    return( aux );
}


aux_info    *AuxLookup( sym_id sym ) {
//====================================

    aux_info    *info;

    if( sym == NULL ) return( &FortranInfo );
    if( ( sym->u.ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        if( sym->u.ns.flags & SY_INTRINSIC ) {
            if( IFVarArgs( sym->u.ns.si.fi.index ) ) {
                return( &IFVarInfo );
            // check for character arguments must come first so that
            // IF@xxx gets generated for intrinsic functions with character
            // arguments (instead of XF@xxxx)
            } else if( IFArgType( sym->u.ns.si.fi.index ) == FT_CHAR ) {
                if( sym->u.ns.flags & SY_IF_ARGUMENT ) {
                    if( !(Options & OPT_DESCRIPTOR) ) {
                        return( &IFChar2Info );
                    }
                }
                return( &IFCharInfo );
            } else if( sym->u.ns.flags & SY_IF_ARGUMENT ) {
                return( &IFXInfo );
            }
            return( &IFInfo );
        } else if( sym->u.ns.flags & SY_RT_ROUTINE ) {
            return( RTAuxInfo( sym ) );
        } else if( ( sym->u.ns.flags & SY_SUBPROG_TYPE ) == SY_PROGRAM ) {
            return( &ProgramInfo );
        } else {
            info = AuxLookupName( sym->u.ns.name, sym->u.ns.u2.name_len );
            if( info == NULL ) return( &FortranInfo );
            return( info );
        }
    } else {
        info = AuxLookupName( sym->u.ns.name, sym->u.ns.u2.name_len );
        if( info == NULL ) return( &FortranInfo );
        return( info );
    }
}
