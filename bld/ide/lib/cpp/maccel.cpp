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


#include "maccel.hpp"
#include "mtoolitm.hpp"
#include "mconfig.hpp"
#include "wobjfile.hpp"

Define( MAccelerator )

typedef struct {
    WKeyCode    code;
    const char  *identifier;
} KeyMap;

static const WKeyCode   ctrlAlph[] = {
    WKeyCtrlA, WKeyCtrlB, WKeyCtrlC, WKeyCtrlD, WKeyCtrlE,
    WKeyCtrlF, WKeyCtrlG, WKeyCtrlH, WKeyCtrlI, WKeyCtrlJ,
    WKeyCtrlK, WKeyCtrlL, WKeyCtrlM, WKeyCtrlN, WKeyCtrlO,
    WKeyCtrlP, WKeyCtrlQ, WKeyCtrlR, WKeyCtrlS, WKeyCtrlT,
    WKeyCtrlU, WKeyCtrlV, WKeyCtrlW, WKeyCtrlX, WKeyCtrlY,
    WKeyCtrlZ
};

static const KeyMap     ctrlSpecialKeys[] = {
    { WKeyCtrlF1,         "F1" },
    { WKeyCtrlF2,         "F2" },
    { WKeyCtrlF3,         "F3" },
    { WKeyCtrlF4,         "F4" },
    { WKeyCtrlF5,         "F5" },
    { WKeyCtrlF6,         "F6" },
    { WKeyCtrlF7,         "F7" },
    { WKeyCtrlF8,         "F8" },
    { WKeyCtrlF9,         "F9" },
    { WKeyCtrlF10,        "F10" },
    { WKeyCtrlF11,        "F11" },
    { WKeyCtrlF12,        "F12" }
};

static const WKeyCode   altAlph[] = {
    WKeyAltA, WKeyAltB, WKeyAltC, WKeyAltD, WKeyAltE,
    WKeyAltF, WKeyAltG, WKeyAltH, WKeyAltI, WKeyAltJ,
    WKeyAltK, WKeyAltL, WKeyAltM, WKeyAltN, WKeyAltO,
    WKeyAltP, WKeyAltQ, WKeyAltR, WKeyAltS, WKeyAltT,
    WKeyAltU, WKeyAltV, WKeyAltW, WKeyAltX, WKeyAltY,
    WKeyAltZ
};

static const KeyMap     altSpecialKeys[] = {
    WKeyAltF1,          "F1",
    WKeyAltF2,          "F2",
    WKeyAltF3,          "F3",
    WKeyAltF4,          "F4",
    WKeyAltF5,          "F5",
    WKeyAltF6,          "F6",
    WKeyAltF7,          "F7",
    WKeyAltF8,          "F8",
    WKeyAltF9,          "F9",
    WKeyAltF10,         "F10",
    WKeyAltF11,         "F11",
    WKeyAltF12,         "F12"
};

static const WKeyCode   plainAlph[] = {
    WKeyA, WKeyB, WKeyC, WKeyD, WKeyE,
    WKeyF, WKeyG, WKeyH, WKeyI, WKeyJ,
    WKeyK, WKeyL, WKeyM, WKeyN, WKeyO,
    WKeyP, WKeyQ, WKeyR, WKeyS, WKeyT,
    WKeyU, WKeyV, WKeyW, WKeyX, WKeyY,
    WKeyZ
};

static const KeyMap     plainSpecialKeys[] = {
    WKeyF1,             "F1",
    WKeyF2,             "F2",
    WKeyF3,             "F3",
    WKeyF4,             "F4",
    WKeyF5,             "F5",
    WKeyF6,             "F6",
    WKeyF7,             "F7",
    WKeyF8,             "F8",
    WKeyF9,             "F9",
    WKeyF10,            "F10",
    WKeyF11,            "F11",
    WKeyF12,            "F12"
};

WEXPORT WKeyCode MAccelerator::lookUpAccel( WTokenFile &fil, WString &tok ) {

    const KeyMap        *sp_keys;
    int                 sp_len;
    const WKeyCode      *alph_keys;

    fil.token( tok );
    _desc.truncate( 0 );
    if( tok == "CTRL" ) {
        sp_keys = ctrlSpecialKeys;
        sp_len = sizeof( ctrlSpecialKeys ) / sizeof( KeyMap );
        alph_keys = ctrlAlph;
        _desc.concat( tok );
        _desc.concat( "+" );
    } else if( tok == "ALT" ) {
        sp_keys = altSpecialKeys;
        sp_len = sizeof( altSpecialKeys ) / sizeof( KeyMap );
        alph_keys = altAlph;
        _desc.concat( tok );
        _desc.concat( "+" );
    } else if( tok == "PLAIN" ) {
        sp_keys = plainSpecialKeys;
        sp_len = sizeof( plainSpecialKeys ) / sizeof( KeyMap );
        alph_keys = plainAlph;
    } else {
        return( WKeyNone );
    }

    WKeyCode    ret;
    int         ch;

    fil.token( tok );
    _desc.concat( tok );
    ret = WKeyNone;
    for( int i=0; i < sp_len; i++ ) {
        if( tok == sp_keys[i].identifier ) {
            ret = sp_keys[i].code;
            break;
        }
    }
    if( ret == WKeyNone ) {
        tok.toLower();
        if( tok.size() == 1 ) {
            ch = *( tok.gets() );
            if( ch >= 'a' && ch <= 'z' ) {
                ch -= 'a';
                ret = alph_keys[ ch ];
            }
        }
    }
    return( ret );
}

WEXPORT MAccelerator::MAccelerator( WTokenFile& fil, WString& tok )
{
    _keystroke = lookUpAccel( fil, tok );
    fil.token( tok );   //read ahead
}

#ifndef NOPERSIST
MAccelerator* WEXPORT MAccelerator::createSelf( WObjectFile& )
{
    return( NULL );
}

void WEXPORT MAccelerator::readSelf( WObjectFile& p )
{
    p=p;
}

void WEXPORT MAccelerator::writeSelf( WObjectFile& p )
{
    p=p;
}
#endif
