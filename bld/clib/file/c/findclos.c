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


#include "variety.h"
#include <io.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <os2.h>
#elif __RDOS__
    #include <rdos.h>
    #include "find.h"
    #include "liballoc.h"
#else
    #include <dos.h>
    #include "liballoc.h"
#endif


_WCRTLINK int _findclose( long handle )
{
#if defined( __NT__ )
    if( FindClose( (HANDLE)handle ) != TRUE ) {
        return( -1 );
    }
#elif defined( __OS2__ )
    if( DosFindClose( (HDIR)handle ) ) {
        return( -1 );
    }
#elif defined( __RDOS__ )
    RDOSFINDTYPE * handlebuf = ( RDOSFINDTYPE * )handle;

    RdosCloseDir( handlebuf->handle );    
    lib_free( (void*) handle );
    return( 0 );
#else   /* DOS */
    unsigned        rc;

    rc = _dos_findclose( (struct find_t *)handle );
    lib_free( (void *)handle );
    if( rc ) {
        return( -1 );
    }
#endif
    return( 0 );
}
