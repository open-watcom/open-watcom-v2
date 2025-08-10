/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#define __FUNCTION_DATA_ACCESS
#include "variety.h"
#if defined( __OS2__ )
    #define INCL_WIN
    #include <wos2.h>
#else
    #define INCLUDE_COMMDLG_H
    #include <wwindows.h>
#endif
#include "win.h"


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
void _WindowsKeyUp( unsigned vk, unsigned data )
{
    (void)data;

    /* this routine does nothing useful anymore : JBS */
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
void _WindowsKeyPush( unsigned key, unsigned data )
{
    unsigned    scan;
    unsigned    ch;
    bool        havekey;
#if defined(__NT__)
    int         char_count;
    WORD        trans_key[3];
#elif defined(__WINDOWS__)
    int         char_count;
    DWORD       trans_key[3];
#endif

    havekey = true;
    scan = data;
#if defined( __OS2__ )
    ch = key;
#else
    char_count = 1;
    trans_key[0] = key;

    /* char_count can be -1, 0, 1 or 2 */
    if( char_count <= 0 ) {
        havekey = false;    /* error or no translation for key */
    } else {
        ch = trans_key[0];  /* 1 or 2 characters */
        if( char_count == 2 ) {
            charList[keyTop] = ch;
            scanList[keyTop] = scan;
            keyTop = ( keyTop + 1 ) % KBFSIZE;
            ch = trans_key[1];
        }
    }
#endif
    if( havekey ) {
        charList[keyTop] = ch;
        scanList[keyTop] = scan;
        keyTop = ( keyTop + 1 ) % KBFSIZE;
    }
} /* _WindowsKeyPush */



/*
 * _WindowsVirtualKeyPush - handle the press of a virtual key
*/
void _WindowsVirtualKeyPush( unsigned vk, unsigned data )
{
    unsigned    scan;
    unsigned    ch;
    bool        havekey;

    havekey = true;
#if defined( __OS2__ )
    ch = vk;
#else
    ch = 0;
#endif
    scan = data;

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
        scan = 0xFF;    /* set as a special editing key to _GetString below */
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
        havekey = false;
        break;
#endif
    default:
        break;
    }

    if( havekey ) {
        charList[keyTop] = ch;
        scanList[keyTop] = scan;
        keyTop = ( keyTop + 1 ) % KBFSIZE;
    }

} /* _WindowsVirtualKeyPush */



/*
 * _KeyboardHit - test for a waiting key
 */
bool _KeyboardHit( bool block )
{
    if( keyTop != keyBottom )
        return( true );
    if( block ) {
        _BlockingMessageLoop( true );
    } else {
        _MessageLoop( true );
    }
    if( keyTop != keyBottom )
        return( true );
    return( false );

} /* _KeyboardHit */

/*
 * _GetKeyboard - get a keyboard result
 */
int _GetKeyboard( int *scan )
{
    int c;

    c = charList[keyBottom];
    if( scan != NULL ) {
        *scan = scanList[keyBottom];
    }
    keyBottom = ( keyBottom + 1 ) % KBFSIZE;
    return( c );

} /* _GetKeyboard */

/*
 * _GetString - read in a string, return the length
 */
int _GetString( LPWDATA w, char *str, unsigned maxbuff )
{
    HWND        hwnd;
    unsigned    buff_end = 0;
    unsigned    curr_pos = 0;
    bool        escape;
    bool        insert_flag;
    unsigned    maxlen = maxbuff;
    LPSTR       res;
    unsigned    wt;
    unsigned    len;
    unsigned    i;
    int         scan;
    int         ci;
    int         cx;
#ifdef _MBCS
    unsigned char *p;
    bool        expectingTrailByte;
    bool        overwrote;
#endif

    escape = false;
    insert_flag = false;
#ifdef _MBCS
    res = FARmalloc( MB_CUR_MAX * ( maxbuff + 1 ) );
#else
    res = FARmalloc( maxbuff + 1 );
#endif
    if( res == NULL)
        _OutOfMemoryExit();

    hwnd = w->hwnd;

    _MoveToLine( w, _GetLastLineNumber( w ), false );
    _NewCursor( w, SMALL_CURSOR );
    _SetInputMode( w, true );

#ifdef _MBCS
    expectingTrailByte = false;
    overwrote = false;
#endif
    _GotEOF = false;
    str[0] = '\0';
    res[0] = '\0';

    for( ;; ) {
        w->curr_pos = curr_pos;
        _DisplayCursor( w );
        while( !_KeyboardHit( true ) )
            /* empty */;
        ci = _GetKeyboard( &scan );
#if defined( __OS2__ )
        WinShowCursor( hwnd, FALSE );
#else
        HideCaret( hwnd );
#endif

        if( escape ) {
#ifdef _MBCS
            p = __mbsninc( (unsigned char *)str, curr_pos++ );
            *p = ci;
#else
            str[curr_pos++] = ci;
#endif
            escape = false;
        } else if( (ci == CTRL_V)
          || (scan != 0xFF) ) {
            if( ci == CTRL_V ) {
                escape = true;      /* This is a VI thing - */
                ci = '^';           /* it permits insertion of any key */
            }
            if( insert_flag ) {
                if( buff_end < maxlen
                  && !TOOWIDE( buff_end, w ) ) {
#ifdef _MBCS
                    if( !expectingTrailByte ) {     /* shift over two bytes */
                        if( _ismbblead( ci ) ) {
                            expectingTrailByte = true;
                            p = __mbsninc( (unsigned char *)str, curr_pos );
                            for( i = strlen( (char *)p ) + 1; i-- > 0; )
                                p[i + 2] = p[i];
                            p[0] = ci;
                        } else {                    /* shift over one byte */
                            p = __mbsninc( (unsigned char *)str, curr_pos );
                            for( i = strlen( (char *)p ) + 1; i-- > 0; )
                                p[i + 1] = p[i];
                            p[0] = ci;
                        }
                    } else {
                        expectingTrailByte = false;
                        p = __mbsninc( (unsigned char *)str, curr_pos );
                        p[1] = ci;
                    }
                    overwrote = false;
#else
                    for( i = buff_end; i >= curr_pos; i-- )
                        str[i + 1] = str[i];
                    buff_end++;
                    str[curr_pos] = ci;
#endif
                } else {
                    continue;
                }
            } else if( curr_pos == buff_end ) {
                if( buff_end < maxlen ) {
#ifdef _MBCS
                    if( !expectingTrailByte ) {
                        if( _ismbblead( ci ) ) {
                            expectingTrailByte = true;
                            p = __mbsninc( (unsigned char *)str, buff_end );
                        } else {
                            p = __mbsninc( (unsigned char *)str, buff_end );
                        }
                        p[0] = ci;
                        p[1] = p[2] = 0;
                    } else {
                        expectingTrailByte = false;
                        p = __mbsninc( (unsigned char *)str, buff_end );
                        p[1] = ci;
                        p[2] = 0;
                    }
                    overwrote = false;
#else
                    str[buff_end++] = ci;
                    str[buff_end] = '\0';
#endif
                } else {
                    continue;
                }
            } else {
#ifdef _MBCS
                p = __mbsninc( (unsigned char *)str, curr_pos );
                if( !expectingTrailByte ) {
                    if( _ismbblead( ci ) ) {
                        expectingTrailByte = true;
                        if( !_ismbblead( *p ) ) {
                            for( i = strlen( (char *)p ) + 1; i-- > 1; ) {
                                p[i + 1] = p[i];    /* shift over one byte */
                            }
                        }
                        p[0] = ci;
                    } else {
                        if( _ismbblead( *p ) ) {   /* shift over one byte */
                            for( i = 1; i < strlen( (char *)p ); i++ ) {
                                p[i] = p[i + 1];
                            }
                        }
                        p[0] = ci;
                    }
                } else {
                    expectingTrailByte = false;
                    p[1] = ci;
                }
                overwrote = true;
#else
                str[curr_pos] = ci;
#endif
            }
#ifdef _MBCS
            if( !escape
              && !expectingTrailByte ) {
                curr_pos++;
                if( !overwrote ) {
                    buff_end++;
                }
            }
#else
            if( !escape ) {
                curr_pos++;
            }
#endif
        } else {
            cx = ci;
            if( cx >= 0x80 )
                cx -= 0x80;
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
                p = __mbsninc( (unsigned char *)str, buff_end );
                *p = '\0';
#else
                str[buff_end] = '\0';
#endif
                _NewCursor( w, ORIGINAL_CURSOR );
#ifdef _MBCS
                _UpdateInputLine( w, str, __mbslen( (unsigned char *)str ), true );
#else
                _UpdateInputLine( w, str, strlen( str ), true );
#endif
                _SetInputMode( w, false );
                FARstrcat( res, str );
                FARstrcpy( str, res );
                FARfree( res );
                /* return number of bytes */
                return( strlen( str ) );
            case VK_LEFT:
                if( curr_pos > 0 )
                    curr_pos--;
                break;
            case VK_RIGHT:
                if( curr_pos < buff_end )
                    curr_pos++;
                break;
            case VK_DELETE:
                if( curr_pos == buff_end )
                    break;  /* DEL, not BS */
                if( curr_pos < buff_end )
                    curr_pos++;
                /* fall through to VK_BACK... */
            case VK_BACK:
                if( curr_pos > 0 ) {
#ifdef _MBCS
                    p = __mbsninc( (unsigned char *)str, curr_pos - 1 );
                    for( i = curr_pos; i < buff_end; i++ ) {
                        _mbccpy( __mbsninc( p, i - curr_pos ), __mbsninc( p, i - curr_pos + 1 ) );
                    }
                    p = __mbsninc( (unsigned char *)str, i - 1 );
                    *p = '\0';
#else
                    for( i = curr_pos; i <= buff_end; i++ )
                        str[i - 1] = str[i];
#endif
                    buff_end--;
                    curr_pos--;
                }
                break;
            case VK_INSERT:
                if( insert_flag ) {
                    insert_flag = false;
                    _NewCursor( w, SMALL_CURSOR );
                } else {
                    insert_flag = true;
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
        len = __mbslen( (unsigned char *)str );
        wt = _UpdateInputLine( w, str, expectingTrailByte ? len - 1 : len, false );
#else
        len = strlen( str );
        wt = _UpdateInputLine( w, str, len, false );
#endif
        if( (int)wt != -1 ) {
            unsigned    len1;
            unsigned    len2;

            /*
             * len and wt is character count
             * len1 and len2 is byte count
             */
#ifdef _MBCS
            len = __mbslen( (unsigned char *)str );
            len2 = __mbsninc( (unsigned char *)str, len - wt ) - (unsigned char *)str;
#else
            len = strlen( str );
            len2 = len - wt;
#endif
            len1 = FARstrlen( res );
            FARstrncpy( res + len1, str, len2 );
            res[len1 + len2] = '\0';
            strcpy( str, str + len2 );

            curr_pos = wt;
            buff_end = wt;
            maxlen -= len + 1;
        }
    }

} /* _GetString */
