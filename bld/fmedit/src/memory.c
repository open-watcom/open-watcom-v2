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
* Description:  Memory routines for the report editor.
*
****************************************************************************/


#include <malloc.h>
#include <string.h>
#include <windows.h>

#define BUFFERLENGTH 30

int     FilePtr;

extern void * EdAlloc( int size )
/*******************************/
{
    /* prints out amount of memory being allocated using LocalAlloc, then
     * calls LocalAlloc to allocate the memory
     */
    void    *hMem;
#ifdef DEBUG_ON
    char    buffer[BUFFERLENGTH];
#endif

    hMem = malloc( size );
    memset( hMem, 0, size );
#ifdef DEBUG_ON
    memset( buffer, 0, BUFFERLENGTH );
    sprintf( buffer, "LocalAlloc %d\r\n", LocalSize( hMem ) );
    _lwrite( FilePtr, buffer, strlen( buffer ) );
#endif
    return( hMem );
}

extern void EdFree( void *hMem )
/******************************/
{
    /* prints out amount of memory being freed using LocalFree, then
     * calls LocalFree to free the memory
     */
#ifdef DEBUG_ON
    char    buffer[BUFFERLENGTH];

    memset( buffer, 0, BUFFERLENGTH );
    sprintf( buffer, "LocalFree %d\r\n", LocalSize( hMem ) );
    _lwrite( FilePtr, buffer, strlen( buffer ) );
#endif
    free( hMem );
}
