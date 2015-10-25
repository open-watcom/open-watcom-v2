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
* Description:  Memory display window, with disassembly.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <io.h>
#include <ctype.h>
#include "bool.h"
#include "memwnd.h"
#include "segmem.h"
#include "sdkasm.h"
#include "font.h"
#include "mem.h"

#define MAX_BACKUPS             1000
#define BYTES_PER_BACKUP        100

extern WORD             FontWidth;
extern WORD             FontHeight;

static DWORD            _Offset;    // conflicts with extern in wdisasm/h/global.h
static WORD             Sel;
static DWORD            Limit;
static bool             Is32Bit;
static char             StatBuf[50];
static DisAsmRtns       DisasmInfo;
static bool             DisasmRegistered;

static void gotoIns( MemWndInfo *info, DWORD ins_cnt );

/*
 * MemWndGetDataByte
 */
static int_16 MemWndGetDataByte( void )
{
    char            buf;

    ReadMem( Sel, _Offset, &buf, 1 );
    _Offset++;
    return( buf );

} /* GetDataByte */

/*
 * MemWndGetDataWord
 */
static int_16 MemWndGetDataWord( void )
{
    int_16      buf;

    ReadMem( Sel, _Offset, (char *)&buf, sizeof( int_16 ) );
    _Offset += sizeof( int_16 );
    return( buf );

} /* GetDataWord */

/*
 * MemWndGetNextByte
 */
static int_16 MemWndGetNextByte( void )
{
    char            buf;

    ReadMem( Sel, _Offset, &buf, 1 );
    return( buf );

} /* MemWndGetNextByte */

/*
 * MemWndGetDataLong
 */
static int_32 MemWndGetDataLong( void )
{
    int_32      buf;

    ReadMem( Sel, _Offset, (char *)&buf, sizeof( buf ) );
    _Offset += sizeof( buf );
    return( buf );

} /* MemWndGetDataLong */

/*
 * MemWndEndOfSegment
 */
static char MemWndEndOfSegment( void )
{
    return( _Offset > Limit );

} /* MemWndEndOfSegment */

/*
 * MemWndGetOffset
 */
static DWORD MemWndGetOffset( void )
{
    return( _Offset );

} /* MemWndGetOffset */

/*
 * MemWndToStr - return a string of length 'length' containing 'value'
 *               in hex notation
 */
static char *MemWndToStr( uint_32 value, uint_16 len, DWORD addr )
{
    int         i;

    addr = addr;
    for( i = len - 1; i >= 0; i-- ) {
        StatBuf[i] = MkHexDigit( value & 0xf );
        value >>= 4;
    }
    StatBuf[len] = '\0';
    return( StatBuf );

} /* MemWndToStr */

/*
 * MemWndJmpLabel - return a string containing addr in segment:offset form
 */
static char *MemWndJmpLabel( uint_32 addr, DWORD off )
{
    unsigned    len;

    off = off;
    len = Is32Bit ? 8 : 4;
    sprintf( StatBuf, "%04X:%0*lX", Sel, len, addr );
    return( StatBuf );

} /* MemWndJmpLabel */

/*
 * MemWndToBrStr - return a string representing 'value' in hex form enclosed in []
 */
static char *MemWndToBrStr( uint_32 value, DWORD addr )
{
    unsigned    len;

    addr = addr;
    len = Is32Bit ? 8 : 4;
    sprintf( StatBuf, "[%0*lX]", len, value );
    return( StatBuf );

} /* MemWndToBrStr */

/*
 * MemWndToIndex - convert value to a hex string with a + or - at the beginning
 */
static char *MemWndToIndex( uint_32 value, uint_32 addr )
{
    char        sign[2];

    addr = addr;
    if( (int_32)value < 0 ) {
        sign[0] = '-';
        value = -(int_32)value;
    } else {
        sign[0] = '+';
    }
    sign[1] = '\0';
    sprintf( StatBuf, "%s%lX", sign, value );
    return( StatBuf );

} /* MemWndToIndex */

/*
 * MemWndToSegStr - convert to seg:off form
 */
static char *MemWndToSegStr( DWORD value, WORD seg, DWORD addr )
{
    unsigned    len;

    addr = addr;
    len = Is32Bit ? 8 : 4;
    sprintf( StatBuf, "%04X:%0*lX", seg, len, value );
    return( StatBuf );

} /* MemWndToSegStr */

/*
 * MemWndGetWtkInsName
 */
static char *MemWndGetWtkInsName( unsigned ins )
{
    ins = ins;
    return( "" );

} /* MemWndGetWtkInsName */

/*
 * MemWndDoWtk
 */
static void MemWndDoWtk( void )
{
} /* MemWndDoWtk */

/*
 * MemWndIsWtk
 */
static int MemWndIsWtk( void )
{
    return( 0 );

} /* MemWndIsWtk */

/*
 * DumpMemAsm
 */
void DumpMemAsm( MemWndInfo *info, int hdl )
{
    char                buf[80];
    unsigned            len;
    instruction         ins;

    _Offset = 0;
    Limit = info->limit;
    Is32Bit = (info->disp_type == MEMINFO_CODE_32);
    Sel = info->sel;
    while( _Offset < Limit ) {
        sprintf( buf, "%08lX  ", _Offset );
        MiscDoCode( &ins, Is32Bit, &DisasmInfo );
        MiscFormatIns( buf + 10 , &ins, 0, &DisasmInfo );
        len = (unsigned)strlen( buf );
        write( hdl, buf, len );
        write( hdl, "\n", 1 );
    }

} /* DumpMemAsm */

/*
 * NeedScrollBar
 */
bool NeedScrollBar( MemWndInfo *info )
{
    WORD                line;
    instruction         ins;
    bool                is_32;

    _Offset = 0;
    Limit = info->limit;
    Sel = info->sel;
    is_32 = (info->disp_type == MEMINFO_CODE_32);
    for( line = 0; line < info->lastline; line ++ ) {
        MiscDoCode( &ins, is_32, &DisasmInfo );
    }
    return( _Offset < info->limit );

} /* NeedScrollBar */

/*
 * genAsmLine
 */
static DWORD genAsmLine( MemWndInfo *info, DWORD ins_cnt, char *buf )
{
    instruction         ins;
    DWORD               offset;

    Is32Bit = (info->disp_type == MEMINFO_CODE_32);
    Sel = info->sel;
    Limit = info->limit;
    gotoIns( info, ins_cnt );
    offset = _Offset;
    sprintf( buf, "%08lX  ", _Offset );
    MiscDoCode( &ins, Is32Bit, &DisasmInfo );
    MiscFormatIns( buf + 10 , &ins, 0, &DisasmInfo );
    return( offset );

} /* genAsmLine */

/*
 * genBackup - Generates a backup reference.
 *             Is32Bit, Limit and Sel must be set before calling this routine
 */
static void genBackup( AsmInfo *asm )
{
    DWORD       cnt;
    WORD        *wptr;
    instruction ins;

    if( asm->usage_cnt < MAX_BACKUPS ) {
        cnt = asm->increment;
        wptr = (WORD *)asm->data;
        _Offset = wptr[asm->usage_cnt];
        while( cnt > 0 ) {
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            cnt--;
        }
        asm->usage_cnt++;
        wptr[asm->usage_cnt] = (WORD)_Offset;
    }

} /* genBackup */

/*
 * genBigBackup - Generates a backup reference for a big code item
 *                Is32Bit, Limit and Sel must be set before calling this routine
 */
static void genBigBackup( AsmInfo *asm )
{
    DWORD       cnt;
    DWORD       *dwptr;
    instruction ins;

    if( asm->usage_cnt < MAX_BACKUPS ) {
        cnt = asm->increment;
        dwptr = (DWORD *)asm->data;
        _Offset = dwptr[asm->usage_cnt];
        while( cnt > 0 ) {
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            cnt--;
        }
        asm->usage_cnt++;
        dwptr[asm->usage_cnt] = _Offset;
    }

} /* genBigBackup */

/*
 * GetInsCnt - finds the number of instructions before the one that
 *             straddles offset
 */
DWORD GetInsCnt( MemWndInfo *info, DWORD offset )
{
    AsmInfo     *asm;
    DWORD       *dwptr;
    DWORD       old_offset;
    DWORD       ins_cnt;
    WORD        *wptr;
    WORD        i;
    instruction ins;

    Is32Bit = (info->disp_type == MEMINFO_CODE_32);
    Sel = info->sel;
    Limit = info->limit;
    asm = info->asm;
    if( asm->big ) {
        dwptr = (DWORD *)asm->data;
        while( dwptr[asm->usage_cnt] < offset ) {
            if( asm->usage_cnt == MAX_BACKUPS ) {
                break;
            }
            genBigBackup( asm );
        }
        for( i = 0; dwptr[i] <= offset && i < MAX_BACKUPS - 1; i++ );
        i--;
        _Offset = dwptr[i];
        ins_cnt = i * asm->increment;
        while( _Offset <= offset ) {
            old_offset = _Offset;
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            ins_cnt++;
        }
    } else {
        wptr = (WORD *)asm->data;
        while( wptr[asm->usage_cnt] < offset ) {
            if( asm->usage_cnt == MAX_BACKUPS ) {
                break;
            }
            genBackup( asm );
        }
        for( i = 0; wptr[i] <= offset && i < MAX_BACKUPS - 1; i++ );
        i--;
        _Offset = wptr[i];
        ins_cnt = i * asm->increment;
        while( _Offset <= offset ) {
            old_offset = _Offset;
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            ins_cnt++;
        }
    }
    /* we go one instruction too far if offset == info->limit */
    if( _Offset >= info->limit ) {
        ins_cnt--;
    }
    if( ins_cnt > 0 ) {
        ins_cnt--;
    }
    return( ins_cnt );

} /* GetInsCnt */

/*
 * ScrollAsm - deal with scroll messages when the memory window is
 *             displaying code
 */
void ScrollAsm( HWND hwnd, WORD wparam, WORD pos, MemWndInfo *info )
{
    RECT        area;
    HDC         dc;
    HFONT       old_font;
    HBRUSH      wbrush;
    char        buf[80];
    DWORD       offset;

    wparam = wparam;
    pos = pos;
    switch( wparam ) {
    case SB_LINEDOWN:
        offset = genAsmLine( info, info->ins_cnt + info->lastline, buf );
        dc = GetDC( hwnd );
        GetClientRect( hwnd, &area );
        area.right = info->width;
        if( offset >= Limit ) {
            offset = Limit;
            wbrush = GetStockObject( WHITE_BRUSH );
            gotoIns( info, info->ins_cnt + 1 );
            if( _Offset < Limit ) {
                area.right = info->width;
                info->ins_cnt++;
                ScrollWindow( hwnd, 0, -FontHeight, &area, NULL );
                area.top = FontHeight * info->lastline;
                area.left = 0;
                area.right = info->width;
                area.bottom = area.top + FontHeight;
                FillRect( dc, &area, wbrush );
                old_font = SelectObject( dc, GetMonoFont() );
            }
        } else {
            /*
             * This is an approximation of the offset of the first displayed
             * instruction.
             */
//          offset -= 2 * info->lastline;
            info->ins_cnt++;
            ScrollWindow( hwnd, 0, -FontHeight, &area, NULL );
            old_font = SelectObject( dc, GetMonoFont() );
            area.top = FontHeight * info->lastline;
            area.left = 0;
            area.right = info->width;
            area.bottom = area.top + FontHeight;
            DrawText( dc, buf, -1, &area, DT_LEFT );
            SelectObject( dc, old_font );
        }
        ReleaseDC( hwnd, dc );
//      mySetScrollPos( info->scrlbar, offset, info->limit );
        break;
    case SB_LINEUP:
        if( info->ins_cnt != 0 ) {
            GetClientRect( hwnd, &area );
            area.right = info->width;
            ScrollWindow( hwnd, 0, FontHeight, &area, NULL );
            area.top = 0;
            area.left = 0;
            area.right = info->width;
            area.bottom = FontHeight;
            info->ins_cnt--;
            offset = genAsmLine( info, info->ins_cnt, buf );
//          mySetScrollPos( info->scrlbar, offset, info->limit );
            dc = GetDC( hwnd );
            old_font = SelectObject( dc, GetMonoFont() );
            DrawText( dc, buf, -1, &area, DT_LEFT );
            SelectObject( dc, old_font );
            ReleaseDC( hwnd, dc );
        }
        break;
    case SB_PAGEUP:
        if( info->ins_cnt < info->lastline ) {
            info->ins_cnt = 0;
        } else {
            info->ins_cnt -= info->lastline;
        }
        dc = GetDC( hwnd );
        RedrawAsCode( dc, info );
        ReleaseDC( hwnd, dc );
        break;
    case SB_PAGEDOWN:
        info->ins_cnt += info->lastline;
        gotoIns( info, info->ins_cnt );
        while( _Offset >= Limit ) {
            info->ins_cnt--;
            gotoIns( info, info->ins_cnt );
        }
        dc = GetDC( hwnd );
        RedrawAsCode( dc, info );
        ReleaseDC( hwnd, dc );
        break;
#if 0
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        offset = ScrollPosToOffset( pos, info->limit );
        if( offset > info->limit ) {
            offset = info->limit;
        }
        info->ins_cnt = GetInsCnt( info, offset );
        dc = GetDC( hwnd );
        RedrawAsCode( dc, info );
        ReleaseDC( hwnd, dc );
        break;
#endif
    case SB_TOP:
        info->ins_cnt = 0;
        dc = GetDC( hwnd );
        RedrawAsCode( dc, info );
        ReleaseDC( hwnd, dc );
        break;
#if 0
    case SB_BOTTOM:
        info->ins_cnt = GetInsCnt( info, info->limit );
        dc = GetDC( hwnd );
        RedrawAsCode( dc, info );
        ReleaseDC( hwnd, dc );
        break;
#endif
    default:
        return;
    }
}

/*
 * gotoIns - sets _Offset to the instruction ins_cnt instructions
 *           from the beginning of the item
 */
static void gotoIns( MemWndInfo *info, DWORD ins_cnt )
{
    instruction ins;
    DWORD       cnt;
    DWORD       *dwptr;
    WORD        *wptr;
    WORD        size;
    WORD        backup_cnt;
    AsmInfo     *asm;

    if( info->asm == NULL ) {
        size = sizeof( AsmInfo );
        backup_cnt = MAX_BACKUPS;
        if( info->limit > 0xffff ) {
            size += backup_cnt * sizeof( DWORD );
        } else {
            size += backup_cnt * sizeof( WORD );
        }
        asm = MemAlloc( size );
        if( asm != NULL ) {
            info->asm = asm;
            asm->big = (info->limit > 0xffff);
            asm->increment = info->limit / backup_cnt;
            if( asm->increment == 0 ) {
                asm->increment = 1;
            }
            asm->usage_cnt = 0;
            memset( asm->data, 0, 4 );
        }
    } else {
        asm = info->asm;
    }
    Is32Bit = (info->disp_type == MEMINFO_CODE_32);
    Sel = info->sel;
    Limit = info->limit;

    /*
     * Generate new backups.
     */
    if( ins_cnt >= asm->increment * (asm->usage_cnt + 1) && asm != NULL ) {
        if( asm->big ) {
            dwptr = (DWORD *)asm->data;
            dwptr += asm->usage_cnt;
            _Offset = *dwptr;
            cnt = ins_cnt - (ins_cnt % asm->increment);
            cnt -= asm->usage_cnt * asm->increment;
            cnt /= asm->increment;
            while( cnt ) {
                genBigBackup( asm );
                cnt--;
            }
        } else {
            wptr = (WORD *)asm->data;
            wptr += asm->usage_cnt;
            _Offset = (DWORD)*wptr;
            cnt = ins_cnt - (ins_cnt % asm->increment);
            cnt -= asm->usage_cnt * asm->increment;
            cnt /= asm->increment;
            while( cnt ) {
                genBackup( asm );
                cnt--;
            }
        }
    }
    if( asm != NULL ) {
        if( asm->big ) {
            dwptr = (DWORD *)asm->data;
            _Offset = dwptr[ins_cnt / asm->increment];
        } else {
            wptr = (WORD *)asm->data;
            _Offset = wptr[ins_cnt / asm->increment];
        }
        ins_cnt = ins_cnt % asm->increment;
    } else {
        _Offset = 0;
    }
    while( ins_cnt ) {
        MiscDoCode( &ins, Is32Bit, &DisasmInfo );
        ins_cnt--;
    }

} /* gotoIns */

/*
 * RedrawAsCode
 */
void RedrawAsCode( HDC dc, MemWndInfo *info )
{
    DWORD       line;
    char        buf[80];
    RECT        area;
    HBRUSH      wbrush;
    instruction ins;
    HFONT       old_font;

    old_font = SelectObject( dc, GetMonoFont() );
    Is32Bit = (info->disp_type == MEMINFO_CODE_32);
    Sel = info->sel;
    Limit = info->limit;
    gotoIns( info, info->ins_cnt );
    wbrush = GetStockObject( WHITE_BRUSH );
    area.top = 0;
    area.bottom = FontHeight;
    area.left = 0;
    area.right = info->width;
//  mySetScrollPos( info->scrlbar, _Offset, info->limit );
    for( line = 0; line <= info->lastline; line ++ ) {
        if( _Offset >= Limit ) {
            break;
        }
        sprintf( buf, "%08lX  ", _Offset );
        MiscDoCode( &ins, Is32Bit, &DisasmInfo );
        MiscFormatIns( buf + 10 , &ins, 0, &DisasmInfo );
        FillRect( dc, &area, wbrush );
        DrawText( dc, buf, -1, &area, DT_LEFT );
        area.top = area.bottom;
        area.bottom += FontHeight;
    }
    /* clear the bottom of the window when there is no data there */
    if( line <= info->lastline ) {
        area.bottom = FontHeight * (info->lastline + 1);
        FillRect( dc, &area, wbrush );
    }
    SelectObject( dc, old_font );

} /* RedrawAsCode */

/*
 * RegDisasmRtns - register us to use the interface to the disasembler
 */
void RegDisasmRtns()
{
    if( !DisasmRegistered ) {
        DisasmInfo.GetDataByte = MemWndGetDataByte;
        DisasmInfo.GetDataWord = MemWndGetDataWord;
        DisasmInfo.GetNextByte = MemWndGetNextByte;
        DisasmInfo.GetDataLong = MemWndGetDataLong;
        DisasmInfo.EndOfSegment = MemWndEndOfSegment;
        DisasmInfo.GetOffset = MemWndGetOffset;
        DisasmInfo.DoWtk = MemWndDoWtk;
        DisasmInfo.IsWtk = MemWndIsWtk;
        DisasmInfo.ToStr = MemWndToStr;
        DisasmInfo.JmpLabel = MemWndJmpLabel;
        DisasmInfo.ToBrStr = MemWndToBrStr;
        DisasmInfo.ToIndex = MemWndToIndex;
        DisasmInfo.ToSegStr = MemWndToSegStr;
        DisasmInfo.GetWtkInsName = MemWndGetWtkInsName;
        RegisterRtns( &DisasmInfo );
    }
    DisasmRegistered = true;

} /* RegDisasmRtns */
