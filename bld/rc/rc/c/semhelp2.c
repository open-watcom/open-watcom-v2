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
#include "errors.h"
#include "semantic.h"
#include "semantc2.h"
#include "rcrtns.h"
#include "reserr.h"
#include "clibext.h"
#include "rccore.h"


static bool ResOS2WriteHelpEntry( HelpTableEntryOS2 *currentry, WResFileID handle )
/**************************************************************************/
{
    if( RCWRITE( handle, currentry, sizeof( HelpTableEntryOS2 ) ) != sizeof( HelpTableEntryOS2 ) ) {
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
    entry.entry.Dummy      = -1;
    entry.entry.ExtendedId = extId;
    entry.prev = entry.next = NULL;

    return( entry );
}

FullHelpTableOS2 *SemOS2NewHelpTable( FullHelpEntryOS2 firstentry )
/***************************************************************/
{
    FullHelpTableOS2   *newtable;
    FullHelpEntryOS2   *newentry;

    newtable = RCALLOC( sizeof( FullHelpTableOS2 ) );
    newentry = RCALLOC( sizeof( FullHelpEntryOS2 ) );

    if( newtable == NULL || newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newentry = firstentry;
    newtable->head = NULL;
    newtable->tail = NULL;

    ResAddLLItemAtEnd( (void **) &(newtable->head), (void **) &(newtable->tail), newentry );

    return( newtable );
}

FullHelpTableOS2 *SemOS2AddHelpItem( FullHelpEntryOS2 currentry,
                                    FullHelpTableOS2 * currtable )
/****************************************************************/
{
    FullHelpEntryOS2     *newentry;

    newentry = RCALLOC( sizeof(FullHelpEntryOS2) );

    if( newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    *newentry = currentry;

    ResAddLLItemAtEnd( (void **) &(currtable->head), (void **) &(currtable->tail), newentry );

    return( currtable );
}

static void SemOS2FreeHelpTable( FullHelpTableOS2 *helptable )
/************************************************************/
{
    FullHelpEntryOS2   *currentry;
    FullHelpEntryOS2   *oldentry;

    if( helptable != NULL ) {
        currentry = helptable->head;
        while( currentry != NULL ) {
            oldentry = currentry;
            currentry = currentry->next;
            RCFREE( oldentry );
        }
        RCFREE( helptable );
    }
}

static bool SemOS2WriteHelpTableEntries( FullHelpTableOS2 * helptable,
                                        WResFileID handle )
/*********************************************************************/
{
    FullHelpEntryOS2    *currentry;
    bool                error;
    uint_16             tmp = 0;

    error = false;
    if( helptable != NULL ) {
        currentry = helptable->head;
        while( currentry != NULL && !error ) {
            error = ResOS2WriteHelpEntry( &currentry->entry, handle );
            currentry = currentry->next;
        }
    }
    if( !error )
        error = ResWriteUint16( &tmp, handle ); // Closing zero

    return( error );
}

void SemOS2WriteHelpTable( WResID * name, ResMemFlags flags,
                               FullHelpTableOS2 * helptable )
/***********************************************************/
{
    ResLocation     loc;
    bool            error;
    int             err_code;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        error = SemOS2WriteHelpTableEntries( helptable, CurrResFile.handle );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( OS2_RT_HELPTABLE ), flags, loc );
    } else {
        RCFREE( name );
    }

    SemOS2FreeHelpTable( helptable );
    return;

OutputWriteError:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
    ErrorHasOccured = true;
    SemOS2FreeHelpTable( helptable );
    return;

}


FullHelpSubTableOS2 *SemOS2NewHelpSubTable( DataElemList * data )
/***************************************************************/
{
    FullHelpSubTableOS2   *newtable;
    FullHelpSubEntryOS2   *newentry;

    newtable = RCALLOC( sizeof( FullHelpSubTableOS2 ) );
    newentry = RCALLOC( sizeof( FullHelpSubEntryOS2 ) );

    if( newtable == NULL || newentry == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = true;
        return( NULL );
    }

    newentry->dataListHead = data;
    newtable->head = NULL;
    newtable->tail = NULL;

    ResAddLLItemAtEnd( (void **) &(newtable->head), (void **) &(newtable->tail), newentry );

    return( newtable );
}

FullHelpSubTableOS2 *SemOS2AddHelpSubItem( DataElemList * data,
                            FullHelpSubTableOS2 * currtable )
/*************************************************************/
{
    FullHelpSubEntryOS2     *newentry;

    newentry = RCALLOC( sizeof( FullHelpSubEntryOS2 ) );

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
    FullHelpSubEntryOS2   *oldentry;

    if( helptable != NULL ) {
        currentry = helptable->head;
        while( currentry != NULL ) {
            SemFreeDataElemList( currentry->dataListHead );
            oldentry = currentry;
            currentry = currentry->next;
            RCFREE( oldentry );
        }
        RCFREE( helptable );
    }
}

static bool SemOS2WriteHelpData( DataElemList *list, WResFileID handle, int count )
/************************************************************************************/
{
    uint_16           data;
    bool              error;
    int               i;

    error = false;
    if( list->count > count ) {
        //TODO: output warning
    } else if( list->count < count ) {
        // error
        return( true );
    }
    for( i = 0; i < count; i++ ) {
        data = (uint_16)list->data[i].Item.Num;
        error = ResWriteUint16( &data, handle );
    }
    return( error );
}

static bool SemOS2WriteHelpSubTableEntries( FullHelpSubTableOS2 *helptable,
                                           WResFileID handle )
/************************************************************************/
{
    FullHelpSubEntryOS2     *currentry = NULL;
    bool                    error;
    uint_16                 tmp = 2;

    if( helptable != NULL ) {
        currentry = helptable->head;
        tmp = helptable->numWords;
    }
    error = ResWriteUint16( &tmp, handle );
    while( currentry != NULL && !error ) {
        error = SemOS2WriteHelpData( currentry->dataListHead, handle,
                                     helptable->numWords );
        currentry = currentry->next;
    }
    if( !error ) {
        tmp = 0;
        error = ResWriteUint16( &tmp, handle ); // Closing zero
    }

    return( error );
}

void SemOS2WriteHelpSubTable( WResID * name, int numWords,
                                     ResMemFlags flags,
                                     FullHelpSubTableOS2 * helptable )
/********************************************************************/
{
    ResLocation     loc;
    bool            error;
    int             err_code;

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();
        if( helptable != NULL ) {
            helptable->numWords = numWords;
        }
        error = SemOS2WriteHelpSubTableEntries( helptable, CurrResFile.handle );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( OS2_RT_HELPSUBTABLE ), flags, loc );
    } else {
        RCFREE( name );
    }

    SemOS2FreeHelpSubTable( helptable );
    return;

OutputWriteError:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
    ErrorHasOccured = true;
    SemOS2FreeHelpSubTable( helptable );
    return;

}

