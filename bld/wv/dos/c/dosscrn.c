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
#include "dbgmem.h"
#include "dbgscrn.h"
#include "dbgerr.h"
#include "stdui.h"
#include "dosscrn.h"
#include "tinyio.h"
#include "uidbg.h"
#include "dbgcmdln.h"
#include "dbglkup.h"


#define EGA_VIDEO_BUFF          0xa000, 0
#define MONO_VIDEO_BUFF         0xb000, 0
#define COLOUR_VIDEO_BUFF       0xb800, 0

#define SwapSegPtr( offs )      SwapSeg, offs

#define TstMono()               ChkCntrlr( VIDMONOINDXREG )
#define TstColour()             ChkCntrlr( VIDCOLRINDXREG )

#define save_to_swap( offs, data, size )       movedata( SwapSeg, offs, data, size )
#define restore_from_swap( offs, data, size )  movedata( data, SwapSeg, offs, size )

#define _NBPARAS( bytes )       ((bytes + 15UL) / 16)

unsigned char           ActFontTbls;  /* assembly file needs access */

static flip_types               FlipMech;
static mode_types               ScrnMode;
static unsigned char            OldRow;
static unsigned char            OldCol;
static CURSOR_TYPE              OldTyp;
static bool                     OnAlt;
static screen_info              StartScrn;
static screen_info              SaveScrn;
static unsigned         VIDPort;
static unsigned         PageSize;
static unsigned         CurOffst;
static unsigned         RegCur;
static unsigned         InsCur;
static unsigned         NoCur;
static unsigned char            DbgBiosMode;
static unsigned char            DbgChrSet;
static unsigned char            DbgRows;
static addr_seg         SwapSeg;
static addr32_off               VidStateOff = 0;
static addr32_off               PgmMouse = 0;
static addr32_off               DbgMouse = 0;
static display_config           HWDisplay;

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

void Ring_Bell( void )
{
    _DoRingBell( BIOSData( BD_ACT_VPAGE, unsigned char ) );
}

static void VIDSetPos( unsigned vidport, unsigned cursorpos )
{
    VIDSetRow( vidport, cursorpos & 0xff );
    VIDSetCol( vidport, cursorpos >> 8 );
}

static void VIDSetCurTyp( unsigned vidport, unsigned cursortyp )
{
    _WriteCRTCReg( vidport, CURS_START_SCANLINE, cursortyp >> 8 );
    _WriteCRTCReg( vidport, CURS_END_SCANLINE, cursortyp & 0x0f );
}

static unsigned VIDGetCurTyp( unsigned vidport )
{
    return( _ReadCRTCReg( vidport, CURS_START_SCANLINE ) * 0x100U
            | _ReadCRTCReg( vidport, CURS_END_SCANLINE ) );
}

static bool ChkCntrlr( unsigned port )
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
    if( HWDisplay.active == DISP_EGA_COLOUR && ISMONOMODE( curr_mode )
     || HWDisplay.active == DISP_EGA_MONO && !ISMONOMODE( curr_mode ) ) {
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
            HWDisplay.alt    = DISP_CGA;
        } else {
            HWDisplay.active = DISP_CGA;
            HWDisplay.alt = DISP_MONOCHROME;
        }
    }
}

static void GetDispConfig( void )
{
    unsigned long       info;
    unsigned char   colour;
    unsigned char   memory;
    unsigned char   swtchs;
    unsigned char   curr_mode;
    unsigned            dev_config;

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
    if( ChkForColour( HWDisplay.active ) )
        return( true );
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
    if( ChkForMono( HWDisplay.active ) )
        return( true );
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
        ScrnMode = MD_EGA;
        return( true );
    case DISP_EGA_MONO:
    case DISP_VGA_MONO:
        ScrnMode = MD_EGA;
        return( true );
    }
        return( false );
    }


static bool ChkEGA( void )
{
    if( ChkForEGA( HWDisplay.active ) )
        return( true );
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


static unsigned GetChrSet( unsigned char rows )
{
    if( rows >= 43 )
        return( DOUBLE_DOT_CHR_SET );
    if( rows >= 28 )
        return( COMPRESSED_CHR_SET );
    return( USER_CHR_SET );
}


static void SetEGA_VGA( unsigned char double_rows )
{
    if( ScrnMode == MD_EGA ) {
        DbgRows = double_rows;
        DbgChrSet = DOUBLE_DOT_CHR_SET;
    } else if( FlipMech == FLIP_SWAP ) {
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
        DbgBiosMode = ( StartScrn.mode == 2 && !OnAlt ) ? 2 : 3;
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
        DbgBiosMode = ( StartScrn.mode == 7 && !OnAlt ) ? 7 : 3;
        SetEGA_VGA( 50 );
        break;
    }
    if( DbgRows == 0 )
        DbgRows = 25;
    VIDPort = ( DbgBiosMode == 7 ) ? VIDMONOINDXREG : VIDCOLRINDXREG;
    if( ( (StartScrn.mode & 0x7f) == DbgBiosMode ) && ( StartScrn.strt.rows == DbgRows ) ) {
        PageSize = BIOSData( BD_REGEN_LEN, unsigned );  /* get size from BIOS */
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

    if( SaveScrn.curtyp == CGA_CURSOR_ON && SaveScrn.mode == 7 ) {
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
        if( !ChkMono() )
            GetDefault();
        break;
    case MD_COLOUR:
        if( !ChkColour() )
            GetDefault();
        break;
    case MD_EGA:
        if( !ChkEGA() )
            GetDefault();
        break;
    }
}

/*
 * ConfigScreen -- figure out screen configuration we're going to use.
 */

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

    /* get adapter to use */
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
    SetMonitor();
    BIOSSetCurTyp( StartScrn.curtyp );
    return( PageSize );
}


static bool SetMode( unsigned char mode )
{
    if( (BIOSGetMode() & 0x7f) == (mode & 0x7f) )
        return( false );
    DoSetMode( mode );
    return( true );
}


static void SetRegenClear( void )
{
    BIOSData( BD_VID_CTRL1, unsigned char ) = (BIOSData( BD_VID_CTRL1, unsigned char ) & 0x7f) | (SaveScrn.mode & 0x80);
}


static unsigned RegenSize( void )
{
    unsigned    regen_size;

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
        regen_size = 2 * PageSize + FONT_TABLE_SIZE;
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
        _VidStateSave( VID_STATE_SWAP, SwapSegPtr( VidStateOff ) );
        /* fall through */
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        SetupEGA();
            _graph_write( GRA_READ_MAP, RMS_MAP_0 );
            save_to_swap( 0 * PageSize, EGA_VIDEO_BUFF, PageSize );
            _graph_write( GRA_READ_MAP, RMS_MAP_1 );
            save_to_swap( 1 * PageSize, EGA_VIDEO_BUFF, PageSize );
            _graph_write( GRA_READ_MAP, RMS_MAP_2 );
        save_to_swap( 2 * PageSize, EGA_VIDEO_BUFF, FONT_TABLE_SIZE );
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

        SetupEGA();
        _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
        restore_from_swap( 0 * PageSize, EGA_VIDEO_BUFF, PageSize );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        restore_from_swap( 1 * PageSize, EGA_VIDEO_BUFF, PageSize );
        mode = SaveScrn.mode & 0x7f;
        if( ISTEXTMODE( mode ) ) {
                DoSetMode( SaveScrn.mode | 0x80 );
        BIOSCharSet( 0, FONT_TABLE_SIZE / 256, 256, 0, SwapSegPtr( 2 * PageSize ) );
                if( !isvga ) {
                    BIOSCharSet( 0x10, SaveScrn.points, 0, 0, 0, 0 );
                }
        } else {
            _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
        restore_from_swap( 2 * PageSize, EGA_VIDEO_BUFF, FONT_TABLE_SIZE );
            DoSetMode( SaveScrn.mode | 0x80 );
        }
    if( isvga ) {
        _VidStateRestore( VID_STATE_SWAP, SwapSegPtr( VidStateOff ) );
    } else {
        _seq_write( SEQ_CHAR_MAP_SEL, ActFontTbls );
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
        MouseStateSave( SwapSegPtr( to ), (unsigned)( DbgMouse - PgmMouse ) );
    }
}

static void RestoreMouse( addr32_off from )
{
    if( DbgMouse != PgmMouse ) {
        MouseStateRestore( SwapSegPtr( from ), (unsigned)( DbgMouse - PgmMouse ) );
    }
}


static void AllocSave( void )
{
    unsigned    state_size;
    unsigned    mouse_size;
    unsigned    regen_size;
    tiny_ret_t  ret;

    regen_size = 0;
    if( FlipMech == FLIP_SWAP )
        regen_size = RegenSize();
    state_size = 64 * _VidStateSize( VID_STATE_SWAP );
    mouse_size = 0;
    if( _IsOn( SW_USE_MOUSE ) )
        mouse_size = MouseStateSize();
    ret = TinyAllocBlock( _NBPARAS( regen_size + state_size + 2 * mouse_size ) );
    if( ret < 0 )
        StartupErr( "unable to allocate swap area" );
    SwapSeg = TINY_INFO( ret );
    /***************************************************************
      Swap section layout
    ****************************************************************
    | regen data | Video state | Mouse state pgm | Mouse state dbg |
    ****************************************************************/
    VidStateOff = regen_size;
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
        CurOffst = BIOSData( BD_REGEN_LEN, unsigned ) / 2;
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

/*
 * InitScreen
 */

void InitScreen( void )
{
    CheckMSMouse();
    AllocSave();
    SaveMouse( PgmMouse );
    SaveMouse( DbgMouse );
    InitScreenMode();
    SetCursorTypes();
    }


/*
 * UsrScrnMode -- setup the user screen mode
 */

bool UsrScrnMode( void )
{
    char    user_mode;
    bool                usr_vis;

    if( ( StartScrn.strt.attr != 0 ) && ( DbgBiosMode == StartScrn.mode ) ) {
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
    if( HWDisplay.active == DISP_VGA_COLOUR || HWDisplay.active == DISP_VGA_MONO ) {
        UIData->colour = M_VGA;
    }
    if( DbgRows != UIData->height ) {
        UIData->height = DbgRows;
        if( _IsOn( SW_USE_MOUSE ) ) {
            /*
                This is a sideways dive into the UI to get the boundries of
                the mouse cursor properly defined.
            */
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


/*
 * DebugScreen -- swap/page to debugger screen
 */

bool DebugScreen( void )
{
    bool                usr_vis;

    usr_vis = true;
    SaveMouse( PgmMouse );
    SaveBIOSSettings();
    switch( FlipMech ) {
    case FLIP_SWAP:
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


/*
 * UserScreen -- swap/page to user screen
 */

bool UserScreen( void )
{
    bool                dbg_vis;

    dbg_vis = true;
    uiswap();
    SaveMouse( DbgMouse );
    switch( FlipMech ) {
    case FLIP_SWAP:
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
    if( StartScrn.strt.attr != 0 ) {
        BIOSSetAttr( StartScrn.strt.attr );
    }
}

/*
 * FiniScreen -- finish screen swapping/paging
 */

void FiniScreen( void )
{
    uifini();
    if( SaveScrn.swtchs != StartScrn.swtchs
     || SaveScrn.mode != StartScrn.mode
     || SaveScrn.points != StartScrn.points
     || FlipMech != FLIP_OVERWRITE ) {
        ReInitScreen();
    } else {
        UserScreen();
    }
}


/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/


void uiinitcursor( void )
{
    }

void uisetcursor( ORD row, ORD col, CURSOR_TYPE typ, int attr )
{
    unsigned    bios_cur_pos;

    if( typ == C_OFF ) {
        uioffcursor();
    } else if( (ScrnState & DBG_SCRN_ACTIVE) && ( VIDPort != 0 ) ) {
        if( row == OldRow && col == OldCol && typ == OldTyp )
            return;
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        bios_cur_pos = BD_CURPOS;
        if( FlipMech == FLIP_PAGE )
            bios_cur_pos += 2;
        BIOSData( bios_cur_pos + 0, unsigned char ) = OldCol;
        BIOSData( bios_cur_pos + 1, unsigned char ) = OldRow;
        VIDSetPos( VIDPort, CurOffst + row * UIData->width + col );
        VIDSetCurTyp( VIDPort, ( typ == C_INSERT ) ? InsCur : RegCur );
    }
}


void uioffcursor( void )
{
    if( (ScrnState & DBG_SCRN_ACTIVE) && ( VIDPort != 0 ) ) {
        OldTyp = C_OFF;
        VIDSetCurTyp( VIDPort, NoCur );
    }
}

void uiswapcursor( void )
{
}

void uifinicursor( void )
{
    }

void uirefresh( void )
{
    if( ScrnState & DBG_SCRN_ACTIVE ) {
        _uirefresh();
    }
}

#if 0
#define OFF_SCREEN      200
static ORD OldMouseRow, OldMouseCol = OFF_SCREEN;
static bool MouseOn;
static ATTR OldAttr;


void uimouse( int func )
{
    if( func == 1 ) {
        MouseOn = true;
    } else {
        uisetmouse( 0, OFF_SCREEN );
        MouseOn = false;
    }
}

static unsigned char __far *RegenPos( unsigned row, unsigned col )
{
    unsigned char   __far *pos;

    pos = (unsigned char __far *)UIData->screen.origin + ( row * UIData->screen.increment + col ) * 2 + 1;
    if( UIData->colour == M_CGA && _IsOff( SW_NOSNOW ) ) {
        /* wait for vertical retrace */
        vertsync();
    }
    return( pos );
}

void uisetmouse( ORD row, ORD col )
{
    unsigned char   __far *old;
    unsigned char   __far *new;

    if( OldMouseRow == row && OldMouseCol == col )
        return;
    if( OldMouseCol != OFF_SCREEN ) {
        old = RegenPos( OldMouseRow, OldMouseCol );
        *old = OldAttr;
    }
    if( MouseOn ) {
        if( col != OFF_SCREEN ) {
            new = RegenPos( row, col );
            OldAttr = *new;
            *new = (OldAttr & 0x77) ^ 0x77;
        }
        OldMouseRow = row;
        OldMouseCol = col;
    }
}
#endif

static void GetLines( void )
{
    unsigned    num;

    if( HasEquals() ) {
        num = GetValue();
        if( num < 10 || num > 255 ) {
            StartupErr( "lines out of range" );
        }
        /* force a specified number of lines for MDA/CGA systems */
        DbgRows = num;
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
