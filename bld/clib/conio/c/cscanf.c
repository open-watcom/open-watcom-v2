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


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include "scanf.h"


static int cget_console( PTR_SCNF_SPECS specs )
    {
        int c;

        if( (c = getche()) == EOF ) {
            specs->eoinp = 1;
        }
        return( c );
    }


static void uncget_console( int c, PTR_SCNF_SPECS specs )
    {
        ungetch( c );
    }


_WCRTLINK int vcscanf( const char *format, va_list args )
    {
        auto SCNF_SPECS specs;

        specs.cget_rtn = cget_console;
        specs.uncget_rtn = uncget_console;
        return( __scnf( (PTR_SCNF_SPECS)&specs, format, args ) );
    }


_WCRTLINK int cscanf( const char *format,... )
    {
        va_list args;

        va_start( args, format );
        return( vcscanf( format, args ) );
    }
