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


#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
#include "uidef.h"
#include "uiattrs.h"
#include "uimouse.h"

/*
    The MONITOR structure contains the following fields:
        ORD             height;         ! number of rows                   !
        ORD             width;          ! number of columns                !
        int             colour;         ! M_MONO, M_CGA, M_EGA, M_VGA, ... !
        ATTR _FARD      *attrs;         ! attributes                       !
        EVENTLIST _FARD *events;        ! event list pointer               !
        UI_WINDOW _FARD *area_head;     ! head of area list                !
        UI_WINDOW _FARD *area_tail;     ! tail of area list                !
        unsigned        mouse_acc_delay;! acceleration delay               !
        unsigned        mouse_rpt_delay;! repeat delay                     !
        unsigned        mouse_clk_delay;! double click delay               !
        unsigned        tick_delay;     ! clock tick delay                 !
        int             cursor_on;      ! cursor on flag                   !
        int             cursor_attr;    ! cursor attribute                 !
        ORD             cursor_row;     ! cursor row                       !
        ORD             cursor_col;     ! cursor column                    !
        int             cursor_type;    ! cursor type                      !
        unsigned char   old_shift;      ! status of shift keys             !
        int             no_snow;        ! snow check flag                  !
        UI_WINDOW               blank;          ! blank window                     !
        BUFFER          screen;         ! screen                           !
        unsigned        desqview:1;     ! desqview present flag            !
        unsigned        f10menus:1;     ! F10 active for menus             !
        unsigned        busy_wait:1;    ! SINK, MOUSE_HOLD or NO_EVENT     !
        unsigned        mouse_swapped:1;! mouse swap flag                  !
        unsigned        no_idle_int:1;  ! disable idle interrupt           !
        unsigned        no_refresh:1;   ! disable refresh on EV_NO_EVENT   !
        unsigned        mouse_speed;    ! mouse speed factor               !
        unsigned char   mouse_xscale;   ! factor to divide mouse x posn    !
        unsigned char   mouse_yscale;   ! factor to divide mouse y posn    !
*/

/* This is the default setting for the some of UI_DATA attributes */
static  MONITOR ui_data = {
                24,             /* height */
                80,             /* width */
                M_FMR,          /* colour */
                NULL,           /* attrs */
                NULL,           /* events */
                NULL,           /* area_head */
                NULL,           /* area_tail */
                4,              /* mouse_acc_delay */
                1               /* mouse_rpt_delay */
};

extern unsigned short BIOSScreenSize( void );
#pragma aux BIOSScreenSize =    \
    "push   dx"                 \
    "mov    ah,04h"             \
    "int    91h"                \
    "mov    ax,dx"              \
    "pop    dx"                 \
    value   [ax];

struct fmr_attr {
    char    char_attr;
    char    disp_attr;
    short   colour_id;
};

struct fmr_screen {
    char            __far *code;
    struct fmr_attr __far *attr;
};

extern char BIOSScreenGet( unsigned short offset, unsigned short segment,
                           short screen_start, short screen_end );
#pragma aux BIOSScreenGet =     \
    "push   ds"                 \
    "push   di"                 \
    "mov    ds,dx"              \
    "mov    di,ax"              \
    "mov    dx,bx"              \
    "mov    bx,cx"              \
    "mov    al,01h"             \
    "mov    ah,16h"             \
    "int    91h"                \
    "pop    di"                 \
    "pop    ds"                 \
    "mov    al,ah"              \
    parm    [ax][dx][bx][cx]    \
    value   [al]                \
    modify  [ax];

extern char BIOSScreenPut( unsigned short offset, unsigned short segment,
                           short screen_start, short screen_end );
#pragma aux BIOSScreenPut =     \
    "push   ds"                 \
    "push   di"                 \
    "mov    ds,dx"              \
    "mov    di,ax"              \
    "mov    dx,bx"              \
    "mov    bx,cx"              \
    "mov    al,01h"             \
    "mov    ah,15h"             \
    "int    91h"                \
    "pop    di"                 \
    "pop    ds"                 \
    "mov    al,ah"              \
    parm    [ax][dx][bx][cx]    \
    value   [al]                \
    modify  [ax];

/* get screen background */

extern char BIOSScreenBG();
#pragma aux BIOSScreenBG =      \
    "push   dx"                 \
    "mov    ah,20h"             \
    "int    91h"                \
    "mov    ax,dx"              \
    "pop    dx"                 \
    value   [al]                \
    modify  [ax];

char            __far  *FMRScreenCodes;
struct fmr_attr __far  *FMRScreenAttrs;
char                    FMRScreenHeight;
char                    FMRScreenWidth;
short                   FMRScreenUpdates = 0;
unsigned long           FMRScreenTime = 0L;

int intern initbios()
/***********************************************************************
 initbios:
 --------
    Input:      NONE
    Output:     NONE

    This function sets video memory origin and initialize allocated
    memory location ( clear screen ) and also do some initialization
    for cursor, keyboard, and mouse
************************************************************************/
{
    int                 initialized;

    initialized = FALSE;
    initmonitor();
    UIData->desqview = FALSE;
    UIData->f10menus = TRUE;

    UIData->screen.increment = UIData->width;

    /* allocate space for a virtual screen of character codes/attributes
        and include a buffer for character/attributes */
    UIData->screen.origin = (LPPIXEL) faralloc(
                    sizeof( PIXEL ) * FMRScreenHeight * FMRScreenWidth
               +     sizeof( char ) * FMRScreenHeight * FMRScreenWidth
        + sizeof( struct fmr_attr ) * FMRScreenHeight * FMRScreenWidth
                               );

    if( UIData->screen.origin != NULL ) {
        initscreen();
        uiinitcursor();
        initkeyboard();
        UIData->mouse_acc_delay = uiclockdelay( 278 );
        UIData->mouse_rpt_delay = uiclockdelay( 56 );
        UIData->mouse_clk_delay = uiclockdelay( 278 );
        UIData->tick_delay = uiclockdelay( 500 );
        UIData->mouse_speed = 8;       /* mickeys to ticks ratio */
        initialized = TRUE;
    } else {
        initialized = FALSE;
    }
    return( initialized );
}

static void intern initmonitor()
/***********************************************************************
 initmonitor:
 -----------
    Input:      NONE
    Output:     NONE

************************************************************************/
{
    unsigned short  screen_size;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }

    screen_size = BIOSScreenSize();
    /* setting screen width */
    FMRScreenWidth = screen_size & 0xff;
    FMRScreenHeight = screen_size >> 8;
    UIData->width = FMRScreenWidth;
    UIData->height = FMRScreenHeight - 1;   /* can't use bottom line */
}

static void setscreen( struct fmr_screen *scr )
{
    scr->code = FMRScreenCodes;
    scr->attr = FMRScreenAttrs;
}

static void initscreen()
{
    struct fmr_screen screen;

    _fmemset( UIData->screen.origin, 0x00,
                sizeof( PIXEL ) * FMRScreenHeight * FMRScreenWidth );
    FMRScreenCodes = (char __far *) ( UIData->screen.origin +
                                FMRScreenHeight * FMRScreenWidth );
    FMRScreenAttrs = (struct fmr_attr __far *) ( FMRScreenCodes +
                                FMRScreenHeight * FMRScreenWidth );
    setscreen( &screen );
    BIOSScreenGet( FP_OFF( &screen ), FP_SEG( &screen ),
            0x0101, (short)( (FMRScreenHeight << 8) | FMRScreenWidth ) );
}

void intern finibios()
/***********************************************************************
 finibios:
 --------
    Input:      NONE
    Output:     NONE

    clean up routine after bios operation is done
************************************************************************/
{
    uifinicursor();
    if( UIData->screen.origin != NULL ) farfree( UIData->screen.origin );
}


unsigned short jmstojis( unsigned short msc )
{
    unsigned char u, l;

    l = msc;
    u = msc >> 8;

    u = (u - ((u < 0xa0) ? 0x71 : 0xb1)) * 2 + ((l >= 0x9f) ? 2 : 1);
    l -= (l >= 0x7f) ? ((l < 0x9f) ? 0x20 : 0x7e) : 0x1f;

    return( ( (unsigned short)u << 8 ) + l );
}

unsigned short jistojms( unsigned short c )
{
    unsigned char u, l;

    l = c;
    u = c >> 8;

    if( u < 0x21 || u > 0x7e || l < 0x21 || l >0x7e ) return 0;

    l += (u % 2) ? ((l < 0x60) ? 0x1f : 0x20) : 0x7e;
    u = (u - 1) / 2 + ((u < 0x5f) ? 0x71 : 0xb1);

    return( ( (unsigned short)u << 8 ) + l);
}

unsigned short iszen( unsigned short c )
{
    unsigned char u, l;

    l = c;
    u = c >> 8;

    if( (u < 0x81) || (u > 0xFC) ) return 0;
    if( (u > 0x9F) && (u < 0xE0) ) return 0;
    if( (l < 0x40) || (l > 0xFC) ) return 0;
    if( l == 0x7f ) return 0;
    return( c );
}

/*
    The following table converts background colours to equivalent
    foreground colour IDs (used to compare foreground colour with
    current background colour).
*/

static const char boxchar[] = {
    0x20,               /* 0x00 space    */
    0x95,               /* 0x01 B_BSBS Ä */
    0xe0,               /* 0x02 B_BDBD Í */
    0x96,               /* 0x03 B_SBSB ³ */
    0x96,               /* 0x04 B_DBDB º N.A. use ³ */
    0x98,               /* 0x05 B_BSSB Ú */
    0x9c,               /* 0x06 B_BSDB Ö N.A. use top-left curve */
    0x9c,               /* 0x07 B_BDDB É N.A. use top-left curve */
    0x99,               /* 0x08 B_BBSS ¿ */
    0x9d,               /* 0x09 B_BBDD » N.A. use top-right curve */
    0x9a,               /* 0x0a B_SSBB À */
    0x9e,               /* 0x0b B_DSBB Ó N.A. use bottom-left curve */
    0x9e,               /* 0x0c B_DDBB È N.A. use bottom-left curve */
    0x9b,               /* 0x0d B_SBBS Ù */
    0x9f,               /* 0x0e B_SBBD ¾ N.A. use bottom-right curve */
    0x9f,               /* 0x0f B_DBBD ¼ N.A. use bottom-right curve */
    0x93,               /* 0x10 B_SSSB Ã */
    0xe1,               /* 0x11 B_SDSB Æ */
    0x92,               /* 0x12 B_SBSS ´ */
    0xe3,               /* 0x13 B_SBSD µ */
    0x90,               /* 0x14 B_DSBS Ð N.A. use Á */
    0x8f,               /* 0x15 B_DDDD Î N.A. use Å */
    0x86,               /* 0x16 B_DBLK Ü */
    0x94,               /* 0x17 B_UBLK ß */
    0xEA,               /* 0x18 DIAMOND  */
    0xEC,               /* 0x19 CLOSE_CIRCLE  */
    0xED,               /* 0x1A OPEN_CIRCLE () */
    0xF0                /* 0x1B CROSSED-X */
};

static const char bgcolours[8] = {
            /*  bg        id   */
    0,      /* black   0 -> black */
    2,      /* red     1 -> red  */
    4,      /* green   2 -> green */
    6,      /* yellow  3 -> yellow */
    1,      /* blue    4 -> blue */
    3,      /* magenta 5 -> purple */
    5,      /* cyan    6 -> light blue */
    7       /* white   7 -> white */
};

/*
    The following table is used to remap a foreground colour
    when it is the same as the background colour.
*/

static const char fgcolours[8] = {
            /*  bg        id   */
    7,      /* black   0 -> white */
    6,      /* blue    1 -> yellow */
    4,      /* red     2 -> green */
    5,      /* magenta 3 -> cyan   */
    2,      /* green   4 -> red  */
    3,      /* cyan    5 -> magenta */
    1,      /* yellow  6 -> blue */
    0       /* white   7 -> black */
};

extern          bool                    MouseInstalled; /* UIMOUSEV.C */
extern          bool                    MouseOn;        /* UIMOUSEV.C */
extern          void global             uimousesave( SAREA *area );
extern          void global             uimouserestore( void );
extern          unsigned long           uiclock( void );

void intern physupdate( SAREA *area )
/************************************
    area->row    \___ starting row,column
    area->col    /
    area->height \___ dimensions of area
    area->width  /
 ************************************/
{
    LPPIXEL             src;
    struct fmr_screen   screen;
    unsigned long start;


    unsigned short      offset1, offset2;
    unsigned short      shift_jis_code;
    unsigned short      jis_code;
    unsigned short      i, j;
    unsigned char       fg;
//  unsigned char       bg;
    unsigned char       c1, c2;
    unsigned char       a1, a2;


    FMRScreenUpdates++;
    setscreen( &screen );

    for( i = area->row; i < (area->row + area->height); i++ ) {
        offset1 = i * UIData->width + area->col;
        offset2 = (i - area->row) * area->width;
        for( j = 0; j < area->width; j++ ) {
            src = &(UIData->screen.origin[ offset1 ]);
            a1 = src->attr;
            fg = a1 & 0x07;         /* get IBM foreground colour */
//          bg = (a1 & 0x70) >> 4;  /* get IBM background colour */

            a2  = (a1 & BRIGHT)    ? 0x20 : 0x00; /* bright -> highlight */
            a2 |= (a1 & BLINK)     ? 0x10 : 0x00; /* blink -> blink */
            a2 |= (a1 & REVERSE)   ? 0x08 : 0x00; /* reverse -> reverse */
            a2 |= (a1 & UNDERLINE) ? 0x80 : 0x00; /* underline -> underscore */
//          if( (a1 & (0x07|BRIGHT)) == bgcolours[ BIOSScreenBG() ] ) {
//              a2 |= BRIGHT;
//          }
            if( fg == bgcolours[ BIOSScreenBG() ] ) {
                fg = fgcolours[ fg ];
            }
            screen.attr[ offset2 ].disp_attr = a2;
            screen.attr[ offset2 ].colour_id = fg;
            c1 = src->ch;
            c2 = (src+1)->ch;
            shift_jis_code = (c1 << 8) + c2;
            if( c1 <= 0x1B ) {
                /* if current character is box character let's draw box */
                screen.code[ offset2 ] = boxchar[ c1 ];
                screen.attr[ offset2 ].char_attr = 0x00;
            } else if( iszen( shift_jis_code ) ) {
                /* if double byte character */
                offset1++;  /* skip 2nd char/attr pair */

                jis_code = jmstojis( shift_jis_code );

                screen.code[ offset2 ] = (char) (jis_code >> 8);
                screen.attr[ offset2 ].char_attr = 0x01;  /* DBCS byte 1 */
                offset2++;
                screen.code[ offset2 ] = (char) jis_code;
                screen.attr[ offset2 ].char_attr = 0x03; /* DBCS byte 2 */
                screen.attr[ offset2 ].disp_attr = a2;
                screen.attr[ offset2 ].colour_id = fg;
            } else {
                /* otherwise just copy current char to our virtual screen */
                screen.code[ offset2 ] = c1;
                screen.attr[ offset2 ].char_attr = 0x00;
            }
            offset1++;
            offset2++;
        } /* for */
    } /* for */

    uimousesave( area );    /* save mouse cursor from erasure */
    i = (area->row << 8) | area->col;
    j = ((area->row + area->height) << 8) | (area->col + area->width);
    start = uiclock();
    BIOSScreenPut( FP_OFF( &screen ), FP_SEG( &screen ),
            (short)( i + 0x0101 ),
            (short)( j ) );
    FMRScreenTime += (uiclock() - start);
    uimouserestore();       /* restore mouse cursor */
}
