/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  QNX Neutrino trap file I/O.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "servio.h"


void OutputLine( const char *str )
{
    write( STDERR_FILENO, str, strlen( str ) );
    write( STDERR_FILENO, "\n", 1 );
}

void ServTerminate( int return_code )
{
    _exit( return_code );
    // never return
}

void StartupErr( const char *err )
{
    OutputLine( err );
}

int KeyPress( void )
{
    int             ret;
    struct termios  old;
    struct termios  new;
    struct timeval  tv;
    fd_set          rdfs;

    tcgetattr( 0, &old );
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new );

    FD_ZERO( &rdfs );
    FD_SET( 0, &rdfs );
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    ret = select(1, &rdfs, NULL, NULL, &tv );

    tcsetattr( 0, TCSANOW, &old );
    return( ret != 0 );
}

int KeyGet( void )
{
    struct termios  old;
    struct termios  new;
    char            key;

    tcgetattr( 0, &old );
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new );
    read( 0, &key, 1 );
    tcsetattr( 0, TCSANOW, &old );
    return( key );
}


int WantUsage( const char *ptr )
{
    /*
        This is a really stupid place to put this, but it's the first
        system dependant code that the servers run. This code sets the
        effective GID and UID back to the real ones so that the server
        can be made set UID root without being a security hole. That allows
        use to run at ring 1 for the parallel server/trap file.
     */
    setegid( getgid() );
    seteuid( getuid() );
    if( *ptr == '?' )
        return( TRUE );
    return( FALSE );
}
