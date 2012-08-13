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
* Description:  Handle cut/copy/paste clipboard actions.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "fmedit.def"
#include "object.def"
#include "global.h"
#include "state.def"
#include "dlist.def"
#include "fmerror.def"

#include "clip.h"

static CLIP_INFO     ClipBoard;

#ifdef __NT__
    #define GET_CURRENT_TASK GetCurrentProcess
#else
    #define GET_CURRENT_TASK GetCurrentTask
#endif

extern void InitClipboard( void )
/*******************************/
{
    memset( &ClipBoard, 0, sizeof( CLIP_INFO ) );
}


static void FreeClipboard( void )
/*******************************/
{
    DLIST       *objlist;
    DLIST_ELT   elt;

    for( objlist = ClipBoard.objs; objlist != NULL; objlist = DListConsume( objlist ) ) {
        elt = DListElement( objlist );
        Destroy( elt.copy, FALSE );
    }
    InitClipboard();
}


void WINEXP FMResetClipboard( void )
/**********************************/
{
    /* Reset the list of copy objects */
    if( ClipBoard.task == GET_CURRENT_TASK() ) {
        FreeClipboard();
    }
}

void WINEXP FMNewClipboard( void )
/********************************/
{
    FreeClipboard();
    ClipBoard.task = GET_CURRENT_TASK();
}


extern void *GetClipList( void )
/******************************/
{
    return( ClipBoard.objs );
}


extern void *NextClipList( DLIST * clist )
/****************************************/
{
    return( DListNext( clist ) );
}


extern OBJPTR GetClipObject( DLIST * clist )
/******************************************/
{
    DLIST_ELT elt;

    elt = DListElement( clist );
    return( elt.copy );
}

static DLIST *FindInsertPoint( OBJPTR original )
/**********************************************/
{
    /* order the clipboard list so that higher level objects are added before
     * lower level ones
     */
    int         priority;
    int         currpr;
    DLIST       *lst;
    DLIST       *prev;
    DLIST_ELT   elt;

    prev = NULL;
    GetPriority( original, &priority );
    for( lst = ClipBoard.objs; lst != NULL; lst = DListNext( lst ) ) {
        elt = DListElement( lst );
        GetPriority( elt.copy, &currpr );
        if( currpr < priority ) {
            break;
        }
        prev = lst;
    }
    return( prev );
}


void WINEXP FMAddClipboard( OBJPTR original, OBJPTR copy )
/********************************************************/
{
    /* Add obj to the list of copy objects */
    DLIST_ELT   elt;
    DLIST       *last;

    if( !FMClipObjExists( original ) ) {
        last = FindInsertPoint( original );
        elt.original = original;
        elt.copy = copy;
        if( last != NULL ) {
            DListInsertElt( last, elt );
        } else {
            DListAddElt( &ClipBoard.objs, elt );
        }
    }
}


BOOL WINEXP FMClipObjExists( OBJPTR obj )
/***************************************/
{
    /* See if obj is already in the list of copy objects */
    DLIST_ELT elt;

    elt.copy = NULL;
    elt.original = obj;
    if( DListFindElt( ClipBoard.objs, elt ) != NULL ) {
        return( TRUE );
    } else {
        elt.copy = obj;
        elt.original = NULL;
        return( DListFindElt( ClipBoard.objs, elt ) != NULL );
    }
}


BOOL WINEXP FMPasteValid( void )
/******************************/
{
    return( ClipBoard.task == GET_CURRENT_TASK() );
}
