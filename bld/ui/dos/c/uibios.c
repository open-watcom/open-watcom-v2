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
* Description:  Low level screen output and DBCS setup routines.
*
****************************************************************************/


#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <string.h>
#include <extender.h>
#include "uidef.h"
#include "uidos.h"
#include "biosui.h"
#include "dpmi.h"
#include "uigchar.h"

typedef struct {
    unsigned short  int_num;
    unsigned short  real_ds;
    unsigned short  real_es;
    unsigned short  real_fs;
    unsigned short  real_gs;
    long            real_eax;
    long            real_edx;
} PHARLAP_block;

static          MONITOR                 ui_data         =       {
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


unsigned    BIOSVidPage;

#ifdef __386__
LP_VOID firstmeg( unsigned segment, unsigned offset )
{
#if defined(__OSI__) || __WATCOMC__ >= 1000
    return( MK_FP( _ExtenderRealModeSelector, (unsigned) (segment << 4) + offset ) );
#else
    if( _IsRational() ) {
        return( MK_FP( FP_SEG( &BIOSVidPage ), (unsigned) ( segment << 4 ) + offset ) );
    } else {
        return( MK_FP( REAL_SEGMENT, (unsigned) ( segment << 4 ) + offset ) );
    }
#endif
}
#endif

#ifdef __386__
void IdleInterrupt( void )
{
#ifdef __OSI__
    return;     /* Can't do anything */
#else
    if( _IsRational() ) {
        DPMIIdle(); /* Assume DPMI if Rational; else dunno */
    }
    return;
#endif
}
#else
extern void DOSIdleInterrupt( void );
#pragma aux DOSIdleInterrupt = "int 28h";

void IdleInterrupt( void )
{
    DOSIdleInterrupt();
}
#endif

void intern setvideomode( unsigned mode )
/***************************************/
{
    BIOSSetMode( mode );
}


bool UIAPI uiset80col( void )
/****************************/

{
    register    bool            status;

    status = false;
    if( UIData->width != 80 ) {
        if( UIData->colour == M_MONO ) {
            setvideomode( 7 );
        } else if( UIData->colour == M_BW ) {
            setvideomode( 2 );
        } else {
            setvideomode( 3 );
        }
        status = true;
    }
    return( status );
}

extern unsigned char DOS_int( unsigned short, unsigned short, unsigned short );
#pragma aux DOS_int =   \
        _INT_21         \
    parm [ax] [cx] [dx] value [al];

#define desqview_present() (DOS_int(0x2b01,0x4445,0x5351)!=0xff)

/*
    The code for video_buffer is identical for DOS/V and desqview
    (Get Video Buffer: int 10h, AH=FEh)
*/

LP_VOID UIAPI dos_uivideobuffer( LP_VOID vbuff )
/**********************************************/
{
#ifdef __386__
    union REGPACK       regs;
    rm_call_struct      dblock;

    if( _IsPharLap() ) {
        memset( &regs, 0, sizeof( regs ) );
        regs.h.ah = 0xfe;
        regs.x.edi = FP_OFF( vbuff );
        regs.w.es = FP_SEG( vbuff );
        intr( BIOS_VIDEO, &regs );
        if( FP_OFF( vbuff ) != regs.x.edi ) {
            /* we use FP_OFF since old_selector==0x34 and new_selector==0x37 */
            vbuff = MK_FP( regs.w.es, regs.x.edi );
        }
    } else if( _IsRational() ) {
        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0xfe00;                /* get video buffer addr */
        dblock.es = FP_OFF( vbuff ) >> 4;
        dblock.edi = (FP_OFF( vbuff ) & 0x0f);
        DPMISimulateRealModeInterrupt( BIOS_VIDEO, 0, 0, &dblock );
        return( firstmeg( dblock.es, dblock.edi ) );
    }
    return( vbuff );
#else
extern LP_VOID get_video_buffer( LP_VOID );
#pragma aux             get_video_buffer = \
        0xb4 0xfe       /* mov ah,0xfe */ \
        0xcd 0x10       /* int 0x10 */ \
        parm caller     [es di] \
        value           [es di] \
        modify          [ah];

    return( get_video_buffer( vbuff ) );
#endif
}


typedef struct {
    unsigned char       start_range;
    unsigned char       end_range;
} dbcs_pair;

/* Some DOS environments ignore INT 21h, fn 63h altogether (NTVDM).
 * Others return success but do not modify DS:SI (US DOS 3.x). We need
 * to return a dummy DBCS pair table in that case.
 */
#ifndef __386__

extern dbcs_pair __far *dos_dbcs_vector_table( void );
#pragma aux             dos_dbcs_vector_table = \
        "push ds"       \
        "xor ax,ax"     /* pre-set DS:SI to zero */ \
        "mov ds,ax"     \
        "mov si,ax"     \
        "mov ax,6300h"  /* get DBCS vector table */ \
        "int 21h"       \
        "mov di,ds"     \
        "pop ds"        \
        value           [di si] \
        modify          [ax];

static dbcs_pair __far *intern dbcs_vector_table( void )
/***************************************************/
{
    static dbcs_pair    dbcs_dummy = { 0, 0 };
    dbcs_pair __far     *dbcs_table;

    if( UIData->colour == M_MONO ) return( &dbcs_dummy );
    dbcs_table = dos_dbcs_vector_table();
    return( dbcs_table ? dbcs_table : &dbcs_dummy );
}

#else

static dbcs_pair __far *intern dbcs_vector_table( void )
/***************************************************/
{
    union       REGPACK                 regs;
    static dbcs_pair dbcs_dummy = { 0, 0 };

    if( UIData->colour == M_MONO ) return( &dbcs_dummy );
    if( _IsPharLap() ) {
        PHARLAP_block pblock;

        memset( &pblock, 0, sizeof( pblock ) );
        memset( &regs, 0, sizeof( regs ) );
        pblock.real_eax = 0x6300;           /* get DBCS vector table */
        pblock.int_num = 0x21;              /* DOS call */
        regs.x.eax = 0x2511;                /* issue real-mode interrupt */
        regs.x.edx = FP_OFF( &pblock );     /* DS:EDX -> parameter block */
        regs.w.ds = FP_SEG( &pblock );
        intr( 0x21, &regs );
        if( pblock.real_ds == 0xFFFF ) { // wierd OS/2 value
            return( &dbcs_dummy );
        } else {
            return( firstmeg( (unsigned) pblock.real_ds, (unsigned) regs.w.si ) );
        }
    } else if( _IsRational() ) {
        rm_call_struct dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6300;                    /* get DBCS vector table */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 && dblock.ds ) {
            return( firstmeg( dblock.ds, dblock.esi ) );
        }
    }
    return( &dbcs_dummy );
}

#endif

static dbcs_pair        Pairs[5];       // safe enough for now
static int              Init;

static void intern initdbcs( void )
{
    dbcs_pair           *p;
    dbcs_pair           __far *s;

    s = dbcs_vector_table();
    p = Pairs;
    while( s->start_range != 0 ) {
        p->start_range = s->start_range;
        p->end_range = s->end_range;
        ++p;
        ++s;
    }
    p->start_range = 0;
    p->end_range = 0;
    Init = true;
}

int UIAPI uiisdbcs( void )
{
    if( !Init )
        initdbcs();
    return( Pairs[0].start_range != 0 );
}

int UIAPI uicharlen( int ch )
{
    dbcs_pair           *p;


    if( !Init )
        initdbcs();
    for( p = Pairs; p->start_range != 0; ++p ) {
        if( ch >= p->start_range && ch <= p->end_range ) {
            return( 2 );
        }
    }
    return( 1 );
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
    unsigned char       mode;
    unsigned char       page;
    struct cursor_pos   cursor_position;
    unsigned short __FAR *video_mem;
    unsigned short      char_attr_bios;
    unsigned short      char_attr_vmem;
    unsigned char       text_mode = 0;

    /* get current video mode */
    mode = BIOSGetMode();
    /* get current video page */
    page = BIOSGetPage();
    /* get cursor position for current page */
    cursor_position = BIOSGetCurPos( page );
    if( mode < GR_MED_4COL || mode == MONOCHROME || mode > VGA_256COL ) {
        video_mem = (unsigned short __FAR *) UIData->screen.origin;
        /* set cursor position to top left corner of screen */
        BIOSSetCurPos( 0, 0, page );
        /* get character/attribute at that location */
        char_attr_bios = BIOSGetCharAttr( page );
        /* get character/attribute from screen memory */
        char_attr_vmem = *video_mem;
        if( char_attr_bios == char_attr_vmem ) {
            /* change the character we read through BIOS call */
            char_attr_bios ^= 1;
            /* write out character using BIOS */
            BIOSSetCharAttr( char_attr_bios, page );
            /* get character/attribute from screen memory */
            char_attr_vmem = *video_mem;
            if( char_attr_bios == char_attr_vmem ) {
                /* restore character that was there */
                *video_mem = char_attr_bios ^ 1;
                text_mode = 1;
            }
        }
    }
    /* restore cursor position for current page */
    BIOSSetCurPos( cursor_position.row, cursor_position.col, page );
    return( text_mode );
}

bool intern initmonitor( void )
/*****************************/
{
    register    bool                    ega;
    register    unsigned char           mode;
    struct      ega_info                info;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }

    BIOSVidPage = BIOSGetPage();
    mode = BIOSGetMode();
    UIData->width = BIOSGetColumns();
    UIData->height = 25;
    info = BIOSEGAInfo();
    if( info.switches < 0x0C && info.mono <= 0x01 && info.mem <= 0x03 ) {
        UIData->height = BIOSGetRows();
        ega = true;
    } else {
        ega = false;
    }
    if( ( mode == MONOCHROME ) || ( mode == EGA_HIGH_MONO ) ) {
        UIData->colour = M_MONO;
    } else if( ( mode == ALPHA_SMALL_COL ) || ( mode == ALPHA_LARGE_COL ) ) {
        if( ega ) {
            UIData->colour = M_EGA;
        } else {
            UIData->colour = M_CGA;
        }
    } else if( ( mode == ALPHA_SMALL_BW ) || ( mode == ALPHA_LARGE_BW ) ) {
        UIData->colour = M_BW;
    } else if( mode > VGA_256COL ) {
        /*
            if mode is out of known range then assume it's a VGA/SVGA mode
        */
        UIData->colour = M_VGA;
    } else {
        return( false );
    }

    return( true );
}


bool intern initbios( void )
/**************************/
{
    bool                    initialized;
    unsigned short __far    *poffset;
    LP_PIXEL                old_origin;

    initialized = false;
    if( initmonitor() ) {
        UIData->desqview = (desqview_present() != 0);
        UIData->f10menus = true;

        poffset = firstmeg( BIOS_PAGE, SCREEN_OFFSET );
        if( UIData->colour == M_MONO ) {
            UIData->screen.origin = firstmeg( 0xb000, *poffset );
        } else {
            UIData->screen.origin = firstmeg( 0xb800, *poffset );
        }
        if( UIData->desqview ) {
            UIData->screen.origin =
             (LP_PIXEL)dos_uivideobuffer( UIData->screen.origin );
        }
        if( uiisdbcs() ) {
            old_origin = UIData->screen.origin;
            UIData->screen.origin =
             (LP_PIXEL)dos_uivideobuffer( UIData->screen.origin );
            if( old_origin != UIData->screen.origin ) {
                UIData->desqview = true;
            }
            DBCSCharacterMap(); /* in UIMAPCH.C */
        }
        UIData->screen.increment = UIData->width;

        /* if we are in a text mode then continue the initialization */
        if( IsTextMode() ) {
            uiinitcursor();
            initkeyboard();
            UIData->mouse_acc_delay = 5;   /* ticks */
            UIData->mouse_rpt_delay = 1;   /* ticks */
            UIData->mouse_clk_delay = 5;   /* ticks */
            UIData->tick_delay = 9;        /* ticks */
            UIData->mouse_speed = 8;       /* mickeys to ticks ratio */
            initialized = true;
        }
    }
    return( initialized );
}

unsigned UIAPI uiclockdelay( unsigned milli )
{
    /* this routine converts milli-seconds into platform  */
    /* dependant units - used to set mouse & timer delays */
    return( milli * 18 / 1000 );
}


void intern finibios( void )
/**************************/
{
    uifinicursor();
}


/* update the physical screen with contents of virtual copy */

void intern physupdate( SAREA *area )
/*************************************/
{
    int i;
    unsigned short offset;
    unsigned short count;

/*
    The code for desqview_update is identical for DOS/V
    (Update Video Display: int 10h, AH=FFh, CX=count, ES:DI=buffer)
*/

#ifdef __386__
            union       REGPACK                 regs;
#else
            #pragma aux desqview_update = 0xcd 0x10 parm [ah] [es] [di] [cx];
            extern void desqview_update( unsigned char, unsigned, unsigned, unsigned );
#endif

        if( UIData->desqview ) {
            count = area->width * sizeof( PIXEL );
            for( i = area->row; i < (area->row + area->height); i++ ) {
                offset = ( i * UIData->width + area->col ) * sizeof( PIXEL );

#ifdef __386__
            memset( &regs, 0, sizeof( regs ) );
            if( _IsPharLap() ) {

// This code does not work as advertised under DOS/V
//              regs.h.ah = 0xff; /* update logical screen buffer request */
//              regs.w.es = FP_SEG( UIData->screen.origin );
//              regs.x.edi = FP_OFF( UIData->screen.origin );
//              regs.x.edi += offset;
//              regs.w.cx = count;
//              intr( BIOS_VIDEO, &regs );

                PHARLAP_block pblock;

                memset( &pblock, 0, sizeof( pblock ) );
                pblock.int_num = BIOS_VIDEO;        /* VIDEO call */
                pblock.real_eax = 0xff00;           /* update from v-screen */
                pblock.real_es = FP_OFF( UIData->screen.origin ) >> 4;
                regs.x.edi = (FP_OFF( UIData->screen.origin ) & 0x0f) + offset;
                regs.w.cx = count;
                regs.x.eax = 0x2511;                /* issue real-mode interrupt */
                regs.x.edx = FP_OFF( &pblock );     /* DS:EDX -> parameter block */
                regs.w.ds = FP_SEG( &pblock );
                intr( 0x21, &regs );

            } else if( _IsRational() ) {
                rm_call_struct  dblock;

                memset( &dblock, 0, sizeof( dblock ) );
                dblock.eax = 0xff00;                /* update from v-screen */
                dblock.es = FP_OFF( UIData->screen.origin ) >> 4;
                dblock.edi = (FP_OFF( UIData->screen.origin ) & 0x0f)
                               + offset;
                dblock.ecx = count;
                DPMISimulateRealModeInterrupt( BIOS_VIDEO, 0, 0, &dblock );
            }
#else
            desqview_update( 0xff, FP_SEG( UIData->screen.origin ),
                            FP_OFF( UIData->screen.origin ) + offset, count );
#endif
            } /* for */
        } /* if */
}
