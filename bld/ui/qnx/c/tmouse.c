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
#include <unistd.h>
#include <string.h>
#include <sys/proxy.h>
#include <sys/types.h>
#include <sys/timers.h>
#include <sys/kernel.h>
#include <sys/osinfo.h>
#include <sys/dev.h>
#include "uidef.h"
#include "uimouse.h"
#include "trie.h"
#include "qdebug.h"

#include "uivirt.h"
#include "qnxuiext.h"

extern void     stopmouse(void);
extern int      nextc(int);
extern char     *GetTermType(void);


#define         MOUSE_SCALE             8


extern          ORD       MouseRow;
extern          ORD       MouseCol;
extern          bool      MouseOn;


extern          unsigned short          MouseStatus;
extern          bool                    MouseInstalled;

static __segment        SysTimeSel;

static enum {
    M_NONE,
    M_QW,       /* QNX mode QNX Windows/Photon */
    M_AW,       /* ANSI mode QNX Windows/Photon */
    M_XT        /* XTerm */
} MouseType;


#define MAXBUF    30
static char buf[ MAXBUF + 1 ];
static int new_sample;

#if defined( __386__ )
    extern unsigned long GetLong( unsigned short time_sel,
                                  unsigned long  time_off );

    #pragma aux GetLong = "mov ES,AX"           \
                          "mov EAX,ES:[EDX]"    \
                                                \
                          parm   [EAX] [EDX]    \
                          value  [EAX]          \
                          modify [ES];

    #define GET_MSECS   (GetLong( SysTimeSel, offsetof( struct _timesel, nsec ) ) / 1000000 \
                       + GetLong( SysTimeSel, offsetof( struct _timesel, seconds ) ) * 1000)
#else
    #define _SysTime    ((struct _timesel far *) MK_FP( SysTimeSel, 0 ))
    #define GET_MSECS   (_SysTime->nsec / 1000000 + (_SysTime->seconds) * 1000)
#endif

#define QW_BELL "\007"

#define QNX_HDR         "\x1b/"
#define ANSI_HDR        "\x1b["

#define QW_TEST                 "10t"
#define QW_TEST_RESPONSE        "10;"
#define QW_INIT                 ">1;6;7;8h\033/>9l"
#define QW_FINI                 ">1;6;7;8l"

#define XT_INIT "\033[?1000h"
#define XT_FINI "\033[?1000l"

static  void tm_error()
/*********************/
{
//    write( UIConHandle, QW_BELL, sizeof( QW_BELL ) - 1 );
}

/*-
 * tm_check
 *   1- parse the mouse string
 *   2- Update externals if valid.
 *   3- set parameters to reflect it externals
 */

static int last_row, last_col, last_status;

/* Parse a QNX Windows/Photon mouse event. */
static void QW_parse()
{
    int func, mrow, mcol, mbut, mclick;
    char *p = buf;

    mclick = 1;
    if((func = strtol(p, &p, 10)) > 3276 || *p != ';') return;
    if((mrow = strtol(p+1, &p, 10)) > 3276 || *p != ';') return;
    if((mcol = strtol(p+1, &p, 10)) > 3276 || *p != ';') return;
    mbut = strtol(p+1, &p, 10);
    if (*p == ';') {
        mclick = strtol(p+1, &p, 10);
    }
    if( *p != 't' ) return;
    last_row = mrow;
    last_col = mcol;
    /*
       The func is actually 31, 32, etc, but the leading "3" was swallowed
       while recognizing that we had a mouse event
    */
    switch( func ) {
    case 1:
        last_status = 0;
        if (mbut & 004) last_status |= MOUSE_PRESS;
        if (mbut & 002) last_status |= MOUSE_PRESS_MIDDLE;
        if (mbut & 001) last_status |= MOUSE_PRESS_RIGHT;
        break;
    case 2:
        last_status = 0;
        break;
    }
}

/* Parse an xterm mouse event. */
static void XT_parse()
{
    last_col = buf[1] - 0x21;
    last_row = buf[2] - 0x21;
    switch( buf[0] & 0x03 ) {
    case 0: last_status |= MOUSE_PRESS; break;
    case 1: last_status |= MOUSE_PRESS_MIDDLE; break;
    case 2: last_status |= MOUSE_PRESS_RIGHT; break;
    case 3: last_status = 0;
    }
}


static int tm_check( unsigned short *status, unsigned short *row,
                        unsigned short *col, unsigned long *time )
/**********************************************/
{
    if( !MouseInstalled ) {
         uisetmouse(*row, *col);
         return 0;
    }
    QNXDebugPrintf1("mouse_string = '%s'", buf);
    if( new_sample ) {
        switch( MouseType ) {
        case M_QW:
        case M_AW:
            QW_parse();
            break;
        case M_XT:
            XT_parse();
            break;
        }
        new_sample = 0;
    }
    *row = last_row;
    *col = last_col;
    *status = last_status;
    *time = GET_MSECS;
    uisetmouse(*row, *col);
    return 0;
}

static int tm_stop()
/*********************/
{
    return 0;
}

static void DoMouseInit( int type, char *init, char *input )
{
    struct _osinfo      osinfo;
    unsigned short      row;
    unsigned short      col;

    MouseType = type;
    uimouseforceoff();
    write( UIConHandle, init, strlen( init ) );
    TrieAdd( EV_MOUSE_PRESS, input );

    MouseInstalled = TRUE;

    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;

    qnx_osinfo( 0, &osinfo );
    SysTimeSel = osinfo.timesel;

    checkmouse( &MouseStatus, &row, &col, &MouseTime );
    MouseRow = row;
    MouseCol = col;
    stopmouse();
}

static int tm_init( bool install )
/******************************/
{
    char        *term;

    MouseInstalled = FALSE;
    MouseType = M_NONE;

    if( !install ) return( FALSE );

    term = GetTermType();
    if( strcmp( term, "xterm" ) == 0 ) {
        DoMouseInit( M_XT, XT_INIT, ANSI_HDR "M" );
        return( TRUE );
    }
    if( strstr( term, "qnx" ) != 0 ) {
        write( UIConHandle, QNX_HDR QW_TEST, sizeof( QNX_HDR QW_TEST ) - 1 );
        TrieAdd( EV_MOUSE_PRESS, QNX_HDR QW_TEST_RESPONSE );
        return( TRUE );
    }
    write( UIConHandle, ANSI_HDR QW_TEST, sizeof( ANSI_HDR QW_TEST ) - 1 );
    TrieAdd( EV_MOUSE_PRESS, ANSI_HDR QW_TEST_RESPONSE );
    return( TRUE );
}

static int tm_fini()
/*********************/
{
    switch( MouseType ) {
    case M_QW:
        write( UIConHandle, QNX_HDR QW_FINI, sizeof( QNX_HDR QW_FINI ) - 1 );
        break;
    case M_AW:
        write( UIConHandle, ANSI_HDR QW_FINI, sizeof( ANSI_HDR QW_FINI ) - 1 );
        break;
    case M_XT:
        write( UIConHandle, XT_FINI, sizeof( XT_FINI ) - 1 );
        break;
    }
    return 0;
}

static int tm_set_speed( unsigned speed )
/****************************************/

/* Set speed of mouse. 1 is fastest; the higher the number the slower
 * it goes.
 * Since the mouse_param "gain" parameter is a "multiplier", a higher
 * number means faster. So, we've got to map from one scheme to another.
 * For now, 10 will be fastest and 1 will be slowest.
 */
{
    speed = speed;
    return 0;
}

void tm_saveevent()
/*****************/
{
    int i;
    int c;

    if( new_sample ) {
        tm_error();
        return;
    }
    switch( MouseType ) {
    case M_NONE:
        /* eat the remainder of the version ID response. */
        for( ;; ) {
            c = nextc( 20 );
            if( c == -1 ) return;
            if( c == 't' ) break;
        }
        if( strstr( GetTermType(), "qnx" ) != 0 ) {
            DoMouseInit( M_QW, QNX_HDR QW_INIT, QNX_HDR "3" );
        } else {
            DoMouseInit( M_AW, ANSI_HDR QW_INIT, ANSI_HDR "3" );
        }
        return;
    case M_QW:
    case M_AW:
        for( i = 0; i < MAXBUF; ++i ) {
            c = nextc( 20 );
            if( c == -1 ) {
               tm_error();
               return;
            }
            buf[i] = c;
            if( buf[i] == 't' ) break;
            if( buf[i] == '\x1b' ) {
                tm_error();
                c = nextc( 20 );
                if( c == -1 ) {
                    tm_error();
                    return;
                }
                i = 0;
            }
        }
        break;
    case M_XT:
        for( i = 0; i < 3; ++i ) {
            c = nextc( 10 );
            if( c == -1 ) {
                tm_error();
                return;
            }
            buf[i] = c;
        }
        break;
    }
    if( i == MAXBUF ) tm_error();
    buf[i+1] = '\0';
    new_sample = 1;
}

Mouse TermMouse = {
        tm_init,
        tm_fini,
        tm_set_speed,
        tm_stop,
        tm_check,
};
