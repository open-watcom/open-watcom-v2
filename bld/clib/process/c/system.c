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


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <process.h>
#include <errno.h>
#if defined(__OS2__)
    #define INCL_DOSMISC
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "seterrno.h"

extern CHAR_TYPE *__F_NAME(__Slash_C,__wSlash_C)( CHAR_TYPE *, CHAR_TYPE );

_WCRTLINK int __F_NAME(system,_wsystem)( const CHAR_TYPE *cmd )
{
    #if defined(__WINDOWS__)
        if( cmd == NULL ) {
            return( 0 );
        } else {
            __set_errno( ENOENT );
            return( -1 );
        }
    #else
        register CHAR_TYPE *name;
        auto CHAR_TYPE switch_c[4];
        char prot_mode286;
        int ret_code;
        #if defined( __NT__ )
            int tmp_fileinfo;
        #endif

        #if defined( __NT__ )
            name = __F_NAME(getenv,_wgetenv)( STRING( "ComSpec" ) );
        #else
            name = __F_NAME(getenv,_wgetenv)( STRING( "COMSPEC" ) );
        #endif
        if( cmd == NULL ) {
            #if 1
                return( 1 );    /* COMMAND.COM is available */
            #else
                if( name != NULL ) {
                    if( __F_NAME(access,_waccess)( name, 0 ) == 0 ) {
                        return( 1 );    /* COMMAND.COM is available */
                    }
                }
                __set_errno( ENOENT );
                return( 0 );    /* indicate no COMMAND.COM available */
            #endif
        }
        #if defined( __NT__ )
            prot_mode286 = 1;
        #elif defined( __WARP__ )
            prot_mode286 = 1;
        #elif defined( __OS2_286__ )
            prot_mode286 = _RWD_osmode;
        #else
            prot_mode286 = 0;
        #endif
        if( name == NULL ) {
            name = prot_mode286 ? STRING( "CMD.EXE" ) : STRING( "COMMAND.COM" );
        }

        #if defined( __NT__ )
            /* disable file handle inheritance for a system call */
            tmp_fileinfo = _fileinfo;
            _fileinfo = 0;
        #endif

        ret_code = __F_NAME(spawnlp,_wspawnlp)( 0, name, prot_mode286 ? STRING( "CMD" ) : STRING( "COMMAND" ),
                            __F_NAME(__Slash_C,__wSlash_C)(switch_c, prot_mode286), cmd, NULL );

        #if defined( __NT__ )
            /* set file handle inheritance to what it was */
            _fileinfo = tmp_fileinfo;
        #endif

        return( ret_code );
    #endif
}
