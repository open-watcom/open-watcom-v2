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


!DELETED BY WPROJ!
 for the regular directory routines.
 * This version uses TinyIO.
 *
 * This code was basically built by disassembling clibs.lib(dir.obj) and
 * then rebuilding it without the use of intdos().
 *
 */

#include <direct.h>
#include <stdlib.h>
#include "tinyio.h"

extern int _set_errno( unsigned int );

extern DIR *_opendir( const char *name, char attr )
/*************************************************/
{
    DIR         tmp;
    DIR         *parent;
    unsigned    ret;

    TinySetDTA( tmp.d_dta );    /* set our DTA */

    ret = TinyFindFirst( name, attr );
    if( ret > 0 ) {
        _set_errno( ret );
        return( NULL );
    }

    parent = malloc( sizeof( *parent ) );
    if( parent == NULL ) {
        _set_errno( 8 );        /* not enough memory */
        return( NULL );
    }

    *parent = tmp;

    parent->d_first = 1;

    return( parent );
}


extern DIR *opendir( const char *name )
/*************************************/
{
    return( _opendir( name, TIO_HIDDEN | TIO_SYSTEM | TIO_SUBDIRECTORY ) );
}


extern DIR *readdir( DIR *parent )
/********************************/
{
    unsigned    ret;

    if( parent == NULL ) {
        _set_errno( 6 );        /* invalid handle */
        return( NULL );
    }

    if( parent->d_first ) {
        parent->d_first = 0;
        return( parent );
    }

    TinySetDTA( parent->d_dta );

    ret = TinyFindNext();
    if( ret > 0 ) {
        _set_errno( ret );
        return( NULL );
    }

    return( parent );
}


extern int closedir( DIR *parent )
/*********************************
 * This is particularly bizarre... but I haven't looked at the CLIB C source
 * to verify why parent->d_first is set to 2 just before freeing.  It appears
 * to be a failsafe to ensure that no attempt is made to free this block
 * twice, but... that's a mistake on the programmer's fault and shouldn't be
 * worried about.  ie:  That's undefined behaviour.
 *
 * The code this produces is identical to the CLIB closedir code.
 */
{
    if( parent == NULL ) {
        _set_errno( 6 );        /* invalid handle */
        return( -1 );
    }

#if 0   /* 91-may-27 DJG */
    if( parent->d_first > 1 ) {
        _set_errno( 6 );        /* invalid handle */
        return( -1 );
    }

    parent->d_first = 2;
#endif

    free( parent );

    return( 0 );
}

