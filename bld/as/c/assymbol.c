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


#include "as.h"

#define HASH_TABLE_SIZE         211

static sym_handle       hashTable[ HASH_TABLE_SIZE ];

typedef struct sym_reloc_handle *sym_reloc_hdl_list;

struct sym_reloc_handle {
    sym_handle          hdl;
    sym_reloc_hdl_list  prev;
    sym_reloc_hdl_list  next;
};

typedef struct {
    sym_reloc_hdl_list  head;
    sym_reloc_hdl_list  tail;
} sym_reloc_hdls;

// These lists link all the sym_handles that have relocs hanging
// off of them, so that we can check for unemitted relocs at the end.
static sym_reloc_hdls  hi_reloc_hdls = { NULL, NULL };
static sym_reloc_hdls  lo_reloc_hdls = { NULL, NULL };

static void hdl_append( sym_reloc_hdls *hdls, sym_handle sym ) {
//**************************************************************
// Append a sym_handle to the list of symbols that have relocs to them.

    sym_reloc_hdl_list  hdl;

    // make sure you only append symbols that have hi/lo relocs to them,
    // depending on what hdls is.
    hdl = MemAlloc( sizeof( struct sym_reloc_handle ) );
    hdl->hdl = sym;
    hdl->prev = NULL;
    hdl->next = NULL;
    if( !hdls->head ) {
        hdls->head = hdls->tail = hdl;
    } else {
        hdl->prev = hdls->tail;
        hdls->tail->next = hdl;
        hdls->tail = hdl;
    }
}

static void hdl_remove( sym_reloc_hdls *hdls, sym_handle sym ) {
//**************************************************************
// Getting rid of a symbol from the list since it doesn't have relocs to
// it any more.

    sym_reloc_hdl_list  hdl;

    // make sure you remove only when there aren't any hi/lo relocs to the
    // symbol, depending on what hdls is.
    hdl = hdls->head;
    while( hdl ) {
        if( hdl->hdl == sym ) {
            if( hdl->prev ) {
                hdl->prev->next = hdl->next;
            } else {
                hdls->head = hdl->next;
            }
            if( hdl->next ) {
                hdl->next->prev = hdl->prev;
            } else {
                hdls->tail = hdl->prev;
            }
            MemFree( hdl );
            return;
        }
        hdl = hdl->next;
    }
    assert( false );    // sym should be in the list!
}

static uint_32 symHash( const char *sym ) {
//*****************************************

    return( AsHashVal( sym, HASH_TABLE_SIZE ) );
}

static sym_handle symAlloc( const char *name ) {
//**********************************************
// Alloc a symbol for a given name and return it

    sym_handle  sym;

    sym = MemAlloc( sizeof( asm_symbol ) + strlen( name ) );
    strcpy( sym->name, name );
    return( sym );
}

static void symFree( sym_handle sym ) {
//*************************************
// Free up a symbol.

    // The 2 lists should be cleaned up already
    assert( sym->hi_relocs == NULL );
    assert( sym->lo_relocs == NULL );
    MemFree( sym );
}

extern void SymInit( void ) {
//***********************
// Init routine for Symbol Table library

}

extern sym_handle SymLookup( const char *sym_name ) {
//***************************************************
// Return a handle for a symbol with the given name, or
// NULL if we don't have anything which matches
// move to front would probably be useful here

    sym_handle  sym;

    sym = hashTable[ symHash( sym_name ) ];
    while( sym != NULL ) {
        if( !strcmp( sym_name, sym->name ) ) break;
        sym = sym->next;
    }
    return( sym );
}

extern sym_handle SymAdd( const char *sym_name, sym_class class ) {
//*****************************************************************
// Add a symbol with the given name and class to our symbol table.
// Other symbol attrs will have to be added individually.

    sym_handle  sym;
    sym_handle  *bucket;

    assert( SymLookup( sym_name ) == NULL );
    bucket = &hashTable[ symHash( sym_name ) ];
    sym = symAlloc( sym_name );
    sym->class = class;
    sym->flags = SF_EXTERN;
    sym->linkage = SL_UNKNOWN;
    sym->hi_relocs = sym->lo_relocs = NULL;
    sym->obj_hdl = NULL;
    sym->next = *bucket;
    *bucket = sym;
    return( sym );
}

#ifndef _STANDALONE_
extern bool SymLocationKnown( sym_handle sym ) {
//**********************************************
// Return true if we know where a given label is
// going to be located.

    assert( sym->class == SYM_LABEL );
    return( sym->flags & SF_LOC_KNOWN );
}

extern sym_handle SymSetLocation( sym_handle sym, sym_location loc ) {
//********************************************************************
// Mark the symbol given as residing at location x. The sym_handle is
// returned, as a matter of convenience.

    assert( sym->class == SYM_LABEL );
    sym->u.location = loc;
    sym->flags |= SF_LOC_KNOWN;
    return( sym );
}

extern sym_location SymGetLocation( sym_handle sym ) {
//****************************************************
// Return the location for a symbol - yes, this does return a
// struct - forgive me

    assert( sym->class == SYM_LABEL );
    return( sym->u.location );
}
#endif

#ifdef _STANDALONE_
extern bool SymIsSectionSymbol( sym_handle sym ) {
//************************************************

    return( sym->flags & SF_SECTION );
}

extern void SymSetSection( sym_handle sym ) {
//*******************************************
// Set the section flag and so SymIsSectionSymbol will know if the symbol is
// a valid section symbol.

    sym->flags |= SF_SECTION;
}
#endif

extern sym_handle SymSetLink( sym_handle sym, sym_link link ) {
//*************************************************************
// set the link field of the symbol - either the instruction or directive
// pointer for instructions and directives respectively

    assert( sym->class == SYM_INSTRUCTION || sym->class == SYM_DIRECTIVE );
    sym->u.link = link;
    return( sym );
}

extern sym_link SymGetLink( sym_handle sym ) {
//********************************************
// return the link field of the symbol - used for
// remembering symbol specific info for instructions and directives.

    assert( sym->class == SYM_INSTRUCTION || sym->class == SYM_DIRECTIVE );
    return( sym->u.link );
}

extern sym_class SymClass( sym_handle sym ) {
//*******************************************

    return( sym->class );
}

extern sym_linkage SymGetLinkage( sym_handle sym ) {
//**************************************************

    assert( sym->class == SYM_LABEL );
    return( sym->linkage );
}

extern sym_handle SymSetLinkage( sym_handle sym, sym_linkage linkage ) {
//**********************************************************************

    assert( sym->class == SYM_LABEL );
    sym->linkage = linkage;
    return( sym );
}

extern char *SymName( sym_handle sym ) {
//**************************************

    return( sym->name );
}

#ifdef _STANDALONE_
extern sym_obj_hdl SymObjHandle( sym_handle sym ) {
//*************************************************

    assert( sym->class == SYM_LABEL );
    if( sym->obj_hdl == NULL ) {
        sym->obj_hdl = ObjSymbolInit( sym->name );
    }
    return( sym->obj_hdl );
}
#endif

extern void SymStackReloc( bool is_high, sym_handle sym, sym_section section, sym_offset offset, bool named ) {
//*************************************************************************************************************

    sym_reloc           reloc;
    sym_reloc           *curr;
    sym_reloc_hdls      *hdls_ptr;

    assert( sym != NULL );
    assert( sym->class == SYM_LABEL );
    reloc = MemAlloc( sizeof( struct sym_reloc_entry ) );
    reloc->location.section = section;
    reloc->location.offset = offset;
    reloc->prev = reloc->next = NULL;
    reloc->named = named;
    if( is_high ) {
        curr = &sym->hi_relocs;
        hdls_ptr = &hi_reloc_hdls;
    } else {
        curr = &sym->lo_relocs;
        hdls_ptr = &lo_reloc_hdls;
    }
    if( *curr ) {
        (*curr)->prev = reloc;
        reloc->next = *curr;
    } else { // First element in the list; add sym to the hdls list
        hdl_append( hdls_ptr, sym );
    }
    *curr = reloc;
}

extern void SymDestroyReloc( sym_handle sym, sym_reloc reloc ) {
//**************************************************************

    assert( sym->class == SYM_LABEL );
    assert( reloc );
    if( reloc->prev ) {
        reloc->prev->next = reloc->next;
    } else {    // the head element
        if( sym->hi_relocs == reloc ) {
            sym->hi_relocs = reloc->next;
            if( sym->hi_relocs == NULL ) { // list empty; remove hdl from hdls
                hdl_remove( &hi_reloc_hdls, sym );
            }
        } else {
            assert( sym->lo_relocs == reloc );
            sym->lo_relocs = reloc->next;
            if( sym->lo_relocs == NULL ) { // list empty; remove hdl from hdls
                hdl_remove( &lo_reloc_hdls, sym );
            }
        }
    }
    if( reloc->next ) {
        reloc->next->prev = reloc->prev;
    }
    MemFree( reloc );
}

extern sym_reloc SymMatchReloc( bool is_high, sym_handle sym, sym_section section ) {
//***********************************************************************************

    sym_reloc           curr;

    assert( sym->class == SYM_LABEL );
    curr = ( is_high ? sym->hi_relocs : sym->lo_relocs );
    while( curr ) {
        if( curr->location.section == section ) {    // want same section handle
            return( curr );
        }
        curr = curr->next;
    }
    return( curr );
}

extern sym_reloc SymGetReloc( bool is_high, sym_handle *get_hdl ) {
//*****************************************************************
// This function returns a sym_reloc from the list of handle with relocs if one
// is available, and returns NULL otherwise. Caller should call SymDestroyReloc
// to free the reloc after its use.

    sym_reloc_hdl_list  lhdl;

    if( is_high ) {     // getting a high reloc
        if( ( lhdl = hi_reloc_hdls.head ) == NULL ) return( NULL );
        assert( lhdl->hdl->hi_relocs != NULL );
        *get_hdl = lhdl->hdl;
        return( lhdl->hdl->hi_relocs );
    }
    // getting a low reloc
    if( ( lhdl = lo_reloc_hdls.head ) == NULL ) return( NULL );
    assert( lhdl->hdl->lo_relocs != NULL );
    *get_hdl = lhdl->hdl;
    return( lhdl->hdl->lo_relocs );
}

#ifdef AS_DEBUG_DUMP
extern bool SymRelocIsClean( bool is_clean ) {
//********************************************
// Just a debug tool to see whether I've cleaned up.

    static bool isClean = false;
    bool        prev;

    prev = isClean;
    isClean = is_clean;
    return( prev );
}
#endif

extern void SymFini( void ) {
//*********************
// Fini routine for Symbol Table library

    sym_handle  sym;
    sym_handle  next;
    int         i;
    int         n;
    sym_reloc   reloc;

#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_SYMBOL_TABLE ) ) DumpSymbolTable();
    #endif
#endif

    reloc = SymGetReloc( true, &sym );
    while( reloc != NULL ) {
#ifdef AS_DEBUG_DUMP
        assert( !SymRelocIsClean( false ) );
#endif
        SymDestroyReloc( sym, reloc );
        reloc = SymGetReloc( true, &sym );
    }
    reloc = SymGetReloc( false, &sym );
    while( reloc != NULL ) {
#ifdef AS_DEBUG_DUMP
        assert( !SymRelocIsClean( false ) );
#endif
        SymDestroyReloc( sym, reloc );
        reloc = SymGetReloc( false, &sym );
    }
    assert( hi_reloc_hdls.head == NULL && hi_reloc_hdls.tail == NULL );
    assert( lo_reloc_hdls.head == NULL && lo_reloc_hdls.tail == NULL );
    n = sizeof( hashTable ) / sizeof( hashTable[ 0 ] );
    for( i = 0; i < n; i++ ) {
        for( sym = hashTable[ i ]; sym != NULL; sym = next ) {
            next = sym->next;
            symFree( sym );
        }
        hashTable[ i ] = 0;
    }
}

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP

static char classChar[] = { 'I', 'D', 'L' };

extern void DumpSymbol( sym_handle sym ) {
//****************************************

    printf( "%s(addr=%x; next=%x; class = %c; ", sym->name, sym, sym->next, classChar[ sym->class ] );
    if( sym->class == SYM_LABEL ) {
        printf( "location = %x:%x )", sym->u.location.section, sym->u.location.offset );
    } else {
        printf( "link = %x )", sym->u.link );
    }
}

extern void DumpSymbolTable( void ) {
//*****************************

    int         i;
    sym_handle  curr;

    for( i = 0; i < HASH_TABLE_SIZE; i++ ) {
        curr = hashTable[ i ];
        if( curr == NULL ) continue;
        printf( "Bucket %d: ", i );
        while( curr != NULL ) {
            DumpSymbol( curr );
            printf( " " );
            curr = curr->next;
        }
        printf( "\n" );
    }
}
#endif
#endif
