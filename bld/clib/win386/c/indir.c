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


#include <windows.h>
#include <stddef.h>
#include <dos.h>
#include <malloc.h>
#include <stdarg.h>

#pragma aux Invoke16BitFunction "*_" value[eax] modify[eax ebx ecx edx];
extern DWORD Invoke16BitFunction( void );

extern void Push( DWORD );
#pragma aux Push = \
    0x50    /* push eax */ \
    parm [eax] modify[esp];

extern void PopStack( DWORD );
#pragma aux PopStack = \
    0x03 0xE0 /* add esp, eax */ \
    parm [eax] modify[esp];

typedef struct indir {
    FARPROC         proc;
    short           num;
    unsigned char   msc;
    char            filler;
    short           lens[1];
} indir;

/*
 * GetIndirectFunctionHandle - give user a handle to an indirect function
 */
HINDIR GetIndirectFunctionHandle( FARPROC proc, ... )
{
    va_list         al;
    indir           *curr;
    short           i, len;
    short           num = 0;
    unsigned char   msc = 0;

    /*
     * get count and validate list
     */
    va_start( al, proc );
    while( 1 ) {

        len = va_arg( al, int );
        if( len == INDIR_ENDLIST ) break;
        if( len == INDIR_CDECL ) {
            msc = 1;
            num++;
        } else if( len == INDIR_PTR || len == INDIR_CHAR ||
            len == INDIR_WORD || len == INDIR_DWORD ) {
            num++;
        } else {
            return( NULL );
        }
        if( num > 128 ) return( NULL ); /* ya, so its arbitrary */

    }
    va_end( al );

    /*
     * get and add new list item
     */
    curr = malloc( sizeof( indir ) + num * sizeof( short ) );
    if( curr == NULL ) return( NULL );
    curr->proc = proc;
    curr->num = num;
    if( msc ) curr->num--;
    curr->msc = msc;

    /*
     * get item sizes
     */
    i = 0;
    va_start( al, proc );
    while( num > 0 ) {

        len = va_arg( al, short );
        if( len != INDIR_CDECL ) {
            curr->lens[i++] = len;
        }
        num--;

    }

    va_end( al );
    return( curr );

} /* GetIndirectFunctionHandle */

/*
 * InvokeIndirectFunction - invoke a 16 bit proc indirectly
 */
DWORD InvokeIndirectFunction( HINDIR handle, ... )
{
    indir       *curr;
    DWORD       *vals=NULL,rc;
    short       i;
    va_list     al;

    curr = (indir *) handle;
    if( curr == NULL ) return( -1L );
    va_start( al, handle );

    /*
     * build type list
     */
    if( curr->num != 0 ) {
        vals = malloc( sizeof( DWORD ) * curr->num );
        if( vals == NULL ) return( -1L );
        for( i=0;i<curr->num;i++ ) {
            if( curr->lens[i] == -1 ) {
                vals[i] = va_arg( al, unsigned long );
                vals[i] = AllocAlias16( (void *) vals[i] );
                if( vals[i] == -1 ) return( -1L );
            } else {
                vals[i] = va_arg( al, DWORD );
            }
        }

    }

    /*
     * build 32-bit stack frame: if microsoft convention, push
     * in the opposite order of pascal calling convention
     */
    if( curr->msc ) {
        for( i=0; i<=curr->num-1;i++ ) {
            Push( vals[i] );
            if( curr->lens[i] == -1 ) Push( 4L );
            else if( curr->lens[i] == 1 ) Push( 2L );
            else Push( curr->lens[i] );
        }
    } else {
        for( i=curr->num-1;i>=0;i-- ) {
            Push( vals[i] );
            if( curr->lens[i] == -1 ) Push( 4L );
            else if( curr->lens[i] == 1 ) Push( 2L );
            else Push( curr->lens[i] );
        }
    }
    Push( (DWORD) curr->proc );
    if( curr->msc ) Push( curr->num + 0x100 );
    else Push( curr->num );
    rc = Invoke16BitFunction();

    /*
     * release the aliases
     */
    for( i=0;i<curr->num;i++ ) {
        if( curr->lens[i] == -1 ) {
            FreeAlias16( vals[i] );
        }
    }
    free( vals );
    PopStack( curr->num*8 + 8 );
    return( rc );

} /* InvokeIndirectFunction */

void FreeIndirectFunctionHandle( HINDIR handle )
{
    free( handle );
}
