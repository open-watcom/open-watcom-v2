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
* Description:  DOS Setup stub program.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <io.h>
#include "pathgrp2.h"

#include "clibext.h"


int main( int argc, char **argv )
{
    char        buff[_MAX_PATH];
    pgroup2     pg;

    _splitpath2( argv[0], pg.buffer, &pg.drive, &pg.dir, NULL, &pg.ext );
    _makepath( buff, pg.drive, pg.dir, "DOSSETUP", pg.ext );
    if( access( buff, F_OK ) == 0 ) {
        argv[0] = buff;
        execv( buff, (const char * const *)argv );
        return( 0 );
    }
    cputs( "Cannot find '" );
    cputs( buff );
    cputs( "'\r\n" );
    return( 0 );
}
