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


#include "winvi.h"
#include <string.h>
#include <dos.h>
#include <io.h>

#define CONFIG_INI "weditor.ini"

static char     *iniFile = "watcom.ini";
static char     *keyToolBar = "ToolBar";
static char     *keyInitialPosition = "InitialPosition";
static char     *keyInitialPositionState = "InitialPositionState";
static char     *keySaveConfig = "SaveConfig";
static char     *keyCfgTime = "CfgTime";
static char     *keyChildrenMaximized = "ChildrenMaximized";
static DWORD    cfgTime;

static bool     saveConfig;
#if defined(__WINDOWS_386__)
#define STUPIDINT       short
#elif defined(__NT__)
#define STUPIDINT       LONG
#else
#define STUPIDINT       int
#endif

/*
 * getInt - parse an int from a string
 */
static bool getInt( char *str, STUPIDINT *i )
{
    DWORD       tmp;
    bool        rc;

    rc = GetDWORD( str, &tmp );
    *i = (STUPIDINT) tmp;
    return( rc );

} /* getInt */

/*
 * getProfileString - get a string from the profile
 */
static void getProfileString( char *key, char *initial, char *buffer )
{
    GetPrivateProfileString( EditorName, key, initial, buffer, MAX_STR,
                                iniFile );

} /* getProfileString */

/*
 * getProfileRect - get a rectangle from the profile
 */
static void getProfileRect( char *key, char *initial, RECT *r )
{
    char        str[ MAX_STR ];

    getProfileString( key, initial, str );
    getInt( str, &r->left );
    getInt( str, &r->top );
    getInt( str, &r->right );
    getInt( str, &r->bottom );

} /* getProfileRect */

/*
 * getProfileLong - get a long integer from the profile
 */
static long getProfileLong( char *key )
{
    char        buffer[32];

    GetPrivateProfileString( EditorName, key, "0", buffer, sizeof( buffer ),
                                iniFile );
    return( atol( buffer ) );

} /* getProfileLong */

/*
 * writeProfileString - write a string to the profile
 */
static void writeProfileString( char *key, char *buffer )
{
    WritePrivateProfileString( EditorName, key, buffer, iniFile );

} /* writeProfileString */

/*
 * writeProfileRect - write a rectangle to the profile
 */
static void writeProfileRect( char *key, RECT *r )
{
    char        str[ MAX_STR ];

    MySprintf( str, "%d %d %d %d", r->left, r->top, r->right, r->bottom );
    writeProfileString( key, str );

} /* writeProfileRect */

/*
 * writeProfileLong - write a long integer to the profile
 */
static void writeProfileLong( char *key, long value  )
{
    char        str[ MAX_STR ];

    MySprintf( str, "%l", value );
    writeProfileString( key, str );

} /* writeProfileLong */

/*
 * writeToolBarSize - get the tool bar size
 */
static void writeToolBarSize( void )
{
    writeProfileRect( keyToolBar, &ToolBarFloatRect );

} /* writeToolBarSize */

/*
 * readToolBarSize - get the tool bar size
 */
static void readToolBarSize( void )
{
    getProfileRect( keyToolBar, "300 200 500 300", &ToolBarFloatRect );

} /* readToolBarSize */

/*
 * readInitialPosition - get the initial position of the editor
 */
static void readInitialPosition( void )
{
    RECT        r;

    getProfileRect( keyInitialPosition, "0 0 0 0", &r );
    SetInitialWindowRect( &r );
    RootState = getProfileLong( keyInitialPositionState );

} /* readInitialPosition */

/*
 * writeInitialPosition - write out the initial position information
 */
void writeInitialPosition( void )
{
    if( !EditFlags.SavePosition ) {
        memset( &RootRect, 0, sizeof( RECT ) );
    }
    if( !IsIconic( Root ) ){
        writeProfileRect( keyInitialPosition, &RootRect );
        writeProfileLong( keyInitialPositionState, RootState );
    }

} /* writeInitialPosition */

/*
 * readConfigFileName - get the name of the config file that we are to read
 */
static void readConfigFileName( void )
{
    char        fname[_MAX_PATH];
    char        cname[_MAX_PATH];
    //char      str[MAX_STR]; // not used if not prompting for new cfg files
    char        *cfgname;
    struct stat cfg;
    int         rc;

    cfgTime = getProfileLong( keyCfgTime );
    GetWindowsDirectory( fname, sizeof( fname ) );
    strcat( fname, "\\" CONFIG_INI );

    cfgname = GetConfigFileName();
    GetFromEnv( cfgname, cname );
    if( cname[0] != 0 ) {
        stat( cname, &cfg );
        if( cfgTime == 0 ) {
            cfgTime = cfg.st_mtime;
        }
    }

    if( access( fname, ACCESS_RD ) != -1 ) {
        rc = IDNO;
        #if 0
        // don't prompt for newer config files
        if( cfg.st_mtime > cfgTime ) {
            MySprintf( str, "The configuration file \"%s\" is newer than your .INI file, do you wish to use the new configuration?",
                        cname );
            rc = MessageBox( (HWND) NULL, str, EditorName, MB_YESNO | MB_TASKMODAL );
            if( rc == IDYES ) {
                cfgTime = cfg.st_mtime;
            }

        }
        #endif
        if( rc == IDNO ) {
            SetConfigFileName( fname );
        }
    } else {
        cfgTime = cfg.st_mtime;
    }
    saveConfig = getProfileLong( keySaveConfig );

} /* readConfigFileName */

/*
 * writeConfigFile - write the current config file name
 */
static void writeConfigFile( void )
{
    char        fname[_MAX_PATH];
    struct stat cfg;

    writeProfileLong( keySaveConfig, EditFlags.SaveConfig );
    if( !EditFlags.SaveConfig ) {
        writeProfileLong( keyCfgTime, cfgTime );
        return;
    }
    writeProfileLong( keyChildrenMaximized, 0 );
    GetWindowsDirectory( fname, sizeof( fname ) );
    strcat( fname, "\\" CONFIG_INI );
    GenerateConfiguration( fname, FALSE );
    stat( fname, &cfg );
    writeProfileLong( keyCfgTime, cfg.st_mtime );

} /* writeConfigFile */

/*
 * SetSaveConfig - set SaveConfig flag to value that was found in profile
 */
void SetSaveConfig( void )
{
    EditFlags.SaveConfig = saveConfig;

} /* SetSaveConfig */

/*
 * ReadProfile - read our current status from the .ini file
 */
void ReadProfile( void )
{
    readToolBarSize();
    readInitialPosition();
    readConfigFileName();

} /* ReadProfile */

/*
 * WriteProfile - write our current status to the .ini file
 */
void WriteProfile( void )
{
    writeToolBarSize();
    writeInitialPosition();
    writeConfigFile();

} /* WriteProfile */
