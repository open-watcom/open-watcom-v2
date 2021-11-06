/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Initialize MBCS support.
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <mbstring.h>
#ifdef __NT__
    #include <windows.h>
    #include <winnls.h>
#elif defined __OS2__
    #define INCL_DOSNLS
    #include <wos2.h>
#elif defined __DOS__
    #include <dos.h>
    #include <i86.h>
    #ifdef __386__
        #include "dpmi.h"
        #include "extender.h"
    #endif
    #include "getcpdos.h"
    #include "getltdos.h"
#elif defined __WINDOWS__
    #include <windows.h>
#elif defined __LINUX__
#endif
#include "mbchar.h"


#if defined __DOS__
#define DOS_FAR     __far
#else
#define DOS_FAR
#endif

#define _set_dbcs_table(low,high)   memset( __MBCSIsTable + low + 1, _MB_LEAD, high - low + 1 )

#ifdef __NT__
#define DEFAULT_CODEPAGE    CP_OEMCP
#else
#define DEFAULT_CODEPAGE    0
#endif

unsigned int __MBCodePage = DEFAULT_CODEPAGE;   /* default code page */

static void clear_dbcs_table( void )
{
    __IsDBCS = 0;                               /* SBCS for now */
    __MBCodePage = DEFAULT_CODEPAGE;
    memset( __MBCSIsTable, 0, 257 );
}

#if defined( __DOS__ ) || defined( __NT__ ) || defined( __OS2__ )
static void set_dbcs_table( unsigned short DOS_FAR *lead_bytes )
{
    unsigned short range;

    if( *lead_bytes ) {
        __IsDBCS = 1;       /* set __IsDBCS if needed */
        for( ; (range = *lead_bytes) != 0; lead_bytes++ ) {
            _set_dbcs_table( (unsigned char)range, (unsigned char)( range >> 8 ) );
        }
    }
}
#elif defined( __WINDOWS__ )
static void set_dbcs_table( void )
{
    unsigned short value;

    for( value = 0; value < 256; value++ ) {
        if( IsDBCSLeadByte( (BYTE)value ) ) {
            __MBCSIsTable[value + 1] = _MB_LEAD;
            __IsDBCS = 1;   /* set __IsDBCS if needed */
        }
    }
}
#endif

/****
***** Initialize a multi-byte character set.  Returns 0 on success.
****/

int __mbinit( int codepage )
{
#if defined( __NT__ )
    CPINFO                  cpInfo;
    BOOL                    rc;
#elif defined( __OS2__ )
    COUNTRYCODE             countryInfo;
    unsigned short          leadBytes[6];
    APIRET                  rc;
    OS_UINT                 buf[8];
    OS_UINT                 bytes;
#elif defined( __DOS__ )
    unsigned short          __far *leadBytes;
#elif defined( __WINDOWS__ )
    DWORD                   version;
#elif defined __LINUX__
#elif defined __RDOS__
#endif

    clear_dbcs_table();
    /*** Handle values from _setmbcp ***/
    if( codepage == _MBINIT_CP_ANSI ) {
#ifdef __NT__
        codepage = GetACP();
#else
        codepage = 0;
#endif
    } else if( codepage == _MBINIT_CP_OEM ) {
#ifdef __NT__
        codepage = GetOEMCP();
#else
        codepage = 0;
#endif
    } else if( codepage == _MBINIT_CP_SBCS ) {
        return( 0 );
    } else if( codepage == _MBINIT_CP_932 ) {
        _set_dbcs_table( 0x81, 0x9F );
        _set_dbcs_table( 0xE0, 0xFC );
        __IsDBCS = 1;
        __MBCodePage = 932;
        return( 0 );
    }

#ifdef __NT__
    /*** Initialize the __MBCSIsTable values ***/
    if( codepage == 0 )
        codepage = CP_OEMCP;
    rc = GetCPInfo( codepage, &cpInfo );    /* get code page info */
    if( rc == FALSE )
        return( 1 );
    set_dbcs_table( (unsigned short *)cpInfo.LeadByte );
    /*** Update __MBCodePage ***/
    if( codepage == CP_OEMCP ) {
        __MBCodePage = GetOEMCP();
    } else {
        __MBCodePage = codepage;
    }
#elif defined __OS2__
    /*** Initialize the __MBCSIsTable values ***/
    countryInfo.country = 0;                /* default country */
    countryInfo.codepage = codepage;        /* specified code page */
  #if defined(_M_I86)
    rc = DosGetDBCSEv( sizeof( leadBytes ), &countryInfo, (PCHAR)leadBytes );
  #else
    rc = DosQueryDBCSEnv( sizeof( leadBytes ), &countryInfo, (PCHAR)leadBytes );
  #endif
    if( rc != 0 )
        return( 1 );
    set_dbcs_table( leadBytes );
    /*** Update __MBCodePage ***/
    if( codepage == 0 ) {
  #if defined(_M_I86)
        rc = DosGetCp( sizeof( buf ), &buf, &bytes );
  #else
        rc = DosQueryCp( sizeof( buf ), &buf, &bytes );
  #endif
        if( rc != 0 ) {
            __MBCodePage = 0;
        } else {
            __MBCodePage = (unsigned int)buf[0];
        }
    } else {
        __MBCodePage = codepage;
    }
#elif defined __DOS__
    /*** Initialize the __MBCSIsTable values ***/
    if( codepage != 0 )
        return( 1 );        /* can only handle default */
    leadBytes = dos_get_dbcs_lead_table();
    if( leadBytes == NULL )
        return( 0 );
    set_dbcs_table( leadBytes );
    __MBCodePage = dos_get_code_page();
#elif defined __WINDOWS__
    /*** Initialize the __MBCSIsTable values ***/
    if( codepage != 0 )
        return( 1 );        /* can only handle default */
    version = GetVersion();
    if( LOWORD(version) < 0x0A03 )
        return( 1 );        /* 3.1+ needed */
    set_dbcs_table();
    __MBCodePage = GetKBCodePage();
#elif defined __LINUX__
#elif defined __RDOS__
#endif
    return( 0 );                                /* return success code */
}
