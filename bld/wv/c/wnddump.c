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
#include "dbgwind.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "dbgadget.h"
#include <string.h>


extern char             *GetDmpName( void );
extern void             ReqEOC( void );
extern bool             ScanItem( bool, char **, unsigned int * );
extern bool             WndDlgTxt( char *buff );
extern char             *StrCopy( char *src, char *dest );

extern gui_resource     WndGadgetArray[];
extern char             *TxtBuff;


typedef void WRITERTN( handle, char * );

static void WriteFile( handle file, char *buff )
{
    WriteText( file, buff, strlen( buff ) );
}

static void WriteLog( handle dummy, char *buff )
{
    dummy = dummy;
    WndDlgTxt( buff );
}

static void DoWndDump( a_window *wnd, WRITERTN *rtn, handle file )
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
    MaxGadgetLength = ( strlen( WndGadgetArray[ 0 ].chars ) + 1 ) * WndAvgCharX( wnd );
    WndSetSysFont( wnd, TRUE );
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
    *p = '\0';
    rtn( file, TxtBuff );
    for( row = -WndTitleSize( wnd );; ++row ) {
        p = buff;
        chars_written = 0;
        for( piece = 0; ; ++piece ) {
            if( !WndGetLineAbs( wnd, row, piece, &line ) ) break;
            indent_pos = line.indent / indent_per_char;
            while( indent_pos > chars_written ) {
                p = StrCopy( " ", p );
                ++chars_written;
            }
            if( line.bitmap ) {
                line.text = WndGadgetArray[ (int)line.text[0] ].chars;
                line.length = strlen( line.text );
            }
            p = StrCopy( line.text, p );
            chars_written += line.length;
        }
        if( piece == 0 ) break;
        rtn( file, buff );
    }
    MaxGadgetLength = gadget_len;
    if( font != NULL ) WndSetFontInfo( wnd, font );
    GUIMemFree( font );
}


static void DoWndDumpFile( char *name, a_window *wnd )
{
    handle              file;

    if( wnd == NULL ) return;
    file = FileOpen( name, OP_WRITE | OP_CREATE | OP_TRUNC );
    if( file == NIL_HANDLE ) {
        Error( ERR_NONE, LIT( ERR_FILE_NOT_OPEN ), name );
    }
    DoWndDump( wnd, WriteFile, file );
    FileClose( file );
}

void WndDumpPrompt( a_window *wnd )
{
    char                *name;

    name = GetDmpName();
    if( name == NULL ) return;
    DoWndDumpFile( name, wnd );
}

void WndDumpFile( a_window *wnd )
{
    char                *start;
    unsigned            len;
    bool                got;

    got = ScanItem( TRUE, &start, &len );
    ReqEOC();
    if( !got ) {
        WndDumpPrompt( wnd );
    } else {
        memcpy( TxtBuff, start, len );
        TxtBuff[ len ] = '\0';
        DoWndDumpFile( TxtBuff, wnd );
    }
}

void WndDumpLog( a_window *wnd )
{
    DoWndDump( wnd, WriteLog, 0 );
}
