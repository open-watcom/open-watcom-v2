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
* Description:  Manage list of dynamic libraries.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dlglist.h"
#include "dbgsrc.h"

#include "clibext.h"

char_ring               *DLLList;

extern char             *GetLastImageName( void );
extern void             RecordEvent( const char *p );
extern void             GetBreakOnImageCmd( const char *, char *, bool );


bool DLLMatch( void )
{
    char_ring *curr;

    strcpy( TxtBuff, GetLastImageName() );
    strupr( TxtBuff );
    for( curr = DLLList; curr != NULL; curr = curr->next ) {
        if( curr->name[0] == '*' ) return( TRUE );
        if( strstr( TxtBuff, curr->name ) != NULL ) return( TRUE );
    }
    return( FALSE );
}

char *DLLListName( char_ring *src )
{
    return( src->name );
}

char_ring *NextDLLList( char_ring *curr )
{
    if( curr == NULL ) return( DLLList );
    return( curr->next );
}

void AddDLLList( const char *start, unsigned len )
{
    InsertRing( RingEnd( &DLLList ), start, len, TRUE );
}

void BreakOnImageLoad( const char *name, unsigned len, bool clear )
{
    if( clear ) {
        DeleteRing( &DLLList, name, len, TRUE );
    } else {
        InsertRing( RingEnd( &DLLList ), name, len, TRUE );
    }
    GetBreakOnImageCmd( name, TxtBuff, clear );
    RecordEvent( TxtBuff );
}

void InitDLLList( void )
{
    DLLList = NULL;
}

void FiniDLLList( void )
{
    FreeRing( DLLList );
    DLLList = NULL;
}

void RecordClearDLLBreaks( void )
{
    char_ring   *dll;

    for( dll = DLLList; dll != NULL; dll = dll->next ) {
        GetBreakOnImageCmd( dll->name, TxtBuff, TRUE );
        RecordEvent( TxtBuff );
    }
}

void RecordSetDLLBreaks( void )
{
    char_ring   *dll;

    for( dll = DLLList; dll != NULL; dll = dll->next ) {
        GetBreakOnImageCmd( dll->name, TxtBuff, FALSE );
        RecordEvent( TxtBuff );
    }
}
