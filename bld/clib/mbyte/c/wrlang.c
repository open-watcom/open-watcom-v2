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
* Description:  Determine resource language from system environment.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef __NT__
    #include <windows.h>
    #include <winnls.h>
#elif defined __OS2__
    #define INCL_DOSNLS
    #include <wos2.h>
#elif defined __DOS__
    #include <dos.h>
    #ifndef __386__
        #include <i86.h>
    #else
        #include "dpmi.h"
        #include "extender.h"
    #endif
    #include "getcpdos.h"
#elif defined __WINDOWS__
    #include <windows.h>
#endif
#include "wreslang.h"


static res_language_enumeration check_code_page( void )
/*****************************************************/
{
    unsigned    codepage;
#if defined __OS2__
    OS_UINT     cp;
    OS_UINT     bytesOutput;
#elif defined __WINDOWS__
    char        lang[4];
#endif

    /*** Determine the system default code page ***/
#if defined __NT__
    codepage = GetOEMCP();
#elif defined __OS2__
    DosGetCp( 2, &cp, &bytesOutput );
    codepage = cp;
#elif defined __DOS__
    codepage = dos_get_code_page();
#elif defined __WINDOWS__
    codepage = 0;
    if( GetProfileString( "Intl", "sLanguage", "ENU", lang, 4 ) ) {
        if( _stricmp( lang, "JPN" ) == 0 ) {
            codepage = 932;
        }
    }
#else
    #define NO_CODEPAGE
    codepage = 0;
#endif

#ifdef NO_CODEPAGE
    return( RLE_ENGLISH );
#else
    /*** Try to match the returned code page to known ones ***/
    switch( codepage ) {
        case 932:   return( RLE_JAPANESE );
        default:    return( RLE_ENGLISH );
    }
#endif
}

_WCRTLINK res_language_enumeration _WResLanguage( void )
{
    char        *env;

    env = getenv( "WLANG" );
    if( env == NULL ) {
        /* Look at the code page value to determine language */
        return( check_code_page() );
    }
    if( _stricmp( env, "english" ) == 0 )
        return( RLE_ENGLISH );
    if( _stricmp( env, "japanese" ) == 0 )
        return( RLE_JAPANESE );
    if( env[0] >= '0' && env[0] <= '9' ) {
        return( env[0] - '0' );
    }
    return( RLE_ENGLISH );
}
