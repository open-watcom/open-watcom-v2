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


#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "error.h"
#include "memory.h"


/*
 * Allocate a block of memory, aborting program execution with an error
 * message if there is insufficient memory.
 */
void *AllocMem( size_t bytes )
/****************************/
{
    void *              p;

    if( bytes == 0 )  Zoinks();
    p = malloc( bytes );
    if( p == NULL )  FatalError( "Insufficient memory." );
    return( p );
}


/*
 * Reallocate a block of memory, aborting program execution with an error
 * message if there is insufficient memory.
 */
void *ReallocMem( void *buf, size_t bytes )
/*****************************************/
{
    void *              p;

    p = realloc( buf, bytes );
    if( p == NULL )  FatalError( "Insufficient memory." );
    return( p );
}


/*
 * Free a block of memory.
 */
void FreeMem( void *buf )
/***********************/
{
    free( buf );
}


/*
 * Duplicate a quoted string.
 * Add quotes if there aren't any there.
 */
char *DupQuoteStrMem( const char *str, char quote )
/*************************************************/
{
    char *              p;
    size_t              len;
    bool                add_quote = false;

    len = strlen( str );
    if( quote != '\0' ) {
        for( ;; ) {
            if( str[0] == '"'  && str[len - 1] == '"'  )
                break;
            if( str[0] == '\'' && str[len - 1] == '\'' )
                break;
            len += 2;
            add_quote = true;
        }
    }
    p = AllocMem( len + 1 );
    if( add_quote ) {
        p[0] = quote;
        strcpy( &(p[1]), str );
        p[len - 1] = quote;
        p[len] = '\0';
    } else {
        strcpy( p, str );
    }
    return( p );
}


/*
 * Duplicate a string.
 */
char *DupStrMem( const char *str )
/********************************/
{
    return( DupQuoteStrMem( str, '\0' ) );
}
