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


#include <windows.h>
#include <ctype.h>
#include "string.h"
#include "wglbl.h"
#include "wvk2str.h"
#include "wresall.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WVKey {
    uint_16      id;
    char        *str;
} WVKey;

typedef struct WShiftMapKeys {
    char  c;
    char  shift_c;
} WShiftMapKeys;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static char     *WGetVKText             ( uint_16, uint_16 );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WShiftMapKeys WShiftMap[] =
{
    { '\`' , '\~' }
,   { '1'  , '\!' }
,   { '2'  , '\@' }
,   { '3'  , '\#' }
,   { '4'  , '\$' }
,   { '5'  , '\%' }
,   { '6'  , '\^' }
,   { '7'  , '\&' }
,   { '8'  , '\*' }
,   { '9'  , '\(' }
,   { '0'  , '\)' }
,   { '\-' , '\_' }
,   { '\=' , '\+' }
,   { '\[' , '\{' }
,   { '\]' , '\}' }
,   { '\\' , '\|' }
,   { '\;' , '\:' }
,   { '\'' , '\"' }
,   { '\,' , '\<' }
,   { '\.' , '\>' }
,   { '\/' , '\?' }
,   {   0  ,   0  }
};

static WVKey WVKeys[] =
{
    { VK_LBUTTON   , "VK_LBUTTON"    }
,   { VK_RBUTTON   , "VK_RBUTTON"    }
,   { VK_CANCEL    , "VK_CANCEL"     }
,   { VK_MBUTTON   , "VK_MBUTTON"    }
,   { VK_BACK      , "VK_BACK"       }
,   { VK_TAB       , "VK_TAB"        }
,   { VK_CLEAR     , "VK_CLEAR"      }
,   { VK_RETURN    , "VK_RETURN"     }
,   { VK_SHIFT     , "VK_SHIFT"      }
,   { VK_CONTROL   , "VK_CONTROL"    }
,   { VK_MENU      , "VK_MENU"       }
,   { VK_PAUSE     , "VK_PAUSE"      }
,   { VK_CAPITAL   , "VK_CAPITAL"    }
,   { VK_ESCAPE    , "VK_ESCAPE"     }
,   { VK_SPACE     , "VK_SPACE"      }
,   { VK_PRIOR     , "VK_PRIOR"      }
,   { VK_NEXT      , "VK_NEXT"       }
,   { VK_END       , "VK_END"        }
,   { VK_HOME      , "VK_HOME"       }
,   { VK_LEFT      , "VK_LEFT"       }
,   { VK_UP        , "VK_UP"         }
,   { VK_RIGHT     , "VK_RIGHT"      }
,   { VK_DOWN      , "VK_DOWN"       }
,   { VK_SELECT    , "VK_SELECT"     }
,   { VK_PRINT     , "VK_PRINT"      }
,   { VK_EXECUTE   , "VK_EXECUTE"    }
,   { VK_SNAPSHOT  , "VK_SNAPSHOT"   }
,   { VK_INSERT    , "VK_INSERT"     }
,   { VK_DELETE    , "VK_DELETE"     }
,   { VK_HELP      , "VK_HELP"       }
,   { VK_NUMPAD0   , "VK_NUMPAD0"    }
,   { VK_NUMPAD1   , "VK_NUMPAD1"    }
,   { VK_NUMPAD2   , "VK_NUMPAD2"    }
,   { VK_NUMPAD3   , "VK_NUMPAD3"    }
,   { VK_NUMPAD4   , "VK_NUMPAD4"    }
,   { VK_NUMPAD5   , "VK_NUMPAD5"    }
,   { VK_NUMPAD6   , "VK_NUMPAD6"    }
,   { VK_NUMPAD7   , "VK_NUMPAD7"    }
,   { VK_NUMPAD8   , "VK_NUMPAD8"    }
,   { VK_NUMPAD9   , "VK_NUMPAD9"    }
,   { VK_MULTIPLY  , "VK_MULTIPLY"   }
,   { VK_ADD       , "VK_ADD"        }
,   { VK_SEPARATOR , "VK_SEPARATOR"  }
,   { VK_SUBTRACT  , "VK_SUBTRACT"   }
,   { VK_DECIMAL   , "VK_DECIMAL"    }
,   { VK_DIVIDE    , "VK_DIVIDE"     }
,   { VK_F1        , "VK_F1"         }
,   { VK_F2        , "VK_F2"         }
,   { VK_F3        , "VK_F3"         }
,   { VK_F4        , "VK_F4"         }
,   { VK_F5        , "VK_F5"         }
,   { VK_F6        , "VK_F6"         }
,   { VK_F7        , "VK_F7"         }
,   { VK_F8        , "VK_F8"         }
,   { VK_F9        , "VK_F9"         }
,   { VK_F10       , "VK_F10"        }
,   { VK_F11       , "VK_F11"        }
,   { VK_F12       , "VK_F12"        }
,   { VK_F13       , "VK_F13"        }
,   { VK_F14       , "VK_F14"        }
,   { VK_F15       , "VK_F15"        }
,   { VK_F16       , "VK_F16"        }
,   { VK_F17       , "VK_F17"        }
,   { VK_F18       , "VK_F18"        }
,   { VK_F19       , "VK_F19"        }
,   { VK_F20       , "VK_F20"        }
,   { VK_F21       , "VK_F21"        }
,   { VK_F22       , "VK_F22"        }
,   { VK_F23       , "VK_F23"        }
,   { VK_F24       , "VK_F24"        }
,   { VK_NUMLOCK   , "VK_NUMLOCK"    }
,   { VK_SCROLL    , "VK_SCROLL"     }
,   { NULL         , NULL            }
};

char WMapKeyToShiftedKey( char c )
{
    int i;

    for( i = 0; WShiftMap[i].c; i++ ) {
        if( WShiftMap[i].c == c ) {
            return( WShiftMap[i].shift_c );
        }
    }

    return( 0 );
}

char WMapShiftedKeyToKey( char c )
{
    int i;

    for( i = 0; WShiftMap[i].shift_c; i++ ) {
        if( WShiftMap[i].shift_c == c ) {
            return( WShiftMap[i].c );
        }
    }

    return( 0 );
}

char *WGetVKeyFromID( uint_16 id )
{
    int i;

    for( i = 0; WVKeys[i].str; i++ ) {
        if( WVKeys[i].id == id ) {
            return( WVKeys[i].str );
        }
    }

    return( NULL );
}

uint_16 WGetVKeyFromStr ( char *str )
{
    int         i;
    uint_16     key;

    if( !str ) {
        return( 0 );
    }

    key = 0;

    for( i = 0; WVKeys[i].str; i++ ) {
        if( !stricmp( WVKeys[i].str, str ) ) {
            key = WVKeys[i].id;
        }
    }

    if( key == 0 ) {
        if( ( strlen( str ) == 4 ) && !strnicmp( str, "VK_", 3  ) ) {
            if( isalnum ( str[3] ) ) {
                key = toupper( str[3] );
            }
        }
    }

    return( key );
}

char *WGetKeyText( uint_16 key, uint_16 flags )
{
    char *text;

    text = NULL;
    if( flags & ACCEL_VIRTKEY ) {
        text = WGetVKText( key, flags );
    } else {
        text = WGetASCIIKeyText( key );
    }

    return ( text );
}

char *WGetVKText ( uint_16 key, uint_16 flags )
{
    char *text;

    _wtouch(flags);

    text = WGetVKeyFromID ( key );
    if ( !text ) {
        text = WGetASCIIVKText( key );
    }

    return ( text );
}

static char t[6];
char *WGetASCIIVKText( uint_16 key )
{
    int  i;

    if( !isalnum( key ) ) {
        return( NULL );
    }

    i = 0;
    t[i++] = '\"';
    t[i++] = toupper( key );
    t[i++] = '\"';
    t[i++] = 0;

    return( t );
}

char *WGetASCIIKeyText( uint_16 key )
{
    int         i;
    Bool        is_cntl;

    i = 0;
    t[i++] = '\"';
    is_cntl = iscntrl( key );
    if( is_cntl ) {
        t[i++] = '^';
        key += '@';
        if ( ( key < '@' ) || ( key > '_' ) ) {
            return ( NULL );
        }
    }
    t[i++] = key;
    t[i++] = '\"';
    t[i++] = 0;

    return( t );
}

Bool WGetKeyFromText( char *text, uint_16 *key, uint_16 *flags, Bool *force_ascii )
{
    Bool     ok, is_cntl;
    uint_16  tkey;
    int      i;

    tkey   = 0;

    ok = ( text && key && flags );

    if( ok ) {
        *key = 0;
        *force_ascii = FALSE;
        tkey = WGetVKeyFromStr( text );
        if( tkey ) {
            *flags |= ACCEL_VIRTKEY;
            *key = tkey;
            return( TRUE );
        }
    }

    if( ok ) {
        i = 0;
        if ( text[i] == '\"' ) {
            i++;
        }
        is_cntl = FALSE;
        if ( text[i] == '\^' ) {
            i++;
            is_cntl = TRUE;
        }
        tkey = text[i];
        if( is_cntl ) {
            *force_ascii = TRUE;
            tkey = toupper( tkey );
            if( ( tkey >= '@' ) && ( tkey <= '_' ) ) {
                tkey -= '@';
            } else {
                ok = FALSE;
            }
        } else {
            if( !isalnum(tkey) ) {
                *force_ascii = TRUE;
            } else {
                if( ( *flags & ACCEL_VIRTKEY ) && islower( tkey ) ) {
                    tkey = toupper( tkey );
                }
            }
            ok = isprint( tkey );
        }
    }

    if( ok ) {
        *key   = tkey;
    }

    return( ok );
}

