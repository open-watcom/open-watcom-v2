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


#include <assert.h>
#include <string.h>

#include "idedll.h"


void IdeMsgInit                 // INITIALIZE MSG STRUCTURE
    ( IDEMsgInfo* info          // - message information
    , IDEMsgSeverity severity   // - message severity
    , char const * msg )        // - the message
{
    info->severity = severity;
    info->msg = msg;
    info->flags = 0;
    info->helpfile = 0;
    info->helpid = 0;
    info->src_symbol = 0;
    info->src_file = 0;
    info->src_line = 0;
    info->src_col = 0;
    info->msg_no = 0;
    info->msg_group[0] = '\0';
}


void IdeMsgSetReadable          // MARK MSG AS "READABLE"
    ( IDEMsgInfo* info )        // - message information
{
    info->flags |= IDE_INFO_READABLE;
}


void IdeMsgSetMsgNo             // SET MESSAGE NUMBER
    ( IDEMsgInfo* info          // - message information
    , unsigned msg_no )         // - message number
{
    info->msg_no = msg_no;
    info->flags |= IDE_INFO_HAS_MSG_NUM;
}


void IdeMsgSetMsgGroup          // SET MESSAGE GROUP
    ( IDEMsgInfo* info          // - message information
    , char const *group )       // - group name
{
    size_t len = strlen( group );
    if( len > sizeof( info->msg_group ) - 1 ) {
        len = sizeof( info->msg_group ) - 1;
    }
    info->msg_group[ len ] = '\0';
    info->flags |= IDE_INFO_HAS_MSG_GRP;
    memcpy( info->msg_group, group, len );
}


void IdeMsgSetLnkFile           // SET LINK FILE
    ( IDEMsgInfo* info          // - message information
    , char const * file )       // - file name
{
    info->src_file = file;
    info->flags |= IDE_INFO_HAS_LNK_FILE;
}


void IdeMsgSetLnkSymbol         // SET LINK SYMBOL
    ( IDEMsgInfo* info          // - message information
    , char const * sym )        // - symbol
{
    info->src_symbol = sym;
    info->flags |= IDE_INFO_HAS_LNK_SYMBOL;
}


void IdeMsgSetSrcFile           // SET SOURCE FILE
    ( IDEMsgInfo* info          // - message information
    , char const * file )       // - file name
{
    info->src_file = file;
    info->flags |= IDE_INFO_HAS_SRC_FILE;
}


void IdeMsgSetSrcLine           // SET SOURCE LINE
    ( IDEMsgInfo* info          // - message information
    , unsigned line )           // - line number
{
    info->src_line = line;
    info->flags |= IDE_INFO_HAS_SRC_LINE;
}


void IdeMsgSetSrcColumn         // SET SOURCE COLUMN
    ( IDEMsgInfo* info          // - message information
    , unsigned col )            // - column number
{
    info->src_col = col;
    info->flags |= IDE_INFO_HAS_SRC_COL;
}


void IdeMsgSetHelp              // SET HELP INFORMATION
    ( IDEMsgInfo* info          // - message information
    , char const * file         // - help file
    , unsigned long id )        // - help id
{
    info->helpfile = file;
    info->helpid = id;
    info->flags |= IDE_INFO_HAS_HELP;
}
