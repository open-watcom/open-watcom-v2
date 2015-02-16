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


//
// SDCLINE   : command line parsing
//

#include "ftnstd.h"
#include "global.h"
#include "cpopt.h"

extern  char            *SkipBlanks(char *);

#if defined( __UNIX__ )
  #define _IsSwitchChar( ch )     ( ch == '-' )
#else
  #define _IsSwitchChar( ch )     ( ( ch == '/' ) || ( ch == '-' ) )
#endif


bool    ParseCmdLine( char **fname, char **rest, char **opt_array, char *p ) {
//============================================================================

    fname = fname; rest = rest; p = p;
    *opt_array = NULL;
    return( TRUE );
}


bool    MainCmdLine( char **fn, char **rest, char **opts, char *ptr ) {
//=====================================================================

    uint        opt_num;
    bool        scanning_file_name;
    bool        quoted;

    *fn = NULL;
    *rest = NULL;
    opt_num = 0;
    for(;;) {
        scanning_file_name = FALSE;
        quoted = FALSE;
        ptr = SkipBlanks( ptr );
        if( *ptr == NULLCHAR ) break;
        if( _IsSwitchChar( *ptr ) ) {
            *ptr = NULLCHAR;    // terminate previous option or filename
            ++ptr;
            if( opt_num < MAX_OPTIONS ) {
                *opts = ptr;
                ++opts;
            }
            ++opt_num;
        } else if( *fn == NULL ) {
            *fn = ptr;
            scanning_file_name = TRUE;
        } else {
            *rest = ptr;
            break;
        }
        for(;;) {
            if( *ptr == NULLCHAR )
                break;
            if( quoted ) {
                if( *ptr == '\"' ) {
                    quoted = FALSE;
                    if(scanning_file_name)
                        *ptr = NULLCHAR;
                }
            } else if( *ptr == '\"' ) {
                quoted = TRUE;
                if(scanning_file_name)
                    *fn = ptr+1;
            } else if( ( *ptr == ' ' ) || ( *ptr == '\t' ) ) {
                *ptr = NULLCHAR;
                ++ptr;
                break;
            }
            if( !scanning_file_name && !quoted ) {
                if( _IsSwitchChar( *ptr ) ) {
                    break;
                }
            }
            ++ptr;
        }
    }
    *opts = NULL;
    return( (*fn != NULL) && (opt_num <= MAX_OPTIONS) && (*rest == NULL) );
}


char    *Batch( char *buffer, uint num ) {
//========================================

    if( num > 0 ) {
        buffer = NULL;
    }
    return( buffer );
}
