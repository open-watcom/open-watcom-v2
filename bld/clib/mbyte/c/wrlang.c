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
#elif defined __WINDOWS__
    #include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "wreslang.h"

#ifdef __DOS__
    unsigned short              dos_get_code_page( void );
#endif

static res_language_enumeration check_code_page( void );


_WCRTLINK res_language_enumeration _WResLanguage(void)
{
    char        *env;

    env = getenv( "WLANG" );
    if( env == NULL ) {
        /* Look at the code page value to determine language */
        return( check_code_page() );
    }
    if( stricmp( env, "english" )       == 0 ) return( RLE_ENGLISH );
    if( stricmp( env, "japanese" )      == 0 ) return( RLE_JAPANESE );
    if( env[0] >= '0' && env[0] <= '9' ) {
        return( env[0] - '0' );
    }
    return( RLE_ENGLISH );
}


static res_language_enumeration check_code_page( void )
/*****************************************************/
{
    unsigned int        codepage = 0;
#if defined __OS2__
    OS_UINT             cp;
    OS_UINT             bytesOutput;
#elif defined __WINDOWS__
    char                lang[4];
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
        if( GetProfileString( "Intl", "sLanguage", "ENU", lang, 4 ) ) {
            if( !stricmp( lang, "JPN" ) )  codepage = 932;
        }
    #endif

    /*** Try to match the returned code page to known ones ***/
    switch( codepage ) {
        case 932:   return( RLE_JAPANESE );
        default:    return( RLE_ENGLISH );
    }
}



/****
***** Query DOS to find the valid lead byte ranges.
****/

#ifdef __DOS__
#ifndef __386__

#pragma aux             dos_get_code_page = \
        "mov ax,6601h"  /* get code page (DOS 3.3+) */ \
        "int 21h"       \
        "jnc NoError"   \
        "mov bx,437"    /* error: return 437, the default */ \
        "NoError:"      /* no error: exit point */ \
        value           [bx] \
        modify          [ax bx cx dx si di es];

//static unsigned short dos_get_code_page( void )
///*********************************************/
//{
//    union REGS                regs;
//
//    regs.w.ax = 0x6601;                           /* get code page (DOS 3.3+) */
//    intdos( &regs, &regs );               /* call DOS */
//    if( regs.w.cflag )  return( 437 );            /* return default on failure */
//    return( regs.w.bx );                  /* return active code page */
//}


#else


static unsigned short dos_get_code_page( void )
/*********************************************/
{
    if( _IsPharLap() ) {
        union REGPACK   regs;

        memset( &regs, 0, sizeof( regs ) );
        regs.w.ax = 0x6601;                 /* get extended country info */
        intr( 0x21, &regs );
        if( (regs.w.flags & 1) == 0 ) {
            return( regs.w.bx );            /* return active code page */
        }
    } else if( _IsRational() ) {
        rm_call_struct  dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6601;                /* get extended country info */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 ) {
            return( (unsigned short)dblock.ebx );
        }
    }
    return( 437 );                          /* return default */
}

#endif
#endif
