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


#include "plusplus.h"
#include <sys/types.h>
#include "wio.h"
#include "macro.h"
#include "cmdline.h"
#include "vbuf.h"


                                    // Static Data (between compilations)
static char batch_buffer[ 1024 ];   // - buffer
static int batch_file;              // - # for batch file
static unsigned batch_bytes;        // - # bytes left in buffer
static char* batch_cursor;          // - cursor thru batch_buffer
static bool real_eof;               // - true ==> a real EOF detected


static void cmdLnBatchClose(    // CLOSE BATCH FILE
    void )
{
    if( batch_file != -1 ) {
        close( batch_file );
        batch_file = -1;
        CompFlags.batch_file_eof = true;
    }
}


static int cmdLnBatchReadBuffer(// READ A BUFFER
    void )
{
    int retn = read( batch_file, batch_buffer, sizeof( batch_buffer ) );
    if( retn == -1 ) {
        BadCmdLine( ERR_FC_READ );
        real_eof = false;
        cmdLnBatchClose();
    } else if( retn == 0 ) {
        real_eof = true;
        cmdLnBatchClose();
    } else {
        batch_cursor = batch_buffer;
        batch_bytes = retn;
    }
    return retn;
}


static int nextChar(            // GET NEXT CHARACTER
    void )
{
    int retn;                   // - next character

    for( ; ; ) {
        if( batch_bytes == 0 ) {
            cmdLnBatchReadBuffer();
        }
        if( CompFlags.batch_file_eof ) {
            retn = 0;
            break;
        } else {
            retn = *batch_cursor;
            ++ batch_cursor;
            -- batch_bytes;
            if( retn != '\r' ) break;
            if( *batch_cursor == '\n' ) {
                continue;
            } else {
                retn = '\n';
            }
            break;
        }
    }
    return retn;
}


static void cmdLnBatchFlushCrap(// FLUSH CARRIAGE-RETURNS, ETC
    void )
{
    int chr;                    // - next char

    if( ! CompFlags.batch_file_eof ) {
        for( ; ; ) {
            chr = nextChar();
            if( CompFlags.batch_file_eof ) break;
            if( chr == '\n' ) {
                ++ CompInfo.fc_file_line;
                continue;
            }
            -- batch_cursor;
            *batch_cursor = chr;
            ++ batch_bytes;
            break;
        }
    }
}


void CmdLnBatchOpen(            // OPEN CMD-LN BATCH FILE
    char const * fname )        // - file name
{
    int retn = open( fname, O_RDONLY | O_BINARY );
    if( retn == -1 ) {
        CompFlags.batch_file_eof = true;
        BadCmdLine( ERR_FC_OPEN );
    } else {
        batch_file = retn;
        batch_bytes = 0;
        CompFlags.batch_file_eof = false;
        CompInfo.fc_file_line = 0;
        cmdLnBatchFlushCrap();
        if( real_eof ) {
            BadCmdLine( ERR_FC_EMPTY );
        }
    }
}


bool CmdLnBatchRead(            // READ NEXT LINE IN COMMAND BUFFER
    VBUF *buf )                 // - virtual buffer
{
    VbufInit( buf );
    for(;;) {
        int c = nextChar();
        if( CompFlags.batch_file_eof ) break;
        if( c == '\n' ) break;
        VbufConcChr( buf, c );
    }
    DbgVerify( VbufLen( buf ) > 0, "CmdLnReadBatch -- nothing" );
    ++ CompInfo.fc_file_line;
    return( VbufLen( buf ) );
}


void CmdLnBatchFreeRecord(      // FREE A CMD-LN RECORD
    VBUF *buf )                 // - virtual buffer
{
    VbufFree( buf );
    cmdLnBatchFlushCrap();
}


void CmdLnBatchAbort(           // ABANDON BATCH-FILE PROCESSING
    void )
{
    cmdLnBatchClose();
}
