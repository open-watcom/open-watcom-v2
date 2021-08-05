/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include <mbstring.h>
#include <string.h>
#include "liballoc.h"
#include "cvtwc2mb.h"
#include "exwc2mb.h"

// Used by __exec_wide_to_mbcs to free all allocated memory in case of error
#define GO_HOME {                                                               \
                    __exec_wide_to_mbcs_cleanup( *mbPath, *mbArgv, *mbEnvp );   \
                    *mbPath = NULL;                                             \
                    *mbArgv = NULL;                                             \
                    *mbEnvp = NULL;                                             \
                    return( 0 );                                                \
                }


/*
** Allocate memory for MBCS versions of the path, arguments, and environment,
** and perform all necessary conversion.  If any input pointer is NULL, no
** attempt will be made to convert the corresponding data.  Returns pointers
** to the newly allocated buffers.  Use __exec_wide_to_mbcs_cleanup to free
** these buffers.  Return value is non-zero on success, or zero on failure.
*/

_WCRTLINK int __exec_wide_to_mbcs( CWCPTR wcPath,       // i: path
                                   CWCPTR const wcArgv[],       // i: arguments
                                   CWCPTR const wcEnvp[],       // i: environment
                                   CPTR *mbPath,        // o: path
                                   CPTR *mbArgv[],      // o: arguments
                                   CPTR *mbEnvp[] )     // o: environment
/*************************************************/
{
    size_t              bytes;
    unsigned            count;

    /*** Set pointers to NULL for easy freeing later if error occurs ***/
    if( mbPath != NULL )
        *mbPath = NULL;
    if( mbArgv != NULL )
        *mbArgv = NULL;
    if( mbEnvp != NULL )
        *mbEnvp = NULL;

    /*** Convert the path ***/
    if( wcPath != NULL ) {
        *mbPath = __lib_cvt_wcstombs( wcPath );
        if( *mbPath == NULL ) {
            return( 0 );
        }
    }

    /*** Convert the arguments ***/
    if( wcArgv != NULL ) {
        /*** Allocate memory for converted arguments ***/
        count = 0;
        while( wcArgv[count] != NULL )
            count++;                                // count the arguments
        bytes = (count+1) * sizeof( char * );       // how much do we need?
        *mbArgv = lib_malloc( bytes );              // allocate it
        if( *mbArgv == NULL )
            GO_HOME;
        memset( *mbArgv, 0, bytes );                // make all pointers NULL

        /*** Convert them, one by one ***/
        count = 0;
        while( wcArgv[count] != NULL ) {
            (*mbArgv)[count] = __lib_cvt_wcstombs( wcArgv[count] );
            if( (*mbArgv)[count] == NULL )
                GO_HOME;
            count++;
        }
    }

    /*** Convert the environment ***/
    if( wcEnvp != NULL ) {
        /*** Allocate memory for converted environment strings ***/
        count = 0;
        while( wcEnvp[count] != NULL )
            count++;                                // count the strings
        bytes = (count+1) * sizeof( char * );       // how much do we need?
        *mbEnvp = lib_malloc( bytes );              // allocate it
        if( *mbEnvp == NULL )
            GO_HOME;
        memset( *mbEnvp, 0, bytes );                // make all pointers NULL

        /*** Convert them, one by one ***/
        count = 0;
        while( wcEnvp[count] != NULL ) {
            (*mbEnvp)[count] = __lib_cvt_wcstombs( wcEnvp[count] );
            if( (*mbEnvp)[count] == NULL )
                GO_HOME;
            count++;
        }
    }

    return( 1 );
}


_WCRTLINK void __exec_wide_to_mbcs_cleanup( CPTR mbPath,
                                            CPTR mbArgv[],
                                            CPTR mbEnvp[] )
/*********************************************************/
{
    int                 count;

    /*** Free the path ***/
    if( mbPath != NULL )
        lib_free( mbPath );

    /*** Free the argument strings ***/
    if( mbArgv != NULL ) {
        count = 0;
        while( mbArgv[count] != NULL ) {
            lib_free( mbArgv[count] );
            count++;
        }
        lib_free( mbArgv );                 // free array of pointers
    }

    /*** Free the environment strings ***/
    if( mbEnvp != NULL ) {
        count = 0;
        while( mbEnvp[count] != NULL ) {
            lib_free( mbEnvp[count] );
            count++;
        }
        lib_free( mbEnvp );                 // free array of pointers
    }
}
