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


#include <string.h>

#include "plusplus.h"
#include "memmgr.h"
#include "cgdata.h"
#include "pragdefn.h"
#include "errdefns.h"
#include "pcheader.h"
#include "pdefn2.h"
#include "preproc.h"
#include "carve.h"

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

static void freeAuxInfo( AUX_INFO *i )
{
    if( i->parms != DefaultParms ) {
        CMemFree( i->parms );
    }
    CMemFree( i->objname );
    CMemFree( i->code );
}

void AuxCopy(                   // COPY AUX STRUCTURE
    AUX_INFO *to,               // - destination
    AUX_INFO *from )            // - source
{
    freeAuxInfo( to );
    *to = *from;
    to->parms = AuxParmDup( from->parms );
    to->objname = AuxObjnameDup( from->objname );
    to->code = AuxCodeDup( from->code );
}

AUX_ENTRY *AuxLookup( char *name )
{
    AUX_ENTRY *ent;

    for( ent = AuxList; ent; ent = ent->next ) {
        if( strcmp( ent->name, name ) == 0 ) break;
    }
    return( ent );
}

char *AuxRetrieve( void *pragma )
{
    AUX_ENTRY *ent;

    for( ent = AuxList; ent; ent = ent->next ) {
        if( ent->info == pragma ) {
            return( ent->name );
        }
    }
    return( NULL );
}

static void readAuxInfo( AUX_INFO *i, unsigned control )
{
    size_t parms_size;
    size_t objname_size;

    if(( control & RAUX_RAW ) == 0 ) {
        freeAuxInfo( i );
    }
    PCHRead( i, sizeof( *i ) );
    AsmSysPCHReadCode( i );
    parms_size = (size_t) i->parms;
    if( parms_size != 0 ) {
        i->parms = CMemAlloc( parms_size );
        PCHRead( i->parms, parms_size );
    }
    objname_size = (size_t) i->objname;
    if( objname_size != 0 ) {
        i->objname = CMemAlloc( objname_size );
        PCHRead( i->objname, objname_size );
    }
}

pch_status PCHReadPragmas( void )
{
    size_t xlat_size;
    size_t entry_len;
    unsigned info_index;
    AUX_ENTRY *e;
    AUX_ENTRY *f;
    AUX_ENTRY *next_f;
    AUX_INFO *i;

    for( f = AuxList; f != NULL; f = next_f ) {
        next_f = f->next;
        freeAuxInfo( f->info );
        CMemFree( f->info );
        CMemFree( f );
    }
    AuxList = NULL;
    readAuxInfo( &DefaultInfo, RAUX_NULL );
    readAuxInfo( &CdeclInfo, RAUX_NULL );
    readAuxInfo( &PascalInfo, RAUX_NULL );
    readAuxInfo( &FortranInfo, RAUX_NULL );
    readAuxInfo( &SyscallInfo, RAUX_NULL );
    readAuxInfo( &OptlinkInfo, RAUX_NULL );
    readAuxInfo( &StdcallInfo, RAUX_NULL );
#ifdef __OLD_STDCALL
    readAuxInfo( &OldStdcallInfo, RAUX_NULL );
#endif
    readAuxInfo( &Far16CdeclInfo, RAUX_NULL );
    readAuxInfo( &Far16PascalInfo, RAUX_NULL );
    for(;;) {
        entry_len = PCHReadUInt();
        if( entry_len == 0 ) break;
        e = CMemAlloc( sizeof( AUX_ENTRY ) + entry_len );
        PCHRead( e->name, entry_len + 1 );
        info_index = PCHReadCVIndex();
        if( info_index == 0 ) {
            i = CMemAlloc( sizeof( AUX_INFO ) );
            e->info = i;
            readAuxInfo( i, RAUX_RAW );
        } else {
            e->info = PragmaMapIndex( (void *) info_index );
        }
        // must be after PragmaMapIndex
        e->next = AuxList;
        AuxList = e;
    }
    info_index = PCHReadCVIndex();
    xlat_size = info_index * sizeof( AUX_INFO * );
    infoTranslate = CMemAlloc( xlat_size );
    memset( infoTranslate, 0, xlat_size );
    for( e = AuxList; e != NULL; e = e->next ) {
        infoTranslate[ e->info->index ] = e->info;
    }
    infoTranslate[ DefaultInfo.index ] = &DefaultInfo;
    infoTranslate[ CdeclInfo.index ] = &CdeclInfo;
    infoTranslate[ PascalInfo.index ] = &PascalInfo;
    infoTranslate[ FortranInfo.index ] = &FortranInfo;
    infoTranslate[ SyscallInfo.index ] = &SyscallInfo;
    infoTranslate[ OptlinkInfo.index ] = &OptlinkInfo;
    infoTranslate[ StdcallInfo.index ] = &StdcallInfo;
#ifdef __OLD_STDCALL
    infoTranslate[ OldStdcallInfo.index ] = &OldStdcallInfo;
#endif
    infoTranslate[ Far16CdeclInfo.index ] = &Far16CdeclInfo;
    infoTranslate[ Far16PascalInfo.index ] = &Far16PascalInfo;
    return( PCHCB_OK );
}

static void writeAuxInfo( AUX_INFO *info, cv_index *index )
{
    size_t parms_size;
    size_t objname_size;
    hw_reg_set *regs;
    hw_reg_set *save_parms;
    char *save_objname;

    info->index = (*index)++;
    save_parms = info->parms;
    save_objname = info->objname;
    parms_size = 0;
    if( save_parms != NULL ) {
        regs = save_parms;
        for(;;) {
            parms_size += sizeof( hw_reg_set );
            if( HW_CEqual( *regs, HW_EMPTY ) ) break;
            ++regs;
        }
        info->parms = (void *) parms_size;
    }
    objname_size = 0;
    if( save_objname != NULL ) {
        objname_size = strlen( save_objname ) + 1;
        info->objname = (void *) objname_size;
    }
    PCHWrite( info, sizeof( *info ) );
    AsmSysPCHWriteCode( info );
    if( parms_size != 0 ) {
        PCHWrite( save_parms, parms_size );
    }
    if( objname_size != 0 ) {
        PCHWrite( save_objname, objname_size );
    }
    info->parms = save_parms;
    info->objname = save_objname;
}

pch_status PCHWritePragmas( void )
{
    cv_index index;
    cv_index write_index;
    size_t len;
    AUX_INFO *info;
    AUX_ENTRY *e;

    for( e = AuxList; e != NULL; e = e->next ) {
        e->info->index = PCH_NULL_INDEX;
    }
    index = PCH_FIRST_INDEX;
    writeAuxInfo( &DefaultInfo, &index );
    writeAuxInfo( &CdeclInfo, &index );
    writeAuxInfo( &PascalInfo, &index );
    writeAuxInfo( &FortranInfo, &index );
    writeAuxInfo( &SyscallInfo, &index );
    writeAuxInfo( &OptlinkInfo, &index );
    writeAuxInfo( &StdcallInfo, &index );
#ifdef __OLD_STDCALL
    writeAuxInfo( &OldStdcallInfo, &index );
#endif
    writeAuxInfo( &Far16CdeclInfo, &index );
    writeAuxInfo( &Far16PascalInfo, &index );
    for( e = AuxList; e != NULL; e = e->next ) {
        len = strlen( e->name );
        PCHWriteUInt( len );
        PCHWrite( e->name, len + 1 );
        info = e->info;
        write_index = info->index;
        PCHWriteCVIndex( write_index );
        if( write_index == PCH_NULL_INDEX ) {
            writeAuxInfo( info, &index );
        }
    }
    len = 0;
    PCHWriteUInt( len );
    PCHWriteCVIndex( index );
    return( PCHCB_OK );
}

void *PragmaGetIndex( void *e )
{
    AUX_INFO *i = e;

    if( i == NULL ) {
        return( PCH_NULL_INDEX );
    }
#ifndef NDEBUG
    if( i->index < PCH_FIRST_INDEX ) {
        CFatal( "aux info not assigned an index" );
    }
#endif
    return (void *) i->index;
}

void *PragmaMapIndex( void *pi )
{
    unsigned i = (unsigned) pi;
    AUX_ENTRY *e;
    AUX_INFO *mapped_info;

    if( i == PCH_NULL_INDEX ) {
        return( NULL );
    }
    mapped_info = NULL;
    if( infoTranslate != NULL ) {
        mapped_info = infoTranslate[ i ];
    } else {
        for( e = AuxList; e != NULL; e = e->next ) {
            if( e->info->index == i ) {
                return( e->info );
            }
        }
        if( i == DefaultInfo.index ) {
            return( &DefaultInfo );
        }
        if( i == CdeclInfo.index ) {
            return( &CdeclInfo );
        }
        if( i == PascalInfo.index ) {
            return( &PascalInfo );
        }
        if( i == FortranInfo.index ) {
            return( &FortranInfo );
        }
        if( i == SyscallInfo.index ) {
            return( &SyscallInfo );
        }
        if( i == OptlinkInfo.index ) {
            return( &OptlinkInfo );
        }
        if( i == StdcallInfo.index ) {
            return( &StdcallInfo );
        }
#ifdef __OLD_STDCALL
        if( i == OldStdcallInfo.index ) {
            return( &OldStdcallInfo );
        }
#endif
        if( i == Far16CdeclInfo.index ) {
            return( &Far16CdeclInfo );
        }
        if( i == Far16PascalInfo.index ) {
            return( &Far16PascalInfo );
        }
#ifndef NDEBUG
        CFatal( "invalid index passed to PragmaMapIndex" );
#endif
    }
    return( mapped_info );
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
