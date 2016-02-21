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
* Description:  Dump window contents to a file.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "dbgadget.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgwdlg.h"
#include "dlgfile.h"
#include "wndsys.h"
#include "wnddump.h"


typedef void WRITERTN( file_handle, const char * );

static void WriteFile( file_handle fh, const char *buff )
{
    WriteText( fh, buff, strlen( buff ) );
}

static void WriteLog( file_handle fh, const char *buff )
{
    fh = fh;
    WndDlgTxt( buff );
}

static void DoWndDump( a_window *wnd, WRITERTN *rtn, file_handle fh )
{
    int                 row;
    int                 piece;
    wnd_line_piece      line;
    int                 indent_per_char;
    int                 chars_written;
    int                 indent_pos;
    char                *font;
    gui_ord             gadget_len;
    char                *p;
    char                buff[TXT_LEN];
//    int                 len;
    int                 i;

    font = WndGetFontInfo( wnd );
    gadget_len = MaxGadgetLength;
    MaxGadgetLength = ( strlen( WndGadgetArray[0].chars ) + 1 ) * WndAvgCharX( wnd );
    WndSetSysFont( wnd, true );
    indent_per_char = WndAvgCharX( wnd );
//    len = WndGetTitle( wnd, buff, TXT_LEN );
    WndGetTitle( wnd, buff, TXT_LEN );
    p = TxtBuff;
    for( i = 0; i < 7; ++i ) {
        *p++ = '=';
    }
    *p++ = '|';
    *p++ = ' ';
    p = StrCopy( buff, p );
    *p++ = ' ';
    *p++ = '|';
    for( i = 0; i < 7; ++i ) {
        *p++ = '=';
    }
    *p = NULLCHAR;
    rtn( fh, TxtBuff );
    for( row = -WndTitleSize( wnd );; ++row ) {
        p = buff;
        chars_written = 0;
        for( piece = 0; ; ++piece ) {
            if( !WndGetLineAbs( wnd, row, piece, &line ) )
                break;
            indent_pos = line.indent / indent_per_char;
            while( indent_pos > chars_written ) {
                p = StrCopy( " ", p );
                ++chars_written;
            }
            if( line.bitmap ) {
                line.text = WndGadgetArray[(int)line.text[0]].chars;
                line.length = strlen( line.text );
            }
            p = StrCopy( line.text, p );
            chars_written += line.length;
        }
        if( piece == 0 )
            break;
        rtn( fh, buff );
    }
    MaxGadgetLength = gadget_len;
    if( font != NULL )
        WndSetFontInfo( wnd, font );
    GUIMemFree( font );
}


static void DoWndDumpFile( const char *name, a_window *wnd )
{
    file_handle     fh;

    if( wnd == NULL )
        return;
    fh = FileOpen( name, OP_WRITE | OP_CREATE | OP_TRUNC );
    if( fh == NIL_HANDLE ) {
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), name );
    }
    DoWndDump( wnd, WriteFile, fh );
    FileClose( fh );
}

void WndDumpPrompt( a_window *wnd )
{
    char                *name;

    name = GetDmpName();
    if( name == NULL )
        return;
    DoWndDumpFile( name, wnd );
}

void WndDumpFile( a_window *wnd )
{
    const char          *start;
    size_t              len;
    bool                got;

    got = ScanItem( true, &start, &len );
    ReqEOC();
    if( !got ) {
        WndDumpPrompt( wnd );
    } else {
        memcpy( TxtBuff, start, len );
        TxtBuff[len] = NULLCHAR;
        DoWndDumpFile( TxtBuff, wnd );
    }
}

void WndDumpLog( a_window *wnd )
{
    DoWndDump( wnd, WriteLog, 0 );
}
