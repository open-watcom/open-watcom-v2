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


#include "variety.h"
#if defined( __OS2__ )
  #define INCL_WIN
  #include <wos2.h>
#endif
#include "win.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "mbdefwin.h"

#define SS_ALT          0x01
#define SS_SHIFT        0x02
#define SS_CAPS         0x04
#define SS_CTRL         0x08

#define KBFSIZE         32
static volatile int     keyTop = 0;
static volatile int     keyBottom = 0;
static unsigned char    charList[KBFSIZE];
static unsigned char    scanList[KBFSIZE];
static int              shiftState = 0;


/*
 * _WindowsKeyUp - process a key up (for shift state)
 */
void _WindowsKeyUp( WORD vk, WORD data )
{
    /* this routine does nothing useful anymore : JBS */
    data = data;
    switch( vk ) {
    case VK_SHIFT:
        shiftState &= ~SS_SHIFT;
        break;
    case VK_CONTROL:
        shiftState &= ~SS_CTRL;
        break;
    }

} /* _WindowsKeyUp */



/*
 * _WindowsKeyPush - handle the press of a key
*/
void _WindowsKeyPush( WORD key, WORD data )
{
    char        scan;
    int         ch;
    BOOL        havekey = TRUE;

#if defined( __OS2__ )
    ch = key;
    scan = data;
#else
    int         char_count;
    #if defined(__NT__)
        WORD    trans_key[3];
    #else
        DWORD   trans_key[3];
    #endif

    scan = LOBYTE( data );
    char_count = 1;
    trans_key[0] = key;

    /* char_count can be -1, 0, 1 or 2 */
    if( char_count <= 0 ) {
        havekey = FALSE;    /* error or no translation for key */
    } else {
        ch = trans_key[0];  /* 1 or 2 characters */
        if( char_count == 2 ) {
            charList[ keyTop ] = ch;
            scanList[ keyTop ] = scan;
            keyTop = (keyTop+1) % KBFSIZE;
            ch = trans_key[1];
        }
    }
#endif
    if( havekey ) {
        charList[ keyTop ] = ch;
        scanList[ keyTop ] = scan;
        keyTop = (keyTop+1) % KBFSIZE;
    }
} /* _WindowsKeyPush */



/*
 * _WindowsVirtualKeyPush - handle the press of a virtual key
*/
void _WindowsVirtualKeyPush( WORD vk, WORD data )
{
    char        scan;
    int         ch;
    BOOL        havekey = TRUE;

#if defined( __OS2__ )
    ch = vk;
#else
    ch = 0;
#endif
    scan = (char) data;

    switch( vk ) {
        case VK_HOME:
        case VK_END:
        case VK_RETURN:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_DELETE:
        case VK_BACK:
        case VK_INSERT:
            ch = 0x80 + vk;
            scan = 0xFF; /* set as a special editing key to _GetString below */
            break;
#ifdef __OS2__
        case VK_SHIFT:
        case VK_CTRL:
        case VK_ALT:
        case VK_ALTGRAF:
        case VK_PAUSE:
        case VK_CAPSLOCK:
        case VK_PRINTSCRN:
        case VK_SCRLLOCK:
        case VK_NUMLOCK:
        case VK_SYSRQ:
            havekey = FALSE;
#endif
        default:
            break;
    }

    if( havekey ) {
        charList[ keyTop ] = ch;
        scanList[ keyTop ] = scan;
        keyTop = (keyTop+1) % KBFSIZE;
    }

} /* _WindowsVirtualKeyPush */



/*
 * _KeyboardHit - test for a waiting key
 */
int _KeyboardHit( BOOL block )
{
    if( keyTop != keyBottom ) return( TRUE );
    if( block ) {
        _BlockingMessageLoop( TRUE );
    } else {
        _MessageLoop( TRUE );
    }
    if( keyTop != keyBottom ) return( TRUE );
    return( FALSE );

} /* _KeyboardHit */

/*
 * _GetKeyboard - get a keyboard result
 */
int _GetKeyboard( int *scan )
{
    int ch;

    ch = (int) charList[ keyBottom ];
    if( scan != NULL ) {
        *scan = (int) scanList[ keyBottom ];
    }
    keyBottom = (keyBottom+1) % KBFSIZE;
    return( ch );

} /* _GetKeyboard */

/*
 * _GetString - read in a string, return the length
 */
int _GetString( LPWDATA w, char *str, int maxbuff )
{
    HWND        hwnd;
    char        ci;
    char        cx;
    int         buff_end = 0;
    int         curr_pos = 0;
    BOOL        escape = FALSE;
    BOOL        insert_flag = FALSE;
    int         maxlen = maxbuff;
    LPSTR       res;
    int         wt;
    int         len;
    int         i;
    int         scan;
#ifdef _MBCS
    char *      p;
    int         expectingTrailByte = 0;
    int         overwrote = 0;
#endif

    #ifdef _MBCS
        res = _MemAlloc( MB_CUR_MAX * (maxbuff+1) );
    #else
        res = _MemAlloc( maxbuff + 1 );
    #endif
    if( res == NULL) return( 0 );

    hwnd = w->hwnd;

    _MoveToLine( w, _GetLastLineNumber( w ), FALSE );
    _NewCursor( w, SMALL_CURSOR );
    _SetInputMode( w, TRUE );
    _GotEOF = FALSE;
    str[0] = 0;

    while( 1 ) {

        w->curr_pos = curr_pos;
        _DisplayCursor( w );
        while( !_KeyboardHit( TRUE ) );
        ci = _GetKeyboard( &scan );
        #if defined( __OS2__ )
            WinShowCursor( hwnd, FALSE );
        #else
            HideCaret( hwnd );
        #endif

        if( escape ) {
            #ifdef _MBCS
                p = __mbsninc( str, curr_pos++ );
                *p = ci;
            #else
                str[ curr_pos++ ] = ci;
            #endif
            escape = FALSE;
        } else if( (ci == CTRL_V) || (scan != 0xFF) ) {
            if( ci == CTRL_V ) {
                escape = TRUE;      /* This is a VI thing - */
                ci = '^';           /* it permits insertion of any key */
            }
            if( insert_flag ) {
                if( buff_end<maxlen && !TOOWIDE(buff_end,w) ) {
                    #ifdef _MBCS
                        if( !expectingTrailByte ) {     /* shift over two bytes */
                            if( _ismbblead(ci) ) {
                                expectingTrailByte = 1;
                                p = __mbsninc( str, curr_pos );
                                for( i=strlen(p); i>=0; i-- )
                                    p[i+2] = p[i];
                                p[0] = ci;
                            } else {                    /* shift over one byte */
                                p = __mbsninc( str, curr_pos );
                                for( i=strlen(p); i>=0; i-- )
                                    p[i+1] = p[i];
                                p[0] = ci;
                            }
                        } else {
                            expectingTrailByte = 0;
                            p = __mbsninc( str, curr_pos );
                            p[1] = ci;
                        }
                        overwrote = 0;
                    #else
                        for(i=buff_end;i>=curr_pos;i--)  str[i+1] = str[i];
                        buff_end++;
                        str[curr_pos] = ci;
                    #endif
                } else
                    continue;
            } else if( curr_pos == buff_end ) {
                if( buff_end < maxlen ) {
                    #ifdef _MBCS
                        if( !expectingTrailByte ) {
                            if( _ismbblead(ci) ) {
                                expectingTrailByte = 1;
                                p = __mbsninc( str, buff_end );
                            } else {
                                p = __mbsninc( str, buff_end );
                            }
                            p[0] = ci;
                            p[1] = p[2] = 0;
                        } else {
                            expectingTrailByte = 0;
                            p = __mbsninc( str, buff_end );
                            p[1] = ci;
                            p[2] = 0;
                        }
                        overwrote = 0;
                    #else
                        str[buff_end++] = ci;
                        str[buff_end] = 0;
                    #endif
                } else
                    continue;
            } else {
                #ifdef _MBCS
                    p = __mbsninc( str, curr_pos );
                    if( !expectingTrailByte ) {
                        if( _ismbblead(ci) ) {
                            expectingTrailByte = 1;
                            if( !_ismbblead(*p) ) {
                                for( i=strlen(p+1); i>=0; i-- )
                                    p[i+2] = p[i+1];    /* shift over one byte */
                            }
                            p[0] = ci;
                        } else {
                            if( _ismbblead(*p) ) {   /* shift over one byte */
                                for( i=1; i<=strlen(p+1); i++ )
                                    p[i] = p[i+1];
                            }
                            p[0] = ci;
                        }
                    } else {
                        expectingTrailByte = 0;
                        p[1] = ci;
                    }
                    overwrote = 1;
                #else
                    str[curr_pos] = ci;
                #endif
            }
            #ifdef _MBCS
                if( !escape && !expectingTrailByte ) {
                    curr_pos++;
                    if( !overwrote )  buff_end++;
                }
            #else
                if( !escape )
                    curr_pos++;
            #endif
        } else {
            cx = ci;
            if( cx >= 0x80 ) cx -= 0x80;
            switch( cx ) {
            case VK_HOME:
                curr_pos = 0;
                break;
            case VK_END:
                curr_pos = buff_end;
                break;
            case VK_RETURN:
            #ifdef __OS2__
                case VK_ENTER:
            #endif
                #ifdef _MBCS
                    p = __mbsninc( str, buff_end );
                    *p = '\0';
                #else
                    str[buff_end] = 0;
                #endif
                _NewCursor( w, ORIGINAL_CURSOR );
                #ifdef _MBCS
                    _UpdateInputLine( w, str, __mbslen(str), TRUE );
                #else
                    _UpdateInputLine( w, str, strlen( str ), TRUE );
                #endif
                _SetInputMode( w, FALSE );
                FARstrcat( res, str );
                FARstrcpy( str, res );
                _MemFree( res );
                /* return number of bytes */
                return( strlen( str ) );
                break;
            case VK_LEFT:
                if( curr_pos > 0 ) curr_pos--;
                break;
            case VK_RIGHT:
                if( curr_pos < buff_end ) curr_pos++;
                break;
            case VK_DELETE:
                if( curr_pos == buff_end )  break;  /* DEL, not BS */
                if( curr_pos < buff_end ) curr_pos++;
                /* fall through to VK_BACK... */
            case VK_BACK:
                if( curr_pos > 0 ) {
                    #ifdef _MBCS
                        p = __mbsninc( str, curr_pos-1 );
                        for( i=curr_pos; i<buff_end; i++ ) {
                            _mbccpy( __mbsninc(p,i-curr_pos),
                                     __mbsninc(p,i-curr_pos+1) );
                        }
                        p = __mbsninc( str, i-1 );
                        *p = '\0';
                    #else
                        for(i=curr_pos;i<=buff_end;i++)  str[i-1]=str[i];
                    #endif
                    buff_end--;
                    curr_pos--;
                }
                break;
            case VK_INSERT:
                if( insert_flag ) {
                    insert_flag = FALSE;
                    _NewCursor( w, SMALL_CURSOR );
                } else {
                    insert_flag = TRUE;
                    _NewCursor( w, FAT_CURSOR );
                }
                break;
            default:
                continue;
            }
        }

        /*
         * update line.  if line was split, then we must reset
         * the current line info.
         */
        #ifdef _MBCS
            wt = _UpdateInputLine( w, str,
                                   expectingTrailByte ? __mbslen(str)-1 : __mbslen(str),
                                   FALSE );
        #else
            wt = _UpdateInputLine( w, str, strlen(str), FALSE );
        #endif

        if( wt >= 0 ) {
            #ifdef _MBCS
                len = __mbslen( str );
                p = __mbsninc( str, len-wt );
                ci = *p;
                *p = '\0';
                FARstrcat( res, str );
                *p = ci;
                for( i=0; i<=wt; i++ )
                    str[i] = str[len-wt + i];
            #else
                len = strlen( str );
                ci = str[ len-wt ];
                str[ len-wt ] = 0;
                FARstrcat( res, str );
                str[ len-wt ] = ci;
                for( i=0;i<=wt;i++ )
                    str[i] = str[len-wt + i];
            #endif
            curr_pos = wt;
            buff_end = wt;
            maxlen -= len+1;
        }
    }

} /* _GetString */
