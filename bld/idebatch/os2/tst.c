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


#include <stdio.h>
#include <conio.h>
#include <process.h>
#include "batcher.h"

main()
{
    char                buff[256];
    unsigned long       stat;
    unsigned            len;
    char                *err;
    int                 linked;

    linked = 0;
    for( ;; ) {
        printf( "cmd> " );
        if( gets( buff ) == NULL ) break;
        if( !linked ) {
            err = BatchLink( NULL );
            if( err != NULL ) {
                printf( "link error: %s\n", err );
                exit( 1 );
            }
            linked = 1;
        }
        if( buff[0] == '.' ) {
            BatchChdir( &buff[1] );
        } else if( buff[0] == 'u' && buff[1] == '\0' ) {
            BatchUnlink( 0 );
            linked = 0;
        } else if( buff[0] == 'q' && buff[1] == '\0' ) {
            BatchUnlink( 1 );
            linked = 0;
            exit( 1 );
        } else {
            BatchSpawn( buff );
            for( ;; ) {
                len = BatchCollect( buff, sizeof( buff ), &stat );
                if( len == -1 ) {
                    printf( "done: status = %d\n", stat );
                    break;
                } else if( kbhit() ) {
                    if( getch() == 'a' ) {
                        BatchAbort();
                    } else {
                        BatchCancel();
                    }
                } else if( len != 0 ) {
                    buff[ len ] = '\0';
                    printf( "%s", buff );
                    fflush( stdout );
                }
            }
        }
    }
    BatchUnlink( 0 );
}
