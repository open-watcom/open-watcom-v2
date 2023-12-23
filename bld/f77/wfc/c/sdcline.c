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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//
// SDCLINE   : command line parsing
//

#include "ftnstd.h"
#include "global.h"
#include "cpopt.h"
#include "sdcline.h"
#include "option.h"


#if defined( __UNIX__ )
  #define _IsSwitchChar( ch )     ( ch == '-' )
#else
  #define _IsSwitchChar( ch )     ( ( ch == '/' ) || ( ch == '-' ) )
#endif


bool MainCmdLine( char **fn, char **rest, char **opts, char *cmd )
//================================================================
{
    uint        opt_num;
    bool        scanning_file_name;
    bool        quoted;

    *fn = NULL;
    *rest = NULL;
    opt_num = 0;
    for(;;) {
        scanning_file_name = false;
        quoted = false;
        cmd = SkipBlanks( cmd );
        if( *cmd == NULLCHAR )
            break;
        if( _IsSwitchChar( *cmd ) ) {
            *cmd++ = NULLCHAR;    // terminate previous option or filename
            if( opt_num < MAX_OPTIONS ) {
                *opts++ = cmd;
            }
            ++opt_num;
        } else if( *fn == NULL ) {
            *fn = cmd;
            scanning_file_name = true;
        } else {
            *rest = cmd;
            break;
        }
        for( ; *cmd != NULLCHAR; cmd++ ) {
            if( quoted ) {
                if( *cmd == '\"' ) {
                    quoted = false;
                    if( scanning_file_name ) {
                        *cmd = NULLCHAR;
                    }
                }
            } else if( *cmd == '\"' ) {
                quoted = true;
                if( scanning_file_name ) {
                    *fn = cmd + 1;
                }
            } else if( ( *cmd == ' ' ) || ( *cmd == '\t' ) ) {
                *cmd++ = NULLCHAR;
                break;
            }
            if( !scanning_file_name && !quoted ) {
                if( _IsSwitchChar( *cmd ) ) {
                    break;
                }
            }
        }
    }
    *opts = NULL;
    return( (*fn != NULL) && (opt_num <= MAX_OPTIONS) && (*rest == NULL) );
}
