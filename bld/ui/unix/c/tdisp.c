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
* Description:  Terminal display routines.
*
****************************************************************************/


#ifdef AIX
    #define _TPARM_COMPAT
#endif
#if defined(HP) && ( OSVER >= 1100 )
    #define __10_10_COMPAT_CODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#ifndef HP
    #include <termios.h>
#else
    #include <stdarg.h>
#endif
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#ifdef _AIX
    #define _HAS_NO_CHAR_BIT_FIELDS
#endif
#include <ctype.h>

#if defined( __WATCOMC__ )
#elif !defined( HP )
#else
    #define TIOCGWINSZ      _IOR('t', 107, struct winsize) /* get window size */
    struct  winsize {
        unsigned short  ws_row;         /* Rows, in characters          */
        unsigned short  ws_col;         /* Columns, in characters       */
        unsigned short  ws_xpixel;      /* Horizontal size, pixels      */
        unsigned short  ws_ypixel;      /* Vertical size, pixels        */
    };
#endif

#include <curses.h>
#include <term.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include <sys/ioctl.h>

#include "uidef.h"
#include "uiattrs.h"
#include "qdebug.h"

#include "uivirt.h"
#include "qnxuiext.h"
#include "ctkeyb.h"

#include "tixparse.h"
#include "walloca.h"

#ifdef AIX
    struct _bool_struct       _aix_cur_bools;
    struct _bool_struct      *cur_bools = &_aix_cur_bools;
#endif

#if defined(HP) && ( OSVER >= 1100 )
    struct __bool_struct      _hp_cur_bools;
    struct __bool_struct *    __cur_bools     = &_hp_cur_bools;
    //struct __num_struct     _hp_cur_nums;
    //struct __num_struct *   __cur_nums      = &_hp_cur_nums;
    struct __str_struct               _hp_cur_strs;
    struct __str_struct       *       __cur_strs      = &_hp_cur_strs;
#endif

// MACRO for checking if a particular escape sequence exists in terminfo for the
// current terminal
#define _capable_of( sequence )         ( (sequence) != NULL )

#define __flush()       {fflush( UIConFile );}
#if defined( SUN )
#define __putp( str )   {tputs( str, 1, (int (*)(char))_con_putchar );}
#elif defined( HP )  && ( OSVER >= 1100 ) && !defined( __GNUC__ )
#define __putp( str )   {tputs( str, 1, _con_putchar );}
#elif defined( HP )
#define __putp( str )   {tputs( str, 1, (void (*)(int))_con_putchar );}
#elif defined( AIX )
#define __putp( str )   {tputs( str, 1, _con_putchar );}
#else
#define __putp( str )   {tputs( str, 1, _con_putchar );}
#endif
#define __putc( c )     {fputc( c, UIConFile );}

bool    UserForcedTermRefresh= FALSE;

static int _con_putchar( int ch )
{
    fputc( ch, UIConFile );
    return( 0 );
}

#if defined(SUN) && defined(UNIX64)

// Define a tparm interface on sun64 that takes 2 / 3 / 10 arguments.
// Sun64 tparm interface does not allow any default arguments for tparm
#define UNIX_TPARM(str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
            tparm((str), (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))
#define UNIX_TPARM2(str, arg1) tparm( (str), (arg1), 0, 0, 0, 0, 0, 0, 0, 0 )
#define UNIX_TPARM3(str, arg1, arg2) tparm((str), (arg1), (arg2), 0, 0, 0, 0, 0, 0, 0)

#else

// dont need this on other unix platforms
#define UNIX_TPARM(str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
            tparm((str), (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))
#define UNIX_TPARM2(str, arg1) tparm((str), (arg1))
#define UNIX_TPARM3(str, arg1, arg2) tparm((str), (arg1), (arg2))

#endif

bool TInfCheck( void )
/********************/
{
    extern unsigned     UIDisableShiftChanges;

    // Check to see if the term variable is set
    if( GetTermType()[0] != '\0' ) {
        UIDisableShiftChanges = TRUE;
        return( TRUE );
    }
    return( FALSE );
}

/*-
 * set initial mode of term
 */


// Whether to use code-changing optimizations in ti
static bool OptimizeTerminfo= TRUE;

/* TermInfo terminal codes
*/

static void TI_SETATTR( void );

// The following are used to remember which attrs we want
static bool TIABold=    0;      // bold
static bool TIABlink=   0;      // blinking
static bool TIAULine=   0;      // underline
static bool TIARev=     0;      // inverted (reverse)
static bool TIAACS=     0;      // alternate character set

// True if clearing/filling operations will use the current back colour
static bool TI_FillColourSet= FALSE;

// Macros for various terminfo capabilities
#define TI_CURSOR_OFF()         if( _capable_of( cursor_invisible ) ) { \
                                    __putp( cursor_invisible );         \
                                }
#define TI_CURSOR_NORMAL()      if( _capable_of( cursor_normal ) ) {    \
                                    __putp( cursor_normal );            \
                                }
#define TI_CURSOR_BOLD()        if( _capable_of( cursor_visible ) ) {   \
                                    __putp( cursor_visible );           \
                                } else {                                \
                                    TI_CURSOR_NORMAL();                 \
                                }
#define TI_BOLD()       { TIABold= 1; __putp( enter_bold_mode ); }
#define TI_NOBOLD()     { TIABold= 0; TI_SETATTR(); }
#define TI_REVERSE()    { TIARev= 1; TI_SETATTR(); }
#define TI_NOREVERSE()  { TIARev= 0; TI_SETATTR(); }
#define TI_BLINK()      { TIABlink= 1; __putp( enter_blink_mode ); }
#define TI_NOBLINK()    { TIABlink= 0; TI_SETATTR(); }
#define TI_ULINE()      { TIAULine= 1; __putp( enter_underline_mode ); }
#define TI_NOULINE()    { TIAULine= 0; __putp( exit_underline_mode ); }
#define TI_ACS_ON()     { TIAACS= 1; __putp( enter_alt_charset_mode ); }
#define TI_ACS_OFF()    { TIAACS= 0;                                    \
                            if( _capable_of( exit_alt_charset_mode ) ) {\
                                __putp( exit_alt_charset_mode );        \
                            } else {                                    \
                                TI_SETATTR();                           \
                            }                                           \
                        }
#define TI_WRAP()               if( _capable_of( enter_am_mode ) ) {    \
                                    __putp( enter_am_mode );            \
                                }
#define TI_NOWRAP()             if( _capable_of( exit_am_mode ) ) {     \
                                    __putp( exit_am_mode );             \
                                }
#define TI_CA_ENABLE()          if( _capable_of( enter_ca_mode ) ) {    \
                                    __putp( enter_ca_mode );            \
                                }
#define TI_CA_DISABLE()         if( _capable_of( exit_ca_mode ) ) {     \
                                    __putp( exit_ca_mode );             \
                                }
#define TI_KP_ENABLE()          if( _capable_of( keypad_xmit ) ) {    \
                                    __putp( keypad_xmit );            \
                                }
#define TI_KP_DISABLE()         if( _capable_of( keypad_local ) ) {     \
                                    __putp( keypad_local );             \
                                }

#define TI_RESTORE_ATTR()                               \
{                                                       \
    TIAACS= TIABold= TIABlink= TIAULine= 0;             \
    __putp( exit_attribute_mode );                      \
}

#define TI_RESTORE_COLOUR()                     \
{                                               \
    if( _capable_of( orig_pair ) ) {            \
        __putp( orig_pair );                    \
    }                                           \
    else if( _capable_of( orig_colors ) ) {     \
        __putp( orig_colors );                  \
    }                                           \
}

#define TI_ENABLE_ACS()         if( _capable_of( ena_acs ) ) {  \
                                    __putp( ena_acs );          \
                                }

#define TI_HOME()               TI_CURSOR_MOVE( 0, 0 )

// This weird "do...while" thing is so that TI_CLS acts like a statement

#define TI_CLS()                do {                                      \
                                    if( _capable_of( clear_screen ) ) {   \
                                        __putp( clear_screen );           \
                                        OldRow = OldCol = 0;              \
                                    } else if( _capable_of( clr_eos ) ) { \
                                        TI_HOME();                        \
                                        __putp( clr_eos );                \
                                    }                                     \
                                } while( 0 )

#define TI_INIT1_STRING()       __putp( init_1string )
#define TI_INIT2_STRING()       __putp( init_2string )
#define TI_INIT3_STRING()       __putp( init_3string )

#define TI_RESET1_STRING()      __putp( reset_1string )
#define TI_RESET2_STRING()      __putp( reset_2string )
#define TI_RESET3_STRING()      __putp( reset_3string )

#define TI_CLEAR_MARGINS()      __putp( clear_margins )

/* Terminal Capabilities
*/

// True if terminal is monochrome
#define TCAP_MONOCHROME ( !( ( _capable_of( set_background ) && _capable_of( set_foreground ) ) || \
                             ( _capable_of( set_a_background ) && _capable_of( set_a_foreground ) ) || \
                             ( _capable_of( set_color_pair ) ) ) )

// True if terminal won't scroll on us
#define TCAP_NOSCROLL   ( !auto_right_margin || _capable_of( exit_am_mode ) )

// True if terminal can clear screen fast
#define TCAP_CLS        ( ( clear_screen && _capable_of( clear_screen ) ) || \
                                ( clr_eos && _capable_of( clr_eos ) ) )

static unsigned         TI_repeat_cutoff = ~0U;
static bool             TI_ignore_bottom_right = TRUE;

static void TI_SETATTR( void )
/****************************/
{
    // we have to reset attributes as some terminals can't turn off
    // attributes with "set_attribues"
    __putp( exit_attribute_mode );

    if( _capable_of( set_attributes ) ) {
        char    *x;

        // Arguments to tparm
        // standout
        // underline
        // reverse
        // blink
        // half intensity
        // bold
        // invisible
        // protected
        // alt. char set
        x = UNIX_TPARM( set_attributes, 0, \
                        TIAULine, TIARev, \
                        TIABlink, 0, TIABold, \
                        0, 0, TIAACS );
        __putp( x );

        UIDebugPrintf0( "\n[******]" );
        UIDebugPrintf1( "%s", set_attributes );
        UIDebugPrintf1( "%s", x );
        UIDebugPrintf0( "[~~~~~~]\n" );
    } else {
/*
        fprintf(stderr, "Doing attributes %s %s %s %s %s\n", \
                TIAULine ? "Underline" : "", \
                TIARev ? "Reverse" : "", \
                TIABlink ? "Blink" : "", \
                TIABold ? "Bold" : "", \
                TIAACS ? "Alternate CS" : "");
*/

        // Believe it or not, some terminals don't have the set_attributes
        // code in the database, so we have to simulate it occasionally
        if( TIAULine && _capable_of( enter_underline_mode ) )
            __putp( enter_underline_mode );

        if( TIARev && _capable_of( enter_reverse_mode ) )
            __putp( enter_reverse_mode );

        if( TIABlink && _capable_of( enter_blink_mode ) )
            __putp( enter_blink_mode );

        if( TIABold && _capable_of( enter_bold_mode ) )
            __putp( enter_bold_mode );

        if( TIAACS && _capable_of( enter_alt_charset_mode ) ) {
            __putp( enter_alt_charset_mode );
        }
    }
}

/* Find out where using the repeat_char code becomes efficient if we
 * are going to be doing optimizations.
 */
static void ti_find_cutoff( void )
/********************************/
{
    if( _capable_of( repeat_char ) && OptimizeTerminfo ) {
        int     i;
        for( i = 2; i < 80; i++ ) {
            if( strlen( UNIX_TPARM3( repeat_char, 'X', i ) ) < i ) {
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
static void TI_REPEAT_CHAR( char c, int n, int a, ORD x )
/*******************************************************/
{
    bool        blank;
    int         len;
    char        *cparm_right;

    if( n == 0 )
        return;

    blank = OptimizeTerminfo && ( TI_FillColourSet ) && c == ' ' && !a;

    if( blank
      && x == ( UIData->width - n )
      && _capable_of( clr_eol )
      && ( len = strlen( clr_eol ) ) > 0
      && n > len ) {
        __putp( clr_eol );
    } else if( blank
      && x == 0
      && _capable_of( clr_bol )
      && _capable_of( parm_right_cursor )
      && n > (len = ( strlen( cparm_right = UNIX_TPARM2( parm_right_cursor, n ) ) + strlen( clr_bol ) ))
      && len > 0 ) {
        __putp( cparm_right );
        __putp( clr_bol );
    } else {
        if( a ) {
            TI_ACS_ON();
        }

        if( n >= TI_repeat_cutoff && _capable_of( repeat_char ) ) {
            __putp( UNIX_TPARM3( repeat_char, c, n ) );
        } else {
            for( ; n > 0; n-- ) {
                fputc( c, UIConFile );
            }
        }

        if( a ) {
            TI_ACS_OFF();
        }
    }
}

int     OldCol = -1,
        OldRow = -1;

// This macro will pick method "x" for axis "a" if method "x" is faster
// and usable (ie: less chars, but not zero chars)
#define pickMethod(a,x) \
    do {                                                        \
        if( (newLen < len.a || len.a == 0) && newLen != 0 ) {   \
            len.a = newLen;                                     \
            method.a = ( x );                                   \
        }                                                       \
    } while( 0 )

// This macro will set v equal to strlen of function f with parameter p if
// f exists on this terminal.  Sets v equal to zero otherwise.
#define setLenParm(v,f,p) \
    if( _capable_of( (f) ) ) {                      \
        (v) = strlen( UNIX_TPARM2( (f), (p) ) );    \
    } else {                                        \
        (v) = 0;                                    \
    }

// move in the optimal way from (OldCol,OldRow) to (c,r)
static void TI_CURSOR_MOVE( register int c, register int r )
/**********************************************************/
{
    unsigned            newLen;
    int                 i;
    unsigned            len_cursor_address;
    unsigned            len_cursor_home;
    int                 row;
    int                 col;

    struct {
        int     r,c;
    }                   len= { 0, 0 };

    struct {
        enum {
            none,
            absolute,
            rel_parm_plus,
            relative_plus,
            rel_parm_minus,
            relative_minus
        }       r,c;
    }                   method= { none, none };

    // Return if we're in the right place
    if( OldCol == c && OldRow == r ) {
        return;
    }

    // Just use cursor_address if we're not supposed to optimize.
    // If cursor_address isn't supported then use cursor_home,
    // cursor_down and cursor_left.
    if( !OptimizeTerminfo ) {
        if( _capable_of( cursor_address ) ) {
            __putp( UNIX_TPARM3( cursor_address, r, c ) );
        } else if( _capable_of( cursor_home )
          && _capable_of( cursor_down )
          && _capable_of( cursor_right ) ) {
            __putp( cursor_home );
            for( row = 0; row < r; row++ ) {
                __putp( cursor_down );
            }
            for( col = 0; col < c; col++ ) {
                __putp( cursor_right );
            }
        }
        OldRow= r;
        OldCol= c;
        return;
    }

    // If we're not supposed to optimize or if the term is
    // not capable of cursor_address:
    // if OldRow or OldCol <0 then the old position is invalid
    if( OldCol >= 0 && OldRow >= 0 ) {
        if( OldRow != r ) {
            method.r = absolute;
            setLenParm( len.r, row_address, r )

            if( OldRow < r ) {
                setLenParm( newLen, parm_down_cursor, r - OldRow )
                pickMethod( r, rel_parm_plus );

                if( _capable_of( cursor_down ) ) {
                    newLen = ( r - OldRow ) * strlen( cursor_down );
                } else {
                    newLen = 0;
                }
                pickMethod( r, relative_plus );
            } else {
                setLenParm( newLen, parm_up_cursor, OldRow - r )
                pickMethod( r, rel_parm_minus );

                if( _capable_of( cursor_up ) ) {
                    newLen = ( OldRow - r ) * strlen( cursor_up );
                } else {
                    newLen = 0;
                }
                pickMethod( r, relative_minus );
            }
        }
        if( OldCol != c ) {
            method.c = absolute;
            setLenParm( len.c, column_address, c )

            if( OldCol < c ) {
                setLenParm( newLen, parm_right_cursor, c - OldCol )
                pickMethod( c, rel_parm_plus );

                if( _capable_of( cursor_right ) ) {
                    newLen = ( c - OldCol ) * strlen( cursor_right );
                } else {
                    newLen = 0;
                }
                pickMethod( c, relative_plus );
            } else {
                setLenParm( newLen, parm_left_cursor, OldCol - c )
                pickMethod( c, rel_parm_minus );

                if( _capable_of( cursor_left ) ) {
                    newLen = ( OldCol - c ) * strlen( cursor_left );
                } else {
                    newLen = 0;
                }
                pickMethod( c, relative_minus );
            }
        }
    }

    // check to make sure the method we have so far is valid
    if( _capable_of( cursor_address ) ) {
        len_cursor_address = strlen( UNIX_TPARM3( cursor_address, r, c ) );
    } else {
        len_cursor_address = 0;
    }

    if( _capable_of( cursor_home ) ) {
        len_cursor_home = strlen( cursor_home );
    } else {
        len_cursor_home = 0;
    }

    // Apply the optimized method if one has been chosen
    if( ( OldRow >= 0 && OldCol >= 0 )
      && ( len.r != 0 || method.r == none )
      && ( len.c != 0 || method.c == none )
      && ( ( len.r + len.c ) < len_cursor_address )
      && ( ( len.r + len.c ) < len_cursor_home ) ) {
        switch( method.c ) {
        case none:
            break;
        case absolute:
            __putp( UNIX_TPARM2( column_address, c ) );
            break;
        case rel_parm_plus:
            __putp( UNIX_TPARM2( parm_right_cursor, c - OldCol ) );
            break;
        case relative_plus:
            for( i = 0; i < c - OldCol; i++ ) {
                __putp( cursor_right );
            }
            break;
        case rel_parm_minus:
            __putp( UNIX_TPARM2( parm_left_cursor, OldCol - c ) );
            break;
        case relative_minus:
            for( i = 0; i < OldCol - c; i++ ) {
                __putp( cursor_left );
            }
            break;
        }

        switch( method.r ) {
        case none:
            break;
        case absolute:
            __putp( UNIX_TPARM2( row_address, r ) );
            break;
        case rel_parm_plus:
            __putp( UNIX_TPARM2( parm_down_cursor, r-OldRow ));
            break;
        case relative_plus:
            for( i = 0; i < r - OldRow; i++ ) {
                __putp( cursor_down );
            }
            break;
        case rel_parm_minus:
            __putp( UNIX_TPARM2( parm_up_cursor, OldRow-r ));
            break;
        case relative_minus:
            for( i = 0; i < OldRow - r; i++ ) {
                __putp( cursor_up );
            }
            break;
        }
    } else if( r == 0
      && c == 0
      && _capable_of( cursor_home )
      && _capable_of( cursor_address )
      && strlen( cursor_home ) <= strlen( UNIX_TPARM3( cursor_address, r, c ) ) ) {
        __putp( cursor_home );
    } else if( _capable_of( cursor_address ) ) {
        __putp( UNIX_TPARM3( cursor_address, r, c ) );
    }

    OldCol = c;
    OldRow = r;
}

static void TI_SETCOLOUR( register int f, register int b )
/********************************************************/
{
    // an array of colour brightnesses
    static int  colorpri[]={ 0, 1, 4, 2, 6, 5, 3, 7 };
    // vga to ansi conversion table
    static int  colorans[]={ 0, 4, 2, 6, 1, 5, 3, 7 };

    UIDebugPrintf2( "TI_SETCOLOUR: %d %d", f, b );

    if( TCAP_MONOCHROME ) {
        // simulate colour using reverse (this assumes background is
        // darker than foreground).
        if( colorpri[f % 8] < colorpri[b % 8] ) {
            UIDebugPrintf0( "[<enter_reverse_mode-vvvvvvvvvvvv>]" );
            UIDebugPrintf1( "\n%s\n", enter_reverse_mode );
            UIDebugPrintf0( "[<enter_reverse_mode-^^^^^^^^^^^^>]" );
            TIARev = 1;
            TI_FillColourSet = FALSE;
        } else {
            TIARev = 0;
            TI_FillColourSet = TRUE;
        }
        TI_SETATTR();
    } else {
        TI_SETATTR();
        TI_FillColourSet = ( b == 0 ) || back_color_erase;
        // If we can set a colour pair then do so
        if( _capable_of( set_a_foreground ) && _capable_of( set_a_background ) ) {
            __putp( UNIX_TPARM2( set_a_foreground, colorans[f] ) );
            __putp( UNIX_TPARM2( set_a_background, colorans[b] ) );
        } else if( _capable_of( set_foreground ) && _capable_of( set_background ) ) {
            __putp( UNIX_TPARM2( set_foreground, colorans[f] ) );
            __putp( UNIX_TPARM2( set_background, colorans[b] ) );
        } else if( _capable_of( set_color_pair ) ) {
            __putp( UNIX_TPARM2( set_color_pair, colorans[f] * 10 + colorans[b] ) );
        }
    }
}


static int TI_PUT_FILE( const char *fnam )
/****************************************/
{
    char        c;
    FILE        *fil;

    if( fnam != NULL && fnam[0] != '\0' ) {
        // open file
        fil = ti_fopen( fnam );
        if( fil == NULL )
            return( FALSE );

        // output file to terminal
        while( feof( fil ) == 0 ) {
            c = fgetc( fil );
            putchar( c );
        }
        fclose( fil );
    }
    return( TRUE );
}

#define TI_PATH_NAME    "/usr/lib/terminfo/?/"
#define TI_PATH_LEN     (sizeof( TI_PATH_NAME ))

static int TI_EXEC_PROG( char *pnam )
/***********************************/
{
    char        *ppath;                 // program path
    int         ret;                    // return code

    if( pnam != NULL && pnam[0] != '\0' ) {
        // get full path name of program
        ppath = (char *)alloca( TI_PATH_LEN + strlen( pnam ) );
        if( ppath == NULL ) {
            return( FALSE );
        }
        strcpy( ppath, TI_PATH_NAME );
        strcat( ppath, pnam );
        ppath[TI_PATH_LEN - 3] = pnam[0]; // replace '?' with first char of pnam

#ifdef __WATCOMC__
        // attempt to call pgm in /usr/lib/terminfo/?/
        ret= spawnl( P_WAIT, ppath, NULL );

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
#else
        ret = -1;
#endif

        // if program failed...
        if( ret == -1 ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

static MONITOR  ui_data = {
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


static  PIXEL _FAR *shadow;
static  int   save_cursor_type;

static bool setupscrnbuff( int rows, int cols )
/*********************************************/
{
    PIXEL               *scrn;
    int                 num;
    int                 i;
    struct winsize      size;

    // if either of the passed in rows or columns are 0 we can assume
    // they're invalid and try to find out what it is ourself.
    if( isatty( UIConHandle ) ) {
        if( ioctl( UIConHandle, TIOCGWINSZ, &size ) < 0 ) {
            if( rows == 0 || cols == 0 ) {
                return( FALSE );
            }
            // otherwise, use the defaults passed in
        } else {
            if( size.ws_row != 0 && size.ws_col != 0 ) {
                // Under EMACS gdb, zero is returned for rows and cols
                rows = size.ws_row;
                cols = size.ws_col;
            }
        }
    }

    UIData->width = cols;
    UIData->height = rows;
    UIData->cursor_type = C_NORMAL;

    num = UIData->width * UIData->height * sizeof( PIXEL );
    scrn = UIData->screen.origin;
    scrn = uirealloc( scrn, num );

    if( scrn == NULL )
        return( FALSE );
    if( (shadow = uirealloc( shadow, num )) == 0 ) {
        uifree( scrn );
        return( FALSE );
    }
    save_cursor_type = -1; /* C_NORMAL; */
    num /= sizeof( PIXEL );
    for( i = 0; i < num; ++i ) {
        scrn[i].ch = ' ';       /* a space with normal attributes */
        scrn[i].attr = 7;       /* a space with normal attributes */
    }
    UIData->screen.origin = scrn;
    UIData->screen.increment = UIData->width;
    return( TRUE );
}

static volatile int SizePending;

static void size_handler( int signo )
/***********************************/
{
    signo = signo;
    SizePending = 1;
}


static EVENT td_sizeevent( void )
/*******************************/
{
    SAREA           area;

    if( !SizePending )
        return( EV_NO_EVENT );
    if( !uiinlist( EV_BACKGROUND_RESIZE ) )
        return( EV_NO_EVENT );
    if( !setupscrnbuff( 0, 0 ) )
        return( EV_NO_EVENT );
    SizePending = 0;
    area.row = 0;
    area.col = 0;
    area.height = UIData->height;
    area.width = UIData->width;
    uidirty( area );
    return( EV_BACKGROUND_RESIZE );
}

static bool ti_initconsole( void )
/********************************/
{
    // Initialize the term as documented in "Terminal Control Under QNX"
    TI_EXEC_PROG( init_prog );

    TI_INIT1_STRING();
    TI_INIT2_STRING();
    TI_CLEAR_MARGINS();
    TI_PUT_FILE( init_file );
    TI_INIT3_STRING();

    // Now we're initialized, so set term to usable mode:
    TI_CA_ENABLE();
    TI_KP_ENABLE();
    TI_ENABLE_ACS();

    // disable auto-right-margin
    TI_NOWRAP();
    // if we can't then we just won't use the bottom right corner
    TI_ignore_bottom_right = !TCAP_NOSCROLL;
    UIDebugPrintf1( "IgnoreLowerRight=%d", TI_ignore_bottom_right );

    TI_NOBOLD();
    TI_NOBLINK();

    __flush();

    return( TRUE );
}

bool intern initmonitor( void )
/*****************************/
{
    struct sigaction sa;

    UIData->colour = M_VGA;

    sa.sa_handler               = size_handler;
#ifndef SA_RESTART
#define SA_RESTART 0
#endif
    sa.sa_flags                 = SA_RESTART;
    memset( &sa.sa_mask, '\0', sizeof( sa.sa_mask ) );

    sigaction( SIGWINCH, &sa, NULL );

    return( TRUE );
}

static int new_attr(int nattr, int oattr)
/***************************************/
{
        union {
                unsigned char   attr;
                struct {
#if defined( _HAS_NO_CHAR_BIT_FIELDS )
                    unsigned char   blink_back_bold_fore;
    #define _attr_blink( a ) (((a).blink_back_bold_fore >> 7) & 1)
    #define _attr_back( a )  (((a).blink_back_bold_fore >> 4) & 7)
    #define _attr_bold( a )  (((a).blink_back_bold_fore >> 3) & 1)
    #define _attr_fore( a )  ( (a).blink_back_bold_fore       & 7)
#else
    #if defined( __BIG_ENDIAN__ )
                    unsigned char   blink:1;
                    unsigned char   back:3;
                    unsigned char   bold:1;
                    unsigned char   fore:3;
    #else
                    unsigned char   fore:3;
                    unsigned char   bold:1;
                    unsigned char   back:3;
                    unsigned char   blink:1;
    #endif
    #define _attr_blink( a ) ((a).blink)
    #define _attr_back( a )  ((a).back)
    #define _attr_bold( a )  ((a).bold)
    #define _attr_fore( a )  ((a).fore)
#endif
                } bits;
        } nval, oval;
        nval.attr = nattr;
        oval.attr = oattr;

        if( oattr == -1 ) {
            oval.attr = ~nval.attr;
        }
        if( _attr_bold( nval.bits ) != _attr_bold( oval.bits )
          || _attr_blink( nval.bits ) != _attr_blink( oval.bits ) ) {
            TIABold  = _attr_bold( nval.bits );
            TIABlink = _attr_blink( nval.bits );
            // Note: the TI_SETCOLOUR below has to set the attributes
            // anyways, so we've just set the flags here
        }

        // if the colours *or* the attributs have changed we have to
        // redo the colours. This is *necessary* for terms like VT's
        // which reset the colour when the attributes are changed
        if( nval.attr != oval.attr ) {
            TI_SETCOLOUR( _attr_fore( nval.bits ), _attr_back( nval.bits ) );
        }
        return( nattr );
}

static int ti_refresh( int must );

static int ti_init( void )
/************************/
{
    int         rows, cols;
    char        *tmp;

    if( UIData == NULL ) {
        UIData = &ui_data;
    }

    if( !ti_initconsole() )
        return( FALSE );

    if( !initmonitor() )
        return( FALSE );
    if( !initkeyboard() )
        return( FALSE );

    if( TCAP_MONOCHROME ) {
        UIData->colour = M_TERMINFO_MONO;
    }

    UIData->no_blowup = TRUE;

    tmp = getenv( "TIOPTIMIZE" );
    if( tmp != NULL ) {
        OptimizeTerminfo = ( strcasecmp( tmp, "no" ) != 0 );
    }

    // Figure out the number of columns to use
    cols = columns;
    tmp = getenv( "COLUMNS" );
    if( tmp != NULL ) {
        cols = atoi( tmp );
    }

    // Figure out the number of rows to use
    rows = lines;
    tmp = getenv( "LINES" );
    if( tmp != NULL ) {
        rows= atoi( tmp );
    }

    // Set up screen buffer
    if( !setupscrnbuff( rows, cols ) )
        return( FALSE );

    uiinitcursor();

    UIData->mouse_acc_delay = 277;
    UIData->mouse_rpt_delay = 55;
    UIData->mouse_clk_delay = 277;
    UIData->tick_delay = 500;
    UIData->f10menus = TRUE;

    //find point at which repeat chars code becomes efficient
    ti_find_cutoff();

    ti_refresh( 1 );
    return( TRUE );
}

static int ti_fini( void )
/************************/
{
    TI_RESTORE_ATTR();
    TI_HOME();
    TI_CLS();
    TI_CURSOR_NORMAL();
    TI_RESTORE_COLOUR();
    TI_ACS_OFF();
    TI_CA_DISABLE();
    TI_KP_DISABLE();

    TI_PUT_FILE( reset_file );
    __flush();

    finikeyboard();
    uifinicursor();
    return( 0 );
}

/* update the physical screen with contents of virtual copy */


static struct {
    int     row0, col0;
    int     row1, col1;
} dirty;


static int td_update( SAREA *area )
{
    if( !area ) {
        UIDebugPrintf0( "td_update: no arg" );
        dirty.row0 = 0;
        dirty.col0 = 0;
        dirty.row1 = UIData->height;
        dirty.col1 = UIData->width;
        return( 0 );
    }
    UIDebugPrintf4( "td_update(%d,%d,%d,%d)", area->row, area->col, area->height, area->width );
    if( area->row < dirty.row0 ) {
        dirty.row0 = area->row;
    }
    if( area->col < dirty.col0 ) {
        dirty.col0 = area->col;
    }
    if( area->row + area->height > dirty.row1 ) {
        dirty.row1 = area->row + area->height;
    }
    if( area->col + area->width > dirty.col1 ) {
        dirty.col1 = area->col + area->width;
    }
    return( 0 );
}

static int ti_hwcursor( void )
/****************************/
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

    __flush();
    return( 0 );
}

// The following macros are used to take advantage of the repeat_chars
// capability.

// Dumps all characters we've slurped. Will use repeat_char capability if
// there are multiple chars
#define TI_DUMPCHARS()  {TI_REPEAT_CHAR( rchar, rcount, ralt, rcol );\
                        rcount = 0;}

static void update_shadow( void )
/*******************************/
{
    LP_PIXEL    bufp, sbufp;    // buffer and shadow buffer
    int         incr = UIData->screen.increment;

    // make sure cursor is back where it belongs
    ti_hwcursor();
    __flush();

    // copy buffer to shadow buffer
    bufp = UIData->screen.origin;
    sbufp = shadow;

    for( ; dirty.row0 < dirty.row1; dirty.row0++ ) {
        memcpy( sbufp + incr * dirty.row0 + dirty.col0,
                bufp + incr * dirty.row0 + dirty.col0,
                ( dirty.col1 - dirty.col0 ) * sizeof( PIXEL ) );
    }

    // set dirty rectangle to be empty
    dirty.row0 = dirty.row1 = dirty.col0 = dirty.col1 = 0;

    return;
}

static int ti_refresh( int must )
/*******************************/
{
    int         i;
    int         incr;           // chars per line
    LP_PIXEL    bufp, sbufp;    // buffer and shadow buffer
    LP_PIXEL    pos;            // the address of the current char
    LP_PIXEL    blankStart;     // start of spaces to eos and then complete
                                // draw
    int         lastattr = -1;
    int         bufSize;
    LP_PIXEL    bufEnd;
    int         cls = dirty.row1;// line on which we should clr_eos
                                 // and then continue to draw
    bool        done = FALSE;


    // Need these for startup and the refresh key
    if( UserForcedTermRefresh ) {
        // in the case where a user has forced a refresh we must assume
        // the screen is trashed, and so our cursor position is unknown
        OldCol = -1;
        OldRow = -1;
    }
    must |= UserForcedTermRefresh;
    UserForcedTermRefresh = FALSE;

    // Move the cursor & return if dirty box contains no chars
    if( dirty.row0 == dirty.row1 && dirty.col0 == dirty.col1 ) {
        ti_hwcursor();
        __flush();
        return( 0 );
    }

    UIDebugPrintf4( "ti_refresh( %d, %d )->( %d, %d )", dirty.row0,
                                    dirty.col0, dirty.row1, dirty.col1 );

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
    blankStart = bufEnd;

    if( blankStart <= UIData->screen.origin && TCAP_CLS ) {
        // if we could do it at the top then we might as well
        // not bother doing anything else
        lastattr = new_attr( UIData->screen.origin->attr, -1 );
        done = TRUE;
    } else {
        lastattr = -1;

        if( !must ) {
            int     r,c;
            int     pos;
            bool    diff = FALSE;

            while( dirty.col0<dirty.col1 ) {
                for( r = dirty.row0; r < dirty.row1; r++ ) {
                    pos= r * incr + dirty.col0;
                    if( bufp[pos].ch != sbufp[pos].ch
                      || bufp[pos].attr != sbufp[pos].attr ) {
                        diff = TRUE;
                        break;
                    }
                }
                if( diff )
                    break;
                dirty.col0++;
            }

            diff = FALSE;
            while( dirty.col0<dirty.col1 ) {
                for( r = dirty.row0; r < dirty.row1; r++ ) {
                    pos = r * incr + dirty.col1 - 1;
                    if( bufp[pos].ch != sbufp[pos].ch
                      || bufp[pos].attr != sbufp[pos].attr ) {
                        diff= TRUE;
                        break;
                    }
                }
                if( diff )
                    break;
                dirty.col1--;
            }

            diff = FALSE;
            while( dirty.row0 < dirty.row1 ) {
                for( c = dirty.col0; c < dirty.col1; c++ ) {
                    pos = dirty.row0 * incr + c;
                    if( bufp[pos].ch != sbufp[pos].ch
                      || bufp[pos].attr != sbufp[pos].attr ) {
                        diff = TRUE;
                        break;
                    }
                }
                if( diff )
                    break;
                dirty.row0++;
            }

            diff = FALSE;
            while( dirty.row0 < dirty.row1 ) {
                for( c = dirty.col0; c < dirty.col1; c++ ) {
                    pos = ( dirty.row1 - 1 ) * incr + c;
                    if( bufp[pos].ch != sbufp[pos].ch
                      || bufp[pos].attr != sbufp[pos].attr ) {
                        diff = TRUE;
                        break;
                    }
                }
                if( diff )
                    break;
                dirty.row1--;
            }
        }

        if( OptimizeTerminfo ) {
            // Set cls if drawing box is bottom part (or whole) of screen
            if( dirty.col0 == 0 &&
                dirty.row1 == UIData->height &&
                dirty.col1 == UIData->width &&
                TCAP_CLS ) {

                if( _capable_of( clr_eos ) ) {
                    cls = dirty.row0;
                } else if( dirty.row0 == 0 ) {
                    cls = 0;
                }
            }

            if( !must ) {
                // Adjust cls so refresh looks pretty
                for( ; cls < dirty.row1; cls++ ) {
                    int         pos;
                    int         pos2;

                    pos = cls * incr;
                    pos2 = pos + UIData->width - 1;
                    if( ( bufp[pos].ch != sbufp[pos].ch ||
                        bufp[pos].attr != sbufp[pos].attr ) &&
                        ( bufp[pos].ch != sbufp[pos2].ch ||
                        bufp[pos].attr == sbufp[pos2].attr ) ) {
                        break;
                    }
                }
            }
            /*
            if( cls<dirty.row1 ) {
                // If cls is set to by this point we've decided to clear the area
                blankStart= bufEnd;
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
        done = FALSE;
    }

    if( !done ) {
        // If the screen isn't completely blank we have to do some work
        int             j;
        bool            ca_valid;       // is cursor address valid?

        int             rcount;         // repeat count
        char            rchar = 0;      // repeated character
        int             ralt = 0;       // if repeated character is in acs
        int             rcol = 0;       // starting column of repeated chars

        bufp += dirty.row0 * incr;
        sbufp += dirty.row0 * incr;

        for( i = dirty.row0; i < dirty.row1; i++ ) {
            ca_valid = FALSE;
            rcount = 0;

            if( i == cls ) {
                TI_RESTORE_COLOUR();
                TI_CURSOR_MOVE( 0, i );
                __putp( clr_eos );
                ca_valid = TRUE;
                //assert( dirty.col0==0 && dirty.col1==UIData->width );
            }

            for( j = dirty.col0; j < dirty.col1; j++ ) {
                pos = &bufp[j];

                if( !must && (
                    ( cls <= i )
                    ? ( bufp[j].ch == ' ' && ( (bufp[j].attr & 112) == 0) )
                    : ( bufp[j].ch == sbufp[j].ch
                        && bufp[j].attr == sbufp[j].attr
                        && pos <= blankStart ) ) ) {
                    ca_valid = FALSE;
                    continue;
                }

                if( !ca_valid ) {
                    UIDebugPrintf2( "cursor address %d, %d\n", j, i );

                    // gotta dump chars before we move
                    TI_DUMPCHARS();
                    TI_CURSOR_MOVE( j, i );
                    ca_valid = TRUE;
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
                        __putp( clr_eos );
                        update_shadow();
                        return( 0 );
                    } else {
                        pos = bufEnd;
                    }
                }

                if( !TI_ignore_bottom_right || ( j != UIData->width - 1 )
                                            || ( i != UIData->height - 1 ) ) {
                    // Slurp up the char to be output. Will dump existing
                    // chars if new char is different.
                    unsigned c = bufp[j].ch;
                    if( rcount != 0 && ( rchar != ti_char_map[c][0] || ralt != ti_alt_map( c ) ) )
                         TI_DUMPCHARS();
                    rcol = ( rcount == 0 ) ? j : rcol;
                    rcount++;
                    if( ti_char_map[c][1] ) {
                         /* a UTF-8 string: write it immediately, 1-byte repeats unlikely */
                         fputs( ti_char_map[c], UIConFile );
                         rcount = 0;
                    }
                    rchar = ti_char_map[c][0];
                    ralt = ti_alt_map( c );
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

static int td_getcur( ORD *row, ORD *col, int *type, int *attr )
/**************************************************************/
{
    *row = UIData->cursor_row;
    *col = UIData->cursor_col;
    *type = UIData->cursor_type;
    *attr = 0;
    return( 0 );
}

static int td_setcur( ORD row, ORD col, int typ, int attr )
/*********************************************************/
{
    attr = attr;

    if( ( typ != UIData->cursor_type ) ||
        ( row != UIData->cursor_row ) ||
        ( col != UIData->cursor_col ) ) {
        UIData->cursor_type = typ;
        UIData->cursor_row = row;
        UIData->cursor_col = col;
        newcursor();
        ti_hwcursor();
    }
    return( 0 );
}


EVENT td_event( void )
{
    EVENT       ev;

    ev = td_sizeevent();
    if( ev > EV_NO_EVENT )
        return( ev );
    /* In a terminal environment we have to go for the keyboard first,
       since that's how the mouse events are coming in */
    ev = tk_keyboardevent();
    if( ev > EV_NO_EVENT ) {
         uihidemouse();
         return( ev );
    }
    return( mouseevent() );
}

Display TInfDisplay = {
    ti_init,
    ti_fini,
    td_update,
    ti_refresh,
    td_getcur,
    td_setcur,
    td_event,
};
