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


#include <process.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "idedrv.h"

static void MakeDllName( char *dst, char const *src ){
    int i;
    for( i=3; i > 0; --i ){
        if( *src == '\0' )break;
        *dst = *src;
        ++dst;
        ++src;
    }
    *dst++ = 'd';
    strcpy( dst, src );
}

int main( int count, char* args[] ){
/**********************************/
    IDEDRV  info;
    char    fname[_MAX_FNAME];
    char    dllname[_MAX_FNAME];
    char    *buffer;
    int     len;
    int     retcode;                 // - return code


    count = count;
    _splitpath( args[0], NULL, NULL, fname, NULL );  // get fname
    strlwr( fname );
    MakeDllName( dllname, fname );
    IdeDrvInit( &info, dllname, NULL );
    retcode = IDEDRV_ERR_RUN_FATAL;
    len = _bgetcmd( NULL, INT_MAX ) + 1;
    buffer = malloc( len );
    if( buffer != NULL ) {
        _bgetcmd( buffer, len );
        retcode = IdeDrvExecDLL( &info, buffer );
        switch( retcode ) {
          case IDEDRV_SUCCESS :
          case IDEDRV_ERR_RUN :
          case IDEDRV_ERR_RUN_EXEC :
          case IDEDRV_ERR_RUN_FATAL :
            break;
          default :
            retcode = IdeDrvPrintError( &info );
            break;
        }
        free( buffer );
    }
    return retcode;
}
