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
* Description:  Profiler test app, Part VII.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>

#include "pt.h"

extern char flags[SIZE+1];
extern int count, niter, iter;


void set_true()
{
    int i, k;

    for( k = 0; k < (iter%10); ++k ) {
        for( i = 0; i <= SIZE; i++) {          /* set all flags true */
                flags[i] = TRUE;
        }
    }
    delay_me();
}

void delay_me()
{
    static char control[] = "Profiler Test Program Output\r\n" "\x1a";
    FILE   *fp;
    char    buf[ sizeof( control ) ];
    unsigned    count;

    for( count = (iter%4); count; --count ) {
        fp = fopen( "PT_TEMP.TMP", "wb" );
        if( fp ) {
            fwrite( control, sizeof( control ), sizeof( char ), fp );
            fclose( fp );
            fp = fopen( "PT_TEMP.TMP", "rb" );
            if( fp ) {
                fread( buf, sizeof( control ), sizeof( char ), fp );
                fputs( "\r\t\t\t\t\t\t", stdout );
                if( strcmp( control, buf ) || (iter%3) == 0 ) {
                    fputs( " ???? \r", stdout );
                } else {
                    fputs( " !!!! \r", stdout );
                }
                fclose( fp );
            }
        }
        memset( buf, 0, sizeof( control ) );
    }
}
