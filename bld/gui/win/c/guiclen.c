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
* Description:  DBCS character support (character length).
*
****************************************************************************/


#include "guiwind.h"

#if defined(__NT__)
    #include <winnls.h>
#elif defined __WINDOWS__
    #include <windows.h>
#endif

char __CharLenTable[256];
static bool Init;
static bool IsDBCS;


static void MBInit( void )
/************************/
{
#if defined( __NT__ ) && !defined( __UNIX__ )
    int                 countRange, countVal;
    CPINFO              cpInfo;
    BOOL                rc;
#elif defined __OS2__ || defined __OS2_PM__
    int                 countRange, countVal;
    COUNTRYCODE         countryInfo;
    CHAR                leadBytes[12];
    #ifdef __386__
        APIRET          rc;
    #else
        USHORT          rc;
    #endif
#elif defined( __WINDOWS__ ) || defined( __UNIX__ )
    int                 countVal;
    DWORD               version;
#endif

    memset( __CharLenTable, 1, sizeof( __CharLenTable ) );              /* zero table to start */

    #if defined( __NT__) && !defined( __UNIX__ )
        /*** Initialize the __CharLenTable values ***/
        rc = GetCPInfo( CP_OEMCP, &cpInfo );    /* get code page info */
        if( rc == FALSE )  return;
        for( countRange=0; !(cpInfo.LeadByte[countRange]==0x00 &&
             cpInfo.LeadByte[countRange+1]==0x00); countRange+=2 ) {
            for( countVal=cpInfo.LeadByte[countRange];
                 countVal<=cpInfo.LeadByte[countRange+1]; countVal++) {
                __CharLenTable[countVal] = 2;
                IsDBCS = TRUE;
            }
        }
    #elif defined __OS2__ || defined __OS2_PM__
        /*** Initialize the __CharLenTable values ***/
        countryInfo.country = 0;                /* default country */
        countryInfo.codepage = 0;       /* specified code page */
        #ifdef __386__
            rc = DosQueryDBCSEnv( 12, &countryInfo, leadBytes );
        #else
            rc = DosGetDBCSEv( 12, &countryInfo, leadBytes );
        #endif
        if( rc != 0 )  return;
        for( countRange=0; !(leadBytes[countRange]==0x00 &&
             leadBytes[countRange+1]==0x00); countRange+=2 ) {
            for( countVal=leadBytes[countRange];
                 countVal<=leadBytes[countRange+1]; countVal++) {
                __CharLenTable[countVal] = 2;
                IsDBCS = TRUE;
            }
        }
    #elif defined( __WINDOWS__ ) || defined( __UNIX__ )
        /*** Initialize the __CharLenTable values ***/
        version = GetVersion();
        if( LOWORD(version) < ((10<<8)+3) )  return;   /* 3.1+ needed */
        for( countVal=0; countVal<256; countVal++ ) {
            if( IsDBCSLeadByte( (BYTE)countVal ) ) {
                __CharLenTable[countVal] = 2;
                IsDBCS = TRUE;
            }
        }
    #endif

}

int GUICharLen( int ch )
{
    if( !Init ) {
        MBInit();
        Init = TRUE;
    }
    return( __CharLenTable[ch] );
}

bool GUIIsDBCS( void )
{
    if( !Init ) {
        MBInit();
        Init = TRUE;
    }
    return( IsDBCS );
}
