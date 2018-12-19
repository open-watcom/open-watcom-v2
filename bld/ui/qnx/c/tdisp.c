/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/seginfo.h>
#include <sys/dev.h>
#include <sys/osinfo.h>
#include <sys/sidinfo.h>
#include <sys/psinfo.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/proxy.h>
#include <sys/vc.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <i86.h>
#include <assert.h>
#include <ctype.h>
#include <sys/qnx_glob.h>
#include <process.h>
#include "walloca.h"
#include "wterm.h"
#include "uidef.h"
#include "uiattrs.h"
#include "qdebug.h"
#include "uivirt.h"
#include "uiextrn.h"
#include "ctkeyb.h"
#include "tixparse.h"
#include "tixsupp.h"
#include "doparse.h"
#include "tdisp.h"


#ifdef __386__
    #define _fmemcpy memcpy
#endif
#define PIXELEQUAL(p1,p2)   ((p1).ch == (p2).ch && (p1).attr == (p2).attr)

extern LP_PIXEL asmNonBlankEnd( LP_PIXEL, int, PIXEL );
#ifdef _M_I86
    #pragma aux asmNonBlankEnd = \
        "std"                       \
        "repe scasw"                \
        "je short L1"               \
        "inc di"                    \
    "L1: cld"                       \
    __parm  [__es __di] [__cx] [__ax] \
    __value [__es __di]
#else
    #pragma aux asmNonBlankEnd = \
        "std"                       \
        "repe scasw"                \
        "je short L1"               \
        "inc edi"                   \
    "L1: cld"                       \
    __parm  [__es __edi] [__ecx] [__ax] \
    __value [__edi]
#endif

bool    UserForcedTermRefresh = false;

// this is handy for those functions that do *almost* the same thing
// for the two terminal types
static bool         TermIsQNXTerm;

static void         TI_SETATTR( void );
static int          new_attr( int nattr, int oattr );
static int          ti_refresh( bool must );

bool TermCheck( void )
/********************/
{
    if( strstr( GetTermType(), "qnx" ) == 0 )
        return( false );
    UIDisableShiftChanges = true;
    TermIsQNXTerm = true;
    return( true );
}

bool TInfCheck( void )
/********************/
{
    // Check to see if the term variable is set
    if( GetTermType()[0] != '\0' ) {
        UIDisableShiftChanges = true;
        TermIsQNXTerm = false;
        return( true );
    }
    return( false );
}


/*-
 * Mini buffered IO code...
 */
struct ostream {
    char    *sbuf, *ebuf;
    char    *curp;
    int     fd;
};

static struct ostream _con_out;

static bool ostream_init( int f )
{
    if( (_con_out.sbuf = uimalloc( 2048 )) == NULL ) {
        return( false );
    }
    _con_out.ebuf = _con_out.sbuf + 2048;
    _con_out.curp = _con_out.sbuf;
    _con_out.fd   = f;
    return( true );
}
#define __putchar(_c) ((_con_out.curp < _con_out.ebuf) ? \
                        (*_con_out.curp++ = (_c)) : __oflush(_c))

static void __flush_con( void )
{
    int     len;
    int     offs;
    int     n;

    offs = 0;
    for( len = _con_out.curp - _con_out.sbuf; len > 0; len -= n ) {
        n = write( _con_out.fd, _con_out.sbuf + offs, len );
        if( n < 0 ) {
            break;
        }
        offs += n;
    }
    _con_out.curp = _con_out.sbuf;
}

static int __oflush( int c )
{
    __flush_con();
    //assert( _con_out.curp < _con_out.ebuf );
    return( __putchar( c ) );
}

static void __puts( char *s )
{
    int     c;

    if( s == NULL )
        return;
    while( (c = *s++) != '\0' ) {
        __putchar(c);
    }
}



/*-
 * Qnx terminal codes
 */

#define QNX_CURSOR_OFF()        __puts(_ESC "y0")
#define QNX_CURSOR_NORMAL()     __puts(_ESC "y1")
#define QNX_CURSOR_BOLD()       __puts(_ESC "y2")

#define QNX_BOLD()              __puts(_ESC "<")
#define QNX_NOBOLD()            __puts(_ESC ">")
#define QNX_BLINK()             __puts(_ESC "{")
#define QNX_NOBLINK()           __puts(_ESC "}")
#define QNX_ULINE()             __puts(_ESC "[")
#define QNX_WRAP()              __puts(_ESC "h")
#define QNX_NOWRAP()            __puts(_ESC "i")

#define QNX_RESTORE_ATTR()      __puts(_ESC "R")

#define QNX_NOULINE()           __puts(_ESC "]")
#define QNX_HOME()              __puts(_ESC "H")
#define QNX_CLS()               __puts("\f")

/*-
 * set initial mode of term
 */


static void QNX_CURSOR_MOVE( int c, int r )
{
    __putchar( _ESC_CHAR ); __putchar( '=' );
    __putchar( r + ' ' );
    __putchar( c + ' ' );
}

static void QNX_SETCOLOUR( int f, int b )
{
    __putchar( _ESC_CHAR ); __putchar( '@' );
    __putchar( '0' + f );
    __putchar( '0' + b );
}



// This is basically a rewrite of the putp that's supposed to appear in
// term.h, except it uses our "mini buffered IO code" from above
static void _putp( char *s )
{
    int         c;
    int         pad;
    int         mand = false;
    int         i;
    char        *bbuf;

    if( s == NULL )
        return;
    while( (c = *s++) != '\0' ) {
        // check and see if we're at the start of a padding sequence
        if( c == '$' && *s == '<' ) {
            bbuf = s;
            s++;
            pad = 0;
            // read until the end of the sequence or the end of the string
            while( (c = *s++) != '\0' && ( c != '>' ) ) {
                // suck up digits
                if( c >= '0' && c <= '9' ) {
                    pad *= 10;
                    pad += c;
                } else if( c == '.' ) {
                    // Skip tenth's
                    if( (c = *s) >= '0' && c <= '9' ) {
                        s++;
                        // cheap rounding
                        if( c >= '5' ) {
                            pad++;
                        }
                    }
                // Pay attention to the mandatory flag
                } else if( c == '/' ) {
                    mand = true;
                }
                // Note that I'm completely ignoring the * flag( proportional
                // to number of lines padding ). I'm just assuming 1 line
                // always. This should work virtually all of the time because
                // we don't do anything excessively weird. ( like insert over
                // multiple lines )
                // Actually, we ignore any extra chars that end up in here.
            }
            if( c == '>' ) {
                // output padding only if required
                if( !xon_xoff || mand ) {
                    mand = false;
                    for( i = 0; i < pad; i++ ) {
                        __putchar( pad_char[0] );
                    }
                }
            } else {
                __putchar( '$' );
                __puts( bbuf );
                return;
            }
        } else {
            __putchar( c );
        }
    }
}

// Whether to use code-changing optimizations in ti
static bool OptimizeTerminfo = true;

/* TermInfo terminal codes
*/

// The following are used to remember which attrs we want
static bool TIABold =   false;  // bold
static bool TIABlink =  false;  // blinking
static bool TIAULine =  false;  // underline
static bool TIARev =    false;  // inverted (reverse)
static bool TIAACS =    false;  // alternate character set

// True if clearing/filling operations will use the current back colour
static bool TI_FillColourSet = false;

// Macros for various terminfo capabilities
#define TI_CURSOR_OFF()         _putp( cursor_invisible )
#define TI_CURSOR_NORMAL()      _putp( cursor_normal )
#define TI_CURSOR_BOLD()        _putp( cursor_visible )

#define TI_BOLD()               ( TIABold = 1, _putp( enter_bold_mode ) )
#define TI_NOBOLD()             ( TIABold = 0, TI_SETATTR() )
#define TI_REVERSE()            ( TIARev = 1, TI_SETATTR() )
#define TI_NOREVERSE()          ( TIARev = 0, TI_SETATTR() )
#define TI_BLINK()              ( TIABlink = 1, _putp( enter_blink_mode ) )
#define TI_NOBLINK()            ( TIABlink = 0, TI_SETATTR() )
#define TI_ULINE()              ( TIAULine = 1, _putp( enter_underline_mode ) )
#define TI_NOULINE()            ( TIAULine = 0, _putp( exit_underline_mode ) )
#define TI_ACS_ON()             ( TIAACS = 1, _putp( enter_alt_charset_mode ) )
#define TI_ACS_OFF()            ( TIAACS = 0, \
                                    ((exit_alt_charset_mode[0] != '\0')\
                                    ?(void)( _putp( exit_alt_charset_mode ) )\
                                    :(void)(TI_SETATTR()) ))
#define TI_WRAP()               _putp( enter_am_mode )
#define TI_NOWRAP()             _putp( exit_am_mode )
#define TI_CA_ENABLE()          _putp( enter_ca_mode )
#define TI_CA_DISABLE()         _putp( exit_ca_mode )

#define TI_RESTORE_ATTR()       ( TIAACS = TIABold = TIABlink = TIAULine = 0, \
                                                _putp( exit_attribute_mode ) )
#define TI_RESTORE_COLOUR()     ( _putp( ( orig_pair[0] == '\0' )\
                                        ? orig_colors : orig_pair ) )

#define TI_ENABLE_ACS()         _putp( ena_acs )

#define TI_HOME()               TI_CURSOR_MOVE( 0, 0 );
// This weird "do...while" thing is so that TI_CLS acts like a statement
#define TI_CLS()                        \
    do {                                \
        if( clear_screen[0] != '\0' ) { \
            _putp( clear_screen );       \
            OldRow = OldCol = 0;        \
        } else {                        \
            TI_HOME();                  \
            _putp( clr_eos );            \
        }                               \
    } while( 0 )

#define TI_INIT1_STRING()       _putp( init_1string )
#define TI_INIT2_STRING()       _putp( init_2string )
#define TI_INIT3_STRING()       _putp( init_3string )

#define TI_RESET1_STRING()      _putp( reset_1string )
#define TI_RESET2_STRING()      _putp( reset_2string )
#define TI_RESET3_STRING()      _putp( reset_3string )

#define TI_CLEAR_MARGINS()      _putp( clear_margins )

/* Terminal Capabilities
*/

// True if terminal is monochrome
#define TCAP_MONOCHROME ( set_color_pair[0] == '\0' && set_background[0] == '\0' )

// True if terminal won't scroll on us
#define TCAP_NOSCROLL   ( !auto_right_margin || ( exit_am_mode[0] != '\0' ) )

// True if terminal can clear screen fast
#define TCAP_CLS        ( clear_screen[0] != '\0' || clr_eos[0] != '\0' )

static unsigned         TI_repeat_cutoff = ~0;
static bool             TI_ignore_bottom_right = true;

/* Find out where using the repeat_char code becomes efficient if we
 * are going to be doing optimizations.
 */
static void ti_find_cutoff( void )
{
    if( repeat_char[0] != '\0' && OptimizeTerminfo ) {
        int     i;
        for( i = 2; i < 80; i++ ) {
            if( strlen( tparm( repeat_char, 'X', i ) ) < i ) {
                TI_repeat_cutoff = i;
                break;
            }
        }
    }
}

/* Repeat character c, n times. "a" indicates alternate character set.
 * x is the first column that we should be printing on. (used for a little
 * optimization)
 */
static void TI_REPEAT_CHAR( char c, int n, bool a, ORD x )
{
    bool        blank;
    int         len;
    char        *cparm_right;

    if( n == 0 )
        return;

    blank = OptimizeTerminfo && ( TI_FillColourSet ) && c == ' ' && !a;
    if( blank
      && x == ( UIData->width - n )
      && (len = strlen( clr_eol )) > 0
      && n > len ) {
        _putp( clr_eol );
    } else if( blank
      && x == 0
      && clr_bol[0] != '\0'
      && n > (len = (strlen( cparm_right = tparm( parm_right_cursor, n )) + strlen( clr_bol ) ))
      && len > 0 ) {
        _putp( cparm_right );
        _putp( clr_bol );
    } else {
        if( a ) {
            TI_ACS_ON();
        }

        if( n >= TI_repeat_cutoff ) {
            _putp( tparm( repeat_char, c, n ) );
        } else {
            for( ; n > 0; n-- ) {
                __putchar( c );
            }
        }

        if( a ) {
            TI_ACS_OFF();
        }
    }
}

int     OldCol= -1,
        OldRow= -1;

// This macro will pick method "x" for axis "a" if method "x" is faster
// and usable (ie: less chars, but not zero chars)
#define pickMethod(a,x)                      \
    do {                                     \
        if( ( newLen < len.a || len.a == 0 ) \
                        && newLen != 0 ) {   \
            len.a = newLen;                  \
            method.a = ( x );                \
        }                                    \
    } while( 0 )

// move in the optimal way from (OldCol,OldRow) to (c,r)
static void TI_CURSOR_MOVE( int c, int r )
{
    unsigned            newLen;
    int                 i;

    struct {
        int     r,c;
    }                   len = { 0, 0 };

    struct {
        enum {
            none,
            absolute,
            rel_parm_plus,
            relative_plus,
            rel_parm_minus,
            relative_minus
        }       r,c;
    }                   method = { none, none };

    // Just use cursor_address if we're not supposed to optimize
    if( !OptimizeTerminfo ) {
        _putp( tparm( cursor_address, r, c ) );
        OldRow = r;
        OldCol = c;
        return;
    }

    // return if we're in the right place
    if( OldCol == c && OldRow == r )
        return;

    // if OldRow or OldCol <0 then the old position is invalid
    if( OldCol >= 0 && OldRow >= 0 ) {
        if( OldRow != r ) {
            method.r = absolute;
            len.r = strlen( tparm( row_address, r ) );

            if( OldRow < r ) {
                newLen = strlen( tparm( parm_down_cursor, r - OldRow ) );
                pickMethod( r, rel_parm_plus );

                newLen = ( r - OldRow ) * strlen( cursor_down );
                pickMethod( r, relative_plus );
            } else {
                newLen = strlen( tparm( parm_up_cursor, OldRow - r ) );
                pickMethod( r, rel_parm_minus );

                newLen = ( OldRow - r ) * strlen( cursor_up );
                pickMethod( r, relative_minus );
            }
        }
        if( OldCol != c ) {
            method.c = absolute;
            len.c = strlen( tparm( column_address, c ) );

            if( OldCol < c ) {
                newLen = strlen( tparm( parm_right_cursor, c - OldCol ) );
                pickMethod( c, rel_parm_plus );

                newLen = ( c - OldCol ) * strlen( cursor_right );
                pickMethod( c, relative_plus );
            } else {
                newLen = strlen( tparm( parm_left_cursor, OldCol - c ) );
                pickMethod( c, rel_parm_minus );

                newLen = ( OldCol - c ) * strlen( cursor_left );
                pickMethod( c, relative_minus );
            }
        }
    }

    // check to make sure the method we have so far is valid
    if( ( OldRow >= 0 && OldCol >= 0 )
      && ( len.r != 0 || method.r == none )
      && ( len.c != 0 || method.c == none )
      && ( ( len.r + len.c ) < strlen( tparm( cursor_address, r, c ) ) )
      && ( ( len.r + len.c ) < strlen( cursor_home ) || cursor_home[0] == '\0' ) ) {
        switch( method.c ) {
        case none:
            break;
        case absolute:
            _putp( tparm( column_address, c ) );
            break;
        case rel_parm_plus:
            _putp( tparm( parm_right_cursor, c - OldCol ) );
            break;
        case relative_plus:
            for( i = 0; i < c - OldCol; i++ ) {
                _putp( cursor_right );
            }
            break;
        case rel_parm_minus:
            _putp( tparm( parm_left_cursor, OldCol - c ));
            break;
        case relative_minus:
            for( i = 0; i < OldCol - c; i++ ) {
                _putp( cursor_left );
            }
            break;
        }

        switch( method.r ) {
        case none:
            break;
        case absolute:
            _putp( tparm( row_address, r ) );
            break;
        case rel_parm_plus:
            _putp( tparm( parm_down_cursor, r - OldRow ) );
            break;
        case relative_plus:
            for( i = 0; i < r - OldRow; i++ ) {
                _putp( cursor_down );
            }
            break;
        case rel_parm_minus:
            _putp( tparm( parm_up_cursor, OldRow - r ) );
            break;
        case relative_minus:
            for( i = 0; i < OldRow - r; i++ ) {
                _putp( cursor_up );
            }
            break;
        }
    } else if( r == 0 && c == 0
      && cursor_home[0] != '\0'
      && strlen( cursor_home ) <= strlen( tparm( cursor_address, r, c ) ) ) {
        _putp( cursor_home );
    } else {
        _putp( tparm( cursor_address, r, c ) );
    }

    OldCol = c;
    OldRow = r;
}

static void TI_SETCOLOUR( int f, int b )
{
    // an array of colour brightnesses
    static char colorpri[] = { 0, 1, 4, 2, 6, 5, 3, 7 };

    if( TCAP_MONOCHROME ) {
        // simulate colour using reverse (this assumes background is
        // darker than foreground).
        if( colorpri[f % 8] < colorpri[b % 8] ) {
QNXDebugPrintf0( "[<enter_reverse_mode-vvvvvvvvvvvv>]" );
QNXDebugPrintf1( "\n%s\n", enter_reverse_mode );
QNXDebugPrintf0( "[<enter_reverse_mode-^^^^^^^^^^^^>]" );
            TIARev = 1;
            TI_FillColourSet = false;
        } else {
            TIARev = 0;
            TI_FillColourSet = true;
        }
        TI_SETATTR();
    } else {
        TI_SETATTR();
        TI_FillColourSet = ( b == 0 ) || back_color_erase;
        // If we can set a colour pair then do so
        if( set_color_pair[0] != '\0' ) {
            _putp( tparm( set_color_pair, f * 10 + b ) );
        } else {
            // else try to set colors individually
            _putp( tparm( set_background, b ) );
            _putp( tparm( set_foreground, f ) );
        }
    }
}

static void TI_SETATTR( void )
{
    // we have to reset attributes as some terminals can't turn off
    // attributes with "set_attribues"
    _putp( exit_attribute_mode );

    if( set_attributes[0] != '\0' ) {
        char    *x;

        _putp( x = tparm( set_attributes,
                0,              // standout
                TIAULine,       // underline
                TIARev,         // reverse
                TIABlink,       // blink
                0,              // half intensity
                TIABold,        // bold
                0,              // invisible
                0,              // protected
                TIAACS ) );     // alt. char set
QNXDebugPrintf0("\n[******]");
QNXDebugPrintf1("%s", set_attributes);
QNXDebugPrintf1("%s",x);
QNXDebugPrintf0("[~~~~~~]\n");
    } else {
        // Believe it or not, some terminals don't have the set_attributes
        // code in the database, so we have to simulate it occasionally
        if( TIAULine )
            _putp( enter_underline_mode );
        if( TIARev )
            _putp( enter_reverse_mode );
        if( TIABlink )
            _putp( enter_blink_mode );
        if( TIABold )
            _putp( enter_bold_mode );
        if( TIAACS ) {
            _putp( enter_alt_charset_mode );
        }
    }
}


static int TI_PUT_FILE( char *fnam )
{
    /* unused parameters */ (void)fnam;

#if 0 //NYI: have to re-implement
    char        c;
    FILE        *fil;

    if( fnam != NULL && fnam[0] != '\0' ) {
        // open file
        fil = ti_fopen( fnam );
        if( fil == NULL )
            return( false );

        // output file to terminal
        while( (c = fgetc( fil )) != EOF ) {
            __putchar( c );
        }
        fclose( fil );
    }
#endif
    return( true );
}

#define TI_PATH_NAME    "/usr/lib/terminfo/?/"
#define TI_PATH_LEN     (sizeof( TI_PATH_NAME ))

static int TI_EXEC_PROG( char *pnam )
{
    int         oStdIn, oStdOut;        // old stdin/stdout
    char        *ppath;                 // program path
    int         ret;                    // return code

    if( pnam != NULL && pnam[0] != '\0' ) {
        // get full path name of program
        ppath = walloca( TI_PATH_LEN + strlen( pnam ) );
        if( ppath == NULL ) {
            return( false );
        }
        strcpy( ppath, TI_PATH_NAME );
        strcat( ppath, pnam );
        ppath[TI_PATH_LEN - 3] = pnam[0]; // replace '?' with first char of fnam

        // record values in qnx_spawn_options
        oStdIn = qnx_spawn_options.iov[0];
        oStdOut = qnx_spawn_options.iov[1];

        // change stdin/out for spawned processes to UIConHandle
        qnx_spawn_options.iov[0] = qnx_spawn_options.iov[1] = UIConHandle;

        // attempt to call pgm in /usr/lib/terminfo/?/
        ret = spawnl( P_WAIT, ppath, NULL );

        if( ret == -1 ) {
            ppath[TI_PATH_LEN - 3] = '\0';
            strcat( ppath, pnam );
            // attempt to call pgm in /usr/lib/terminfo/
            ret = spawnl( P_WAIT, ppath, NULL );
            if( ret == -1 ) {
                // attempt to call pgm in path
                ret = spawnlp( P_WAIT, pnam, NULL );
            }
        }

        // restore values in qnx_spawn_options
        qnx_spawn_options.iov[0] = oStdIn;
        qnx_spawn_options.iov[1] = oStdOut;

        // if program failed...
        if( ret == -1 ) {
            return( false );
        }
    }
    return( true );
}

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


static  LP_PIXEL    shadow;
static  int         save_cursor_type;

static bool setupscrnbuff( uisize srows, uisize scols )
/*****************************************************/
{
    LP_PIXEL    scrn;
    size_t      size;
    size_t      i;
    int         rows;
    int         cols;

    if( dev_size( UIConHandle, -1, -1, &rows, &cols ) == -1 )
        return( false );
    if( rows == 0 ) {
        rows = srows;
        if( rows == 0 ) {
            rows = lines;
            if( rows == 0 ) {
                rows = 25;
            }
        }
    }
    if( cols == 0 ) {
        cols = scols;
        if( cols == 0 ) {
            cols = columns;
            if( columns == 0 ) {
                cols = 80;
            }
        }
    }

    UIData->width = cols;
    UIData->height = rows;
    UIData->cursor_type = C_NORMAL;

    size = UIData->width * UIData->height * sizeof( PIXEL );
    scrn = UIData->screen.origin;
    {
#ifdef _M_I86
        unsigned        seg;

        if( scrn == NULL ) {
            seg = qnx_segment_alloc( size );
        } else {
            seg = qnx_segment_realloc( FP_SEG( scrn ), size );
        }
        if( seg == -1 )
            return( false );
        scrn = MK_FP( seg, 0 );
        if( shadow == NULL ) {
            seg = qnx_segment_alloc( size );
        } else {
            seg = qnx_segment_realloc( FP_SEG( shadow ), size );
        }
        if( seg == -1 ) {
            qnx_segment_free( FP_SEG( scrn ) );
            return( false );
        }
        shadow = MK_FP( seg, 0 );
#else
        scrn = uirealloc( scrn, size );
        if( scrn == NULL )
            return( false );
        if( (shadow = uirealloc( shadow, size )) == NULL ) {
            uifree( scrn );
            return( false );
        }
#endif
    }
    save_cursor_type = -1; /* C_NORMAL; */
    size /= sizeof( PIXEL );
    for( i = 0; i < size; ++i ) {
        scrn[i].ch = ' ';       /* a space with normal attributes */
        scrn[i].attr = 7;       /* a space with normal attributes */
    }
    UIData->screen.origin = scrn;
    UIData->screen.increment = UIData->width;
    return( true );
}

static volatile bool SizePending = false;

static void size_handler( int signo )
/***********************************/
{
    /* unused parameters */ (void)signo;

    SizePending = true;
}


static ui_event td_sizeevent( void )
/**********************************/
{
    SAREA           area;

    if( !SizePending )
        return( EV_NO_EVENT );
    if( !uiinlists( EV_BACKGROUND_RESIZE ) )
        return( EV_NO_EVENT );
    if( !setupscrnbuff( UIData->height, UIData->width ) )
        return( EV_NO_EVENT );
    SizePending = false;
    area.row = 0;
    area.col = 0;
    area.height = UIData->height;
    area.width = UIData->width;
    uidirty( area );
    return( EV_BACKGROUND_RESIZE );
}



#if 0
static bool td_initconsole( void )
/********************************/
{
    if( !ostream_init( UIConHandle ) )
        return( false );
    QNX_NOWRAP();
    QNX_NOBOLD();
    QNX_NOBLINK();
    __flush_con();
    return( true );
}
#endif

static bool td_initconsole( void )
/********************************/
{
    if( !ostream_init( UIConHandle ) )
        return( false );

    // Initialize the term as documented in "Terminal Control Under QNX"
    TI_EXEC_PROG( init_prog );

    TI_INIT1_STRING();
    TI_INIT2_STRING();
    TI_CLEAR_MARGINS();
    TI_PUT_FILE( init_file );
    TI_INIT3_STRING();

    // Now we're initialized, so set term to usable mode:
    TI_CA_ENABLE();
    TI_ENABLE_ACS();

    // disable auto-right-margin
    TI_NOWRAP();
    // if we can't then we just won't use the bottom right corner
    TI_ignore_bottom_right = !TCAP_NOSCROLL;
QNXDebugPrintf1( "IgnoreLowerRight=%d", TI_ignore_bottom_right );

    TI_NOBOLD();
    TI_NOBLINK();

    __flush_con();

    return( true );
}

static bool initmonitor( void )
/*****************************/
{
    UIData->colour = M_VGA;
    /* notify if screen size changes */
    signal( SIGDEV, &size_handler );
    return( true );
}


#if 0
static bool td_init( void )
/*************************/
{
    if( UIData == NULL ) {
        UIData = &ui_data;
    }
    if( !td_initconsole() )
        return( false );

    if( !initmonitor() )
        return( false );
    if( !setupscrnbuff( 0, 0 ) )
        return( false );

    uiinitcursor();
    initkeyboard();
    UIData->mouse_acc_delay = uiclockdelay( 277 /* ms */ );
    UIData->mouse_rpt_delay = uiclockdelay( 55  /* ms */ );
    UIData->mouse_clk_delay = uiclockdelay( 277 /* ms */ );
    UIData->tick_delay      = uiclockdelay( 500 /* ms */ );
    UIData->f10menus        = true;
    td_refresh( true );
    return( true );
}
#endif

static bool td_init( void )
/*************************/
{
    uisize      rows;
    uisize      cols;
    const char  *tmp;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }

    if( !td_initconsole() )
        return( false );

    if( !initmonitor() )
        return( false );

    if( TCAP_MONOCHROME ) {
        UIData->colour = M_TERMINFO_MONO;
    }

    tmp = getenv( "TIOPTIMIZE" );
    if( tmp != NULL )
        OptimizeTerminfo = ( stricmp( tmp, "no" ) != 0 );

    // Figure out the number of columns to use
    cols = 0;
    tmp = getenv( "COLUMNS" );
    if( tmp != NULL )
        cols = strtoul( tmp, NULL, 0 );

    // Figure out the number of rows to use
    rows = 0;
    tmp = getenv( "LINES" );
    if( tmp != NULL )
        rows = strtoul( tmp, NULL, 0 );

    // Set up screen buffer
    if( !setupscrnbuff( rows, cols ) )
        return( false );

    uiinitcursor();
    if( !initkeyboard() )
        return( false );

    UIData->mouse_acc_delay = uiclockdelay( 277 /* ms */ );
    UIData->mouse_rpt_delay = uiclockdelay( 100 /* ms */ );
    UIData->mouse_clk_delay = uiclockdelay( 277 /* ms */ );
    UIData->tick_delay      = uiclockdelay( 500 /* ms */ );
    UIData->f10menus        = true;

    //find point at which repeat chars code becomes efficient
    ti_find_cutoff();

    ti_refresh( true );
    return( true );
}


#if 0
static bool td_fini( void )
/*************************/
{
    QNX_RESTORE_ATTR();
    QNX_HOME();
    QNX_CLS();
    QNX_CURSOR_NORMAL();
    __flush_con();
    finikeyboard();
    uifinicursor();
    return( false );
}
#endif

static bool td_fini( void )
/*************************/
{
    TI_RESTORE_ATTR();
    TI_HOME();
    TI_CLS();
    TI_CURSOR_NORMAL();
    TI_RESTORE_COLOUR();

    TI_RESET1_STRING();
    TI_RESET2_STRING();
    TI_PUT_FILE( reset_file );
    TI_RESET3_STRING();
    __flush_con();

    finikeyboard();
    uifinicursor();
    return( false );
}

/* update the physical screen with contents of virtual copy */


static struct {
    int     row0, col0;
    int     row1, col1;
} dirty_area;


static int td_update( SAREA *area )
{
    if( !area ) {
QNXDebugPrintf0("td_update: no arg");
        dirty_area.row0 = 0;
        dirty_area.col0 = 0;
        dirty_area.row1 = UIData->height;
        dirty_area.col1 = UIData->width;
        return( 0 );
    }
QNXDebugPrintf4("td_update(%d,%d,%d,%d)", area->row, area->col, area->height, area->width);
    if( area->row < dirty_area.row0 ) {
        dirty_area.row0 = area->row;
    }
    if( area->col < dirty_area.col0 ) {
        dirty_area.col0 = area->col;
    }
    if( area->row + area->height > dirty_area.row1 ) {
        dirty_area.row1 = area->row + area->height;
    }
    if( area->col + area->width > dirty_area.col1 ) {
        dirty_area.col1 = area->col + area->width;
    }
    return( 0 );
}


static void td_hwcursor( void )
/*****************************/
{
    switch( UIData->cursor_type ) {
    case C_OFF:
        QNX_CURSOR_OFF();
        break;
    case C_NORMAL:
        QNX_CURSOR_NORMAL();
        break;
    case C_INSERT:
        QNX_CURSOR_BOLD();
        break;
    }
    QNX_CURSOR_MOVE( UIData->cursor_col, UIData->cursor_row );
    __flush_con();
}

static void ti_hwcursor( void )
/*****************************/
{
    // Set cursor to correct visibility
    switch( UIData->cursor_type ) {
    case C_OFF:
        TI_CURSOR_OFF();
        break;
    case C_NORMAL:
        TI_CURSOR_NORMAL();
        break;
    case C_INSERT:
        TI_CURSOR_BOLD();
        break;
    }

    if( UIData->cursor_type == C_OFF ) {
        // Since some terminals can't hide the cursor we'll also
        // move the cursor into an inconspicuous(sp?) location
        TI_CURSOR_MOVE( 0, UIData->height - 1 );
    } else {
        TI_CURSOR_MOVE( UIData->cursor_col, UIData->cursor_row );
    }

    __flush_con();
}


static int td_refresh( bool must )
/********************************/
{
    int             i;
    unsigned        incr;
    LP_PIXEL        bufp, sbufp;

    must |= UserForcedTermRefresh;
    UserForcedTermRefresh = false;

    if( dirty_area.row0 == dirty_area.row1 && dirty_area.col0 == dirty_area.col1 ) {
        td_hwcursor();
        __flush_con();
        return( 0 );
    }

QNXDebugPrintf4("td_refresh (%d,%d)->(%d,%d)", dirty_area.row0, dirty_area.col0, dirty_area.row1, dirty_area.col1);

    if( UIData->cursor_type != C_OFF )
        QNX_CURSOR_OFF();

    bufp = UIData->screen.origin;
    incr = UIData->screen.increment;
    sbufp = shadow;
    bufp += dirty_area.row0 * incr;
    sbufp += dirty_area.row0 * incr;

/*-
* minimize updates
*/
    for( i = dirty_area.row0; i < dirty_area.row1; i++ ) {
        int     j;
        int     lastattr = -1;      // invalid to start
        bool    ca_valid = false;   // is cursor address valid?

        for( j = dirty_area.col0; j < dirty_area.col1; j++ ) {
            if( !must && PIXELEQUAL( bufp[j], sbufp[j] ) ) {
                ca_valid = false;
                lastattr = -1;
                continue;
            }

            if( !ca_valid ) {
QNXDebugPrintf2("cursor address %d,%d\n",j,i);
                QNX_CURSOR_MOVE( j, i );
                ca_valid = true;
            }
            if( bufp[j].attr != lastattr ) {
                lastattr = new_attr( bufp[j].attr, lastattr );
            }
            if( bufp[j].ch < 0x20 )
                __putchar( _ESC_CHAR );
            __putchar( bufp[j].ch );
            sbufp[j] = bufp[j];
        }
        bufp += incr;
        sbufp += incr;
    }
    dirty_area.row0 = dirty_area.row1 = 0;
    dirty_area.col0 = dirty_area.col1 = 0;

    td_hwcursor();
    __flush_con();

    return( 0 );
}

// The following macros are used to take advantage of the repeat_chars
// capability.

// Dumps all characters we've slurped. Will use repeat_char capability if
// there are multiple chars
#define TI_DUMPCHARS()  {TI_REPEAT_CHAR( rchar, rcount, ralt, rcol ); rcount = 0;}

// Slurps a char to be output. Will dump existing chars if new char is
// different.
#define TI_SLURPCHAR( __ch )  \
{                             \
    unsigned char __c = __ch; \
    if( rcount != 0 && ( rchar != ti_char_map[__c] || ralt != ti_alt_map( __c ) ) ) \
        TI_DUMPCHARS();       \
    rcol = (rcount == 0) ? j : rcol; \
    rcount++;                 \
    rchar = ti_char_map[__c]; \
    ralt = ti_alt_map( __c ); \
}


#define NonBlankEnd(b,n,c) (((c).ch == ' ')?(asmNonBlankEnd((b),(n),(c))):(b))


static void update_shadow( void )
/*******************************/
{
    LP_PIXEL    bufp, sbufp;    // buffer and shadow buffer
    unsigned    incr = UIData->screen.increment;

    // make sure cursor is back where it belongs
    ti_hwcursor();
    __flush_con();

    // copy buffer to shadow buffer
    bufp = UIData->screen.origin;
    sbufp = shadow;
    for( ; dirty_area.row0 < dirty_area.row1; dirty_area.row0++ ) {
        _fmemcpy( sbufp + incr * dirty_area.row0 + dirty_area.col0,
                bufp + incr * dirty_area.row0 + dirty_area.col0,
                ( dirty_area.col1 - dirty_area.col0 ) * sizeof( PIXEL ) );
    }

    // set dirty_area rectangle to be empty
    dirty_area.row0 = dirty_area.row1 = dirty_area.col0 = dirty_area.col1 = 0;
}


static int ti_refresh( bool must )
/********************************/
{
    int         i;
    unsigned    incr;               // chars per line
    LP_PIXEL    bufp, sbufp;        // buffer and shadow buffer
    LP_PIXEL    pos;                // the address of the current char
    LP_PIXEL    blankStart;         // start of spaces to eos and then complete
                                    // draw
    int         lastattr = -1;
    int         bufSize;
    LP_PIXEL    bufEnd;
    bool        cls = dirty_area.row1;   // line on which we should clr_eos
                                    // and then continue to draw
    bool        done = false;

    // Need these for startup and the refresh key
    if( UserForcedTermRefresh ) {
        // in the case where a user has forced a refresh we must assume
        // the screen is trashed, and so our cursor position is unknown
        OldCol = -1;
        OldRow = -1;
    }
    must |= UserForcedTermRefresh;
    UserForcedTermRefresh = false;

    // Move the cursor & return if dirty box contains no chars
    if( dirty_area.row0 == dirty_area.row1 && dirty_area.col0 == dirty_area.col1 ) {
        ti_hwcursor();
        __flush_con();
        return( 0 );
    }

QNXDebugPrintf4( "ti_refresh( %d, %d )->( %d, %d )", dirty_area.row0, dirty_area.col0, dirty_area.row1, dirty_area.col1 );

    // Disable cursor during draw if we can
    if( UIData->cursor_type != C_OFF ) {
        TI_CURSOR_OFF();
    }

    // Get our new and old buffers
    bufp = UIData->screen.origin;
    sbufp = shadow;

    // Get some screen statistics
    bufSize = UIData->height * UIData->width;
    bufEnd = &UIData->screen.origin[bufSize - 1];
    incr = UIData->screen.increment;

    // Find a place where we could use clear to end of screen
    if( OptimizeTerminfo ) {
        blankStart = NonBlankEnd( bufEnd, bufSize, *bufEnd ) + 1;
    } else {
        blankStart = bufEnd;
    }

    if( blankStart <= UIData->screen.origin && TCAP_CLS ) {
        // if we could do it at the top then we might as well
        // not bother doing anything else
        lastattr = new_attr( UIData->screen.origin->attr, -1 );
        done = true;
    } else {
        lastattr = -1;

        if( !must ) {
            int         r,c;
            int         pos;
            bool        diff;

            diff = false;
            for( ; dirty_area.col0 < dirty_area.col1; dirty_area.col0++ ) {
                for( r = dirty_area.row0; r < dirty_area.row1; r++ ) {
                    pos = r * incr + dirty_area.col0;
                    if( !PIXELEQUAL( bufp[pos], sbufp[pos] ) ) {
                        diff = true;
                        break;
                    }
                }
                if( diff ) {
                    break;
                }
            }

            diff = false;
            for( ; dirty_area.col0 < dirty_area.col1; dirty_area.col1-- ) {
                for( r = dirty_area.row0; r < dirty_area.row1; r++ ) {
                    pos = r * incr + dirty_area.col1 - 1;
                    if( !PIXELEQUAL( bufp[pos], sbufp[pos] ) ) {
                        diff = true;
                        break;
                    }
                }
                if( diff ) {
                    break;
                }
            }

            diff = false;
            for( ; dirty_area.row0 < dirty_area.row1; dirty_area.row0++ ) {
                for( c = dirty_area.col0; c < dirty_area.col1; c++ ) {
                    pos = dirty_area.row0 * incr + c;
                    if( !PIXELEQUAL( bufp[pos], sbufp[pos] ) ) {
                        diff = true;
                        break;
                    }
                }
                if( diff ) {
                    break;
                }
            }

            diff = false;
            for( ; dirty_area.row0 < dirty_area.row1; dirty_area.row1-- ) {
                for( c = dirty_area.col0; c < dirty_area.col1; c++ ) {
                    pos = ( dirty_area.row1 - 1 ) * incr + c;
                    if( !PIXELEQUAL( bufp[pos], sbufp[pos] ) ) {
                        diff = true;
                        break;
                    }
                }
                if( diff ) {
                    break;
                }
            }
        }

        if( OptimizeTerminfo ) {
            // Set cls if drawing box is bottom part (or whole) of screen
            if( dirty_area.col0 == 0 &&
                dirty_area.row1 == UIData->height &&
                dirty_area.col1 == UIData->width &&
                TCAP_CLS ) {

                if( clr_eos[0] != '\0' ) {
                    cls = dirty_area.row0;
                } else if( dirty_area.row0 == 0 ) {
                    cls = 0;
                }
            }

            if( !must ) {
                // Adjust cls so refresh looks pretty
                for( ; cls < dirty_area.row1; cls++ ) {
                    int         pos;
                    int         pos2;

                    pos = cls * incr;
                    pos2 = pos + UIData->width - 1;
                    if( !PIXELEQUAL( bufp[pos], sbufp[pos] )
                      && ( bufp[pos].ch != sbufp[pos2].ch
                      || bufp[pos].attr == sbufp[pos2].attr) ) {
                        break;
                    }
                }
            }

            /*
            if( cls < dirty_area.row1 ) {
                // If cls is set to by this point we've decided to clear the area
                blankStart = bufEnd;
            }
            */
        }
    }

    if( cls == 0 || ( TI_FillColourSet && blankStart < bufp && TCAP_CLS ) ) {
        // Clear the screen if cls is set to 0 or if the screen
        // is supposed to be blank
        if( cls == 0 ) {
            TI_RESTORE_COLOUR();
        }
        TI_CLS();
    } else {
        // we still have work to do if it turned out we couldn't use the
        // blank start after all
        done = false;
    }

    if( !done ) {
        // If the screen isn't completely blank we have to do some work
        int             j;
        bool            ca_valid;       // is cursor address valid?

        int             rcount;         // repeat count
        char            rchar;          // repeated character
        bool            ralt;           // if repeated character is in acs
        int             rcol;           // starting column of repeated chars

        bufp += dirty_area.row0 * incr;
        sbufp += dirty_area.row0 * incr;

        for( i = dirty_area.row0; i < dirty_area.row1; i++ ) {
            ca_valid = false;
            rcount = 0;

            if( i == cls ) {
                TI_RESTORE_COLOUR();
                TI_CURSOR_MOVE( 0, i );
                _putp( clr_eos );
                ca_valid = true;
                //assert( dirty_area.col0==0 && dirty_area.col1==UIData->width );
            }

            for( j = dirty_area.col0; j < dirty_area.col1; j++ ) {
                pos = &bufp[j];
                if( !must && (
                    ( cls <= i )
                    ? ( bufp[j].ch == ' ' && ( (bufp[j].attr & 0x70) == 0 ) )
                    : ( PIXELEQUAL( bufp[j], sbufp[j] ) && pos <= blankStart ) ) ) {
                    ca_valid = false;
                    continue;
                }

                if( !ca_valid ) {
QNXDebugPrintf2( "cursor address %d, %d\n", j, i );

                    // gotta dump chars before we move
                    TI_DUMPCHARS();
                    TI_CURSOR_MOVE( j, i );
                    ca_valid = true;
                }

                if( bufp[j].attr != lastattr ) {
                    // dump before changing attrs too...
                    TI_DUMPCHARS();
                    lastattr = new_attr( bufp[j].attr, lastattr );
                }

                // Clear to end of screen if we can
                if( pos > blankStart ) {
                    if( TI_FillColourSet ) {
                        // Dump before blank to end of screen...
                        TI_DUMPCHARS();
                        _putp( clr_eos );
                        update_shadow();
                        return( 0 );
                    } else {
                        pos = bufEnd;
                    }
                }

                if( !TI_ignore_bottom_right || (j != UIData->width - 1) || (i != UIData->height - 1) ) {
                    // slurp up the char
                    TI_SLURPCHAR( bufp[j].ch );
                    OldCol++;

                    // if we walk off the edge our position is undefined
                    if( OldCol >= UIData->width ) {
                        OldCol = -1;
                        OldRow = -1;
                    }
                }
            }
            // Make sure we dump any stragglers
            TI_DUMPCHARS();

            bufp += incr;
            sbufp += incr;
        }
    }

    update_shadow();
    return( 0 );
}


static int new_attr( int nattr, int oattr )
{
    union {
        unsigned char   attr;
        attr_bits       bits;
    } nval, oval;

    nval.attr = nattr;
    oval.attr = oattr;
    if( oattr == -1 ) {
        oval.attr = ~nval.attr;
    }
    if( TermIsQNXTerm ) {
        if( _attr_bold( nval ) != _attr_bold( oval ) || _attr_blink( nval ) != _attr_blink( oval ) ) {
            if( _attr_bold( nval ) ) {
                QNX_BOLD();
            } else {
                QNX_NOBOLD();
            }
            if( _attr_blink( nval ) ) {
                QNX_BLINK();
            } else {
                QNX_NOBLINK();
            }
        }
        if( _attr_fore( nval ) != _attr_fore( oval ) || _attr_back( nval ) != _attr_back( oval ) ) {
QNXDebugPrintf2( "colour[%d, %d]\n", _attr_fore( nval ), _attr_back( nval ) );
            QNX_SETCOLOUR( _attr_fore( nval ), _attr_back( nval ) );
        }
    } else {
        if( _attr_bold( nval ) != _attr_bold( oval ) || _attr_blink( nval ) != _attr_blink( oval ) ) {
            TIABold = _attr_bold( nval );
            TIABlink = _attr_blink( nval );
            // Note: the TI_SETCOLOUR below has to set the attributes
            // anyways, so we've just set the flags here
        }

        // if the colours *or* the attributs have changed we have to
        // redo the colours. This is *necessary* for terms like VT's
        // which reset the colour when the attributes are changed
        if( _attr_bold( nval ) != _attr_bold( oval ) ||
            _attr_blink( nval ) != _attr_blink( oval ) ||
            _attr_fore( nval ) != _attr_fore( oval ) ||
            _attr_back( nval ) != _attr_back( oval ) ) {
            TI_SETCOLOUR( _attr_fore( nval ), _attr_back( nval ) );
        }
    }
    return( nattr );
}

static int td_getcur( ORD *row, ORD *col, CURSOR_TYPE *type, CATTR *attr )
/************************************************************************/
{
    *row = UIData->cursor_row;
    *col = UIData->cursor_col;
    *type = UIData->cursor_type;
    *attr = 0;
    return( 0 );
}

static int td_setcur( ORD row, ORD col, CURSOR_TYPE typ, CATTR attr )
/*******************************************************************/
{
    /* unused parameters */ (void)attr;

    if( ( typ != UIData->cursor_type ) ||
        ( row != UIData->cursor_row ) ||
        ( col != UIData->cursor_col ) ) {
        UIData->cursor_type = typ;
        UIData->cursor_row = row;
        UIData->cursor_col = col;
        newcursor();
        if( TermIsQNXTerm ) {
            td_hwcursor();
        } else {
            ti_hwcursor();
        }
    }
    return( 0 );
}


ui_event td_event( void )
{
    ui_event    ui_ev;

    ui_ev = td_sizeevent();
    if( ui_ev > EV_NO_EVENT )
        return( ui_ev );
    /* In a terminal environment we have to go for the keyboard first,
       since that's how the mouse events are coming in */
    ui_ev = tk_keyboardevent();
    if( ui_ev > EV_NO_EVENT ) {
        uihidemouse();
        return( ui_ev );
    }
    return( mouseevent() );
}

Display TermDisplay = {
    td_init,
    td_fini,
    td_update,
    td_refresh,
    td_getcur,
    td_setcur,
    td_event,
};

Display TInfDisplay= {
    td_init,
    td_fini,
    td_update,
    ti_refresh,
    td_getcur,
    td_setcur,
    td_event,
};
