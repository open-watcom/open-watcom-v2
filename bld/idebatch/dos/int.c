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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <ctype.h>
#include <malloc.h>
#include "winprag.h"
#include "link.h"
#include "common.h"
#include "wdebug.h"

extern void far *far OldInt21;
extern void far TrapInt21( void );

void far        *OldInt1B;
char            **NameList;
int             NameCount;
int             RealNameStrLen = sizeof( GET_REAL_NAME );
char            GetRealNamePrefix[sizeof( GET_REAL_NAME ) + _MAX_PATH + 3] = GET_REAL_NAME " ";
char            TmpBuff[_MAX_PATH];


void __interrupt __far BreakHandler() {
}

static void installBreakHandler( void ) {
    OldInt1B = _dos_getvect( 0x1B );
    _dos_setvect( 0x1B, BreakHandler );
}

static void removeBreakHandler( void ) {
    _dos_setvect( 0x1B, OldInt1B );
}

static void startClog( void )
{
    OldInt21 = _dos_getvect( 0x21 );
    _dos_setvect( 0x21, TrapInt21 );
}

static void endClog( void )
{
    _dos_setvect( 0x21, OldInt21 );
}

int main( void )
{
    char        buff[512];
    char        names[2*_MAX_PATH+2];
    char        *ptr;
    char        *nptr;
    long        rc;
    int         i,len;

    cputs( "WATCOM DOS Driver\r\n" );
    cputs( "-----------------\r\n" );
    if( !VxDPresent() ) {
        cputs( "WDEBUG.386 not present!\n\r" );
        exit( 1 );
    }
    installBreakHandler();
    while( 1 ) {
#ifdef DEBUG
        if( kbhit() ) {
            if( getch() == 'q' ) {
                return( 0 );
            }
        }
#endif
        if( VxDLink( LINK_NAME ) == NULL ) {
            VxDConnect();
            break;
        }
        TimeSlice();
    }
    cputs( "DOS Driver started\r\n" );
    while( 1 ) {
        rc = VxDGet( buff, sizeof( buff ) );
        if( rc < 0 ) {
            ltoa( rc, buff, 10 );
            cputs( "Error " );
            cputs( buff );
            cputs( "encountered, aborting!\r\n" );
            break;
        }
        if( !stricmp( buff,TERMINATE_CLIENT_STR ) ) {
            break;
        }
        if( !stricmp( buff, NEW_OPEN_LIST ) ) {
            /*
             * free old list of names
             */
            for( i=0;i<NameCount;i++ ) {
                free( NameList[i] );
            }
            free( NameList );
            NameList = NULL;
            NameCount = 0;
            _heapshrink();
            VxDPut( GOT_OPEN_STR, sizeof( GOT_OPEN_STR ) );

            /*
             * get list of names
             */
            while( 1 ) {
                rc = VxDGet( buff, sizeof( buff ) );
                if( !stricmp( buff, END_OPEN_LIST ) ) {
                    break;
                }
                ptr = buff;
                nptr = names;
                while( !isspace( *ptr ) ) {
                    *nptr++ = tolower( *ptr++ );
                }
                *nptr++ = 0;
                while( isspace( *ptr ) ) {
                    ptr++;
                }
                while( 1 ) {
                    *nptr++ = tolower( *ptr );
                    if( *ptr == 0 ) {
                        break;
                    }
                    ptr++;
                }
                len = nptr - names;
                NameList = realloc( NameList, (NameCount+1)*sizeof(char *) );
                if( NameList != NULL ) {
                    NameList[ NameCount ] = malloc( len );
                    if( NameList[ NameCount ] != NULL ) {
                        memcpy( NameList[ NameCount ], names, len );
                    }
                    NameCount++;
                }
                VxDPut( GOT_OPEN_STR, sizeof( GOT_OPEN_STR ) );
            }
            VxDPut( GOT_OPEN_STR, sizeof( GOT_OPEN_STR ) );
            continue;
        }
        cputs( "Command: \"" );
        cputs( buff );
        cputs( "\"\r\n" );
        removeBreakHandler();
        startClog();
        system( buff );
        endClog();
        installBreakHandler();
        VxDPut( TERMINATE_COMMAND_STR, sizeof( TERMINATE_COMMAND_STR ) );
    }
    removeBreakHandler();
    VxDDisconnect();
    VxDUnLink();

    return( 0 );
}
