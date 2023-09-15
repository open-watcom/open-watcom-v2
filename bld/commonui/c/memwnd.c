/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Memory dump display window.
*
****************************************************************************/


#include "commonui.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include "bool.h"
#include "watcom.h"
#include "wpi.h"
#include "font.h"
#include "segmem.h"
#include "cguimem.h"
#include "dlgmod.h"
#include "descript.h"
#include "savelbox.h"
#include "memwnd.h"
#include "deasm.h"
#include "ldstr.h"
#include "uistr.grh"
#ifndef NOUSE3D
    #include "ctl3dcvr.h"
#endif


/* Window callback functions prototypes */
WINEXPORT INT_PTR CALLBACK OffsetDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT LRESULT CALLBACK MemDisplayProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT INT_PTR CALLBACK SegInfoDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#define ISCODE( x )     ((x)->disp_type == MEMINFO_CODE_16 || \
                        (x)->disp_type == MEMINFO_CODE_32)

#define WPI_GET_WNDINFO( w )    ((MemWndInfo *)_wpi_getwindowlongptr( w, 0 ))
#define WPI_SET_WNDINFO( w, d ) (_wpi_setwindowlongptr( w, 0, d ))

#define MAX_BYTES       50

/*
 * NO_DATA_SPACE consists of 11 bytes to display the offsets on the left
 * and 1 additional byte so that italic fonts don't get cut off on the right
 */
#define NO_DATA_SPACE   12

int                     FontWidth = 8;
int                     FontHeight = 15;

static  HFONT           CurFont;
static  HWND            CurWindow;
static  char            Buffer[MAX_BYTES * 4 + 20];
#ifndef __NT__
static  DLGPROC         DialProc;
static  unsigned        DialCount;
#endif

static MemWndConfig     MemConfigInfo;

static msg_id Disp_Types[] = {
    MWND_DISP_BYTES,
    MWND_DISP_WORDS,
    MWND_DISP_DWORDS,
    MWND_DISP_CODE_16,
    MWND_DISP_CODE_32
};

/* forward declarations */
static void calcTextDimensions( HWND hwnd, HDC dc, MemWndInfo *info );
static void displaySegInfo( HWND parent, HANDLE instance, MemWndInfo *info );
static void positionSegInfo( HWND hwnd );
static bool genLine( unsigned char digits, DWORD limit, ctl_id disp_type, WORD sel, char *buf, DWORD offset );

typedef enum {
    MT_FREE,
    MT_RESERVE,
    MT_UNREADABLE,
    MT_READABLE
} MemType;

#ifdef __NT__

static HANDLE   ProcessHdl;
static DWORD    CurLimit;
static DWORD    CurBase;


/*
 * GetASelectorLimit
 */
DWORD GetASelectorLimit( WORD sel )
{
    sel = sel;
    return( CurLimit );

} /* GetASelectorLimit */

/*
 * ReadMem
 */
ULONG_PTR ReadMem( WORD sel, ULONG_PTR off, void *buff, ULONG_PTR size )
{
    ULONG_PTR   bytesread;

    sel = sel;
    bytesread = 0;
    size++;
    while( bytesread != size && size != 0 ) {
        size--;
        ReadProcessMemory( ProcessHdl, (LPCSTR)(pointer_uint)off, buff, size, &bytesread );
    }
    return( bytesread );

} /* ReadMem */

#else

/*
 * createAccessString
 */
static void createAccessString( char *ptr, descriptor *desc )
{
    if( desc->u1.flags.nonsystem && !desc->u1.flags.execute )  {
        *ptr++ =  'R';
        if( desc->u1.flags_data.writeable ) {
            *ptr++ = '/';
            *ptr++ = 'W';
        }
    } else {
        *ptr++ = 'E';
        *ptr++ = 'x';
        if( desc->u1.flags_exec.readable ) {
            *ptr++ = '/';
            *ptr++ = 'R';
        }
    }
    *ptr = '\0';

} /* createAccessString */

#endif

/*
 * memDumpHeader - put summary information at the top of a memory dump
 */
static void memDumpHeader( int hdl, MemWndInfo *info )
{
    time_t              tm;
    char                buf[80];
    unsigned            len;
    unsigned            type_index;
    char                *rcstr;
#ifndef __NT__
    GLOBALENTRY         ge;
    descriptor          desc;
    char                access[20];
#endif

    tm = time( NULL );
    RCsprintf( buf, MWND_MEM_DMP_CREATED, asctime( localtime( &tm ) ), &len );
    write( hdl, buf, len );

#ifndef __NT__
    if( info->isdpmi ) {
        rcstr = AllocRCString( MWND_DPMI_ITEM );
        len = strlen( rcstr );
        write( hdl, rcstr, len );
        FreeRCString( rcstr );
        GetADescriptor( info->sel, &desc );
        RCsprintf( buf, MWND_SELECTOR, info->sel, &len );
        write( hdl, buf, len );
        RCsprintf( buf, MWND_BASE, GET_DESC_BASE( desc ), &len );
        write( hdl, buf, len );
        RCsprintf( buf, MWND_LIMIT, GET_DESC_LIMIT( desc), &len );
        write( hdl, buf, len );
        if( desc.u1.flags.nonsystem && !desc.u1.flags.execute )  {
            RCsprintf( buf, MWND_TYPE_DATA, &len );
        } else {
            RCsprintf( buf, MWND_TYPE_CODE, &len );
        }
        write( hdl, buf, len );
        sprintf( buf, "DPL:         \t%1d\n%n", desc.u1.flags.dpl, &len );
        write( hdl, buf, len );
        if( desc.u2.flags.page_granular ) {
            RCsprintf( buf, MWND_GRANULARITY_PAGE, &len );
        } else {
            RCsprintf( buf, MWND_GRANULARITY_BYTE, &len );
        }
        write( hdl, buf, len );
        createAccessString( access, &desc );
        RCsprintf( buf, MWND_ACCESS, access, &len );
        write( hdl, buf, len );
    } else {
        ge.dwSize = sizeof( GLOBALENTRY );
        GlobalEntryHandle( &ge, (HGLOBAL)info->sel );
        RCsprintf( buf, MWND_BLOCK_ADDR, ge.dwAddress, &len );
        write( hdl, buf, len );
        RCsprintf( buf, MWND_BLOCK_HDL, ge.hBlock, &len );
        write( hdl, buf, len );
        RCsprintf( buf, MWND_BLOCK_SIZE, ge.dwBlockSize, &len );
        write( hdl, buf, len );
        RCsprintf( buf, MWND_LOCK_CNT, ge.wcLock, &len );
        write( hdl, buf, len );
        RCsprintf( buf, MWND_PAGE_LOCK_CNT, ge.wcPageLock, &len );
        write( hdl, buf, len );
    }
#endif

    type_index = info->disp_type - MEMINFO_BYTE;
    rcstr = AllocRCString( Disp_Types[type_index] );
    sprintf( buf, "%s\n\n%n", rcstr, &len );
    FreeRCString( rcstr );
    write( hdl, buf, len );

} /* MemDumpHeader */

/*
 * MemSave - save the contents of a memory display box
 */
static void MemSave( MemWndInfo *info, HWND hwnd, bool gen_name )
{
    char        fname[_MAX_PATH];
//  OFSTRUCT    finfo;
    DWORD       offset;
    DWORD       limit;
    int         hdl;
    unsigned    len;
    bool        ok;
    HCURSOR     hourglass;
    HCURSOR     oldcursor;

    if( gen_name ) {
        ok = GenTmpFileName( MemConfigInfo.fname, fname );
        if( !ok ) {
            ReportSave( hwnd, fname, MemConfigInfo.appname, ok );
        }
    } else {
        ok = GetSaveFName( hwnd, fname );
    }
    if( ok ) {
//      hdl = OpenFile( fname, &finfo, OF_CREATE | OF_WRITE );
        hdl = open( fname, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE );
        if( hdl == -1 ) {
            ok = false;
        } else {
            hourglass = LoadCursor( NULLHANDLE, IDC_WAIT );
            SetCapture( hwnd );
            oldcursor= SetCursor( hourglass );
            memDumpHeader( hdl, info );
            if( ISCODE( info ) ) {
                DumpMemAsm( info, hdl );
            } else {
                offset = info->base;
                limit = info->limit;
                while( offset < limit ) {
                    genLine( 16, limit, info->disp_type, info->sel, Buffer, offset );
                    len = (unsigned)strlen( Buffer );
                    write( hdl, Buffer, len );
                    write( hdl, "\n", 1 );
                    offset += 16;
                }
            }
            close( hdl );
            SetCursor( oldcursor );
            ReleaseCapture();
        }
        ReportSave( hwnd, fname, MemConfigInfo.appname, ok );
    }

} /* MemSave */

/*
 * RegMemWndClass - must be called by the first instance of a using
 *                  program to register window classes
 */
bool RegMemWndClass( HANDLE instance )
{
    WNDCLASS    wc;

    wc.style = 0L;
    wc.lpfnWndProc = GetWndProc( MemDisplayProc );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = instance;
    wc.hIcon = NULLHANDLE;
    wc.hCursor = LoadCursor( NULLHANDLE, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(pointer_uint)(COLOR_WINDOW + 1);
    wc.lpszMenuName = "MEMINFOMENU";
    wc.lpszClassName = MEM_DISPLAY_CLASS;
    return( RegisterClass( &wc ) != 0 );

} /* RegMemWndClass */

/*
 * SetMemWndConfig - set memory display window configuration info
 */
void SetMemWndConfig( MemWndConfig *cfg )
{
   MemConfigInfo = *cfg;
   MemConfigInfo.init = true;

} /* SetMemWndConfig */

/*
 * GetMemWndConfig - get memory display window configuration info
 */
void GetMemWndConfig( MemWndConfig *cfg )
{
    *cfg = MemConfigInfo;

} /* GetMemWndConfig */

/*
 * GetMemWndDefault
 */
void GetMemWndDefault( MemWndConfig *cfg )
{
    cfg->init = true;
    cfg->xpos = 0;
    cfg->ypos = 0;
    cfg->ysize = GetSystemMetrics( SM_CYSCREEN ) / 5;
    cfg->xsize = GetSystemMetrics( SM_CXSCREEN );
    cfg->disp_info = true;
    cfg->maximized = false;
    cfg->allowmult = WND_MULTI;
    strcpy( cfg->fname, "mem.txt" );
    cfg->appname = "";
    cfg->autopos_info = true;
    cfg->forget_pos = false;
    cfg->data_type = MEMINFO_BYTE;
    cfg->code_type = MEMINFO_CODE_16;

} /* GetMemWndDefault */

/*
 * SetDefMemConfig
 */
void SetDefMemConfig( void )
{
    GetMemWndDefault( &MemConfigInfo );

} /* SetDefMemConfig */

/*
 * MkHexDigit
 */
char MkHexDigit( char ch )
{
    if( (ch &= 0xF) < 0xA ) {
        return( '0' + ch );
    }
    return( 'A' + ch - 0xA );

} /* MkHexDigit */

/*
 * genByte
 */
static char *genByte( char ch, char *ptr )
{
    *ptr++ = MkHexDigit( ch >> 4 );
    *ptr++ = MkHexDigit( ch );
    return( ptr );

} /* genByte */

/*
 * genLine - create a line for output of the form:
XXXXXXXX  dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd  cccccccccccccccc
 */
static bool genLine( unsigned char digits, DWORD limit, ctl_id disp_type,
                     WORD sel, char *buf, DWORD offset )
{
    char        *ptr;
    char        pad_char;
    char        data[MAX_BYTES];
    size_t      len;
    unsigned    i;

#ifdef __NT__
    if( ReadMem( sel, offset, data, digits ) == 0 ) {
        return( false );
    }
    pad_char = ' ';
#else
    ReadMem( sel, offset, data, digits );
    pad_char = ' ';
#endif

    sprintf( buf, "%08lX  ", offset );
    ptr = buf + 10;
    switch( disp_type ) {
    case MEMINFO_BYTE:
        for( i = 0; i < digits; i++ ) {
            if( offset + i >= limit ) {
               *ptr++ = pad_char;
               *ptr++ = pad_char;
            } else {
                ptr = genByte( data[i], ptr );
            }
            *ptr++ = ' ';
        }
//      if( i < digits ) {
//          len = ( digits - i ) * 3;
//          memset( ptr, ' ', len );
//          ptr += len;
//      }
        break;
    case MEMINFO_WORD:
        for( i = 0; i < digits; i += 2 ) {
            if( offset + i == limit -1 ) {
                ptr = genByte( data[i], ptr );
                memset( ptr, ' ', 3 );
                ptr += 3;
                i += 2;
            }
            if( offset + i >= limit ) {
                break;
            }
            ptr = genByte( data[i + 1], ptr );
            ptr = genByte( data[i], ptr );
            *ptr++ = ' ';
        }
        if( i < digits ) {
            len = ((digits - i) / 2) * 5;
            memset( ptr, ' ', len );
            ptr += len;
        }
        break;
    case MEMINFO_DWORD:
        for( i = 0; i < digits; i += 4 ) {
            if( offset + i < limit && offset + i > limit - 4 ) {
                len = 0;
                for( ; offset + i < limit; i++ ) {
                    ptr = genByte( data[i], ptr );
                    *ptr++ = ' ';
                    len += 3;
                }
                memset( ptr, ' ', 9 - len );
                ptr += 9 - len;
                i += 4;
            }
            if( offset + i >= limit ) {
                break;
            }
            ptr = genByte( data[i + 3], ptr );
            ptr = genByte( data[i + 2], ptr );
            ptr = genByte( data[i + 1], ptr );
            ptr = genByte( data[i], ptr );
            *ptr++ = ' ';
        }
        if( i < digits ) {
            len = ((digits - i) / 4) * 9;
            memset( ptr, ' ', len );
            ptr += len;
        }
        break;
    }
    *ptr++ = ' ';
    for( i = 0; i < digits; i++ ) {
        unsigned char    c;

        if( offset + i >= limit ) {
            memset( ptr, ' ', digits - i );
            ptr += digits - i;
            break;
        }
        c = data[i];
        if( isalnum( c ) || ispunct( c ) ) {
            *ptr++ = c;
        } else {
            *ptr++ = '.';
        }
    }
    *ptr = '\0';
    return( true );

} /* genLine */

/*
 * mySetScrollPos
 */
static void mySetScrollPos( MemWndInfo *info, DWORD offset )
{
    DWORD       val;

    val = ((offset - info->base) * SCROLL_RANGE) / (info->limit - info->base);
    SetScrollPos( info->scrlbar, SB_CTL, val, TRUE );

} /* mySetScrollPos */

/*
 * scrollPosToOffset
 */
static DWORD scrollPosToOffset( WORD pos, MemWndInfo *info )
{
    DWORD       offset;

    offset = (pos * (info->limit - info->base)) / SCROLL_RANGE;
    offset += info->base;
    return( offset );

} /* scrollPosToOffset */

/*
 * redrawMemWnd
 */
static void redrawMemWnd( HWND hwnd, HDC dc, MemWndInfo *info )
{
    unsigned    i;
    DWORD       offset;
    RECT        area;
    RECT        fill;
    HBRUSH      wbrush;
    HFONT       old_font;
    SIZE        txtsize;
    bool        rc;

    if( CurFont != GetMonoFont() ) {
        calcTextDimensions( hwnd, dc, info );
    }
    old_font = SelectObject( dc, CurFont );
    if( ISCODE( info ) ) {
        mySetScrollPos( info, (info->limit - info->base) / 2 );
        RedrawAsCode( dc, info );
        return;
    }
    i = 0;
    area.left = 0;
    area.top = 0;
    area.right = info->width;
    area.bottom = FontHeight;
    fill.right = area.right;
    offset = info->offset;
    wbrush = GetStockObject( WHITE_BRUSH );
    while( offset < info->limit ) {
        rc = genLine( info->bytesdisp, info->limit, info->disp_type,
                      info->sel, Buffer, offset );
        DrawText( dc, Buffer, -1, &area, DT_LEFT | DT_NOCLIP );
        fill.top = area.top;
        fill.bottom = area.bottom;
        GetTextExtentPoint( dc, Buffer, (int)strlen( Buffer ), &txtsize );
        fill.left = area.left + txtsize.cx;
        FillRect( dc, &fill, wbrush );
        area.top += FontHeight;
        area.bottom += FontHeight;
        offset += info->bytesdisp;
        i++;
        if( i > info->lastline ) {
            break;
        }
    }
    if( area.top < ((long)info->lastline + 1) * FontHeight ) {
        area.bottom = (info->lastline + 1) * FontHeight;
        FillRect( dc, &area, wbrush );
    }
    mySetScrollPos( info, info->offset );
    SelectObject( dc, old_font );

} /* redrawMemWnd */

/*
 * bytesToDisplay
 */
static unsigned char bytesToDisplay( int width, ctl_id disp_type )
{
    int     bytes;

    bytes = width / FontWidth;
    if( bytes < NO_DATA_SPACE ) {
        bytes = 0;
    } else {
        switch( disp_type ) {
        case MEMINFO_WORD:
            bytes = 2 * ((bytes - NO_DATA_SPACE) / 7);
            break;
        case MEMINFO_DWORD:
            bytes = 4 * ((bytes - NO_DATA_SPACE) / 14);
            break;
        case MEMINFO_BYTE:
        default:
            bytes = (bytes - NO_DATA_SPACE) / 4;
//          if( bytes > 16 ) bytes = 16;
            break;
        }
    }
    if( bytes < 1 ) {
        bytes = 1;
    }
    if( bytes > MAX_BYTES ) {
        bytes = MAX_BYTES;
    }
    return( (unsigned char)bytes );

} /* bytesToDisplay */

/*
 * calcTextDimensions - find the number of bytes to display on a line,
 *                      the number of lines in the window, whether to
 *                      display scroll bars etc... when the window is
 *                      resized or the font changes
 */
static void calcTextDimensions( HWND hwnd, HDC dc, MemWndInfo *info )
{
    bool        owndc;
    RECT        rect;
    int         width;
    int         height;
    HFONT       old_font;
    unsigned    lines;
    bool        need_scrlbar;
    int         scrl_width;
    SIZE        fontsize;

    if( dc == NULL ) {
        dc = GetDC( hwnd );
        owndc = true;
    } else {
        owndc = false;
    }

    CurFont = GetMonoFont();
    old_font = SelectObject( dc, GetMonoFont() );
    GetTextExtentPoint( dc, "0000000000", 10, &fontsize );
    FontWidth = (int)( fontsize.cx / 10 );
    FontHeight = (int)fontsize.cy;

    GetClientRect( hwnd, &rect );
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    info->lastline = height / FontHeight;

    /*
     * Decide if we need a scroll bar and calculate the number
     * of bytes to display on each line.
     */
    info->bytesdisp = bytesToDisplay( width, info->disp_type );
    lines = (unsigned)( info->limit / info->bytesdisp );
    if( ( ( info->limit - info->base ) % info->bytesdisp ) != 0 ) {
        lines++;
    }
    if( ISCODE( info ) ) {
        need_scrlbar = NeedScrollBar( info );
    } else {
        need_scrlbar = (lines > info->lastline);
    }
    if( need_scrlbar ) {
        scrl_width = GetSystemMetrics( SM_CXVSCROLL );
        if( width < scrl_width ) {
            width = 0;
        } else {
            width -= scrl_width;
        }
        MoveWindow( info->scrlbar, width, 0, scrl_width, height, TRUE );
        ShowWindow( info->scrlbar, SW_NORMAL );
        info->bytesdisp = bytesToDisplay( width, info->disp_type );
    } else {
        ShowWindow( info->scrlbar, SW_HIDE );
    }
    info->offset -= ( info->offset - info->base ) % info->bytesdisp;
    info->width = width;
    SelectObject( dc, old_font );
    if( owndc ) {
        ReleaseDC( hwnd, dc );
    }

} /* calcTextDimensions */

/*
 * resizeMemBox
 */
static void resizeMemBox( HWND hwnd, DWORD size, MemWndInfo *info )
{
    HDC         dc;
    RECT        area;
    HBRUSH      wbrush;

    dc = GetDC( hwnd );
//  old_font = SelectObject( dc, CurFont );
    calcTextDimensions( hwnd, dc, info );

    /*
     * Clear edge areas that may not get refreshed.
     */
    wbrush = GetStockObject( WHITE_BRUSH );
    area.top = 0;
    area.bottom = HIWORD( size );
    area.right = LOWORD( size );
    area.left = area.right - 5 * FontWidth;
    FillRect( dc, &area, wbrush );
    area.top = area.bottom - FontHeight;
    area.left = 0;
    FillRect( dc, &area, wbrush );
    redrawMemWnd( hwnd, dc, info );
    InvalidateRect( info->scrlbar, NULL, TRUE );
    UpdateWindow( info->scrlbar );
//  SelectObject( dc, old_font );
    ReleaseDC( hwnd, dc );

} /* resizeMemBox */

/*
 * scrollData
 */
static void scrollData( HWND hwnd, WORD wparam, WORD pos, MemWndInfo *info )
{
    HDC         dc;
    DWORD       offset;

    switch( wparam ) {
    case SB_LINEUP:
        if( info->offset == info->base ) {
            return;
        }
        info->offset -= info->bytesdisp;
        break;
    case SB_LINEDOWN:
        if( info->limit - info->offset <= info->bytesdisp ) {
            return;
        }
        info->offset += info->bytesdisp;
        break;
    case SB_PAGEUP:
        if( info->offset == info->base ) {
            return;
        }
        if( info->offset < info->lastline * info->bytesdisp + info->base ) {
            info->offset = info->base;
        } else {
            info->offset -= info->lastline * info->bytesdisp;
        }
        break;
    case SB_PAGEDOWN:
        if( info->offset + info->lastline * info->bytesdisp > info->limit ) {
            if( info->limit - info->offset <= info->bytesdisp ) {
                return;
            } else {
                info->offset = info->limit - ( ( info->limit - info->base ) % info->bytesdisp );
                if( info->offset == info->limit ) {
                    info->offset = info->limit - info->bytesdisp;
                }
            }
        } else {
            info->offset += info->lastline * info->bytesdisp;
        }
        break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        offset = scrollPosToOffset( pos, info );
        offset -= ( offset - info->base ) % info->bytesdisp;
        info->offset = offset;
        if( offset >= info->limit ) {
            info->offset = info->limit - ( ( info->limit - info->base ) % info->bytesdisp );
        }
        if( offset == info->limit ) {
            info->offset = info->limit - info->bytesdisp;
        }
        break;
    case SB_TOP:
        info->offset = info->base;
        break;
    case SB_BOTTOM:
        info->offset = info->limit - info->lastline * info->bytesdisp;
        break;
    default:
        return;
    }
    dc = GetDC( hwnd );
    redrawMemWnd( hwnd, dc, info );
    ReleaseDC( hwnd, dc );

} /* ScrollData */


/*
 * OffsetDlgProc
 */
INT_PTR CALLBACK OffsetDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char                buf[41];
    unsigned long       offset;
    char                *end;
    HWND                parent;
    HWND                ctl;
    MemWndInfo          *info;
    HDC                 dc;
    HCURSOR             hourglass;
    HCURSOR             oldcursor;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        ctl = GetDlgItem( hwnd, OFF_OFFSET );
        SetFocus( ctl );
        return( FALSE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            /* get the request */
            GetDlgItemText( hwnd, OFF_OFFSET, buf, 40 );
            offset = strtoul( buf, &end, 0 );
            if( *end != '\0' ) {
                while( isspace( *end ) ) {
                    end++;
                }
                if( *end != '\0' ) {
                    RCMessageBox( hwnd, MWND_ENTER_NUMERIC_VALUE, MemConfigInfo.appname,
                                  MB_OK | MB_ICONEXCLAMATION );
                    break;
                }
            }
            /* set the offset */
            parent = GetParent( hwnd );
            info = WPI_GET_WNDINFO( parent );
            if( offset > info->limit ) {
                RCMessageBox( hwnd, MWND_OFFSET_TOO_BIG, MemConfigInfo.appname,
                              MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            if( offset < info->base ) {
                RCMessageBox( hwnd, MWND_OFFSET_TOO_SMALL, MemConfigInfo.appname,
                              MB_OK | MB_ICONEXCLAMATION );
                break;
            }
            hourglass = LoadCursor( NULLHANDLE, IDC_WAIT );
            SetCapture( parent );
            oldcursor = SetCursor( hourglass );
            if( ISCODE( info ) ) {
                info->ins_cnt = GetInsCnt( info, offset );
            } else {
                offset -= ( offset - info->base ) % info->bytesdisp;
                info->offset = offset;
            }
            dc = GetDC( parent );
            redrawMemWnd( parent, dc, info );
            ReleaseDC( parent, dc );
            SetCursor( oldcursor );
            ReleaseCapture();
            /* fall through */
        case IDCANCEL:
            EndDialog( hwnd, FALSE );
            break;
        }
        break;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* OffsetDlgProc */


/*
 * MemDisplayProc
 */
LRESULT CALLBACK MemDisplayProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HDC                 dc;
    MemWndInfo          *info;
    PAINTSTRUCT         paint;
    RECT                area;
    HANDLE              inst;
    HMENU               hmenu;
    DWORD               state;
    DWORD               size;
    HBRUSH              wbrush;
    DLGPROC             dlgproc;
    unsigned            cmd;

    info = WPI_GET_WNDINFO( hwnd );
    switch( msg ) {
    case WM_CREATE:
        RegDisasmRtns();
        info = (MemWndInfo *)((CREATESTRUCT *)lparam)->lpCreateParams;
        WPI_SET_WNDINFO( hwnd, (LONG_PTR)info );
        hmenu = GetMenu( hwnd );
        CheckMenuItem( hmenu, info->disp_type, MF_CHECKED );
        if( info->autopos ) {
            CheckMenuItem( hmenu, MEMINFO_AUTO_POS, MF_CHECKED | MF_BYCOMMAND );
        }
        inst = GET_HINSTANCE( hwnd );
        info->scrlbar = CreateWindow(
            "ScrollBar",                /* Window class name */
            "",
            WS_CHILD | SBS_VERT,        /* Window style */
            0,                          /* Initial X position */
            0,                          /* Initial Y position */
            0,                          /* Initial X size */
            0,                          /* Initial Y size */
            hwnd,                       /* Parent window handle */
            NULLHANDLE,                 /* Window menu handle */
            inst,                       /* Program instance handle */
            NULL );                     /* Create parameters */
        MoveWindow( hwnd, MemConfigInfo.xpos, MemConfigInfo.ypos,
                    MemConfigInfo.xsize, MemConfigInfo.ysize, TRUE );
        ShowWindow( info->scrlbar, SW_HIDE );
        SetScrollRange( info->scrlbar, SB_CTL, 0, SCROLL_RANGE, FALSE );
        if( MemConfigInfo.disp_info ) {
            displaySegInfo( hwnd, inst, info );
        }
        break;
    case WM_SIZE:
        info->maximized = ( wparam == SIZE_MAXIMIZED );
        resizeMemBox( hwnd, (DWORD)lparam, info );
    case WM_MOVE:
        if( info->autopos && IsWindow( info->dialog ) ) {
            positionSegInfo( info->dialog );
        }
        break;
    case WM_PAINT:
        BeginPaint( hwnd, &paint );
        redrawMemWnd( hwnd, paint.hdc, info );
        EndPaint( hwnd, &paint );
        break;
    case WM_SHOWWINDOW:
        if( IsWindow( info->dialog ) ) {
            if( !wparam && LOWORD( lparam ) == SW_PARENTCLOSING ) {
                ShowWindow( info->dialog, SW_HIDE );
            } else if( wparam && LOWORD( lparam ) == SW_PARENTOPENING ) {
                ShowWindow( info->dialog, SW_SHOWNOACTIVATE );
            }
        }
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    case WM_KEYDOWN:
        switch( wparam ) {
        case VK_UP:
        case VK_LEFT:
            wparam = SB_LINEUP;
            break;
        case VK_DOWN:
        case VK_RIGHT:
            wparam = SB_LINEDOWN;
            break;
        case VK_PRIOR:
            wparam = SB_PAGEUP;
            break;
        case VK_NEXT:
            wparam = SB_PAGEDOWN;
            break;
#if 0
        /*
         * I don't support these functions because it takes too long
         * to perform an end operation on an assembly window if it is the
         * first operation.
         */
        case VK_HOME:
            wparam = SB_TOP;
            break;
        case VK_END:
            wparam = SB_BOTTOM;
            break;
#endif
        default:
            return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        }
    case WM_VSCROLL:
        if( ISCODE( info ) ) {
            ScrollAsm( hwnd, GET_WM_VSCROLL_CODE( wparam, lparam ),
                       GET_WM_VSCROLL_POS( wparam, lparam ), info );
        } else {
            scrollData( hwnd, GET_WM_VSCROLL_CODE( wparam, lparam ),
                        GET_WM_VSCROLL_POS( wparam, lparam ), info );
        }
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case MEMINFO_SAVE:
            MemSave( info, hwnd, true );
            break;
        case MEMINFO_SAVE_TO:
            MemSave( info, hwnd, false );
            break;
        case MEMINFO_SHOW:
            inst = GET_HINSTANCE( hwnd );
            displaySegInfo( hwnd, inst, info );
            break;
        case MEMINFO_AUTO_POS:
            hmenu = GetMenu( hwnd );
            state = CheckMenuItem( hmenu, MEMINFO_AUTO_POS, MF_CHECKED );
            if( state == MF_CHECKED ) {
                CheckMenuItem( hmenu, MEMINFO_AUTO_POS, MF_UNCHECKED );
                info->autopos = false;
            } else {
                info->autopos = true;
                positionSegInfo( info->dialog );
            }
            break;
        case MEMINFO_EXIT:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case MEMINFO_CODE_16:
        case MEMINFO_CODE_32:
            info->ins_cnt = 0;
            info->offset = info->base;
            /* fall through */
        case MEMINFO_BYTE:
        case MEMINFO_WORD:
        case MEMINFO_DWORD:
            if( ISCODE( info ) ) {
                info->offset -= (info->offset - info->base) % info->bytesdisp;
            }
            hmenu = GetMenu( hwnd );
            CheckMenuItem( hmenu, info->disp_type, MF_UNCHECKED );
            CheckMenuItem( hmenu, cmd, MF_CHECKED );
            info->disp_type = cmd;
            GetClientRect( hwnd, &area );
            size = MAKELONG( area.right - area.left, area.bottom - area.top );
            wbrush = GetStockObject( WHITE_BRUSH );
            if( IsWindowVisible( info->scrlbar ) ) {
                area.right -= GetSystemMetrics( SM_CXVSCROLL );
            }
            dc = GetDC( hwnd );
            FillRect( dc, &area, wbrush );
            ReleaseDC( hwnd, dc );
            GetClientRect( hwnd, &area );
            resizeMemBox( hwnd, size, info );
            break;
        case MEMINFO_OFFSET:
            inst = GET_HINSTANCE( hwnd );
            dlgproc = MakeProcInstance_DLG( OffsetDlgProc, inst );
            DialogBox( inst, "OFFSETDLG", hwnd, dlgproc );
            FreeProcInstance_DLG( dlgproc );
            break;
        }
        break;
    case WM_CLOSE:
        if( !MemConfigInfo.forget_pos ) {
            MemConfigInfo.maximized = info->maximized;
            MemConfigInfo.autopos_info = info->autopos;
            if( ISCODE( info ) ) {
                MemConfigInfo.code_type = info->disp_type;
            } else {
                MemConfigInfo.data_type = info->disp_type;
            }
            if( !info->maximized ) {
                GetWindowRect( hwnd, &area );
                MemConfigInfo.xpos = area.left;
                MemConfigInfo.ypos = area.top;
                MemConfigInfo.xsize = area.right - area.left;
                MemConfigInfo.ysize = area.bottom - area.top;
            }
        }
        DestroyWindow( hwnd );
        break;
    case WM_DESTROY:
        SendMessage( info->parent, WM_USER, 0, 0 );
        if( info != NULL && info->curwnd ) {
            CurWindow = NULLHANDLE;
        }
        break;
    case WM_NCDESTROY:
        if( info != NULL ) {
            if( info->asm_info != NULL ) {
                MemFree( info->asm_info );
            }
            MemFree( info );
        }
        /* fall through */
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( TRUE );

} /* MemDisplayProc */

/*
 * positionSegInfo
 */
static void positionSegInfo( HWND hwnd )
{
    HWND        parent;
    RECT        psize;
    RECT        dsize;
    int         dwidth;
    int         dheight;
    int         scrn_width;
    int         scrn_hite;
    int         xpos;
    int         ypos;

    parent = GetParent( hwnd );
    GetWindowRect( parent, &psize );
    GetWindowRect( hwnd, &dsize );
    dwidth = dsize.right - dsize.left;
    dheight = dsize.bottom - dsize.top;
    scrn_hite = GetSystemMetrics( SM_CYSCREEN );
    scrn_width = GetSystemMetrics( SM_CXSCREEN );
    xpos = psize.left;
    ypos = psize.top;
    if( ypos < 0 ) {
        ypos = 0;
    }
    if( psize.bottom + dheight <= scrn_hite && psize.left + dwidth <= scrn_width ) {
        ypos = psize.bottom;
    } else if( psize.top > dheight && psize.left + dwidth <= scrn_width ) {
        ypos = psize.top - dheight;
    } else if( psize.left > dwidth ) {
        xpos = psize.left - dwidth;
    } else if( psize.right + dwidth < scrn_width ) {
        xpos = psize.right;
    } else {
        xpos = psize.left;
        ypos = psize.bottom - dheight;
    }
    SetWindowPos( hwnd, NULLHANDLE, xpos, ypos, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

} /* positionSegInfo */

/*
 * SegInfoDlgProc
 */
INT_PTR CALLBACK SegInfoDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HWND        parent;
    HMENU       hmenu;

    wparam = wparam;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        positionSegInfo( hwnd );
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_CLOSE:
        DestroyWindow( hwnd );
        break;
    case WM_DESTROY:
        parent = GetParent( hwnd );
        hmenu = GetMenu( parent );
        EnableMenuItem( hmenu, MEMINFO_SHOW, MF_ENABLED );
        break;
    case WM_NCDESTROY:
#ifndef __NT__
        DialCount--;
        if( DialCount == 0 ) {
            FreeProcInstance_DLG( DialProc );
        }
#endif
        return( FALSE ); /* we need to let Windows see this message
                            or fonts are left undeleted */
    default:
        return( FALSE );
    }
    return( TRUE );

} /* SegInfoDlgProc */

/*
 * displaySegInfo
 */
static void displaySegInfo( HWND parent, HANDLE instance, MemWndInfo *info )
{
#ifdef __NT__
    parent = parent;
    instance = instance;
    info = info;
#else
    GLOBALENTRY         ge;
    descriptor          desc;
    HWND                hwnd;
    char                buf[10];
    char                *ptr;
    char                *rcstr;
    HMENU               hmenu;

    hmenu = GetMenu( parent );
    EnableMenuItem( hmenu, MEMINFO_SHOW, MF_GRAYED );
    if( DialCount == 0 ) {
        DialProc = MakeProcInstance_DLG( SegInfoDlgProc, instance );
    }
    DialCount++;
    if( info->isdpmi ) {
        GetADescriptor( info->sel, &desc );
        hwnd = CreateDialog( instance, "SEL_INFO", parent, DialProc );

        sprintf( buf, "%04X", info->sel );
        SetDlgItemText( hwnd, SEL_INFO_SEL, buf );
//      SetWORDStaticField( hwnd, SEL_INFO_SEL, info->sel );

        sprintf( buf, "%08X", GET_DESC_BASE( desc ) );
        SetDlgItemText( hwnd, SEL_INFO_BASE, buf );
//      SetDWORDStaticField( hwnd, SEL_INFO_BASE, GET_DESC_BASE( desc ) );

        sprintf( buf, "%08X", GET_DESC_LIMIT( desc ) );
        SetDlgItemText( hwnd, SEL_INFO_LIMIT, buf );
//      SetDWORDStaticField( hwnd, SEL_INFO_LIMIT, GET_DESC_LIMIT( desc ) );

        if( desc.u1.flags.nonsystem && !desc.u1.flags.execute )  {
            rcstr = AllocRCString( MWND_DATA );
        } else {
            rcstr = AllocRCString( MWND_CODE );
        }
        SetDlgItemText( hwnd, SEL_INFO_TYPE, rcstr );
        FreeRCString( rcstr );
        sprintf( buf, "%1d", desc.u1.flags.dpl );
        SetDlgItemText( hwnd, SEL_INFO_DPL, buf );
        if( desc.u2.flags.page_granular ) {
            rcstr = AllocRCString( MWND_PAGE );
        } else {
            rcstr = AllocRCString( MWND_BYTE );
        }
        SetDlgItemText( hwnd, SEL_INFO_GRAN, rcstr );
        FreeRCString( rcstr );
        ptr = buf;
        createAccessString( ptr, &desc );
        SetDlgItemText( hwnd, SEL_INFO_ACCESS, buf );
    } else {
        ge.dwSize = sizeof( GLOBALENTRY );
        GlobalEntryHandle( &ge, (HGLOBAL)info->sel );
        hwnd = CreateDialog( instance, "HDL_INFO", parent, DialProc );

        sprintf( buf, "%04X", ge.hBlock );
        SetDlgItemText( hwnd, HDL_INFO_HDL, buf );
//      SetWORDStaticField( hwnd, HDL_INFO_HDL, ge.hBlock );

        sprintf( buf, "%08X", ge.dwAddress );
        SetDlgItemText( hwnd, HDL_INFO_ADDR, buf );
//      SetDWORDStaticField( hwnd, HDL_INFO_ADDR, ge.dwAddress );

        sprintf( buf, "%04X", ge.dwBlockSize );
        SetDlgItemText( hwnd, HDL_INFO_SIZE, buf );
//      SetWORDStaticField( hwnd, HDL_INFO_SIZE, ge.dwBlockSize );

        sprintf( buf, "%04X", ge.wcLock );
        SetDlgItemText( hwnd, HDL_INFO_LOCK, buf );
//      SetWORDStaticField( hwnd, HDL_INFO_LOCK, ge.wcLock );

        sprintf( buf, "%04X", ge.wcPageLock );
        SetDlgItemText( hwnd, HDL_INFO_PLOCK, buf );
//      SetWORDStaticField( hwnd, HDL_INFO_PLOCK, ge.wcPageLock );
    }
    info->dialog = hwnd;
#endif

} /* displaySegInfo */


/*
 * DispMem
 */
HWND DispMem( HANDLE instance, HWND parent, WORD seg, bool isdpmi )
{
    HWND                hdl;
    char                buf[50];
    MemWndInfo          *info;
    bool                maximize;

#ifndef __NT__
    descriptor          desc;
    GLOBALENTRY         ge;

    desc = desc;
    if( isdpmi ) {
        RCsprintf( buf, MWND_MEM_DISP_FOR_SEL_X, seg );
    } else {
        RCsprintf( buf, MWND_MEM_DISP_FOR_HDL_X, seg );
    }
#endif

    if( !MemConfigInfo.init ) {
        SetDefMemConfig();
    }
    maximize = MemConfigInfo.maximized;
    if( CurWindow != NULLHANDLE ) {
        if( MemConfigInfo.allowmult == WND_REPLACE ) {
            SendMessage( CurWindow, WM_CLOSE, 0, 0L );
        } else if( MemConfigInfo.allowmult == WND_SINGLE ) {
            return( NULLHANDLE );
        }
    }
    info = MemAlloc( sizeof( MemWndInfo ) );
    if( info == NULL ) {
        RCMessageBox( parent, MWND_CANT_DISP_MEM_WND, MemConfigInfo.appname, MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
        return( NULLHANDLE );
    }
    info->sel = seg;
    info->limit = GetASelectorLimit( seg );
    info->lastline = 0;
    info->bytesdisp = 1;
    info->ins_cnt = 0;
    info->width = 0;
    info->parent = parent;
    info->dialog = NULLHANDLE;
    info->isdpmi = isdpmi;
    info->autopos = MemConfigInfo.autopos_info;
#ifndef __NT__
    if( isdpmi ) {
        GetADescriptor( seg, &desc );
        if( desc.u1.flags.nonsystem && !desc.u1.flags.execute )  {
            info->disp_type = MemConfigInfo.data_type;
        } else {
            info->disp_type = MemConfigInfo.code_type;
        }
    } else {
        memset( &ge, 0, sizeof( GLOBALENTRY ) );
        ge.dwSize = sizeof( GLOBALENTRY );
        GlobalEntryHandle( &ge, (HGLOBAL)seg );
        if( ge.wType == GT_CODE ) {
            info->disp_type = MemConfigInfo.code_type;
        } else {
            info->disp_type = MemConfigInfo.data_type;
        }
    }
    info->base = 0;
#else
    info->disp_type = MemConfigInfo.data_type;
    info->base = CurBase;
#endif
    info->offset = info->base;
    info->asm_info = NULL;
    if( MemConfigInfo.allowmult != WND_MULTI ) {
        info->curwnd = true;
        CurWindow = NULLHANDLE;
    } else {
        info->curwnd = false;
    }

    hdl = CreateWindow(
        MEM_DISPLAY_CLASS,      /* Window class name */
        buf,                    /* Window caption */
                                /* Window style */
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX,
        0,                      /* Initial X position */
        0,                      /* Initial Y position */
        0,                      /* Initial X size */
        0,                      /* Initial Y size */
        parent,                 /* Parent window handle */
        NULLHANDLE,             /* Window menu handle */
        instance,               /* Program instance handle */
        info );                 /* Create parameters */
    if( hdl == NULLHANDLE || info->scrlbar == NULL ) {
        RCMessageBox( parent, MWND_CANT_DISP_MEM_WND, MemConfigInfo.appname, MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
        DestroyWindow( hdl );
        MemFree( info );
        return( NULLHANDLE );
    }
    if( maximize ) {
        ShowWindow( hdl, SW_SHOWMAXIMIZED );
    } else {
        ShowWindow( hdl, SW_SHOWNORMAL );
    }
    if( MemConfigInfo.allowmult != WND_MULTI ) {
        CurWindow = hdl;
    }
    return( hdl );

} /* DispMem */

#ifdef __NT__

/*
 * DispNTMem
 */
HWND DispNTMem( HWND parent, HANDLE instance, HANDLE prochdl, DWORD offset, DWORD limit, char *title )
{
    HWND        ret;

    ProcessHdl = prochdl;
    CurLimit = limit;
    CurBase = offset;
    ret = DispMem( instance, parent, 0, false );
    SetWindowText( ret, title );
    return( ret );

} /* DispNTMem */

#endif
