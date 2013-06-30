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
* Description:  Spy Z-order functions.
*
****************************************************************************/


#include "spy.h"

typedef struct winarea {
    struct winarea      *next;
    HWND                hwnd;
    RECT                area;
    struct winarea      *child;     /* not used if this is a child window */
} WinArea;

static WinArea          *AreaList;

static HWND SearchChildren( WinArea *cur, POINT *pt )
{
    WinArea     *tmp;

    tmp = cur->child;
    while( tmp != NULL ) {
        if( PtInRect( &tmp->area, *pt ) ) {
            return( SearchChildren( tmp, pt ) );
        }
        tmp = tmp->next;
    }
    return( cur->hwnd );
}

HWND GetHwndFromPt( POINT *pt )
{
    WinArea     *cur;

    cur = AreaList;
    while( cur != NULL ) {
        if( PtInRect( &cur->area, *pt ) ) {
            return( SearchChildren( cur, pt ) );
        }
        cur = cur->next;
    }
    return( GetDesktopWindow() );
}

static void FreeNode( WinArea *cur )
{
    if( cur == NULL ) {
        return;
    }
    FreeNode( cur->child );
    FreeNode( cur->next );
    MemFree( cur );
}

static void FreeList( WinArea *cur )
{
    FreeNode( cur );
}


static void AddChildren( WinArea *parent )
{
    HWND        child;
    WinArea     *ptr;

    child = GetWindow( parent->hwnd, GW_CHILD );
    if( child == NULL ) {
        return;
    }
    child = GetWindow( child, GW_HWNDLAST );
    while( child != NULL ) {
        if( IsWindowVisible( child ) ) {
            ptr = MemAlloc( sizeof( WinArea ) );
            ptr->next = parent->child;
            ptr->hwnd = child;
            ptr->child = NULL;
            GetWindowRect( child, &ptr->area );
            parent->child = ptr;
            AddChildren( ptr );
        }
        child = GetWindow( child, GW_HWNDPREV );
    }
}

static void AddWindows( HWND curwin )
{
    WinArea     *ptr;

    while( curwin != NULL ) {
        if( IsWindowVisible( curwin ) ) {
            ptr = MemAlloc( sizeof( WinArea ) );
            ptr->next = AreaList;
            ptr->hwnd = curwin;
            ptr->child = NULL;
            GetWindowRect( curwin, &ptr->area );
            AreaList = ptr;
            if( !IsIconic( curwin ) ) {
                AddChildren( ptr );
            }
        }
        curwin = GetWindow( curwin, GW_HWNDPREV );
    }
}

/*
 * RemoveWindow - remove a top level window  and all its children
 *                from the list
 */
void RemoveWindow( HWND hwnd )
{
    WinArea     **cur;
    WinArea     *tmp;

    cur = &AreaList;
    /*
     * one window may appear more than once in the list
     * ( I don't understand how but it can )
     */
    for( ;; ) {
        while( *cur != NULL ) {
            if( (*cur)->hwnd == hwnd ) {
                break;
            }
            cur = &(*cur)->next;
        }
        if( *cur == NULL ) {
            return;
        }
        tmp = *cur;
        *cur = tmp->next;
        tmp->next = NULL;
        FreeNode( tmp );
    }
}

void IdentifyWindows( HWND toplevel, HWND topmost )
{
    HWND        curwin;

    FreeList( AreaList );
    AreaList = NULL;
    curwin = GetWindow( toplevel, GW_HWNDLAST );
    AddWindows( curwin );
    curwin = GetWindow( topmost, GW_HWNDLAST );
    AddWindows( curwin );
}

