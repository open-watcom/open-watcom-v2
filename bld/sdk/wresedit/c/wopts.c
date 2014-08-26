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


#include "precomp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wglbl.h"
#include "wgetfn.h"
#include "wstrdup.h"
#include "wopts.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    RECT    screen_pos;
    bool    screen_maxed;
    char    *last_dir;
    int     last_filter;
} WOptState;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/*****************************************************************************/
static void WWriteOpts( WOptState * );
static bool WReadOpts( WOptState * );
static bool WWriteIntOpt( char *, int );
static bool WGetBoolOpt( char *, bool *);
static bool WGetIntOpt( char *, int *);
static bool WWriteRectOpt( char *, RECT * );
static bool WGetRectOpt( char *, RECT * );
static char *WRectToStr( RECT * );
static void WStrToRect( char *, RECT * );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char *WProfileName = NULL;
static char *WSectionName = NULL;

static WOptState WCurrentState;

static WOptState WDefaultState = {
    { CW_USEDEFAULT,     /* default window pos               */
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT },
    FALSE,               /* is the window maximized          */
    NULL,                /* last open/save directory         */
    1                    /* last file filter                 */
};

void WOptsShutdown( void )
{
    if( WCurrentState.last_dir != NULL ) {
        WRMemFree( WCurrentState.last_dir );
    }
    WCurrentState.last_dir = WStrDup( WGetInitialDir() );
    WCurrentState.last_filter = WGetFileFilter();
    WWriteOpts( &WCurrentState );
    if( WCurrentState.last_dir != NULL ) {
        WRMemFree( WCurrentState.last_dir );
    }
}

void WWriteOpts( WOptState *o )
{
    WWriteRectOpt( "ScreenPos", &o->screen_pos );
    WWriteIntOpt( "ScreenMaxed", o->screen_maxed );
    WWriteIntOpt( "FileFilter", o->last_filter );
    WritePrivateProfileString( WSectionName, "LastDir", o->last_dir, WProfileName );
}

bool WReadOpts( WOptState *s )
{
    char    str[_MAX_PATH];
    bool    ret;

    ret = WGetRectOpt( "ScreenPos", &s->screen_pos );
    ret &= WGetBoolOpt( "ScreenMaxed", &s->screen_maxed );
    ret &= WGetIntOpt( "FileFilter", &s->last_filter );

    if( ret ) {
        ret = GetPrivateProfileString( WSectionName, "LastDir", "",
                                       str, _MAX_PATH - 1, WProfileName ) != 0;
    }

    if( ret ) {
        ret = ((s->last_dir = WStrDup( str )) != NULL);
    }

    return( ret );
}

bool WWriteIntOpt( char *entry, int i )
{
    char    str[12];
    bool    ret;

    ltoa( i, str, 10 );

    ret = WritePrivateProfileString( WSectionName, entry, str, WProfileName ) != 0;

    return( ret );
}

bool WGetBoolOpt( char *entry, bool *i )
{
    int opt;

    opt = (int)GetPrivateProfileInt( WSectionName, entry, 0x7fff, WProfileName );

    if( opt != 0x7fff ) {
        *i = ( opt != 0 );
    }

    return( opt != 0x7fff );
}

bool WGetIntOpt( char *entry, int *i )
{
    int opt;

    opt = (int)GetPrivateProfileInt( WSectionName, entry, 0x7fff, WProfileName );

    if( opt != 0x7fff ) {
        *i = opt;
    }

    return( opt != 0x7fff );
}

bool WWriteRectOpt( char *entry, RECT *r )
{
    char    *str;
    bool    ret;

    ret = FALSE;
    str = WRectToStr( r );
    if( str != NULL ) {
        ret = WritePrivateProfileString( WSectionName, entry, str, WProfileName ) != 0;
        WRMemFree( str );
    }

    return( ret );
}

bool WGetRectOpt( char *entry, RECT *r )
{
    char    str[41];
    bool    ret;

    ret = GetPrivateProfileString( WSectionName, entry, "0, 0, 0, 0",
                                   str, 40, WProfileName ) != 0;
    if( ret && strcmp( "0, 0, 0, 0", str ) ) {
        WStrToRect( str, r );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

char *WRectToStr( RECT *r )
{
    char temp[41];

    sprintf( temp, "%d, %d, %d, %d", r->left, r->top, r->right, r->bottom );

    return( WStrDup ( temp ) );
}

void WStrToRect( char *str, RECT *r )
{
    memset( r, 0, sizeof( RECT ) );
    sscanf( str, "%d, %d, %d, %d", &r->left, &r->top, &r->right, &r->bottom );
}

int WInitOpts( char *ini_file, char *section_name )
{
    if( ini_file == NULL || section_name == NULL ) {
        return( FALSE );
    }
    WProfileName = ini_file;
    WSectionName = section_name;
    WCurrentState = WDefaultState;
    WReadOpts( &WCurrentState );
    if( WCurrentState.last_dir ) {
        WSetInitialDir( WCurrentState.last_dir );
    }
    WSetFileFilter( WCurrentState.last_filter );
    return( TRUE );
}

int WGetOption( WOptReq req )
{
    int ret;

    switch( req ) {
    case WOptScreenMax:
        ret = WCurrentState.screen_maxed;
        break;

    default:
        ret = BAD_OPT_REQ;
        break;
    }

    return( ret );
}

void WGetScreenPosOption( RECT *pos )
{
    *pos = WCurrentState.screen_pos;
}

void WSetScreenPosOption( RECT *pos )
{
    WCurrentState.screen_pos = *pos;
}

int WSetOption( WOptReq req, int val )
{
    int old;

    switch( req ) {
    case WOptScreenMax:
        old = WCurrentState.screen_maxed;
        WCurrentState.screen_maxed = val != 0;
        break;

    default:
        old = BAD_OPT_REQ;
        break;
    }

    return( old );
}
