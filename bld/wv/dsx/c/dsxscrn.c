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


#include <dos.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "stdui.h"
#include "dosscrn.h"
#include "dbgmem.h"
#include "dsxutil.h"
#include "pcscrnio.h"
#include "dpmi.h"
#include "dbgscrn.h"
#include "uidbg.h"


#define _64K                    (64UL*1024)
#define EGA_VIDEO_BUFF          MK_PM( 0xa000, 0 )
#define MONO_VIDEO_BUFF         MK_PM( 0xb000, 0 )
#define COLOUR_VIDEO_BUFF       MK_PM( 0xb800, 0 )

#define CURS_LOCATION_LOW       0xf
#define CURS_LOCATION_HI        0xe
#define CURS_START_SCANLINE     0xa
#define CURS_END_SCANLINE       0xb

#define DOUBLE_DOT_CHR_SET      0x12
#define COMPRESSED_CHR_SET      0x11
#define USER_CHR_SET            0

#define EGA_CURSOR_OFF          0x1e00
#define NORM_CURSOR_OFF         0x2000
#define CGA_CURSOR_ON           0x0607
#define MON_CURSOR_ON           0x0b0c

#define MSMOUSE_VECTOR          0x33
#define VID_STATE_SWAP          VID_STATE_ALL

#define GetBIOSData( off, var ) var =                                        \
                                sizeof( var ) == 1 ?                         \
                                    *(uint_8 __far *)MK_PM( BD_SEG, off ) :     \
                                sizeof( var ) == 2 ?                         \
                                    *(uint_16 __far *)MK_PM( BD_SEG, off ) :    \
                                *(uint_32 __far *)MK_PM( BD_SEG, off );

#define SetBIOSData( off, var ) ( sizeof( var ) == 1 ) ?                             \
                                    ( *(uint_8 __far *)MK_PM( BD_SEG, off )          \
                                        = var ) :                            \
                                ( ( sizeof( var ) == 2 ) ?                           \
                                    ( *(uint_16 __far *)MK_PM( BD_SEG, off )         \
                                        = var ) :                            \
                                ( *(uint_32 __far *)MK_PM( BD_SEG, off ) = var ) );


enum {
    BD_SEG          = 0x40,
    BD_EQUIP_LIST   = 0x10,
    BD_CURR_MODE    = 0x49,
    BD_REGEN_LEN    = 0x4c,
    BD_CURPOS       = 0x50,
    BD_MODE_CTRL    = 0x65,
    BD_VID_CTRL1    = 0x87,
};

typedef enum {
    DISP_NONE,
    DISP_MONOCHROME,
    DISP_CGA,
    DISP_RESERVED1,
    DISP_EGA_COLOUR,
    DISP_EGA_MONO,
    DISP_PGA,
    DISP_VGA_MONO,
    DISP_VGA_COLOUR,
    DISP_RESERVED2,
    DISP_RESERVED3,
    DISP_MODEL30_MONO,
    DISP_MODEL30_COLOUR
} hw_display_type;

enum {
    ADAPTER_MONO   = -1,
    ADAPTER_NONE   = 0,
    ADAPTER_COLOUR = 1
};

typedef struct {
    hw_display_type     active;
    hw_display_type     alt;
} display_configuration;

typedef struct {
    uint_8              points;
    uint_8              mode;
    uint_8              swtchs;
    int_16              curtyp;
    union {
        struct {
            uint_8      rows;
            uint_8      attr;
        } strt;
        struct {
            uint_8      page;
            int_16      curpos;
        } save;
    };
} screen_info;

extern bool                     HasEquals( void );
extern unsigned                 GetValue( void );
extern unsigned                 Lookup( const char *, const char *, unsigned );

extern void                     StartupErr( const char * );
extern int                      GUIInitMouse( int );
extern void                     GUIFiniMouse( void );
extern void                     GUIInitGraphicsMouse( gui_window_styles );
extern bool                     UserScreen( void );

extern void                     __far *video_buffer( void __far * );

extern flip_types               FlipMech = 0;
extern mode_types               ScrnMode = 0;
extern gui_window_styles        WndStyle;


static rm_call_struct           CallStruct;
static uint_8                   OldRow;
static uint_8                   OldCol;
static uint_8                   OldTyp;
static bool                     OnAlt;
static screen_info              StartScrn;
static screen_info              SaveScrn;
static uint_16                  VIDPort;
static uint_16                  PageSize;
static uint_16                  CurOffst;
static uint_16                  RegCur;
static uint_16                  InsCur;
static uint_16                  NoCur;
static uint_8                   DbgBiosMode;
static uint_8                   DbgChrSet;
static uint_8                   DbgRows;
static dos_memory               SwapSeg;
static addr32_off               StateOff;
static addr32_off               PgmMouse;
static addr32_off               DbgMouse;
static display_configuration    HWDisplay;
static unsigned_8               *RegenSave;
static void                     __far *VirtScreen;
static int_8                    ColourAdapters[] = {
                                    ADAPTER_NONE,     /* NONE              */
                                    ADAPTER_MONO,     /* MONOCHROME        */
                                    ADAPTER_COLOUR,   /* CGA               */
                                    ADAPTER_NONE,     /* RESERVED          */
                                    ADAPTER_COLOUR,   /* EGA COLOUR        */
                                    ADAPTER_MONO,     /* EGA MONO          */
                                    ADAPTER_COLOUR,   /* PGA               */
                                    ADAPTER_COLOUR,   /* VGA MONO          */
                                    ADAPTER_COLOUR,   /* VGA COLOUR        */
                                    ADAPTER_NONE,     /* RESERVED          */
                                    ADAPTER_NONE,     /* RESERVED          */
                                    ADAPTER_COLOUR,   /* MODEL 30 MONO     */
                                    ADAPTER_COLOUR    /* MODEL 30 COLOUR   */
                                };



static void _VidStateSave( uint_16 requested_state, addr_seg buff_rmseg,
                           addr32_off buff_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1c01;
    CallStruct.es = buff_rmseg;
    CallStruct.ebx = buff_offset;
    CallStruct.ecx = requested_state;
    DPMISimulateRealModeInterrupt( 0x10, 0, 0, &CallStruct );
}

static void _VidStateRestore( uint_16 requested_state, addr_seg buff_rmseg,
                              addr32_off buff_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x1c02;
    CallStruct.es = buff_rmseg;
    CallStruct.ebx = buff_offset;
    CallStruct.ecx = requested_state;
    DPMISimulateRealModeInterrupt( 0x10, 0, 0, &CallStruct );
}

static void BIOSCharSet( uint_8 vidroutine, uint_8 bytesperchar,
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
    DPMISimulateRealModeInterrupt( 0x10, 0, 0, &CallStruct );
}

static void MouseSaveState( addr_seg buff_rmseg, addr32_off buff_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x16;
    CallStruct.es = buff_rmseg;
    CallStruct.edx = buff_offset;
    DPMISimulateRealModeInterrupt( 0x33, 0, 0, &CallStruct );
}

static void MouseRestoreState( addr_seg buff_rmseg, addr32_off buff_offset )
{
    memset( &CallStruct, 0, sizeof( CallStruct ) );
    CallStruct.eax = 0x17;
    CallStruct.es = buff_rmseg;
    CallStruct.edx = buff_offset;
    DPMISimulateRealModeInterrupt( 0x33, 0, 0, &CallStruct );
}

extern uint_16 MouseSaveSize( void );
#pragma aux MouseSaveSize =                             \
        " mov    ax, 15h        ",                      \
        " int    33h "                                  \
        modify exact [ ax bx ]                          \
        value [ bx ];

extern display_configuration BIOSDevCombCode( void );
#pragma aux BIOSDevCombCode =                           \
        " push   ebp            "                       \
        " mov    ax, 1a00h      "                       \
        _INT_10                                         \
        " cmp    al, 1ah        "                       \
        " jz     end            "                       \
        " mov    bx, 0          "                       \
        " end:                  "                       \
        " pop    ebp            "                       \
        modify exact [ ax bx ]                          \
        value [ bx ];

extern void DoRingBell( void );
#pragma aux DoRingBell =                                \
        " push   ebp            "                       \
        " mov    ax, 0e07h      "                       \
        _INT_10                                         \
        " pop    ebp            "                       \
        modify exact [ ax ];

void Ring_Bell( void )
{
    DoRingBell();
}

static uint_8 VIDGetRow( uint_16 vidport )
{
    return( _ReadCRTCReg( vidport, CURS_LOCATION_LOW ) );
}

static void VIDSetRow( uint_16 vidport, uint_8 row )
{
    _WriteCRTCReg( vidport, CURS_LOCATION_LOW, row );
}

static void VIDSetCol( uint_16 vidport, uint_8 col )
{
    _WriteCRTCReg( vidport, CURS_LOCATION_HI, col );
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
    return( (uint_16)_ReadCRTCReg( vidport, CURS_START_SCANLINE ) << 8 |
            _ReadCRTCReg( vidport, CURS_END_SCANLINE ) );
}

static bool ChkCntrlr( int_16 port )
{
    uint_8              curr;
    bool                rtrn;

    curr = VIDGetRow( port );
    VIDSetRow( port, 0x5a );
    VIDWait();
    VIDWait();
    VIDWait();
    rtrn = VIDGetRow( port ) == 0x5a;
    VIDSetRow( port, curr );
    return( rtrn );
}

static uint_8 GetSwtchs( void )
{
    uint_8              equip;

    GetBIOSData( BD_EQUIP_LIST, equip );
    return( equip );
}

static void SetSwtchs( uint_8 new )
{
    SetBIOSData( BD_EQUIP_LIST, new );
}

static void DoSetMode( uint_8 mode )
{
    switch( mode & 0x7f ) {
    case 0x7:
    case 0xf:
        SetSwtchs( ( StartScrn.swtchs & 0xcf ) | 0x30 );
        break;
    default:
        SetSwtchs( ( StartScrn.swtchs & 0xcf ) | 0x20 );
        break;
    }
    BIOSSetMode( mode );
}

static bool TstMono( void )
{
    return( ChkCntrlr( VIDMONOINDXREG ) ? TRUE : FALSE );
}

static bool TstColour( void )
{
    return( ChkCntrlr( VIDCOLRINDXREG ) ? TRUE : FALSE );
}

static void GetEGAConfig( uint_8 colour, uint_8 curr_mode )
{
    hw_display_type     temp;

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
    if( ( ( HWDisplay.active == DISP_EGA_COLOUR ) &&
          ( ( curr_mode == 7 ) || ( curr_mode == 15 ) ) ) ||
        ( ( HWDisplay.active == DISP_EGA_MONO ) &&
          ( ( curr_mode != 7 ) && ( curr_mode != 15 ) ) ) ) {
        /* EGA is not the active display */
        temp = HWDisplay.active;
        HWDisplay.active = HWDisplay.alt;
        HWDisplay.alt = temp;
    }
}

static void GetMonoConfig( uint_8 curr_mode )
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
    int_32       info;
    uint_8       colour;
    uint_8       memory;
    uint_8       swtchs;
    uint_8       curr_mode;

    HWDisplay = BIOSDevCombCode();
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
    if( ColourAdapters[ display ] <= 0 ) {
        return( FALSE );
    }
    ScrnMode = MD_COLOUR;
    return( TRUE );
}

static void SwapActAlt( void )
{
    hw_display_type     temp;

    temp = HWDisplay.active;
    HWDisplay.active = HWDisplay.alt;
    HWDisplay.alt = temp;
    OnAlt = TRUE;
}

static bool ChkColour( void )
{
    if( ChkForColour( HWDisplay.active ) ) {
        return( TRUE );
    }
    if( ChkForColour( HWDisplay.alt ) ) {
        SwapActAlt();
        return( TRUE );
    }
    return( FALSE );
}

static bool ChkForMono( hw_display_type display )
{
    if( ColourAdapters[ display ] >= 0 ) {
        return( FALSE );
    }
    ScrnMode = MD_MONO;
    return( TRUE );
}

static bool ChkMono( void )
{
    if( ChkForMono( HWDisplay.active ) ) {
        return( TRUE );
    }
    if( ChkForMono( HWDisplay.alt ) ) {
        SwapActAlt();
        return( TRUE );
    }
    return( FALSE );
}

static bool ChkForEGA( hw_display_type display )
{
    switch( display ) {
    case DISP_EGA_COLOUR:
    case DISP_VGA_COLOUR:
    case DISP_EGA_MONO:
    case DISP_VGA_MONO:
        ScrnMode = MD_EGA;
        return( TRUE );
    default:
        return( FALSE );
    }
}

static bool ChkEGA( void )
{
    if( ChkForEGA( HWDisplay.active ) ) {
        return( TRUE );
    }
    if( ChkForEGA( HWDisplay.alt ) ) {
        SwapActAlt();
        return( TRUE );
    }
    return( FALSE );
}

static void GetDefault( void )
{
    if( StartScrn.mode == 0x07 || StartScrn.mode == 0x0f ) {
        if( FlipMech == FLIP_TWO ) {
            if( ChkColour() == FALSE ) {
                FlipMech = FLIP_SWAP;
                ChkMono();
            }
        } else {
            ChkMono();
        }
    } else {
        if( FlipMech == FLIP_TWO ) {
            if( ChkMono() == FALSE ) {
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

static void SetChrSet( uint_16 set )
{
    if( set != USER_CHR_SET ) {
        BIOSEGAChrSet( set );
    }
}

static unsigned GetChrSet( uint_8 rows )
{
    if( rows >= 43 ) {
        return( DOUBLE_DOT_CHR_SET );
    }
    if( rows >= 28 ) {
        return( COMPRESSED_CHR_SET );
    }
    return( USER_CHR_SET );
}

static void SetEGA_VGA( int_16 double_rows )
{
    if( ScrnMode == MD_EGA ) {
        DbgRows = double_rows;
        DbgChrSet = DOUBLE_DOT_CHR_SET;
    } else if( FlipMech != FLIP_SWAP && FlipMech != FLIP_CHEAPSWAP ) {
        DbgRows = BIOSGetRows();
        DbgChrSet = USER_CHR_SET;
    } else {
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
        if( ( StartScrn.mode == 2 ) && !OnAlt ) {
            DbgBiosMode = 2;
        } else {
            DbgBiosMode = 3;
        }
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
        if( ( StartScrn.mode == 7 ) && !OnAlt ) {
            DbgBiosMode = 7;
        } else {
            DbgBiosMode = 3;
        }
        SetEGA_VGA( 50 );
        break;
    }
    if( DbgRows == 0 ) {
        DbgRows = 25;
    }
    VIDPort = DbgBiosMode == 7 ? VIDMONOINDXREG : VIDCOLRINDXREG;
    if( ( ( StartScrn.mode & 0x7f ) == DbgBiosMode ) &&
        ( StartScrn.strt.rows == DbgRows ) ) {
        GetBIOSData( BD_REGEN_LEN, PageSize );  /* get size from BIOS */
    } else {
        PageSize = DbgRows == 25 ? 4096 : ( DbgRows * 80 * 2 + 256 );
    }
}

static void SaveBIOSSettings( void )
{
    SaveScrn.swtchs = GetSwtchs();
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
        if( ChkMono() == FALSE ) {
            GetDefault();
        }
        break;
    case MD_COLOUR:
        if( ChkColour() == FALSE ) {
            GetDefault();
        }
        break;
    case MD_EGA:
        if( ChkEGA() == FALSE ) {
            GetDefault();
        }
        break;
    }
}

/* ConfigScreen -- figure out screen configuration we're going to use. */
unsigned ConfigScreen( void )
{
    OnAlt = FALSE;
    GetDispConfig();
    SaveBIOSSettings();
    StartScrn.points = SaveScrn.points;
    StartScrn.curtyp = SaveScrn.curtyp;
    StartScrn.swtchs = SaveScrn.swtchs;
    StartScrn.mode = SaveScrn.mode;
    StartScrn.strt.attr = ( StartScrn.mode < 4 ) || ( StartScrn.mode == 7 ) ?
                          BIOSGetAttr( SaveScrn.save.page ) & 0x7f : 0;
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
        VirtScreen = video_buffer( COLOUR_VIDEO_BUFF );
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

static bool SetMode( uint_8 mode )
{
    if( ( BIOSGetMode() & 0x7f ) == ( mode & 0x7f ) ) {
        return( FALSE );
    }
    DoSetMode( mode );
    return( TRUE );
}

static void SetRegenClear( void )
{
    uint_8              regen;

    GetBIOSData( BD_VID_CTRL1, regen );
    regen &= 0x7f;
    regen |= SaveScrn.mode & 0x80;
    SetBIOSData( BD_VID_CTRL1, regen );
}

static uint_16 RegenSize( void )
{
    uint_16     regen_size;

    switch( HWDisplay.active ) {
    case DISP_MONOCHROME:
        regen_size = ( DbgRows * 80 * 2 + 0x3ff ) & ~0x3ff;
        break;
    case DISP_CGA:
    case DISP_PGA:
    case DISP_MODEL30_MONO:
    case DISP_MODEL30_COLOUR:
        regen_size = ( ( DbgRows * 80 * 2 + 0x3ff ) & ~0x3ff ) * 4;
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
    case DISP_EGA_COLOUR:
    case DISP_EGA_MONO:
        regen_size = PageSize * 2;
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
        _VidStateSave( VID_STATE_SWAP, SwapSeg.segm.rm, StateOff );
        /* fall through */
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        SetupEGA();
        if( FlipMech == FLIP_CHEAPSWAP ) {
            _graph_write( GRA_READ_MAP, RMS_MAP_0 );
            _fmemcpy( &RegenSave[0], EGA_VIDEO_BUFF, PageSize );
            _graph_write( GRA_READ_MAP, RMS_MAP_1 );
            _fmemcpy( &RegenSave[PageSize], EGA_VIDEO_BUFF, PageSize );
            _graph_write( GRA_READ_MAP, RMS_MAP_2 );
            _fmemcpy( MK_PM( SwapSeg.segm.rm, 0 ), EGA_VIDEO_BUFF, 8*1024 );
            if( VirtScreen != NULL ) {
                _fmemcpy( &RegenSave[PageSize * 2], VirtScreen,  PageSize );
            }
        } else {
            _graph_write( GRA_READ_MAP, RMS_MAP_0 );
            _fmemcpy( &RegenSave[0*_64K], EGA_VIDEO_BUFF, _64K );
            _graph_write( GRA_READ_MAP, RMS_MAP_1 );
            _fmemcpy( &RegenSave[1*_64K], EGA_VIDEO_BUFF, _64K );
            _graph_write( GRA_READ_MAP, RMS_MAP_2 );
            _fmemcpy( &RegenSave[2*_64K], EGA_VIDEO_BUFF, _64K );
            _graph_write( GRA_READ_MAP, RMS_MAP_3 );
            _fmemcpy( &RegenSave[3*_64K], EGA_VIDEO_BUFF, _64K );
            if( VirtScreen != NULL ) {
                _fmemcpy( &RegenSave[4*_64K], VirtScreen,  PageSize );
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
        _fmemcpy( &RegenSave[0], MONO_VIDEO_BUFF, RegenSize() );
        SetMode( DbgBiosMode );
        break;
    default:
        _fmemcpy( &RegenSave[0], COLOUR_VIDEO_BUFF, RegenSize() );
        SetMode( DbgBiosMode );
        break;
    }
}

static uint_8 RestoreEGA_VGA( void )
{
    uint_8       mode;

    if( FlipMech == FLIP_CHEAPSWAP ) {
        SetupEGA();
        _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
        _fmemcpy( EGA_VIDEO_BUFF, &RegenSave[0], PageSize );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        _fmemcpy( EGA_VIDEO_BUFF, &RegenSave[PageSize], PageSize );
        mode = SaveScrn.mode & 0x7f;
        if( ( mode < 4 ) || ( mode == 7 ) ) {
            if( VirtScreen != NULL ) {
                _fmemcpy( VirtScreen, &RegenSave[PageSize * 2], PageSize );
                _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
                _fmemcpy( EGA_VIDEO_BUFF, MK_PM( SwapSeg.segm.rm, 0 ), 8*1024 );
                DoSetMode( SaveScrn.mode | 0x80 );
            } else {
                DoSetMode( SaveScrn.mode | 0x80 );
                BIOSCharSet( 0, 32, 256, 0, SwapSeg.segm.rm, 0 );
            }
        } else {
            _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
            _fmemcpy( EGA_VIDEO_BUFF, MK_PM( SwapSeg.segm.rm, 0 ), 8*1024 );
            DoSetMode( SaveScrn.mode | 0x80 );
        }
    } else {
        /* stupid thing doesn't respect the no-clear bit in DBCS mode */
        DoSetMode( SaveScrn.mode );
        if( VirtScreen != NULL ) {
            _fmemcpy( VirtScreen, &RegenSave[4*_64K], PageSize );
        }
        SetupEGA();
        _seq_write( SEQ_MAP_MASK, MSK_MAP_0 );
        _fmemcpy( EGA_VIDEO_BUFF, &RegenSave[0*_64K], _64K );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_1 );
        _fmemcpy( EGA_VIDEO_BUFF, &RegenSave[1*_64K], _64K );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_2 );
        _fmemcpy( EGA_VIDEO_BUFF, &RegenSave[2*_64K], _64K );
        _seq_write( SEQ_MAP_MASK, MSK_MAP_3 );
        _fmemcpy( EGA_VIDEO_BUFF, &RegenSave[3*_64K], _64K );
    }
    SetRegenClear();
    return( mode );
}

static void SwapRestore( void )
{
    uint_8              mode;

    switch( HWDisplay.active ) {
    case DISP_EGA_MONO:
    case DISP_EGA_COLOUR:
        mode = RestoreEGA_VGA();
        if( ( mode < 4 ) || ( mode == 7 ) ) {
            BIOSCharSet( 0x10, SaveScrn.points, 0, 0, 0, 0 );
        }
        _seq_write( SEQ_CHAR_MAP_SEL, PMData->act_font_tbls );
        break;
    case DISP_VGA_MONO:
    case DISP_VGA_COLOUR:
        RestoreEGA_VGA();
        _VidStateRestore( VID_STATE_SWAP, SwapSeg.segm.rm, StateOff );
        break;
    case DISP_MONOCHROME:
        SetMode( SaveScrn.mode );
        _fmemcpy( MONO_VIDEO_BUFF, &RegenSave[0], RegenSize() );
        break;
    default:
        SetMode( SaveScrn.mode );
        _fmemcpy( COLOUR_VIDEO_BUFF, &RegenSave[0], RegenSize() );
        break;
    }
}

static void SaveMouse( addr32_off to )
{
    if( to != 0 ) {
        MouseSaveState( SwapSeg.segm.rm, to );
    }
}

static void RestoreMouse( addr32_off from )
{
    if( from != 0 ) {
        MouseRestoreState( SwapSeg.segm.rm, from );
    }
}

static void AllocSave( void )
{
    uint_16     state_size;
    uint_16     mouse_size;
    uint_16     regen_size;

    switch( FlipMech ) {
    case FLIP_SWAP:
        if( VirtScreen != NULL ) {
            _Alloc( RegenSave, _64K*4 + PageSize );
            if( RegenSave == NULL ) {
                StartupErr( LIT_ENG( ERR_NO_MEMORY ) );
            }
            break;
        }
        /* fall through */
    case FLIP_CHEAPSWAP:
        FlipMech = FLIP_CHEAPSWAP;
        regen_size = RegenSize();
        if( VirtScreen != NULL ) regen_size += PageSize;
        _Alloc( RegenSave, regen_size );
        if( RegenSave == NULL ) {
            StartupErr( LIT_ENG( ERR_NO_MEMORY ) );
        }
        regen_size = 8*1024;    /* Font table has to go in low memory */
        break;
    default:
        /* regen_size is at least 2 to allow mouse swap detection to work */
        regen_size = 2;
        break;
    }
    state_size = _vidstatesize( VID_STATE_SWAP ) * 64;
    mouse_size = _IsOn( SW_USE_MOUSE ) ? MouseSaveSize() : 0;
    SwapSeg.dpmi_adr = DPMIAllocateDOSMemoryBlock( _NBPARAS( regen_size + state_size +
                                          mouse_size * 2 ) );
    if( SwapSeg.segm.pm == 0 ) {
        StartupErr( LIT_ENG( Unable_to_alloc_DOS_mem ) );
    }
    StateOff = regen_size;
    if( mouse_size != 0 ) {
        PgmMouse = regen_size + state_size;
        DbgMouse = PgmMouse + mouse_size;
    }
}

static void SetCursorTypes( void )
{
    uint_16     scan_lines;

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
        RegCur = ( scan_lines - 1 ) + ( ( scan_lines - 2 ) << 8 );
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
    InsCur = ( ( ( RegCur + 0x100 ) >> 1 & 0xff00 ) + 0x100 ) |
             ( RegCur & 0x00ff );
}

void InitScreenMode( void )
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
        GetBIOSData( BD_REGEN_LEN, CurOffst );
        CurOffst /= 2;
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

/* check for Microsoft mouse */
static void CheckMSMouse( void )
{
    memptr              vect;

    vect = *(memptr __far *)MK_PM( 0, MSMOUSE_VECTOR * sizeof( memptr ) );
    if( vect.a == 0 ) {
        _SwitchOff( SW_USE_MOUSE );
    } else if( *(uint_8 __far*)MK_PM( vect.s.segment, vect.s.offset ) == 0xCF ) { // mad: nyi
        _SwitchOff( SW_USE_MOUSE );
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
    if( _IsOn( SW_USE_MOUSE ) ) GUIInitMouse( 2 );
}

/* UsrScrnMode -- setup the user screen mode */
extern bool UsrScrnMode( void )
{
    uint_8              user_mode;
    bool                usr_vis;

    if( StartScrn.strt.attr && ( DbgBiosMode == StartScrn.mode ) ) {
        UIData->attrs[ ATTR_NORMAL ] = StartScrn.strt.attr;
        UIData->attrs[ ATTR_BRIGHT ] = StartScrn.strt.attr ^ 8;
        UIData->attrs[ ATTR_REVERSE ] = ( ( StartScrn.strt.attr & 7 ) << 4 ) |
                                        ( StartScrn.strt.attr & 0x70 ) >> 4;
    }
    if( FlipMech != FLIP_TWO ) {
        usr_vis = FALSE;
    } else {
        usr_vis = TRUE;
        SaveMouse( DbgMouse );
        RestoreMouse( PgmMouse );
        user_mode = ( DbgBiosMode == 7 ) ? 3 : 7;
        DoSetMode( user_mode );
        SaveBIOSSettings();
        SaveMouse( PgmMouse );
        RestoreMouse( DbgMouse );
    }
    SaveScrn.swtchs = GetSwtchs();
    if( ( HWDisplay.active == DISP_VGA_COLOUR ) ||
        ( HWDisplay.active == DISP_VGA_MONO ) ) {
        UIData->colour = M_VGA;
    }
    if( DbgRows != UIData->height ) {
        UIData->height = DbgRows;
        if( _IsOn( SW_USE_MOUSE ) ) {
            /* This is a sideways dive into the UI to get the boundries of
               the mouse cursor properly defined. */
            initmouse( 1 );
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
extern bool DebugScreen( void )
{
    bool                usr_vis;

    usr_vis = TRUE;
    SaveMouse( PgmMouse );
    SaveBIOSSettings();
    switch( FlipMech ) {
    case FLIP_SWAP:
    case FLIP_CHEAPSWAP:
        SwapSave();
        BIOSSetPage( 0 );
        WndDirty( NULL );
        usr_vis = FALSE;
        break;
    case FLIP_PAGE:
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
            WndDirty( NULL );
        }
        BIOSSetPage( 1 );
        usr_vis = FALSE;
        break;
    case FLIP_OVERWRITE:
        if( SetMode( DbgBiosMode ) ) {
            SetChrSet( DbgChrSet );
            SaveBIOSSettings();
        }
        WndDirty( NULL );
        usr_vis = FALSE;
        break;
    }
    RestoreMouse( DbgMouse );
    uiswap();
    return( usr_vis );
}


bool DebugScreenRecover( void )
{
    return( TRUE );
}

/* UserScreen -- swap/page to user screen */
extern bool UserScreen( void )
{
    bool                dbg_vis;

    dbg_vis = TRUE;
    uiswap();
    SaveMouse( DbgMouse );
    switch( FlipMech ) {
    case FLIP_SWAP:
    case FLIP_CHEAPSWAP:
        SwapRestore();
        dbg_vis = FALSE;
        break;
    case FLIP_PAGE:
        dbg_vis = FALSE;
        break;
    }
    BIOSSetPage( SaveScrn.save.page );
    BIOSSetCurTyp( SaveScrn.curtyp );
    BIOSSetCurPos( SaveScrn.save.curpos, SaveScrn.save.page );
    SetSwtchs( SaveScrn.swtchs );
    RestoreMouse( PgmMouse );
    return( dbg_vis );
}

static void ReInitScreen( void )
{
    RestoreMouse( PgmMouse );
    SetSwtchs( StartScrn.swtchs );
    BIOSSetMode( StartScrn.mode );
    switch( StartScrn.mode & 0x7f ) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 7:
        switch( HWDisplay.active ) {
        case DISP_EGA_MONO:
        case DISP_EGA_COLOUR:
        case DISP_VGA_MONO:
        case DISP_VGA_COLOUR:
            SetChrSet( GetChrSet( StartScrn.strt.rows ) );
            break;
        }
        break;
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
extern void FiniScreen( void )
{
    if( _IsOn( SW_USE_MOUSE ) ) GUIFiniMouse();
    uistop();
    if( ( SaveScrn.swtchs != StartScrn.swtchs ) ||
        ( SaveScrn.mode != StartScrn.mode ) ||
        ( SaveScrn.points != StartScrn.points ) ||
        ( FlipMech != FLIP_OVERWRITE ) ) {
        ReInitScreen();
    } else {
        UserScreen();
    }
    DPMIFreeDOSMemoryBlock( SwapSeg.segm.pm );
    _Free( RegenSave );
}



/*****************************************************************************\
 *                                                                           *
 *            Replacement routines for User Interface library                *
 *                                                                           *
\*****************************************************************************/

extern void uiinitcursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uiinitcursor();
    }
}

void uisetcursor( ORD row, ORD col, int typ, int attr )
{
    uint_16     bios_cur_pos;

    if( FlipMech != FLIP_TWO ) {
        _uisetcursor( row, col, typ, attr );
    } else if( typ == C_OFF ) {
        uioffcursor();
    } else if( VIDPort && ( ScrnState & DBG_SCRN_ACTIVE ) &&
               ( ( row != OldRow ) || ( col != OldCol ) ||
               ( typ != OldTyp ) ) ) {
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        bios_cur_pos = BD_CURPOS;
        if( FlipMech == FLIP_PAGE ) {
            bios_cur_pos += 2;
        }
        SetBIOSData( bios_cur_pos + 0, OldCol );
        SetBIOSData( bios_cur_pos + 1, OldRow );
        VIDSetPos( VIDPort, CurOffst + row * UIData->width + col );
        VIDSetCurTyp( VIDPort, typ == C_INSERT ? InsCur : RegCur );
    }
}

extern void uioffcursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uioffcursor();
    } else if( ( ScrnState & DBG_SCRN_ACTIVE ) && ( VIDPort != NULL ) ) {
        OldTyp = C_OFF;
        VIDSetCurTyp( VIDPort, NoCur );
    }
}

extern void uiswapcursor( void )
{
    if( FlipMech != FLIP_TWO ) {
        _uiswapcursor();
    }
}

extern void uifinicursor( void )
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

static const char ScreenOptNameTab[] = {
    "Monochrome\0"
    "Color\0"
    "Colour\0"
    "Ega43\0"
    "Vga50\0"
    "Overwrite\0"
    "Page\0"
    "Swap\0"
    "Two\0"
};

enum {
    OPT_MONO = 1,
    OPT_COLOR,
    OPT_COLOUR,
    OPT_EGA43,
    OPT_VGA50,

    OPT_OVERWRITE,
    OPT_PAGE,
    OPT_SWAP,
    OPT_TWO
};

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
        WndStyle &= ~( GUI_CHARMAP_MOUSE|GUI_CHARMAP_DLG);
        break;
    case OPT_TWO:
        FlipMech = FLIP_TWO;
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}


void ScreenOptInit( void )
{
    ScrnMode = MD_DEFAULT;
    FlipMech = FLIP_TWO;
}
