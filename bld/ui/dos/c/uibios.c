/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Low level screen output and DBCS setup routines.
*
****************************************************************************/


#include <dos.h>
#include <conio.h>
#include <string.h>
#include "extender.h"
#include "uidef.h"
#include "dpmi.h"
#include "uigchar.h"
#include "getltdos.h"
#include "uicurshk.h"
#include "realmod.h"
#include "int10.h"


typedef struct {
    unsigned short  int_num;
    unsigned short  real_ds;
    unsigned short  real_es;
    unsigned short  real_fs;
    unsigned short  real_gs;
    long            real_eax;
    long            real_edx;
} PHARLAP_block;

static unsigned     BIOSVidPage;

static MONITOR ui_data = {
    25,
    80,
    M_CGA,
    NULL,
    NULL,
    NULL,
    NULL,
    4,
    1
};

bool UIAPI uiset80col( void )
/****************************/

{
    bool        status;

    status = false;
    if( UIData->width != 80 ) {
        if( UIData->colour == M_MONO ) {
            _BIOSVideoSetMode( 7 );
        } else if( UIData->colour == M_BW ) {
            _BIOSVideoSetMode( 2 );
        } else {
            _BIOSVideoSetMode( 3 );
        }
        status = true;
    }
    return( status );
}

/*
    The code for video_buffer is identical for DOS/V and desqview
    (Get Video Buffer: int 10h, AH=FEh)
*/

LP_PIXEL UIAPI dos_uishadowbuffer( LP_PIXEL vbuff )
/*************************************************/
{
#ifdef _M_I86
    return( _BIOSVideo_desqview_shadow_buffer( vbuff ) );
#else
    if( _IsPharLap() ) {
        union REGPACK   regs;

        memset( &regs, 0, sizeof( regs ) );
        regs.h.ah = 0xfe;
        regs.x.edi = _FP_OFF( vbuff );
        regs.w.es = _FP_SEG( vbuff );
        intr( VECTOR_VIDEO, &regs );
        if( _FP_OFF( vbuff ) != regs.x.edi ) {
            /* we use _FP_OFF since old_selector==0x34 and new_selector==0x37 */
            vbuff = _MK_FP( regs.w.es, regs.x.edi );
        }
    } else if( _IsRational() ) {
        rm_call_struct  dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0xfe00;                /* get video buffer addr */
        dblock.es = _FP_OFF( vbuff ) >> 4;
        dblock.edi = (_FP_OFF( vbuff ) & 0x0f);
        DPMISimulateRealModeInterrupt( VECTOR_VIDEO, 0, 0, &dblock );
        vbuff = RealModeDataPtr( dblock.es, dblock.edi );
    }
    return( vbuff );
#endif
}

#define ALPHA_SMALL_BW  0
#define ALPHA_SMALL_COL 1
#define ALPHA_LARGE_BW  2
#define ALPHA_LARGE_COL 3
#define GR_MED_4COL     4           // 16K memory (40 characters wide)
#define GR_MED_BW       5           // 16K memory (40 characters wide)
#define GR_HIGH_BW      6           // 16K memory (80 characters wide)
#define MONOCHROME      7
#define GR_LOW_16COL    8           // 16K memory (20 characters wide)
#define GR_MED_16COL    9           // 32K memory
#define GR_HIGH_4COL   10           // 32K memory
#define EGA_LOW_16COL  13           // 64K memory
#define EGA_HIGH_16COL 14           // 64K memory
#define EGA_HIGH_MONO  15           // 64K memory
#define EGA_HIGH_4COL  16           // 64K memory
#define VGA_2COL       17
#define VGA_16COL      18
#define VGA_256COL     19
#define TSENG_44x132   34           // Text Mode
#define TSENG_25x132   35           // Text Mode
#define TSENG_28x132   36           // Text Mode
#define TSENG_60x80    38           // Text Mode
#define TSENG_40x100   42           // Text Mode
#define ACER_25X132    35           // Text Mode
#define ACER_44X132    51           // Text Mode
#define TRIDENT_start  80           // Text Modes 80-90
#define TRIDENT_end    90           // Text Mode
#define DELL_43X132    84           // Text Mode
#define DELL_25X132    85           // Text Mode


static int IsTextMode( void )
{
    int10_mode_info     info;
    int10_cursor_pos    pos;
    LP_PIXEL            video_mem;
    int10_pixel_data    pixel_bios;
    int10_pixel_data    pixel_vmem;
    unsigned char       text_mode = 0;

    /* get current video mode info */
    info = _BIOSVideoGetModeInfo();
    /* get cursor position for current page */
    pos = _BIOSVideoGetCursorPos( info.page );
    if( info.mode < GR_MED_4COL || info.mode == MONOCHROME || info.mode > VGA_256COL ) {
        video_mem = UIData->screen.origin;
        /* set cursor position to top left corner of screen */
        _BIOSVideoSetCursorPosZero( info.page );
        /* get character/attribute at that location */
        pixel_bios = _BIOSVideoGetCharPixel( info.page );
        /* get character/attribute from screen memory */
        pixel_vmem.s.ch = video_mem->ch;
        pixel_vmem.s.attr = video_mem->attr;
        if( pixel_bios.value == pixel_vmem.value ) {
            /* change the character we read through BIOS call */
            pixel_bios.s.ch ^= 1;
            /* write out character using BIOS */
            _BIOSVideoSetCharPixel( info.page, pixel_bios );
            /* get character/attribute from screen memory */
            pixel_vmem.s.ch = video_mem->ch;
            pixel_vmem.s.attr = video_mem->attr;
            if( pixel_bios.value == pixel_vmem.value ) {
                /* restore character that was there */
                pixel_bios.s.ch ^= 1;
                video_mem->ch = pixel_bios.s.ch;
                video_mem->attr = pixel_bios.s.attr;
                text_mode = 1;
            }
        }
    }
    /* restore cursor position for current page */
    _BIOSVideoSetCursorPos( info.page, pos );
    return( text_mode );
}

static bool initmonitor( void )
/*****************************/
{
    bool                ega;
    int10_mode_info     info;
    int10_ega_info      info_ega;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }
    /* get current video mode info */
    info = _BIOSVideoGetModeInfo();
    BIOSVidPage = info.page;
    UIData->width = info.columns;
    UIData->height = 25;
    info_ega = _BIOSVideoEGAInfo();
    if( info_ega.switches < 0x0C && info_ega.mono <= 0x01 && info_ega.mem <= 0x03 ) {
        UIData->height = _BIOSVideoGetRowCount();
        ega = true;
    } else {
        ega = false;
    }
    if( ( info.mode == MONOCHROME ) || ( info.mode == EGA_HIGH_MONO ) ) {
        UIData->colour = M_MONO;
    } else if( ( info.mode == ALPHA_SMALL_COL ) || ( info.mode == ALPHA_LARGE_COL ) ) {
        if( ega ) {
            UIData->colour = M_EGA;
        } else {
            UIData->colour = M_CGA;
        }
    } else if( ( info.mode == ALPHA_SMALL_BW ) || ( info.mode == ALPHA_LARGE_BW ) ) {
        UIData->colour = M_BW;
    } else if( info.mode > VGA_256COL ) {
        /*
            if mode is out of known range then assume it's a VGA/SVGA mode
        */
        UIData->colour = M_VGA;
    } else {
        return( false );
    }

    return( true );
}

extern bool desqview_present( void );
#pragma aux desqview_present = \
        "mov  ax,2b01h"     \
        "mov  cx,4445h"     \
        "mov  dx,5351h"     \
        "int 21h"           \
        "cmp  al,0ffh"      \
        "jz L1"             \
        "xor  al,al"        \
    "L1: inc  al"           \
    __parm      [] \
    __value     [__al] \
    __modify    [__ah __bx __cx __dx]

bool intern initbios( void )
/**************************/
{
    bool            initialized;
    LP_PIXEL        old_origin;

    initialized = false;
    if( initmonitor() ) {
        UIData->desqview = desqview_present();
        UIData->f10menus = true;
        UIData->screen.origin = RealModeDataPtr( ( UIData->colour == M_MONO ) ? 0xb000 : 0xb800,
                                        BIOSData( BDATA_SCREEN_OFFSET, unsigned short ) );
        if( UIData->desqview ) {
            UIData->screen.origin = dos_uishadowbuffer( UIData->screen.origin );
        }
        if( uiisdbcs() ) {
            old_origin = UIData->screen.origin;
            UIData->screen.origin = dos_uishadowbuffer( UIData->screen.origin );
            if( old_origin != UIData->screen.origin ) {
                UIData->desqview = true;
            }
        }
        UIData->screen.increment = UIData->width;

        /* if we are in a text mode then continue the initialization */
        if( IsTextMode() ) {
            uiinitcursor();
            initkeyboard();
            UIData->mouse_acc_delay = uiclockdelay( 277 /* ms */ );  /* 5 ticks */
            UIData->mouse_rpt_delay = uiclockdelay( 55  /* ms */ );  /* 1 ticks */
            UIData->mouse_clk_delay = uiclockdelay( 277 /* ms */ );  /* 5 ticks */
            UIData->tick_delay      = uiclockdelay( 500 /* ms */ );  /* 9 ticks */
            UIData->mouse_speed = 8;       /* mickeys to ticks ratio */
            initialized = true;
        }
    }
    return( initialized );
}

void intern finibios( void )
/**************************/
{
    uifinicursor();
}

/*
    The code for desqview_update is identical for DOS/V
    (Update Video Display: int 10h, AH=FFh, CX=count, ES:DI=buffer)
*/

static void desqview_update( unsigned short offset, unsigned short count )
{
#ifdef _M_I86
    _BIOSVideo_desqview_update( UIData->screen.origin + offset, count );
#else
    if( _IsPharLap() ) {
        PHARLAP_block   pblock;
        union REGPACK   regs;

        memset( &pblock, 0, sizeof( pblock ) );
        memset( &regs, 0, sizeof( regs ) );
        pblock.int_num = VECTOR_VIDEO;      /* VIDEO call */
        pblock.real_eax = 0xff00;           /* update from v-screen */
        pblock.real_es = _FP_OFF( UIData->screen.origin ) >> 4;
        regs.x.edi = (_FP_OFF( UIData->screen.origin ) & 0x0f) + offset;
        regs.w.cx = count;
        regs.x.eax = 0x2511;                /* issue real-mode interrupt */
        regs.x.edx = _FP_OFF( &pblock );    /* DS:EDX -> parameter block */
        regs.w.ds = _FP_SEG( &pblock );
        intr( 0x21, &regs );
    } else if( _IsRational() ) {
        rm_call_struct  dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0xff00;                /* update from v-screen */
        dblock.es = _FP_OFF( UIData->screen.origin ) >> 4;
        dblock.edi = (_FP_OFF( UIData->screen.origin ) & 0x0f) + offset;
        dblock.ecx = count;
        DPMISimulateRealModeInterrupt( VECTOR_VIDEO, 0, 0, &dblock );
    }
#endif
}

void intern physupdate( SAREA *area )
/*************************************
 * update the physical screen with contents of virtual copy
 */
{
    unsigned short  i;
    unsigned short  offset;
    unsigned short  count;

    if( UIData->desqview ) {
        count = area->width * sizeof( PIXEL );
        for( i = area->row; i < (area->row + area->height); i++ ) {
            offset = ( i * UIData->width + area->col ) * sizeof( PIXEL );
            desqview_update( offset, count );
        }
    }
}
