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
* Description:  Typing information
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "symtypes.h"
#include "cpopt.h"
#include "types.h"

typedef struct type_info {
    char    *text;
    uint    size;
    PTYPE   ptyp;
} type_info;

#ifdef pick
#undef pick
#endif

#define pick(id,text,size,ptype) {text,size,ptype},

static type_info TypeInfo[] = {
#include "symdefn.h"
};

uint            TypeSize( TYPE typ ) {
//====================================

// Get the size of the storage unit for the given data type.

    return( TypeInfo[ typ ].size );
}


uint            StorageSize( TYPE typ ) {
//=======================================

// Get the size of the storage unit for a the given data type.
// This function is sensitive to the "short" option.

    if( Options & OPT_SHORT ) {
        if( typ == FT_INTEGER )
            return( sizeof( intstar2 ) );
        if( typ == FT_LOGICAL ) {
            return( sizeof( logstar1 ) );
        }
    }
    if( Options & OPT_EXTEND_REAL ) {
        if( typ == FT_REAL )
            return( sizeof( double ) );
        if( typ == FT_DOUBLE ) {
            return( TypeInfo[ FT_EXTENDED ].size );
        }
    }
    return( TypeInfo[ typ ].size );
}


intstar4        ITIntValue( itnode *it ) {
//========================================

// Get integer value from i.t. node.

    if( it->size == sizeof( intstar1 ) ) {
        if( it->is_unsigned ) {
            return( (unsigned_8)it->value.intstar1 );
        } else {
            return( it->value.intstar1 );
        }
    }
    if( it->size == sizeof( intstar2 ) ) {
        if( it->is_unsigned ) {
            return( (unsigned_16)it->value.intstar2 );
        } else {
            return( it->value.intstar2 );
        }
    }
    // must be sizeof( intstar4 )
    return( it->value.intstar4 );
}


char    *TypeKW( TYPE typ ) {
//===========================

// Get the keyword string for the given data type.

    return( TypeInfo[ typ ].text );
}

PTYPE   ParmType( TYPE typ, uint size ) {
//=======================================

    size = size;
    return( TypeInfo[ typ ].ptyp );
}
