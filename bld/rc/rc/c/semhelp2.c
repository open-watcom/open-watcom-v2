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
* Description:  OS/2 Help table related semantic actions.
*
****************************************************************************/


#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantc2.h"
#include "rcrtns.h"
#include "reserr.h"
#include "rccore.h"


static bool ResOS2WriteHelpEntry( HelpTableEntryOS2 *currentry, FILE *fp )
/************************************************************************/
{
    if( RESWRITE( fp, currentry, sizeof( HelpTableEntryOS2 ) ) != sizeof( HelpTableEntryOS2 ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    }
    return( false );
}

FullHelpEntryOS2 SemOS2MakeHelpItem( unsigned long winId, unsigned long subId,
                                     unsigned long extId )
/****************************************************************************/
{
    FullHelpEntryOS2      entry;

    entry.entry.WindowId   = winId;
    entry.entry.SubtableId = subId;
    entry.entry.Dummy      = (uint_16)-1;
    entry.entry.ExtendedId = extId;
    entry.prev = entry.next = NULL;

    return( entry );
}

FullHelpTableOS2 *SemOS2NewHelpTable( FullHelpEntryOS2 firstentry )
/***************************************************************/
{
    FullHelpTableOS2   *newtable;
    FullHelpEntryOS2   *newentry;

    newtable = RESALLOC( sizeof( FullHelpTableOS2 ) );
    newentry = RESALLOC( sizeof( FullHelpEntryOS2 ) );

    if( newtable == NULL || newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newentry = firstentry;
    newtable->head = NULL;
    newtable->tail = NULL;

    ResAddLLItemAtEnd( (void **)&(newtable->head), (void **)&(newtable->tail), newentry );

    return( newtable );
}

FullHelpTableOS2 *SemOS2AddHelpItem( FullHelpEntryOS2 currentry,
                                    FullHelpTableOS2 *currtable )
/***************************************************************/
{
    FullHelpEntryOS2     *newentry;

    newentry = RESALLOC( sizeof( FullHelpEntryOS2 ) );

    if( newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newentry = currentry;

    ResAddLLItemAtEnd( (void **)&(currtable->head), (void **)&(currtable->tail), newentry );

    return( currtable );
}

static void SemOS2FreeHelpTable( FullHelpTableOS2 *helptable )
/************************************************************/
{
    FullHelpEntryOS2   *currentry;
    FullHelpEntryOS2   *nextentry;

    if( helptable != NULL ) {
        for( currentry = helptable->head; currentry != NULL; currentry = nextentry ) {
            nextentry = currentry->next;
            RESFREE( currentry );
        }
        RESFREE( helptable );
    }
}

static bool SemOS2WriteHelpTableEntries( FullHelpTableOS2 *helptable, FILE *fp )
/******************************************************************************/
{
    FullHelpEntryOS2    *currentry;
    bool                error;

    error = false;
    if( helptable != NULL ) {
        for( currentry = helptable->head; currentry != NULL && !error; currentry = currentry->next ) {
            error = ResOS2WriteHelpEntry( &currentry->entry, fp );
        }
    }
    if( !error )
        error = ResWriteUint16( 0, fp ); // Closing zero

    return( error );
}

void SemOS2WriteHelpTable( WResID *name, ResMemFlags flags,
                               FullHelpTableOS2 *helptable )
/**********************************************************/
{
    ResLocation     loc;
    bool            error;
    int             err_code;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        error = SemOS2WriteHelpTableEntries( helptable, CurrResFile.fp );
        if( error ) {
            err_code = LastWresErr();
            RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
            ErrorHasOccured = true;
        } else {
            loc.len = SemEndResource( loc.start );
            SemAddResourceFree( name, WResIDFromNum( OS2_RT_HELPTABLE ), flags, loc );
        }
    } else {
        RESFREE( name );
    }
    SemOS2FreeHelpTable( helptable );
}


FullHelpSubTableOS2 *SemOS2NewHelpSubTable( DataElemList *data )
/**************************************************************/
{
    FullHelpSubTableOS2   *newtable;
    FullHelpSubEntryOS2   *newentry;

    newtable = RESALLOC( sizeof( FullHelpSubTableOS2 ) );
    newentry = RESALLOC( sizeof( FullHelpSubEntryOS2 ) );

    if( newtable == NULL || newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    newentry->dataListHead = data;
    newtable->head = NULL;
    newtable->tail = NULL;

    ResAddLLItemAtEnd( (void **)&(newtable->head), (void **)&(newtable->tail), newentry );

    return( newtable );
}

FullHelpSubTableOS2 *SemOS2AddHelpSubItem( DataElemList *data,
                            FullHelpSubTableOS2 *currtable )
/************************************************************/
{
    FullHelpSubEntryOS2     *newentry;

    newentry = RESALLOC( sizeof( FullHelpSubEntryOS2 ) );

    if( newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    newentry->dataListHead = data;

    ResAddLLItemAtEnd( (void **)&(currtable->head), (void **)&(currtable->tail), newentry );

    return( currtable );
}

static void SemOS2FreeHelpSubTable( FullHelpSubTableOS2 *helptable )
/******************************************************************/
{
    FullHelpSubEntryOS2   *currentry;
    FullHelpSubEntryOS2   *nextentry;

    if( helptable != NULL ) {
        for( currentry = helptable->head; currentry != NULL; currentry = nextentry ) {
            nextentry = currentry->next;
            SemFreeDataElemList( currentry->dataListHead );
            RESFREE( currentry );
        }
        RESFREE( helptable );
    }
}

static bool SemOS2WriteHelpData( DataElemList *list, FILE *fp, unsigned count )
/*****************************************************************************/
{
    bool              error;
    unsigned          i;

    error = false;
    if( list->count > count ) {
        //TODO: output warning
    } else if( list->count < count ) {
        // error
        return( true );
    }
    for( i = 0; i < count; i++ ) {
        error = ResWriteUint16( list->data[i].Item.Num, fp );
    }
    return( error );
}

static bool SemOS2WriteHelpSubTableEntries( FullHelpSubTableOS2 *helptable, FILE *fp )
/************************************************************************************/
{
    FullHelpSubEntryOS2     *currentry = NULL;
    bool                    error;
    uint_16                 tmp = 2;

    if( helptable != NULL ) {
        currentry = helptable->head;
        tmp = helptable->numWords;
    }
    error = ResWriteUint16( tmp, fp );
    for( ; currentry != NULL && !error; currentry = currentry->next ) {
        error = SemOS2WriteHelpData( currentry->dataListHead, fp, helptable->numWords );
    }
    if( !error ) {
        error = ResWriteUint16( 0, fp ); // Closing zero
    }

    return( error );
}

void SemOS2WriteHelpSubTable( WResID *name, unsigned numWords,
                                     ResMemFlags flags,
                                     FullHelpSubTableOS2 *helptable )
/*******************************************************************/
{
    ResLocation     loc;
    bool            error;
    int             err_code;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        if( helptable != NULL ) {
            helptable->numWords = numWords;
        }
        error = SemOS2WriteHelpSubTableEntries( helptable, CurrResFile.fp );
        if( error ) {
            err_code = LastWresErr();
            RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
            ErrorHasOccured = true;
        } else {
            loc.len = SemEndResource( loc.start );
            SemAddResourceFree( name, WResIDFromNum( OS2_RT_HELPSUBTABLE ), flags, loc );
        }
    } else {
        RESFREE( name );
    }
    SemOS2FreeHelpSubTable( helptable );
}
