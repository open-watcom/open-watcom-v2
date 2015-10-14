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


#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wreglbl.h"
#include "wregetfn.h"
#include "wrestrdp.h"
#include "wreopts.h"
#include "watini.h"
#include "inipath.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    RECT        screen_pos;
    bool        screen_maxed;
    char        *last_dir;
    char        *last_filter;
} WREOptState;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/*****************************************************************************/
static void WREWriteOpts( WREOptState * );
static bool WREReadOpts( WREOptState * );
static bool WREWriteIntOpt( char *, int );
static bool WREGetBoolOpt( char *, bool * );
#if 0
static bool WREGetIntOpt( char *, int * );
#endif
static bool WREWriteRectOpt( char *, RECT * );
static bool WREGetRectOpt( char *, RECT * );
static char *WRERectToStr( RECT * );
static void WREStrToRect( char *, RECT * );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
char WREProfileName[_MAX_PATH] = WATCOM_INI;
char WRESectionName[] = "wre";

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WREOptState WRECurrentState;

static WREOptState WREDefaultState = {
    { CW_USEDEFAULT,    /* default window pos               */
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT },
    FALSE,              /* is the window maximized          */
    NULL,               /* last open/save directory         */
    NULL,               /* last file filter                 */
};

static bool WREGetStrOpt( char *entry, char **opt )
{
    char        str[_MAX_PATH];
    bool        ret;

    ret = GetPrivateProfileString( WRESectionName, entry, "",
                                   str, _MAX_PATH - 1, WREProfileName );

    if( ret ) {
        ret = ((*opt = WREStrDup( str )) != NULL);
    }

    return( ret );
}

void WREOptsShutdown( void )
{
    if( WRECurrentState.last_filter != NULL ) {
        WRMemFree( WRECurrentState.last_filter );
    }
    if( WRECurrentState.last_dir != NULL ) {
        WRMemFree( WRECurrentState.last_dir );
    }

    WRECurrentState.last_dir = WREStrDup( WREGetInitialDir() );
    WRECurrentState.last_filter = WREGetFileFilter();

    WREWriteOpts( &WRECurrentState );

    if( WRECurrentState.last_dir != NULL ) {
        WRMemFree( WRECurrentState.last_dir );
    }
}

void WREWriteOpts( WREOptState *o )
{
    WREWriteRectOpt( "ScreenPos", &o->screen_pos );
    WREWriteIntOpt( "ScreenMaxed", o->screen_maxed );
    WritePrivateProfileString( WRESectionName, "LastDir",
                               o->last_dir, WREProfileName );
    WritePrivateProfileString( WRESectionName, "FileFilter",
                               o->last_filter, WREProfileName );
}

bool WREReadOpts( WREOptState *s )
{
    bool ret;

    ret = WREGetRectOpt( "ScreenPos", &s->screen_pos );
    ret &= WREGetBoolOpt( "ScreenMaxed", &s->screen_maxed );
    ret &= WREGetStrOpt( "FileFilter", &s->last_filter );
    ret &= WREGetStrOpt( "LastDir", &s->last_dir );

    return( ret );
}

bool WREWriteIntOpt( char *entry, int i )
{
    char  str[12];
    bool  ret;

    ltoa( i, str, 10 );

    ret = ( WritePrivateProfileString( WRESectionName, entry, str, WREProfileName ) != 0 );

    return( ret );
}

bool WREGetBoolOpt( char *entry, bool *i )
{
    int opt;

    opt = (int)GetPrivateProfileInt( WRESectionName, entry, 0x7fff, WREProfileName );

    if( opt != 0x7fff ) {
        *i = ( opt != 0 );
    }

    return( opt != 0x7fff );
}

#if 0
bool WREGetIntOpt( char *entry, int *i )
{
    int opt;

    opt = (int)GetPrivateProfileInt( WRESectionName, entry, 0x7fff, WREProfileName );

    if( opt != 0x7fff ) {
        *i = opt;
    }

    return( opt != 0x7fff );
}
#endif

bool WREWriteRectOpt( char *entry, RECT *r )
{
    char    *str;
    bool    ret;

    ret = false;
    str = WRERectToStr( r );
    if( str != NULL ) {
        ret = ( WritePrivateProfileString( WRESectionName, entry, str, WREProfileName ) != 0 );
        WRMemFree( str );
    }

    return( ret );
}

bool WREGetRectOpt( char *entry, RECT *r )
{
    char    str[41];
    bool    ret;

    ret = ( GetPrivateProfileString( WRESectionName, entry, "0, 0, 0, 0", str, 40, WREProfileName ) != 0 );
    if( ret && strcmp( "0, 0, 0, 0", str ) ) {
        WREStrToRect( str, r );
        return( true );
    } else {
        return( false );
    }
}

char *WRERectToStr( RECT *r )
{
    char temp[41];

    sprintf( temp, "%d, %d, %d, %d", r->left, r->top, r->right, r->bottom );

    return( WREStrDup( temp ) );
}

void WREStrToRect( char *str, RECT *r )
{
    memset( r, 0, sizeof( RECT ) );
    sscanf( str, "%d, %d, %d, %d", &r->left, &r->top, &r->right, &r->bottom );
}

void WREInitOpts( void )
{
    WRECurrentState = WREDefaultState;
    GetConfigFilePath( WREProfileName, sizeof( WREProfileName ) );
    strcat( WREProfileName, "\\" WATCOM_INI );
    WREReadOpts( &WRECurrentState );
    if( WRECurrentState.last_dir != NULL ) {
        WRESetInitialDir( WRECurrentState.last_dir );
    }
    WRESetFileFilter( WRECurrentState.last_filter );
}

int WREGetOption( WREOptReq req )
{
    int ret;

    switch( req ) {
    case WREOptScreenMax:
        ret = WRECurrentState.screen_maxed;
        break;

    default:
        ret = WRE_BAD_OPT_REQ;
    }

    return( ret );
}

void WREGetScreenPosOption( RECT *pos )
{
    *pos = WRECurrentState.screen_pos;
}

void WRESetScreenPosOption( RECT *pos )
{
    WRECurrentState.screen_pos = *pos;
}

int WRESetOption( WREOptReq req, int val )
{
    int old;

    switch( req ) {
    case WREOptScreenMax:
        old = WRECurrentState.screen_maxed;
        WRECurrentState.screen_maxed = ( val != 0 );
        break;

    default:
        old = WRE_BAD_OPT_REQ;
        break;
    }

    return( old );
}
