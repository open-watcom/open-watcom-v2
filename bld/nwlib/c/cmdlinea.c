/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Librarian command line parsing (AR mode).
*
****************************************************************************/


#include "wlib.h"
#include "cmdlinea.h"
#include "cmdline.h"


static bool ParseOption( operation *ar_mode )
{
    const char  *start;
    int         ch;

    start = CmdGetPos();
    for( ; (ch = CmdPeekChar()) != '\0' && !isspace( ch ); CmdGetChar() ) {
        switch( tolower( ch ) ) {
        case '?':
            Usage();
            break;
        case 'c':
            if( Options.no_create_warn ) {
                FatalError( ERR_DUPLICATE_OPTION, start );
            }
            Options.no_create_warn = true;
            break;
        case 'd':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, ch );
            }
            *ar_mode = OP_DELETE;
            break;
        case 'p':
            //ignore not implemented
            break;
        case 'r':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, ch );
            }
            *ar_mode = OP_ADD | OP_DELETE;
            break;
        case 't':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, ch );
            }
            *ar_mode = OP_TABLE;
            Options.list_contents = true;
            break;
        case 'u':
            if( Options.update ) {
                FatalError( ERR_DUPLICATE_OPTION, start );
            }
            Options.update = true;
            break;
        case 'v':
            if( Options.verbose ) {
                FatalError( ERR_DUPLICATE_OPTION, start );
            }
            Options.verbose = true;
            break;
        case 'x':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, ch );
            }
            *ar_mode = OP_EXTRACT;
            break;
        case '-':
            break;
        default:
            FatalError( ERR_BAD_OPTION, ch );
        }
    }
    return( CmdGetPos() != start );
}

void ParseOneLineAr( const char *cmd, operation *ar_mode )
{
    const char  *old_cmd;
    bool        done_options;

    old_cmd = CmdSetPos( cmd );
    done_options = false;
    for( ;; ) {
        CmdSkipWhite();
        switch( CmdPeekChar() ) {
        case '\0':
            if( *ar_mode == OP_EXTRACT ) {
                Options.explode = true;
            }
            CmdSetPos( old_cmd );
            return;
        case '-':
            if( !done_options ) {
                CmdGetChar();           /* skip '-' character */
                if( CmdRecogChar( '-' ) ) {
                    done_options = true;
                    break;
                }
                ParseOption( ar_mode );
                break;
            }
            /* fall through */
        default:
            if( *ar_mode == OP_NONE ) {
                ParseOption( ar_mode );
                break;
            }
            if( Options.input_name == NULL ) {
                Options.input_name = GetFilenameExt( SCTRL_NORMAL, EXT_LIB );
            } else {
                AddCommand( *ar_mode, SCTRL_SINGLE );
            }
            break;
        }
    }
}
