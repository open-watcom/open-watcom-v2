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
* Description:  Implementation of fscanf() - formatted stream input.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include "rtdata.h"
#include "scanf.h"
#include "fileacc.h"
#include "orient.h"


static int cget_file( PTR_SCNF_SPECS specs )
{
    int     c;

    if( (c = __F_NAME(getc,getwc)( (FILE *)specs->ptr )) == __F_NAME(EOF,WEOF) ) {
        specs->eoinp = 1;
    }
    return( c );
}


static void uncget_file( int c, PTR_SCNF_SPECS specs )
{
    __F_NAME(ungetc,ungetwc)( c, (FILE *)specs->ptr );
}


_WCRTLINK int __F_NAME(vfscanf,vfwscanf)( FILE *io, const CHAR_TYPE *format, va_list args )
{
    int         rc;
    SCNF_SPECS  specs;

    _AccessFile( io );

    /*** Deal with stream orientation ***/
    ORIENT_STREAM(io,0);

    specs.ptr = (CHAR_TYPE *)io;
    specs.cget_rtn = cget_file;
    specs.uncget_rtn = uncget_file;
    rc = __F_NAME(__scnf,__wscnf)( (PTR_SCNF_SPECS)&specs, format, args );
    _ReleaseFile( io );
    return( rc );
}


_WCRTLINK int __F_NAME(fscanf,fwscanf)( FILE *io, const CHAR_TYPE *format, ... )
{
    va_list     args;

    va_start( args, format );
    return( __F_NAME(vfscanf,vfwscanf)( io, format, args ) );
}
