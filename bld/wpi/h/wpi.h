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


#ifndef WPI_INTERFAC
#define WPI_INTERFAC
/*****************************************************************/
/*      JOINT ROUTINES OF OS2 AND WINDOWS                        */
/* An attempt to develop a set of macro definitions so that code */
/* from OS2 and WINDOWS can be commonalized                      */
/*****************************************************************/

/*************/
/* typedef's */
/*************/

#include "wpitypes.h"
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

extern void _wpi_free( void * ptr );
extern void * _wpi_malloc( size_t size );
extern void * _wpi_realloc( void *ptr, size_t size );

#ifndef _wpi_malloc2
#define _wpi_malloc2(ptr,x) ptr = (void *) _wpi_malloc( sizeof( *ptr ) * x )
#endif

#ifndef _wpi_gfree
#define _wpi_gfree(x) _wpi_free(x)
#endif

#ifndef _wpi_gmalloc
#define _wpi_gmalloc(x) _wpi_malloc(x)
#endif

#ifndef _wpi_gmalloc2
#define _wpi_gmalloc2(ptr,x) ptr = (void *) _wpi_gmalloc( sizeof( *ptr ) * x )
#endif

#ifndef _wpi_grealloc
#define _wpi_grealloc(ptr,x) _wpi_realloc(ptr,x)
#endif

#if defined( __OS2_PM__) || defined( __OS2__ )

    /*
     * All the OS2_PM macros have been moved to wpi_os2.h.  All new macros
     * should be added there.
     */
    #include "wpi_os2.h"

#ifdef _WPI_TEST_
    /*
     * This is used by paul to add new functions.  It is temporary.
     */
    #include "new_os2.h"
#endif

#ifdef _WPI_TEST2_
    /*
     * This is used by david to add new functions.  It is temporary.
     */
    #include "djb_os2.h"
#endif

#else

    /*
     * All the Windows macros have been moved to wpi_win.h.  All new macros
     * should be added there.
     */
    #include "wpi_win.h"

#ifdef _WPI_TEST_
    /*
     * This is used by paul to add new functions.  It is temporary.
     */
    #include "new_win.h"
#endif

#endif

#endif
