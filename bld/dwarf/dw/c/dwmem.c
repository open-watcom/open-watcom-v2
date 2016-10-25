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


#include "dwpriv.h"
#include "dwmem.h"

char *StrDup(
    dw_client                   cli,
    const char *                str )
{
    size_t                      len;

    _Validate( cli != NULL && str != NULL );

    len = strlen( str ) + 1;
    return( memcpy( CLIAlloc( cli, len ), str, len ) );
}


void *FreeLink(
    dw_client                   cli,
    void *                      node )
{
    void *                      p;

    p = *(void **)node;
    CLIFree( cli, node );
    return( p );
}


void FreeChain(
    dw_client                   cli,
    void *                      list )
{
    void *                      cur;

    while( list != NULL ) {
        cur = list;
        list = *(void **)list;
        CLIFree( cli, cur );
    }
}


void *ReverseChain(
    void *                      head )
/* reverse a singly linked list (link is first field in structure) */
{
    void *                      current;
    void *                      next;

    current = head;
    head = NULL;
    while( current != NULL ) {
        next = *(void **)current;
        *(void **)current = head;
        head = current;
        current = next;
    }
    return( head );
}
