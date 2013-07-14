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


#include "plusplus.h"
#include "memmgr.h"
#include "cgdata.h"
#include "pragdefn.h"
#include "pcheader.h"
#include "pdefn2.h"
#include "preproc.h"
#include "carve.h"

#define PCH_FIRST_USER_INDEX    (PCH_FIRST_INDEX + MAX_BUILTIN_AUXINFO)

enum {
    RAUX_RAW    = 0x01,         // auxinfo doesn't exist
    RAUX_NULL   = 0x00
};

static AUX_INFO **infoTranslate;


hw_reg_set *AuxParmDup(         // DUPLICATE AUX PARMS
    hw_reg_set *parms )
{
    unsigned amt;
    hw_reg_set *c;
    hw_reg_set *p;

    if( parms == NULL ) {
        return( parms );
    }
    amt = sizeof( hw_reg_set );
    c = parms;
    for(;;) {
        if( HW_CEqual( *c, HW_EMPTY ) ) break;
        ++c;
        amt += sizeof( hw_reg_set );
    }
    p = CMemAlloc( amt );
    memcpy( p, parms, amt );
    return( p );
}

char *AuxObjnameDup(            // DUPLICATE AUX OBJNAME
    char *objname )
{
    if( objname == NULL ) {
        return( objname );
    }
    return( strsave( objname ) );
}

void freeAuxInfo( AUX_INFO *i ) // FREE ALL AUX INFO MEM
{
    if( !IsAuxParmsBuiltIn( i->parms ) ) {
        CMemFree( i->parms );
    }
    CMemFree( i->objname );
    CMemFree( i->code );
}

AUX_ENTRY *AuxLookup( const char *name )
{
    AUX_ENTRY *ent;

    for( ent = AuxList; ent; ent = ent->next ) {
        if( strcmp( ent->name, name ) == 0 ) break;
    }
    return( ent );
}

char *AuxRetrieve( AUX_INFO *pragma )
{
    AUX_ENTRY *ent;

    for( ent = AuxList; ent; ent = ent->next ) {
        if( ent->info == pragma ) {
            return( ent->name );
        }
    }
    return( NULL );
}

static void readAuxInfo( AUX_INFO *i )
{
    unsigned    len;

    PCHReadVar( *i );
    AsmSysPCHReadCode( i );
    len = PCHGetUInt( i->parms );
    if( len != 0 ) {
        i->parms = CMemAlloc( len );
        PCHRead( i->parms, len );
    }
    len = PCHGetUInt( i->objname );
    if( len != 0 ) {
        i->objname = CMemAlloc( len );
        PCHRead( i->objname, len );
    }
}

pch_status PCHReadPragmas( void )
{
    unsigned    size;
    unsigned    max_index;
    unsigned    entry_len;
    unsigned    index;
    AUX_ENTRY   *e;
    AUX_ENTRY   *f;
    AUX_ENTRY   *next_f;
    AUX_INFO    *info;

    for( f = AuxList; f != NULL; f = next_f ) {
        next_f = f->next;
        freeAuxInfo( f->info );
        CMemFree( f->info );
        CMemFree( f );
    }
    max_index = PCHReadUInt();
    size = max_index * sizeof( AUX_INFO * );
    infoTranslate = CMemAlloc( size );
    memset( infoTranslate, 0, size );
    // read all aux_info
    for( index = PCH_FIRST_INDEX; index < max_index; ++index ) {
        if( index < PCH_FIRST_USER_INDEX ) {
            info = BuiltinAuxInfo + index - PCH_FIRST_INDEX;
        } else {
            info = CMemAlloc( sizeof( AUX_INFO ) );
        }
        infoTranslate[index] = info;
        readAuxInfo( info );
    }
    // read aux entries
    AuxList = NULL;
    for( ; (entry_len = PCHReadUInt()) != 0; ) {
        e = CMemAlloc( offsetof( AUX_ENTRY, name ) + entry_len + 1 );
        PCHRead( e->name, entry_len + 1 );
        e->info = PragmaMapIndex( PCHSetUInt( PCHReadUInt() ) );
        e->next = AuxList;
        AuxList = e;
    }
    return( PCHCB_OK );
}

static void writeAuxInfo( AUX_INFO *info, unsigned index )
{
    hw_reg_set  *regs;
    hw_reg_set  *save_parms;
    char        *save_objname;
    unsigned    len;

    info->index = index;
    save_parms = info->parms;
    save_objname = info->objname;
    if( save_parms != NULL ) {
        len = 0;
        regs = save_parms;
        for(;;) {
            len += sizeof( hw_reg_set );
            if( HW_CEqual( *regs, HW_EMPTY ) ) break;
            ++regs;
        }
        info->parms = PCHSetUInt( len );
    }
    if( save_objname != NULL ) {
        info->objname = PCHSetUInt( strlen( save_objname ) + 1 );
    }
    PCHWriteVar( *info );
    AsmSysPCHWriteCode( info );
    if( save_parms != NULL ) {
        PCHWrite( save_parms, PCHGetUInt( info->parms ) );
    }
    if( save_objname != NULL ) {
        PCHWrite( save_objname, PCHGetUInt( info->objname ) );
    }
    info->parms = save_parms;
    info->objname = save_objname;
}

pch_status PCHWritePragmas( void )
{
    unsigned    index;
    unsigned    len;
    AUX_INFO    *info;
    AUX_ENTRY   *e;

    // get aux_info count
    for( e = AuxList; e != NULL; e = e->next ) {
        e->info->index = PCH_NULL_INDEX;
    }
    index = PCH_FIRST_USER_INDEX;
    for( e = AuxList; e != NULL; e = e->next ) {
        if( e->info->index == PCH_NULL_INDEX ) {
            e->info->index = index++;
        }
    }
    PCHWriteUInt( index );
    for( e = AuxList; e != NULL; e = e->next ) {
        e->info->index = PCH_NULL_INDEX;
    }
    // write built-in aux_info
    for( index = PCH_FIRST_INDEX; index < PCH_FIRST_USER_INDEX; ++index ) {
        info = BuiltinAuxInfo + index - PCH_FIRST_INDEX;
        writeAuxInfo( info, index );
    }
    // write user aux_info
    for( e = AuxList; e != NULL; e = e->next ) {
        info = e->info;
        if( info->index == PCH_NULL_INDEX ) {
            writeAuxInfo( info, index++ );
        }
    }
    // write aux entries
    for( e = AuxList; e != NULL; e = e->next ) {
        len = strlen( e->name );
        PCHWriteUInt( len );
        PCHWrite( e->name, len + 1 );
        PCHWriteUInt( PCHGetUInt( PragmaGetIndex( e->info ) ) );
    }
    PCHWriteUInt( 0 );
    return( PCHCB_OK );
}

AUX_INFO *PragmaGetIndex( AUX_INFO *i )
{
    if( i == NULL ) {
        return( (AUX_INFO *)PCH_NULL_INDEX );
    }
#ifndef NDEBUG
    if( i->index < PCH_FIRST_INDEX ) {
        CFatal( "aux info not assigned an index" );
    }
#endif
    return( PCHSetUInt( i->index ) );
}

AUX_INFO *PragmaMapIndex( AUX_INFO *i )
{
    if( PCHGetUInt( i ) < PCH_FIRST_INDEX ) {
        return( NULL );
    }
    return( infoTranslate[PCHGetUInt( i )] );
}

pch_status PCHInitPragmas( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniPragmas( boolean writing )
{
    if( !writing ) {
        CMemFreePtr( &infoTranslate );
    }
    return( PCHCB_OK );
}
