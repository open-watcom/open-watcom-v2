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


#include <string.h>
#include <io.h>
#include <ctype.h>
#include <windows.h>
#include "memwnd.h"
#include "segmem.h"
#include "sdkasm.h"
#include "font.h"
#include "mem.h"
#ifndef __NT__
 #include "toolhelp.h"
#endif

#define MAX_BACKUPS             1000
#define BYTES_PER_BACKUP        100

extern WORD             FontWidth;
extern WORD             FontHeight;

static DWORD            Offset;
static HANDLE           Sel;
static DWORD            Limit;
static BOOL             Is32Bit;
static char             StatBuf[50];
static DisAsmRtns       DisasmInfo;
static BOOL             DisasmRegistered;

/*
  disassembler interface routines
*/

int_16 MemWndGetDataByte() {

    char        buf;

    ReadMem( Sel, Offset, &buf, 1 );
    Offset++;
    return( buf );
} /* GetDataByte */

int_16 MemWndGetDataWord() {

    int_16      buf;

    ReadMem( Sel, Offset, (char *)&buf, sizeof( int_16 ) );
    Offset += sizeof( int_16 );
    return( buf );
} /* GetDataWord */


int_16 MemWndGetNextByte() {

    char        buf;

    ReadMem( Sel, Offset, &buf, 1 );
    return( buf );
}

long MemWndGetDataLong() {

    long        buf;

    ReadMem( Sel, Offset, (char *)&buf, sizeof( long ) );
    Offset += sizeof( long );
    return( buf );
}

char MemWndEndOfSegment() {
    return( Offset > Limit );
}

DWORD MemWndGetOffset() {
    return( Offset );
}

/*
 * ToStr - return a string of length 'length' containing 'value'
 *         in Hex notation
 */

char *MemWndToStr( unsigned long value, uint_16 len, DWORD addr ) {

    int         i;

    addr = addr;
    for( i = len-1; i >= 0; --i ) {
        StatBuf[ i ] = MkHexDigit( value & 0xf );
        value >>= 4;
    }
    StatBuf[ len ] = '\0';
    return( StatBuf );
}

/*
 * JmpLabel - return a string containing addr in segment:offset form
 */

char *MemWndJmpLabel( unsigned long addr, DWORD off ) {

    unsigned    len;

    off = off;
    len = Is32Bit?8:4;
    sprintf( StatBuf, "%04X:%0*lX", Sel, len, addr );
    return( StatBuf );
}

/*
 * ToBrStr - return a string representing 'value' in hex form enclosed in []
 */
char *MemWndToBrStr( unsigned long value, DWORD addr ) {

    unsigned    len;

    addr = addr;
    len = Is32Bit?8:4;
    sprintf( StatBuf, "[%0*lX]", len, value );
    return( StatBuf );
}

/*
 * ToIndex - convert value to a hex string with a + or - at the begining
 */
char *MemWndToIndex( long value, unsigned long addr ) {

    char        sign[2];

    addr = addr;
    if( value >= 0 ) {
        sign[0] = '+';
    } else if( value < 0 ) {
        sign[0] = '-';
        value = -value;
    }
    sign[1] = '\0';
    sprintf( StatBuf, "%s%lX", sign, value );
    return( StatBuf );
}

/*
 * ToSegStr - convert to seg:off form
 */
char *MemWndToSegStr( DWORD value, WORD seg, DWORD addr ) {
    unsigned    len;

    addr = addr;
    len = Is32Bit?8:4;
    sprintf( StatBuf, "%04X:%0*lX", seg, len, value );
    return( StatBuf );
}

char *MemWndGetWtkInsName( unsigned ins )
{
    ins = ins;
    return( "" );
}

void MemWndDoWtk(void)
{
}

int MemWndIsWtk()
{
    return( 0 );
}

void DumpMemAsm( MemWndInfo *info, int hdl ) {

    char                buf[80];
    WORD                len;
    instruction         ins;

    Offset = 0;
    Limit = info->limit;
    Is32Bit = ( info->disp_type == MEMINFO_CODE_32 );
    Sel = info->sel;
    while( Offset < Limit ) {
        sprintf( buf, "%08lX  ", Offset );
        MiscDoCode( &ins, Is32Bit, &DisasmInfo );
        MiscFormatIns( buf + 10 , &ins, 0, &DisasmInfo );
        len = strlen( buf );
        write( hdl, buf, len );
        write( hdl, "\n", 1 );
    }
} /* DumpMemAsm */

BOOL NeedScrollBar( MemWndInfo *info ) {

    WORD                line;
    instruction         ins;
    BOOL                is_32;

    Offset = 0;
    Limit = info->limit;
    Sel = info->sel;
    is_32 = ( info->disp_type == MEMINFO_CODE_32 );
    for( line = 0; line < info->lastline; line ++ ) {
        MiscDoCode( &ins, is_32, &DisasmInfo );
    }
    return( Offset < info->limit );
} /* NeedScrollBar */

static DWORD GenAsmLine( MemWndInfo *info, DWORD ins_cnt, char *buf )
{
    instruction         ins;
    DWORD               offset;

    Is32Bit = ( info->disp_type == MEMINFO_CODE_32 );
    Sel = info->sel;
    Limit = info->limit;
    GotoIns( info, ins_cnt );
    offset = Offset;
    sprintf( buf, "%08lX  ", Offset );
    MiscDoCode( &ins, Is32Bit, &DisasmInfo );
    MiscFormatIns( buf + 10 , &ins, 0, &DisasmInfo );
    return( offset );
} /* GenAsmLine */

/*
 * GenBackup - Generates a backup reference.
 *             Is32Bit, Limit and Sel must be set before calling this routine
 */

static void GenBackup( AsmInfo *asm ) {

    WORD        cnt;
    WORD        *wptr;
    instruction ins;

    if( asm->usage_cnt < MAX_BACKUPS ) {
        cnt = asm->increment;
        wptr = (WORD *)asm->data;
        Offset = wptr[asm->usage_cnt];
        while( cnt > 0 ) {
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            cnt--;
        }
        asm->usage_cnt ++;
        wptr[asm->usage_cnt] = Offset;
    }
} /* GenBackup */

/*
 * GenBigBackup - Generates a backup reference for a big code item
 *             Is32Bit, Limit and Sel must be set before calling this routine
 */

static GenBigBackup( AsmInfo *asm ) {

    DWORD       cnt;
    DWORD       *dwptr;
    instruction ins;

    if( asm->usage_cnt < MAX_BACKUPS ) {
        cnt = asm->increment;
        dwptr = (DWORD *)asm->data;
        Offset = dwptr[asm->usage_cnt];
        while( cnt > 0 ) {
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            cnt--;
        }
        asm->usage_cnt ++;
        dwptr[asm->usage_cnt] = Offset;
    }
} /* GenBigBackup */

/*
 * GetInsCnt - finds the number of instructions before the one that
 *             straddles offset
 */

DWORD GetInsCnt( MemWndInfo *info, DWORD offset ) {

    AsmInfo     *asm;
    DWORD       *dwptr;
    DWORD       old_offset;
    DWORD       ins_cnt;
    WORD        *wptr;
    WORD        i;
    instruction ins;

    Is32Bit = ( info->disp_type == MEMINFO_CODE_32 );
    Sel = info->sel;
    Limit = info->limit;
    asm = info->asm;
    if( asm->big ) {
        dwptr = (DWORD *)asm->data;
        while( dwptr[asm->usage_cnt] < offset ) {
            if( asm->usage_cnt == MAX_BACKUPS ) break;
            GenBigBackup( asm );
        }
        for( i=0; dwptr[i] <= offset && i < MAX_BACKUPS - 1; i++ );
        i--;
        Offset = dwptr[i];
        ins_cnt = i * asm->increment;
        while( Offset <= offset ) {
            old_offset = Offset;
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            ins_cnt ++;
        }
    } else {
        wptr = (WORD *)asm->data;
        while( wptr[asm->usage_cnt] < offset ) {
            if( asm->usage_cnt == MAX_BACKUPS ) break;
            GenBackup( asm );
        }
        for( i=0; wptr[i] <= offset && i < MAX_BACKUPS - 1; i++ );
        i--;
        Offset = wptr[i];
        ins_cnt = i * asm->increment;
        while( Offset <= offset ) {
            old_offset = Offset;
            MiscDoCode( &ins, Is32Bit, &DisasmInfo );
            ins_cnt ++;
        }
    }
    /* we go one instruction too far if offset == info->limit */
    if( Offset >= info->limit ) ins_cnt --;
    if( ins_cnt > 0 ) ins_cnt--;
    return( ins_cnt );
} /* GetInsCnt */

/*
 * ScrollAsm - deal with scroll messages when the memory window is
 *              displaying code
 */
void ScrollAsm( HWND hwnd, WORD wparam, WORD pos, MemWndInfo *info ) {

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
        offset = GenAsmLine( info, info->ins_cnt + info->lastline, buf );
        dc = GetDC( hwnd );
        GetClientRect( hwnd, &area );
        area.right = info->width;
        if( offset >= Limit ) {
            offset = Limit;
            wbrush = GetStockObject( WHITE_BRUSH );
            GotoIns( info, info->ins_cnt + 1 );
            if( Offset < Limit ) {
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
            /* this is an approximation of the offset of the first displayed
               instruction */
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
//      MySetScrollPos( info->scrlbar, offset, info->limit );
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
            offset = GenAsmLine( info, info->ins_cnt, buf );
//          MySetScrollPos( info->scrlbar, offset, info->limit );
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
        GotoIns( info, info->ins_cnt );
        while( Offset >= Limit ) {
            info->ins_cnt --;
            GotoIns( info, info->ins_cnt );
        }
        dc = GetDC( hwnd );
        RedrawAsCode( dc, info );
        ReleaseDC( hwnd, dc );
        break;
#if(0)
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        offset = ScrollPosToOffset( pos, info->limit );
        if( offset > info->limit ) offset = info->limit;
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
#if(0)
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
 * GotoIns - sets Offset to the instruction ins_cnt instructions
 *           from the beginning of the item
 */
static void GotoIns( MemWndInfo *info, DWORD ins_cnt ) {

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
            asm->big = ( info->limit > 0xffff );
            asm->increment = info->limit / backup_cnt;
            if( asm->increment == 0 ) asm->increment = 1;
            asm->usage_cnt = 0;
            memset( asm->data, 0, 4 );
        }
    } else {
        asm = info->asm;
    }
    Is32Bit = ( info->disp_type == MEMINFO_CODE_32 );
    Sel = info->sel;
    Limit = info->limit;

    /*
     * generate new backups
     */

    if( ins_cnt >= asm->increment * ( asm->usage_cnt + 1 ) && asm != NULL ) {
        if( asm->big ) {
            dwptr = (DWORD *)asm->data;
            dwptr += asm->usage_cnt;
            Offset = *dwptr;
            cnt = ins_cnt - ( ins_cnt % asm->increment );
            cnt -= asm->usage_cnt * asm->increment;
            cnt /= asm->increment;
            while( cnt ) {
                GenBigBackup( asm );
                cnt--;
            }
        } else {
            wptr = (WORD *)asm->data;
            wptr += asm->usage_cnt;
            Offset = (DWORD)*wptr;
            cnt = ins_cnt - ( ins_cnt % asm->increment );
            cnt -= asm->usage_cnt * asm->increment;
            cnt /= asm->increment;
            while( cnt ) {
                GenBackup( asm );
                cnt--;
            }
        }
    }
    if( asm != NULL ) {
        if( asm->big ) {
            dwptr = (DWORD *)asm->data;
            Offset = dwptr[ins_cnt/asm->increment];
        } else {
            wptr = (WORD *) asm->data;
            Offset = wptr[ins_cnt/asm->increment];
        }
        ins_cnt = ins_cnt % asm->increment;
    } else {
        Offset = 0;
    }
    while( ins_cnt ) {
        MiscDoCode( &ins, Is32Bit, &DisasmInfo );
        ins_cnt --;
    }
} /* GotoIns */

/*
 * RedrawAsCode
 */

void RedrawAsCode( HDC dc, MemWndInfo *info ) {

    DWORD       line;
    char        buf[80];
    RECT        area;
    HBRUSH      wbrush;
    instruction ins;
    HFONT       old_font;

    old_font = SelectObject( dc, GetMonoFont() );
    Is32Bit = ( info->disp_type == MEMINFO_CODE_32 );
    Sel = info->sel;
    Limit = info->limit;
    GotoIns( info, info->ins_cnt );
    wbrush = GetStockObject( WHITE_BRUSH );
    area.top = 0;
    area.bottom = FontHeight;
    area.left = 0;
    area.right = info->width;
//    MySetScrollPos( info->scrlbar, Offset, info->limit );
    for( line = 0; line <= info->lastline; line ++ ) {
        if( Offset >= Limit ) break;
        sprintf( buf, "%08lX  ", Offset );
        MiscDoCode( &ins, Is32Bit, &DisasmInfo );
        MiscFormatIns( buf + 10 , &ins, 0, &DisasmInfo );
        FillRect( dc, &area, wbrush );
        DrawText( dc, buf, -1, &area, DT_LEFT );
        area.top = area.bottom;
        area.bottom += FontHeight;
    }
    /* clear the bottom of the window when there is no data there */
    if( line <= info->lastline ) {
        area.bottom = FontHeight * ( info->lastline + 1 );
        FillRect( dc, &area, wbrush );
    }
    SelectObject( dc, old_font );
} /* RedrawAsCode */

/*
 * RegDisasmRtns - register us to use the interface to the disasembler
 */

void RegDisasmRtns() {

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
    DisasmRegistered = TRUE;
}
