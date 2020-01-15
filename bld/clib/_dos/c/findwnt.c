/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include <stdlib.h>
#include <direct.h>
#include <dos.h>
#include <windows.h>
#include "rtdata.h"
#include "ntext.h"
#include "ntinfo.h"
#include "_dtaxxx.h"
#include "seterrno.h"
#include "libwin32.h"


_WCRTLINK unsigned _dos_findfirst( const char *path, unsigned dos_attrib, struct find_t *findt )
{
    HANDLE              h;
    int                 error;
    WIN32_FIND_DATA     ffd;
    unsigned            nt_attrib;

    h = __lib_FindFirstFile( (LPTSTR)path, &ffd );
//  if( dos_attrib == _A_NORMAL ) {
//      dos_attrib = ~(_A_SUBDIR|_A_VOLID);
//  }
    nt_attrib = DOS2NTATTR( dos_attrib );
    if( h == INVALID_HANDLE_VALUE ) {
        DTAXXX_HANDLE_OF( findt->reserved ) = DTAXXX_INVALID_HANDLE;
        return( __set_errno_nt_reterr() );
    }
    if( !__NTFindNextFileWithAttr( h, nt_attrib, &ffd ) ) {
        error = GetLastError();
        DTAXXX_HANDLE_OF( findt->reserved ) = DTAXXX_INVALID_HANDLE;
        FindClose( h );
        return( __set_errno_dos_reterr( error ) );
    }
    DTAXXX_HANDLE_OF( findt->reserved ) = h;
    DTAXXX_ATTR_OF( findt->reserved ) = nt_attrib;
    __GetNTFindInfo( findt, &ffd );

    return( 0 );
}

_WCRTLINK unsigned _dos_findnext( struct find_t *findt )
{
    WIN32_FIND_DATA     ffd;

    if( !__fixed_FindNextFile( DTAXXX_HANDLE_OF( findt->reserved ), &ffd ) ) {
        return( __set_errno_nt_reterr() );
    }
    if( !__NTFindNextFileWithAttr( DTAXXX_HANDLE_OF( findt->reserved ), DTAXXX_ATTR_OF( findt->reserved ), &ffd ) ) {
        return( __set_errno_nt_reterr() );
    }
    __GetNTFindInfo( findt, &ffd );

    return( 0 );
}

_WCRTLINK unsigned _dos_findclose( struct find_t *findt )
{
    if( DTAXXX_HANDLE_OF( findt->reserved ) != DTAXXX_INVALID_HANDLE ) {
        if( !FindClose( DTAXXX_HANDLE_OF( findt->reserved ) ) ) {
            return( __set_errno_nt_reterr() );
        }
    }
    return( 0 );
}
