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
#include "cmdlineb.h"
#include "cmdline.h"
#include "cmdlnprs.h"
#include "cmdscan.h"


#include "cmdlprsa.gc"
#include "cmdlprsb.gc"


static bool check_ar_mode( OPT_STORAGE_A *data )
{
    return( data->d || data->r || data->t || data->x );
}

void ParseOneLineAr( const char *cmd, OPT_STORAGE_A *data, bool comment )
{
    const char  *old_cmd;
    bool        done_options;

    /* unused parameters */ (void)comment;

    old_cmd = CmdScanLineInit( cmd );
    done_options = false;
    for( ;; ) {
        CmdScanSkipWhiteSpace();
        option_start = CmdScanAddr();
        switch( CmdPeekChar() ) {
        case '\0':
            CmdScanLineInit( old_cmd );
            return;
        case '-':
            if( !done_options ) {
                CmdScanChar();           /* skip '-' character */
                if( CmdRecogChar( '-' ) ) {
                    done_options = true;
                    break;
                }
                OPT_PROCESS_A( data );
                break;
            }
            /* fall through */
        default:
            if( !check_ar_mode( data ) ) {
                OPT_PROCESS_B( data );
                break;
            }
            if( Options.input_name == NULL ) {
                Options.input_name = GetFilenameExt( EXT_LIB );
            } else {
                AddCommand( OP_NONE );
            }
            break;
        }
    }
}

void SetOptionsAr( OPT_STORAGE_A *data )
{
    operation   ar_mode;
    int         cmd_count;

    cmd_count = 0;
    ar_mode = OP_NONE;

    if( data->_question ) {
        Usage();
    }
    if( data->c ) {
        Options.no_create_warn = true;
    }
    if( data->d ) {
        ar_mode |= OP_DELETE;
        cmd_count++;
    }
    if( data->r ) {
        ar_mode |= OP_ADD | OP_DELETE;
        cmd_count++;
    }
    if( data->t ) {
        ar_mode |= OP_TABLE;
        cmd_count++;
        Options.list_contents = true;
    }
    if( data->u ) {
        Options.update = true;
    }
    if( data->v ) {
        Options.verbose = true;
    }
    if( data->x ) {
        ar_mode |= OP_EXTRACT;
        cmd_count++;
    }

    if( cmd_count > 1 ) {
        FatalError( ERR_BAD_CMDLINE, "commands mixing" );
    } else {
        lib_cmd     *cmd;

        for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
            cmd->ops = ar_mode;
        }
    }

    if( CmdList == NULL ) {
        if( ar_mode == OP_EXTRACT ) {
            Options.explode = true;
        } else {
            if( !Options.list_contents ) {
                /* Default action: List the input lib */
                Options.list_contents = true;
                Options.list_file = MemDupStr( "" );
            }
        }
    }
}
