/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


#include <dos.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "stdui.h"
#include "dbgmem.h"
#include "dsxutil.h"
#include "dbgscrn.h"
#include "uidbg.h"
#include "guigmous.h"
#include "dbgcmdln.h"
#include "dbglkup.h"
#include "dbgerr.h"
#include "dsxscrn.h"


#define EGA_VIDEO_BUFF          (LP_PIXEL)RealModeSegmPtr( 0xa000 )
#define MONO_VIDEO_BUFF         (LP_PIXEL)RealModeSegmPtr( 0xb000 )
#define COLOUR_VIDEO_BUFF       (LP_PIXEL)RealModeSegmPtr( 0xb800 )

#define RmSwapSegPtr( offs )    SwapSeg.rm, offs

#define TstMono()               ChkCntrlr( VIDMONOINDXREG )
#define TstColour()             ChkCntrlr( VIDCOLRINDXREG )

#define save_to_rm_swap( off, data, size )      _fmemcpy( EXTENDER_RM2PM( SwapSeg.rm, off ), data, size )
#define restore_from_rm_swap( off, data, size ) _fmemcpy( data, EXTENDER_RM2PM( SwapSeg.rm, off ), size )

#define save_to_swap( off, data, size )         _fmemcpy( RegenSave + off, data, size )
#define restore_from_swap( off, data, size )    _fmemcpy( data, RegenSave + off, size )

#define _NBPARAS( bytes )       ((bytes + 15UL) / 16)

extern bool                     UserScreen( void );

extern gui_window_styles        WndStyle;

static flip_types               FlipMech;
static mode_types               ScrnMode;
static rm_call_struct           CallStruct;
static unsigned char            OldRow;
static unsigned char            OldCol;
static CURSOR_TYPE              OldTyp;
static bool                     OnAlt;
static screen_info              StartScrn;
static screen_info              SaveScrn;
static uint_16                  VIDPort;
static uint_16                  PageSize;
static uint_16                  CurOffst;
static uint_16                  RegCur;
static uint_16                  InsCur;
static uint_16                  NoCur;
static unsigned char            DbgBiosMode;
static unsigned char            DbgChrSet;
static unsigned char            DbgRows;
static dpmi_dos_block           SwapSeg;
static addr32_off               VidStateOff = 0;
static addr32_off               PgmMouse = 0;
static addr32_off               DbgMouse = 0;
static display_config           HWDisplay;
static unsigned char            *RegenSave;
static LP_PIXEL                 VirtScreen;

static const adapter_type       ColourAdapters[] = {
    #define pick_disp(e,t) t,
        DISP_TYPES()
    #undef pick_disp
};

static const char               ScreenOptNameTab[] = {
    #define pick_opt(e,t) t "\0"
        SCREEN_OPTS()
    #undef pick_opt
};

static uint_16 _VidStateSize( uint_16 requested_state )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1c00;
    CallStruct.ecx = requested_state;
    DPMISimulateRealModeInterrupt( VIDEO_VECTOR, 0, 0, &CallStruct );
    if( (CallStruct.eax & 0xff) != 0x1c )
        return( 0 );
    return( CallStruct.ebx );
}

static void _VidStateSave( uint_16 requested_state, addr_seg buff_rmseg, addr32_off buff_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1c01;
    CallStruct.es = buff_rmseg;
    CallStruct.ebx = buff_offset;
    CallStruct.ecx = requested_state;
    DPMISimulateRealModeInterrupt( VIDEO_VECTOR, 0, 0, &CallStruct );
}

static void _VidStateRestore( uint_16 requested_state, addr_seg buff_rmseg, addr32_off buff_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1c02;
    CallStruct.es = buff_rmseg;
    CallStruct.ebx = buff_offset;
    CallStruct.ecx = requested_state;
    DPMISimulateRealModeInterrupt( VIDEO_VECTOR, 0, 0, &CallStruct );
}

static void BIOSCharSet( unsigned char vidroutine, unsigned char bytesperchar,
                         uint_16 patterncount, uint_16 charoffset,
                         addr_seg table_rmseg, addr32_off table_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1100 | vidroutine;
    CallStruct.ebx = (uint_32)bytesperchar << 8;
    CallStruct.ecx = patterncount;
    CallStruct.edx = charoffset;
    CallStruct.es = table_rmseg;
    CallStruct.ebp = table_offset;
    DPMISimulateRealModeInterrupt( VIDEO_VECTOR, 0, 0, &CallStruct );
}

static uint_16 BIOSDevCombCode( void )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1a00;
    DPMISimulateRealModeInterrupt( VIDEO_VECTOR, 0, 0, &CallStruct );
    if( (CallStruct.eax & 0xff) != 0x1a )
        return( 0 );
    return( CallStruct.ebx );
}

static uint_16 MouseStateSize( void )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x15;
    DPMISimulateRealModeInterrupt( MSMOUSE_VECTOR, 0, 0, &CallStruct );
    return( CallStruct.ebx );
}

static void MouseStateSave( addr_seg buff_rmseg, addr32_off buff_offset, uint_16 size )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x16;
    CallStruct.ebx = size;
    CallStruct.es = buff_rmseg;
    CallStruct.edx = buff_offset;
    DPMISimulateRealModeInterrupt( MSMOUSE_VECTOR, 0, 0, &CallStruct );
}

static void MouseStateRestore( addr_seg buff_rmseg, addr32_off buff_offset, uint_16 size )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x17;
    CallStruct.ebx = size;
    CallStruct.es = buff_rmseg;
    CallStruct.edx = buff_offset;
    DPMISimulateRealModeInterrupt( MSMOUSE_VECTOR, 0, 0, &CallStruct );
}

static void _DoRingBell( unsigned char page )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.ebx = page * 0x100U;
    CallStruct.eax = 0x0e07;
    DPMISimulateRealModeInterrupt( VIDEO_VECTOR, 0, 0, &CallStruct );
}

void Ring_Bell( void )
{
    _DoRingBell( BIOSData( BD_ACT_VPAGE, unsigned char ) );
}

static void VIDSetPos( uint_16 vidport, uint_16 cursorpos )
{
    VIDSetRow( vidport, cursorpos & 0xff );
    VIDSetCol( vidport, cursorpos >> 8 );
}

static void VIDSetCurTyp( uint_16 vidport, uint_16 cursortyp )
{
    _WriteCRTCReg( vidport, CURS_START_SCANLINE, cursortyp >> 8 );
    _WriteCRTCReg( vidport, CURS_END_SCANLINE, cursortyp & 0xf );
}

static uint_16 VIDGetCurTyp( uint_16 vidport )
{
    return( _ReadCRTCReg( vidport, CURS_START_SCANLINE ) * 0x100U
            | _ReadCRTCReg( vidport, CURS_END_SCANLINE ) );
}

static bool ChkCntrlr( uint_16 port )
{
    unsigned char       curr;
    bool                rtrn;

    curr = VIDGetRow( port );
    VIDSetRow( port, 0x5a );
    VIDWait();
    VIDWait();
    VIDWait();
    rtrn = ( VIDGetRow( port ) == 0x5a );
    VIDSetRow( port, curr );
    return( rtrn );
}

static void DoSetMode( unsigned char mode )
{
    unsigned char   equip;

    equip = StartScrn.swtchs & ~0x30;
    if( ISMONOMODE( mode & 0x7f ) ) {
        equip |= 0x30;
    } else {
        equip |= 0x20;
    }
    BIOSData( BD_EQUIP_LIST, unsigned char ) = equip;
    BIOSSetMode( mode );
}

static void GetEGAConfig( unsigned char colour, unsigned char curr_mode )
{
    hw_display_type     display;

    if( colour ) {
        HWDisplay.active = DISP_EGA_MONO;
        if( TstColour() ) {
            HWDisplay.alt = DISP_CGA;
        }
    } else {
        HWDisplay.active = DISP_EGA_COLOUR;
        if( TstMono() ) {
            HWDisplay.alt = DISP_MONOCHROME;
        }
    }
    if( ( HWDisplay.active == DISP_EGA_COLOUR ) && ISMONOMODE( curr_mode )
      || ( HWDisplay.active == DISP_EGA_MONO ) && !ISMONOMODE( curr_mode ) ) {
        /* EGA is not the active display */
        display = HWDisplay.active;
        HWDisplay.active = HWDisplay.alt;
        HWDisplay.alt = display;
    }
}

static void GetMonoConfig( unsigned char curr_mode )
{
    HWDisplay.active = DISP_MONOCHROME;
    if( TstColour() ) {
        if( curr_mode == 7 ) {
            HWDisplay.alt = DISP_CGA;
        } else {
            HWDisplay.active = DISP_CGA;
            HWDisplay.alt = DISP_MONOCHROME;
        }
    }
}

static void GetDispConfig( void )
{
    uint_32         info;
    unsigned char   colour;
    unsigned char   memory;
    unsigned char   swtchs;
    unsigned char   curr_mode;
    uint_16         dev_config;

    dev_config = BIOSDevCombCode();
    HWDisplay.active = dev_config & 0xff;
    HWDisplay.alt = (dev_config >> 8) & 0xff;
    if( HWDisplay.active == DISP_NONE ) {
        /* have to figure it out ourselves */
        curr_mode = BIOSGetMode() & 0x7f;
        info = BIOSEGAInfo();
        memory = info;
        colour = info >> 8;
        swtchs = info >> 16;
        if( ( swtchs < 12 ) && ( memory <= 3 ) && ( colour <= 1 ) ) {
            /* we have an EGA display */
            GetEGAConfig( colour, curr_mode );
        } else if( TstMono() ) {
            /* we have a monochrome display */
            GetMonoConfig( curr_mode );
        } else {
            /* only thing left is a single CGA display */
            HWDisplay.active = DISP_CGA;
        }
    }
}

static bool ChkForColour( hw_display_type display )
{
    if( ColourAdapters[display] == ADAPTER_COLOUR ) {
        ScrnMode = MD_COLOUR;
        return( true );
    }
    return( false );
}

static void SwapActAlt( void )
{
    hw_display_type     display;

    display = HWDisplay.active;
    HWDisplay.active = HWDisplay.alt;
    HWDisplay.alt = display;
    OnAlt = true;
}

static bool ChkColour( void )
{
    if( ChkForColour( HWDisplay.active ) ) {
        return( true );
    }
    if( ChkForColour( HWDisplay.alt ) ) {
        SwapActAlt();
        return( true );
    }
    return( false );
}

static bool ChkForMono( hw_display_type display )
{
    if( ColourAdapters[display] == ADAPTER_MONO ) {
        ScrnMode = MD_MONO;
        return( true );
    }
    return( false );
}

static bool ChkMono( void )
{
    if( ChkForMono( HWDisplay.active ) ) {
        return( true );
    }
    if( ChkForMono( HWDisplay.alt ) ) {
        SwapActAlt();
        return( true );
    }
    return( false );
}

static bool ChkForEGA( hw_display_type display )
{
    switch( display ) {
    case DISP_EGA_COLOUR:
    case DISP_VGA_COLOUR:
    case DISP_EGA_MONO:
    case DISP_VGA_MONO:
        ScrnMode = MD_EGA;
        return( true );
    default:
        return( false );
    }
}

static bool ChkEGA( void )
{
    if( ChkForEGA( HWDisplay.active ) ) {
        return( true );
    }
    if( ChkForEGA( HWDisplay.alt ) ) {
        SwapActAlt();
        return( true );
    }
    return( false );
}

static void GetDefault( void )
{
    if( ISMONOMODE( StartScrn.mode ) ) {
        if( FlipMech == FLIP_TWO ) {
            if( !ChkColour() ) {
                FlipMech = FLIP_SWAP;
                ChkMono();
            }
        } else {
            ChkMono();
        }
    } else {
        if( FlipMech == FLIP_TWO ) {
            if( !ChkMono() ) {
                FlipMech = FLIP_PAGE;
                ChkColour();
            }
        } else {
            ChkColour();
        }
    }
}

static void ChkPage( void )
{
    switch( ScrnMode ) {
    case MD_MONO:
        FlipMech = FLIP_SWAP;
        break;
    case MD_EGA:
    case MD_COLOUR:
        break;
    default:
        FlipMech = FLIP_SWAP; /* for now */
        break;
    }
}

static void ChkTwo( void )
{
    if( HWDisplay.alt == DISP_NONE ) {
        FlipMech = FLIP_PAGE;
        ChkPage();
    }
}

static void SetChrSet( unsigned char set )
{
    if( set != USER_CHR_SET ) {
        BIOSEGAChrSet( set );
    }
}

static unsigned char GetChrSet( unsigned char rows )
{
    if( rows >= 43 ) {
        return( DOUBLE_DOT_CHR_SET );
    }
    if( rows >= 28 ) {
        return( COMPRESSED_CHR_SET );
    }
    return( USER_CHR_SET );
}

static void SetEGA_VGA( unsigned char double_rows )
{
    if( ScrnMode == MD_EGA ) {
        DbgRows = double_rows;
        DbgChrSet = DOUBLE_DOT_CHR_SET;
    } else if( FlipMech == FLIP_SWAP || FlipMech == FLIP_CHEAPSWAP ) {
        DbgChrSet = GetChrSet( BIOSGetRows() );
        switch( DbgChrSet ) {
        case USER_CHR_SET:
            DbgRows = 25;
            break;
        case COMPRESSED_CHR_SET:
            DbgRows = 28;
            break;
        case DOUBLE_DOT_CHR_SET:
            DbgRows = double_rows;
            break;
        }
    } else {
        DbgRows = BIOSGetRows();
        DbgChrSet = USER_CHR_SET;
    }
}

static void SetMonitor( void )
{
    DbgChrSet = USER_CHR_SET;
    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        DbgBiosMode = 7;
        break;
    case DISP_CGA:
    case DISP_PGA:              /* just guessing here */
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
        DbgBiosMode = ( ( StartScrn.mode == 2 ) && !OnAlt ) ? 2 : 3;
        break;
    case DISP_EGA_MONO:
        DbgBiosMode = 7;
        SetEGA_VGA( 43 );
        break;
    case DISP_EGA_COLOUR:
        DbgBiosMode = 3;
        SetEGA_VGA( 43 );
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        DbgBiosMode = ( ( StartScrn.mode == 7 ) && !OnAlt ) ? 7 : 3;
        SetEGA_VGA( 50 );
        break;
    }
    if( DbgRows == 0 ) {
        DbgRows = 25;
    }
    VIDPort = ( DbgBiosMode == 7 ) ? VIDMONOINDXREG : VIDCOLRINDXREG;
    if( ( (StartScrn.mode & 0x7f) == DbgBiosMode ) && ( StartScrn.strt.rows == DbgRows ) ) {
        PageSize = BIOSData( BD_REGEN_LEN, uint_16 );   /* get size from BIOS */
    } else {
        PageSize = ( DbgRows == 25 ) ? 4096 : ( 2 * DbgRows * 80 + 256 );
    }
}

static void SaveBIOSSettings( void )
{
    SaveScrn.swtchs = BIOSData( BD_EQUIP_LIST, unsigned char );
    SaveScrn.mode = BIOSGetMode();
    SaveScrn.save.page = BIOSGetPage();
    SaveScrn.save.curpos = BIOSGetCurPos( SaveScrn.save.page );
    SaveScrn.curtyp = BIOSGetCurTyp( SaveScrn.save.page );
    if( ( SaveScrn.curtyp == CGA_CURSOR_ON ) && ( SaveScrn.mode == 7 ) ) {
        /* screwy hercules card lying about cursor type */
        SaveScrn.curtyp = MON_CURSOR_ON;
    }
    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        SaveScrn.points = BIOSGetPoints();
        break;
    default:
        SaveScrn.points = 8;
        break;
    }
}

static void GetAdapter( void )
{
    switch( ScrnMode ) {
    case MD_HERC: /* temp */
    case MD_DEFAULT:
        GetDefault();
        break;
    case MD_MONO:
        if( !ChkMono() ) {
            GetDefault();
        }
        break;
    case MD_COLOUR:
        if( !ChkColour() ) {
            GetDefault();
        }
        break;
    case MD_EGA:
        if( !ChkEGA() ) {
            GetDefault();
        }
        break;
    }
}

/* ConfigScreen -- figure out screen configuration we're going to use. */
unsigned ConfigScreen( void )
{
    OnAlt = false;
    GetDispConfig();
    SaveBIOSSettings();
    StartScrn.points = SaveScrn.points;
    StartScrn.curtyp = SaveScrn.curtyp;
    StartScrn.swtchs = SaveScrn.swtchs;
    StartScrn.mode = SaveScrn.mode;
    StartScrn.strt.attr = 0;
    if( ISTEXTMODE( StartScrn.mode ) )
        StartScrn.strt.attr = BIOSGetAttr( SaveScrn.save.page ) & 0x7f;
    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        StartScrn.strt.rows = BIOSGetRows();
        break;
    }
    GetAdapter();
    /* get flip mechanism to use */
    switch( FlipMech ) {
    case FLIP_PAGE:
        ChkPage();
        break;
    case FLIP_TWO:
        ChkTwo();
        break;
    }
    if( uiisdbcs() ) {
        VirtScreen = dos_uishadowbuffer( COLOUR_VIDEO_BUFF );
        if( VirtScreen == COLOUR_VIDEO_BUFF ) {
            VirtScreen = NULL;
        } else if( FlipMech == FLIP_PAGE ) {
            FlipMech = FLIP_CHEAPSWAP;
        }
    } else if( FlipMech == FLIP_SWAP ) {
        FlipMech = FLIP_CHEAPSWAP;
    }
    SetMonitor();
    BIOSSetCurTyp( StartScrn.curtyp );
    return( PageSize );
}

static bool SetMode( unsigned char mode )
{
    if( (BIOSGetMode() & 0x7f) == (mode & 0x7f) ) {
        return( false );
    }
    DoSetMode( mode );
    return( true );
}

static void SetRegenClear( void )
{
    BIOSData( BD_VID_CTRL1, unsigned char ) = (BIOSData( BD_VID_CTRL1, unsigned char ) & 0x7f) | (SaveScrn.mode & 0x80);
}

static uint_16 RegenSize( void )
{
    uint_16     regen_size;

    regen_size = 0;
    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        regen_size = RoundTo1K( 2 * DbgRows * 80 );
        break;
    case DISP_CGA:
    case DISP_PGA:
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
        regen_size = 4 * RoundTo1K( 2 * DbgRows * 80 );
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
    case DISP_EGA_COLOUR:
    case DISP_EGA_MONO:
        regen_size = 2 * PageSize;
        break;
    }
    return( regen_size );
}

static void SetupEGA( void )
{
    _disablev( VIDPort + 6 );
    _seq_write( SEQ_MEM_MODE, MEM_NOT_ODD_EVEN );
    _graph_write( GRA_MISC, MIS_A000_64 | MIS_GRAPH_MODE );
    _graph_write( GRA_ENABLE_SR, 0 );
    _graph_write( GRA_DATA_ROT, ROT_UNMOD | 0 );
    _graph_write( GRA_GRAPH_MODE, GRM_EN_ROT );
}

static void SwapSave( void )
{
    switch( HWDisplay.active ) {
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        _VidStateSave( VID_STATE_SWAP, RmSwapSegPtr( VidStateOff ) );
        /* fall through */
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        SetupEGA();
        if( FlipMech == FLIP_CHEAPSWAP ) {
            _graph_write( GRA_READ_MAP, RMS_MAP_0 );
            save_to_swap( 0 * PageSize, EGA_VIDEO_BUFF, PageSize );
            _graph_write( GRA_READ_MAP, RMS_MAP_1 );
            save_to_swap( 1 * PageSize, EGA_VIDEO_BUFF, PageSize );
            _graph_write( GRA_READ_MAP, RMS_MAP_2 );
            save_to_rm_swap( 0, EGA_VIDEO_BUFF, FONT_TABLE_SIZE );
            if( VirtScreen != NULL ) {
                save_to_swap( 2 * PageSize, VirtScreen,  PageSize );
            }
        } else {
            _graph_write( GRA_READ_MAP, RMS_MAP_0 );
            save_to_swap( 0 * _64k, EGA_VIDEO_BUFF, _64k );
            _graph_write( GRA_READ_MAP, RMS_MAP_1 );
            save_to_swap( 1 * _64k, EGA_VIDEO_BUFF, _64k );
            _graph_write( GRA_READ_MAP, RMS_MAP_2 );
            save_to_swap( 2 * _64k, EGA_VIDEO_BUFF, _64k );
            _graph_write( GRA_READ_MAP, RMS_MAP_3 );
            save_to_swap( 3 * _64k, EGA_VIDEO_BUFF, _64k );
            if( VirtScreen != NULL ) {
                save_to_swap( 4 * _64k, VirtScreen,  PageSize );
            }
        }
        _graph_write( GRA_READ_MAP, RMS_MAP_0 );
        /* blank regen area (attributes) */
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        Fillb( EGA_VIDEO_BUFF, 0, PageSize );
        DoSetMode( DbgBiosMode | 0x80 );
        SetChrSet( DbgChrSet );
        break;
    case DISP_MONOCHROME:
        save_to_swap( 0, MONO_VIDEO_BUFF, RegenSize() );
        SetMode( DbgBiosMode );
        break;
    default:
        save_to_swap( 0, COLOUR_VIDEO_BUFF, RegenSize() );
        SetMode( DbgBiosMode );
        break;
    }
}

static void RestoreEGA_VGA( bool isvga )
{
    unsigned char   mode;

    if( FlipMech == FLIP_CHEAPSWAP ) {
        SetupEGA();
        _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
        restore_from_swap( 0 * PageSize, EGA_VIDEO_BUFF, PageSize );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        restore_from_swap( 1 * PageSize, EGA_VIDEO_BUFF, PageSize );
        mode = SaveScrn.mode & 0x7f;
        if( ISTEXTMODE( mode ) ) {
            if( VirtScreen != NULL ) {
                restore_from_swap( 2 * PageSize, VirtScreen, PageSize );
                _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
                restore_from_rm_swap( 0, EGA_VIDEO_BUFF, FONT_TABLE_SIZE );
                DoSetMode( SaveScrn.mode | 0x80 );
            } else {
                DoSetMode( SaveScrn.mode | 0x80 );
                BIOSCharSet( 0, FONT_TABLE_SIZE / 256, 256, 0, RmSwapSegPtr( 0 ) );
                if( !isvga ) {
                    BIOSCharSet( 0x10, SaveScrn.points, 0, 0, 0, 0 );
                }
            }
        } else {
            _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
            restore_from_rm_swap( 0, EGA_VIDEO_BUFF, FONT_TABLE_SIZE );
            DoSetMode( SaveScrn.mode | 0x80 );
        }
    } else {
        /* stupid thing doesn't respect the no-clear bit in DBCS mode */
        DoSetMode( SaveScrn.mode );
        if( VirtScreen != NULL ) {
            restore_from_swap( 4 * _64k, VirtScreen, PageSize );
        }
        SetupEGA();
        _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
        restore_from_swap( 0 * _64k, EGA_VIDEO_BUFF, _64k );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        restore_from_swap( 1 * _64k, EGA_VIDEO_BUFF, _64k );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
        restore_from_swap( 2 * _64k, EGA_VIDEO_BUFF, _64k );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_3 );
        restore_from_swap( 3 * _64k, EGA_VIDEO_BUFF, _64k );
    }
    if( isvga ) {
        _VidStateRestore( VID_STATE_SWAP, RmSwapSegPtr( VidStateOff ) );
    } else {
        _seq_write( SEQ_CHAR_MAP_SEL, PMData->act_font_tbls );
    }
    SetRegenClear();
}

static void SwapRestore( void )
{
    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        RestoreEGA_VGA( false );
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        RestoreEGA_VGA( true );
        break;
    case DISP_MONOCHROME:
        SetMode( SaveScrn.mode );
        restore_from_swap( 0, MONO_VIDEO_BUFF, RegenSize() );
        break;
    default:
        SetMode( SaveScrn.mode );
        restore_from_swap( 0, COLOUR_VIDEO_BUFF, RegenSize() );
        break;
    }
}

static void SaveMouse( addr32_off to )
{
    if( DbgMouse != PgmMouse ) {
        MouseStateSave( RmSwapSegPtr( to ), (uint_16)( DbgMouse - PgmMouse ) );
    }
}

static void RestoreMouse( addr32_off from )
{
    if( DbgMouse != PgmMouse ) {
        MouseStateRestore( RmSwapSegPtr( from ), (uint_16)( DbgMouse - PgmMouse ) );
    }
}

static void AllocSave( void )
{
    uint_16     state_size;
    uint_16     mouse_size;
    uint_16     regen_size;
    uint_16     rm_regen_size;

    rm_regen_size = 0;
    switch( FlipMech ) {
    case FLIP_SWAP:
        if( VirtScreen != NULL ) {
            _Alloc( RegenSave, 4 * _64k + PageSize );
            if( RegenSave == NULL ) {
                StartupErr( LIT_ENG( ERR_NO_MEMORY ) );
            }
            break;
        }
        /* fall through */
    case FLIP_CHEAPSWAP:
        FlipMech = FLIP_CHEAPSWAP;
        regen_size = RegenSize();
        if( VirtScreen != NULL )
            regen_size += PageSize;
        _Alloc( RegenSave, regen_size );
        if( RegenSave == NULL ) {
            StartupErr( LIT_ENG( ERR_NO_MEMORY ) );
        }
        /* Font table has to go in low memory */
        rm_regen_size = FONT_TABLE_SIZE;
        break;
    }
    state_size = 64 * _VidStateSize( VID_STATE_SWAP );
    mouse_size = 0;
    if( _IsOn( SW_USE_MOUSE ) )
        mouse_size = MouseStateSize();
    SwapSeg = DPMIAllocateDOSMemoryBlock( _NBPARAS( rm_regen_size + state_size + 2 * mouse_size ) );
    if( SwapSeg.pm == 0 ) {
        StartupErr( LIT_ENG( Unable_to_alloc_DOS_mem ) );
    }
    /***************************************************************
      Real-mode Swap section layout
    ****************************************************************
    | regen data | Video state | Mouse state pgm | Mouse state dbg |
    ****************************************************************/
    VidStateOff = rm_regen_size;
    PgmMouse = regen_size + state_size;
    DbgMouse = PgmMouse + mouse_size;
}


static void CheckMSMouse( void )
/* check for Microsoft mouse */
{
    addr32_ptr      vect;

    vect = RealModeData( 0, MSMOUSE_VECTOR * 4, addr32_ptr );
    if( vect.segment == 0 && vect.offset == 0
      || RealModeData( vect.segment, vect.offset, unsigned char ) == IRET ) {
        _SwitchOff( SW_USE_MOUSE );
    }
}

static void SetCursorTypes( void )
{
    unsigned char   scan_lines;

    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        RegCur = MON_CURSOR_ON;
        NoCur = NORM_CURSOR_OFF;
        break;
    case DISP_CGA:
    case DISP_PGA:              /* just guessing here */
        RegCur = CGA_CURSOR_ON;
        NoCur = NORM_CURSOR_OFF;
        break;
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        /* scan lines per character */
        scan_lines = BIOSGetPoints();
        RegCur = ( scan_lines - 1 ) + ( ( scan_lines - 2 ) * 0x100U );
        NoCur = EGA_CURSOR_OFF;
        break;
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        RegCur = VIDGetCurTyp( VIDPort );
        NoCur = NORM_CURSOR_OFF;
        break;
    }
    InsCur = CURSOR_REG2INS( RegCur );
}

static void InitScreenMode( void )
{
    CurOffst = 0;
    switch( FlipMech ) {
    case FLIP_SWAP:
    case FLIP_CHEAPSWAP:
        SwapSave();
        SetRegenClear();
        SetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        break;
    case FLIP_PAGE:
        SetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        SaveBIOSSettings();
        BIOSSetPage( 1 );
        CurOffst = BIOSData( BD_REGEN_LEN, uint_16 ) / 2;
        break;
    case FLIP_TWO:
        DoSetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        break;
    case FLIP_OVERWRITE:
        SetMode( DbgBiosMode );
        SetChrSet( DbgChrSet );
        SaveBIOSSettings();
        break;
    }
}

void InitScreen( void )
{
    CheckMSMouse();
    AllocSave();
    SaveMouse( PgmMouse );
    SaveMouse( DbgMouse );
    InitScreenMode();
    SetCursorTypes();
    if( DbgRows > 25 ) {
        WndStyle &= ~GUI_CHARMAP_DLG;
    }
    GUIInitGraphicsMouse( WndStyle );
    if( !uistart() ) {
        UserScreen();
        StartupErr( LIT_DUI( Unable_to_init_ui ) );
    }
    if( _IsOn( SW_USE_MOUSE ) ) {
        GUIInitMouse( INIT_MOUSE_INITIALIZED );
    }
}

/* UsrScrnMode -- setup the user screen mode */
bool UsrScrnMode( void )
{
    unsigned char       user_mode;
    bool                usr_vis;

    if( StartScrn.strt.attr && ( DbgBiosMode == StartScrn.mode ) ) {
        UIData->attrs[ATTR_NORMAL] = StartScrn.strt.attr;
        UIData->attrs[ATTR_BRIGHT] = StartScrn.strt.attr ^ 8;
        UIData->attrs[ATTR_REVERSE] = ( (StartScrn.strt.attr & 7) << 4 ) | ( (StartScrn.strt.attr & 0x70) >> 4 );
    }
    usr_vis = false;
    if( FlipMech == FLIP_TWO ) {
        usr_vis = true;
        SaveMouse( DbgMouse );
        RestoreMouse( PgmMouse );
        user_mode = ( DbgBiosMode == 7 ) ? 3 : 7;
        DoSetMode( user_mode );
        SaveBIOSSettings();
        SaveMouse( PgmMouse );
        RestoreMouse( DbgMouse );
    }
    SaveScrn.swtchs = BIOSData( BD_EQUIP_LIST, unsigned char );
    if( ( HWDisplay.active == DISP_VGA_COLOUR ) || ( HWDisplay.active == DISP_VGA_MONO ) ) {
        UIData->colour = M_VGA;
    }
    if( DbgRows != UIData->height ) {
        UIData->height = DbgRows;
        if( _IsOn( SW_USE_MOUSE ) ) {
            /* This is a sideways dive into the UI to get the boundries of
               the mouse cursor properly defined. */
            initmouse( INIT_MOUSE );
        }
    }
    return( usr_vis );
}


void DbgScrnMode( void )
{
    if( FlipMech == FLIP_PAGE ) {
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
            WndDirty( NULL );
        }
        BIOSSetPage( 1 );
    }
}

/* DebugScreen -- swap/page to debugger screen */
bool DebugScreen( void )
{
    bool                usr_vis;

    usr_vis = true;
    SaveMouse( PgmMouse );
    SaveBIOSSettings();
    switch( FlipMech ) {
    case FLIP_SWAP:
    case FLIP_CHEAPSWAP:
        SwapSave();
        BIOSSetPage( 0 );
        WndDirty( NULL );
        usr_vis = false;
        break;
    case FLIP_PAGE:
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
            WndDirty( NULL );
        }
        BIOSSetPage( 1 );
        usr_vis = false;
        break;
    case FLIP_OVERWRITE:
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
        }
        WndDirty( NULL );
        usr_vis = false;
        break;
    }
    RestoreMouse( DbgMouse );
    uiswap();
    return( usr_vis );
}


bool DebugScreenRecover( void )
{
    return( true );
}

/* UserScreen -- swap/page to user screen */
bool UserScreen( void )
{
    bool                dbg_vis;

    dbg_vis = true;
    uiswap();
    SaveMouse( DbgMouse );
    switch( FlipMech ) {
    case FLIP_SWAP:
    case FLIP_CHEAPSWAP:
        SwapRestore();
        dbg_vis = false;
        break;
    case FLIP_PAGE:
        dbg_vis = false;
        break;
    }
    BIOSSetPage( SaveScrn.save.page );
    BIOSSetCurTyp( SaveScrn.curtyp );
    BIOSSetCurPos( SaveScrn.save.curpos, SaveScrn.save.page );
    BIOSData( BD_EQUIP_LIST, unsigned char ) = SaveScrn.swtchs;
    RestoreMouse( PgmMouse );
    return( dbg_vis );
}

static void ReInitScreen( void )
{
    unsigned char   mode;

    RestoreMouse( PgmMouse );
    BIOSData( BD_EQUIP_LIST, unsigned char ) = StartScrn.swtchs;
    BIOSSetMode( StartScrn.mode );
    mode = StartScrn.mode & 0x7f;
    if( ISTEXTMODE( mode ) ) {
        switch( HWDisplay.active ) {
        case DISP_EGA_MONO:
        case DISP_EGA_COLOUR:
        case DISP_VGA_MONO:
        case DISP_VGA_COLOUR:
            SetChrSet( GetChrSet( StartScrn.strt.rows ) );
            break;
        }
    }
    BIOSSetCurTyp( StartScrn.curtyp );
    if( StartScrn.strt.attr ) {
        BIOSSetAttr( StartScrn.strt.attr );
    }
}

void SaveMainWindowPos( void )
{
}

/* FiniScreen -- finish screen swapping/paging */
void FiniScreen( void )
{
    if( _IsOn( SW_USE_MOUSE ) )
        GUIFiniMouse();
    uistop();
    if( ( SaveScrn.swtchs != StartScrn.swtchs ) ||
        ( SaveScrn.mode != StartScrn.mode ) ||
        ( SaveScrn.points != StartScrn.points ) ||
        ( FlipMech != FLIP_OVERWRITE ) ) {
        ReInitScreen();
    } else {
        UserScreen();
    }
    DPMIFreeDOSMemoryBlock( SwapSeg.pm );
    _Free( RegenSave );
}



/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

void uiinitcursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uiinitcursor();
    }
}

void uisetcursor( ORD row, ORD col, CURSOR_TYPE typ, int attr )
{
    uint_16     bios_cur_pos;

    if( FlipMech != FLIP_TWO ) {
        _uisetcursor( row, col, typ, attr );
    } else if( typ == C_OFF ) {
        uioffcursor();
    } else if( VIDPort != 0 && (ScrnState & DBG_SCRN_ACTIVE)
      && ( ( row != OldRow ) || ( col != OldCol ) || ( typ != OldTyp ) ) ) {
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        bios_cur_pos = BD_CURPOS;
        if( FlipMech == FLIP_PAGE ) {
            bios_cur_pos += 2;
        }
        BIOSData( bios_cur_pos + 0, unsigned char ) = OldCol;
        BIOSData( bios_cur_pos + 1, unsigned char ) = OldRow;
        VIDSetPos( VIDPort, CurOffst + row * UIData->width + col );
        VIDSetCurTyp( VIDPort, ( typ == C_INSERT ) ? InsCur : RegCur );
    }
}

void uioffcursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uioffcursor();
    } else if( (ScrnState & DBG_SCRN_ACTIVE) && ( VIDPort != 0 ) ) {
        OldTyp = C_OFF;
        VIDSetCurTyp( VIDPort, NoCur );
    }
}

void uiswapcursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uiswapcursor();
    }
}

void uifinicursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uifinicursor();
    }
}

void uirefresh( void )
{
    if( ScrnState & DBG_SCRN_ACTIVE ) {
        _uirefresh();
    }
}

void SetNumLines( int num )
{
    if( num >= 43 ) {
        ScrnMode = MD_EGA;
    }
}

void SetNumColumns( int num )
{
    num=num;
}

static void GetLines( void )
{
    if( HasEquals() ) {
        /* force a specified number of lines for MDA/CGA systems */
        DbgRows = GetValue();
    }
}

bool ScreenOption( const char *start, unsigned len, int pass )
{
    pass=pass;
    switch( Lookup( ScreenOptNameTab, start, len ) ) {
    case OPT_MONO:
        ScrnMode = MD_MONO;
        GetLines();
        break;
    case OPT_COLOR:
    case OPT_COLOUR:
        ScrnMode = MD_COLOUR;
        GetLines();
        break;
    case OPT_EGA43:
    case OPT_VGA50:
        ScrnMode = MD_EGA;
        break;
    case OPT_OVERWRITE:
        FlipMech = FLIP_OVERWRITE;
        break;
    case OPT_PAGE:
        FlipMech = FLIP_PAGE;
        break;
    case OPT_SWAP:
        FlipMech = FLIP_SWAP;
        WndStyle &= ~(GUI_CHARMAP_MOUSE | GUI_CHARMAP_DLG);
        break;
    case OPT_TWO:
        FlipMech = FLIP_TWO;
        break;
    default:
        return( false );
    }
    return( true );
}


void ScreenOptInit( void )
{
    ScrnMode = MD_DEFAULT;
    FlipMech = FLIP_TWO;
}
