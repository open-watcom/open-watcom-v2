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
* Description:  Resident symbol table manager.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "segsw.h"
#include "stmtsw.h"
#include "errcod.h"
#include "fmemmgr.h"
#include "insert.h"

#include <string.h>

extern  uint                    AllocName(uint);


static  sym_id  AddStruct( char *name, int length ) {
//===================================================

// Add a symbol table entry to the symbol table. Return a pointer to the
// new symbol table entry.

    sym_id      sym;

    sym = FMemAlloc( sizeof( fstruct ) + AllocName( length ) );
    sym->sd.dbh = 0;
    sym->sd.name_len = length;
    memcpy( &sym->sd.name, name, length );
    return( sym );
}


sym_id  FindStruct( char *name, int len ) {
//=========================================

// Search symbol table for given name.

    sym_id      head;

    head = RList;
    for(;;) {
        if( head == NULL ) return( NULL );
        if( head->sd.name_len == len ) {
            if( memcmp( name, &head->sd.name, len ) == 0 ) return( head );
        }
        head = head->sd.link;
    }
}


sym_id  STStruct( char *name, int length ) {
//==========================================

// Lookup the specified structure name in the symbol table.

    sym_id      sym;

    if( length > MAX_SYMLEN ) {
        length = MAX_SYMLEN;
    }
    if( name != NULL ) {
        sym = FindStruct( name, length );
    } else {
        // This is so that we don't issue SP_DUPLICATE_FIELD errors when
        // the user makes mistakes such as:
        //      STRUCTURE               ! forgot the name
        //          INTEGER K
        //      END STRUCTURE
        //      STRUCTURE               ! forgot the name again
        //          REAL K
        //      END STRUCTURE
        // or,
        //      STRUCTURE /FOO/
        //          INTEGER     K
        //      END STRUCTURE
        //      STRUCTURE /FOO/         ! CpStructure will pass NULL name here
        //          INTEGER     K
        //      END STRUCTURE
        sym = NULL;
    }
    if( sym == NULL ) {
        sym = AddStruct( name, length );
        sym->sd.link = RList;
        sym->sd.fl.fields = NULL;
        sym->sd.size = 0;
        RList = sym;
    }
    return( sym );
}


char    *STStructName( sym_id sym, char *buff ) {
//===============================================

// Get structure name.

    memcpy( buff, &sym->sd.name, sym->sd.name_len );
    buff += sym->sd.name_len;
    *buff = NULLCHAR;
    return( buff );

}


char    *STFieldName( sym_id sym, char *buff ) {
//===============================================

// Get structure name.

    memcpy( buff, &sym->fd.name, sym->fd.name_len );
    buff += sym->fd.name_len;
    *buff = NULLCHAR;
    return( buff );

}


static  sym_id  *Strut( sym_id *p_field, char *name, uint len ) {
//===============================================================

    sym_id      map;
    sym_id      field;
    sym_id      *q_field;

    for(;;) {
        field = *p_field;
        if( field == NULL ) return( p_field );
        if( field->fd.typ == FT_UNION ) {
            map = field->fd.xt.sym_record;
            for(;;) {
                if( map == NULL ) break;
                q_field = Strut( &map->sd.fl.sym_fields, name, len );
                if( *q_field != NULL ) {
                    FieldErr( SP_DUPLICATE_FIELD, *q_field );
                }
                map = map->sd.link;
            }
            if( ( SgmtSw & SG_DEFINING_MAP ) && ( field->fd.link == NULL ) ) {
                return( q_field );
            }
        } else {
            if( field->fd.name_len == len ) {
                if( memcmp( name, &field->fd.name, len ) == 0 ) {
                    FieldErr( SP_DUPLICATE_FIELD, field );
                }
            }
        }
        p_field = &field->fd.link;
    }
}


static  sym_id  AddField( char *name, int length ) {
//==================================================

// Add a symbol table entry to the symbol table. Return a pointer to the
// new symbol table entry.

    sym_id      sym;

    sym = FMemAlloc( sizeof( field ) + AllocName( length ) );
    sym->fd.name_len = length;
    memcpy( &sym->fd.name, name, length );
    return( sym );
}


sym_id  STField( char *name, uint len ) {
//=======================================

// Allocate a field name.

    sym_id      *p_field;

    if( len > MAX_SYMLEN ) {
        len = MAX_SYMLEN;
    }
    p_field = Strut( &CurrStruct->sd.fl.sym_fields, name, len );
    *p_field = AddField( name, len );
    (*p_field)->fd.link = NULL;
    (*p_field)->fd.dim_ext = NULL;
    return( *p_field );
}


static  sym_id  LookupField( sym_id field, char *name, uint len,
                             intstar4 *offset ) {
//==============================================================

    sym_id      map;
    sym_id      u_field;
    intstar4    size;
    intstar4    f_size;
    intstar4    f_offset;

    f_offset = 0;
    for(;;) {
        if( field == NULL ) return( NULL );
        if( field->fd.typ == FT_UNION ) {
            size = 0;
            map = field->fd.xt.sym_record;
            while( map != NULL ) {
                u_field = LookupField( map->sd.fl.sym_fields, name, len, &f_size );
                if( u_field != NULL ) {
                    *offset = f_offset + f_size;
                    return( u_field );
                }
                if( size < map->sd.size ) {
                    size = map->sd.size;
                }
                map = map->sd.link;
            }
        } else {
            if( field->fd.name_len == len ) {
                if( memcmp( name, &field->fd.name, len ) == 0 ) {
                    *offset = f_offset;
                    return( field );
                }
            }
            if( field->fd.typ == FT_STRUCTURE ) {
                size = field->fd.xt.record->size;
            } else {
                size = field->fd.xt.size;
            }
            if( field->fd.dim_ext != NULL ) {
                size *= field->fd.dim_ext->num_elts;
            }
        }
        f_offset += size;
        field = field->fd.link;
    }
}


sym_id  FieldLookup( sym_id prev, char *name, uint len, intstar4 *offset ) {
//==========================================================================

// Search for a field name.

    *offset = 0;
    return( LookupField( prev, name, len, offset ) );
}


bool    CalcStructSize( sym_id sd ) {
//===================================

// determine the size of a structure
// return TRUE if recursion detected, FALSE otherwise

    sym_id      map;
    sym_id      field;
    sym_id      saved_link;
    intstar4    size;
    intstar4    total_size;

    // in case size of structure already calculated
    if( sd->sd.size != 0 ) return( FALSE );
    saved_link = sd->sd.link;
    if( saved_link == sd ) {
        return( TRUE );         // recursion detected!
    }
    sd->sd.link = sd;           // to protect against recursion
    total_size = 0;
    field = sd->sd.fl.sym_fields;
    while( field != NULL ) {
        size = 0;
        if( field->fd.typ == FT_UNION ) {
            map = field->fd.xt.sym_record;
            while( map != NULL ) {
                if( CalcStructSize( map ) ) {
                    sd->sd.link = saved_link;
                    return( TRUE );             // recursion detected
                }
                if( size < map->sd.size ) {
                    size = map->sd.size;
                }
                map = map->sd.link;
            }
        } else {
            if( field->fd.typ == FT_STRUCTURE ) {
                if( StmtSw & SS_DATA_INIT ) {
                    if( field->fd.xt.record->fl.fields == NULL ) {
                        StructErr( SP_UNDEF_STRUCT, field->fd.xt.sym_record );
                    }
                }
                if( CalcStructSize( field->fd.xt.sym_record ) ) {
                    sd->sd.link = saved_link;
                    return( TRUE );             // recursion detected
                }
                size = field->fd.xt.record->size;
            } else {
                size = field->fd.xt.size;
            }
            if( field->fd.dim_ext != NULL ) {
                size *= field->fd.dim_ext->num_elts;
            }
        }
        total_size += size;
        field = field->fd.link;
    }
    sd->sd.size = total_size;
    sd->sd.link = saved_link;                   // restore saved link
    return( FALSE );
}


void    STUnion( void ) {
//=================

// Start a union definition.

    sym_id      un;
    sym_id      field;

    // make sure that a STRUCTURE was defined
    // Consider:        UNION
    //                      MAP
    //                          INTEGER I
    //                      ENDMAP
    if( CurrStruct == NULL ) return;
    un = FMemAlloc( sizeof( funion ) );
    un->fd.typ = FT_UNION;
    un->fd.link = NULL;
    un->fd.xt.record = NULL;
    field = CurrStruct->sd.fl.sym_fields;
    if( field == NULL ) {
        CurrStruct->sd.fl.sym_fields = un;
    } else {
        while( field->fd.link != NULL ) {
            field = field->fd.link;
        }
        field->fd.link = un;
    }
}


void    STMap( void ) {
//===============

// Start a map definition.

    sym_id      map;
    sym_id      md;
    sym_id      field;

    // make sure that a STRUCTURE was defined
    // Consider:        MAP
    //                      INTEGER I
    if( CurrStruct == NULL ) return;
    field = CurrStruct->sd.fl.sym_fields;
    // make sure that a UNION was defined
    // Consider:      STRUCTURE /STRUCT/
    //                    MAP
    //                        INTEGER I
    if( field == NULL ) return;
    md = FMemAlloc( sizeof( fmap ) );
    md->sd.link = NULL;
    md->sd.fl.fields = NULL;
    md->sd.size = 0;
    while( field->fd.link != NULL ) {
        field = field->fd.link;
    }
    map = field->fd.xt.sym_record;
    if( map == NULL ) {
        field->fd.xt.sym_record = md;
    } else {
        while( map->sd.link != NULL ) {
            map = map->sd.link;
        }
        map->sd.link = md;
    }
}
