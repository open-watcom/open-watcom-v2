/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Windows specific utility routines.
*
****************************************************************************/


#define INCLUDE_COMMDLG_H
#define INCL_WINSHELLDATA
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include "wio.h"
#include "watcom.h"
#include "setup.h"
#if defined( GUI_IS_GUI ) && ( defined( __NT__ ) || defined( __WINDOWS__ ) )
    #include "fontstr.h"
#endif
#include "setupinf.h"
#include "utils.h"
#include "genvbl.h"


#define FILE_SIZE       _MAX_PATH
#define VALUE_SIZE      MAXVALUE

// *********************** Function for writing to WIN.INI *******************

#if defined( __NT__ )

static void CreateRegEntry( const VBUF *hive_key, const VBUF *app_name, const VBUF *key_name,
                     const VBUF *value, const VBUF *file_name, bool add )
{
    VBUF                buf;
    long                rc;
    HKEY                hkey1;
    DWORD               disposition;
    DWORD               type;
    DWORD               old_type;
    size_t              len;
    long                dword_val;
    HKEY                key;
    unsigned char       *bin_buf;
    int                 i;

    VbufInit( &buf );
    VbufConcVbuf( &buf, file_name );
    if( VbufString( &buf )[VbufLen( &buf ) - 1] != '\\' ) {
        VbufConcChr( &buf, '\\' );
    }
    VbufConcVbuf( &buf, app_name );
    if( VbufCompStr( hive_key, "local_machine", true ) == 0 ) {
        key = HKEY_LOCAL_MACHINE;
    } else if( VbufCompStr( hive_key, "current_user", true ) == 0 ) {
        key = HKEY_CURRENT_USER;
    } else {
        key = HKEY_LOCAL_MACHINE;
    }
    if( add ) {
        rc = RegCreateKeyEx( key, VbufString( &buf ), 0, NULL, REG_OPTION_NON_VOLATILE,
                             KEY_WRITE, NULL, &hkey1, &disposition );
        if( VbufLen( key_name ) > 0 ) {
            if( VbufString( value )[0] == '#' ) {     // dword
                dword_val = atoi( VbufString( value ) + 1 );
                rc = RegSetValueEx( hkey1, VbufString( key_name ), 0, REG_DWORD, (CONST BYTE *)&dword_val, sizeof( long ) );
            } else if( VbufString( value )[0] == '%' ) {      // binary
                const char *p;

                p = VbufString( value ) + 1;
                len = VbufLen( value ) - 1;
                bin_buf = malloc( len / 2 );
                if( bin_buf != NULL ) {
                    for( i = 0; i < len / 2; ++i ) {
                        if( tolower( p[0] ) >= 'a' ) {
                            bin_buf[i] = p[0] - 'a' + 10;
                        } else {
                            bin_buf[i] = p[0] - '0';
                        }
                        bin_buf[i] = bin_buf[i] * 16;
                        if( tolower( p[1] ) >= 'a' ) {
                            bin_buf[i] += p[1] - 'a' + 10;
                        } else {
                            bin_buf[i] += p[1] - '0';
                        }
                        p += 2;
                    }
                    rc = RegSetValueEx( hkey1, VbufString( key_name ), 0, REG_BINARY, bin_buf, (DWORD)( len / 2 ) );
                    free( bin_buf );
                }
            } else {
                rc = RegQueryValueEx( hkey1, VbufString( key_name ), NULL, &old_type, NULL, NULL );
                if( rc == 0 ) {
                    type = old_type;
                } else {
                    type = REG_SZ;
                }
                rc = RegSetValueEx( hkey1, VbufString( key_name ), 0, type, (CONST BYTE *)VbufString( value ), (DWORD)( VbufLen( value )  + 1 ) );
            }
        }
    } else {
        rc = RegDeleteKey( key, VbufString( &buf ) );
    }
}


bool GetRegString( HKEY hive, const char *section, const char *value, VBUF *str )
/*******************************************************************************/
{
    HKEY                hkey;
    LONG                rc;
    DWORD               type;
    bool                ret;
    char                buffer[MAXBUF];
    DWORD               buff_size;

    VbufRewind( str );
    buff_size = sizeof( buffer );
    ret = false;
    rc = RegOpenKeyEx( hive, section, 0L, KEY_ALL_ACCESS, &hkey );
    if( rc == ERROR_SUCCESS ) {
        // get the value
        rc = RegQueryValueEx( hkey, value, NULL, &type, (LPBYTE)buffer, &buff_size );
        RegCloseKey( hkey );
        VbufConcStr( str, buffer );
        ret = ( rc == ERROR_SUCCESS );
    }
    return( ret );
}

static DWORD ConvertDataToDWORD( BYTE *data, DWORD num_bytes, DWORD type )
/************************************************************************/
{
    DWORD                       i;
    DWORD                       temp;

    if( type == REG_DWORD || type == REG_DWORD_LITTLE_ENDIAN || type  == REG_BINARY ) {
        return( (DWORD)(*data) );
    } else if( type == REG_DWORD_BIG_ENDIAN ) {
        temp = 0;
        for( i = 0; i < num_bytes; i++ ) {
            temp |= ((DWORD)data[num_bytes - 1 - i]) << (i * 8);
        }
        return( temp );
    }
    return( 0 );
}

static BYTE *ConvertDWORDToData( DWORD number, DWORD type )
/*********************************************************/
{
    int                         i;
    static BYTE                 buff[5];

    memset( buff, 0, sizeof( buff ) );
    if( type == REG_DWORD || type == REG_DWORD_LITTLE_ENDIAN || type  == REG_BINARY ) {
        memcpy( buff, &number, sizeof( number ) );
    } else if( type == REG_DWORD_BIG_ENDIAN ) {
        for( i = 0; i < sizeof( number ); i++ ) {
            buff[i] = ((BYTE *)(&number))[sizeof( number ) - 1 - i];
        }
    }
    return( buff );
}

static signed int AddToUsageCount( const VBUF *path, signed int value )
/*********************************************************************/
{
    HKEY                        key_handle;
    LONG                        result;
    DWORD                       value_type;
    DWORD                       orig_value;
    BYTE                        buff[5];
    DWORD                       buff_size = sizeof( buff );
    signed int                  return_value;
    LONG                        new_value;

    result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                           "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\SharedDLLs",
                           0, KEY_ALL_ACCESS, &key_handle );
    if( result != ERROR_SUCCESS ) {
        return( -1 );
    }

    result = RegQueryValueEx( key_handle, VbufString( path ), 0, &value_type, buff, &buff_size );
    if( result != ERROR_SUCCESS ) {
        orig_value = 0;
        value_type = REG_DWORD;
    } else {
        orig_value = ConvertDataToDWORD( buff, buff_size, value_type );
    }

    // Don't increment if reinstalling and file already has a nonzero count
    if( GetVariableBoolVal( "ReInstall" ) && orig_value != 0 && value > 0 ) {
        value = 0;
    }

    new_value = (long)orig_value + value;

    if( new_value > 0 ) {
        memcpy( buff, ConvertDWORDToData( new_value, value_type ), sizeof( new_value ) );
        result = RegSetValueEx( key_handle, VbufString( path ), 0, value_type, buff, sizeof( new_value ) );
    } else if( new_value == 0 ) {
        result = RegDeleteValue( key_handle, VbufString( path ) );
    }

    return_value = new_value;

    if( new_value >= 0 && result != ERROR_SUCCESS ) {
        return_value = -1;
    }

    if( RegFlushKey( key_handle ) != ERROR_SUCCESS ) {
        return_value = -1;
    }

    if( RegCloseKey( key_handle ) != ERROR_SUCCESS ) {
        return_value = -1;
    }

    return( return_value );
}

signed int IncrementDLLUsageCount( const VBUF *path )
/***************************************************/
{
    return( AddToUsageCount( path, 1 ) );
}

signed int DecrementDLLUsageCount( const VBUF *path )
/***************************************************/
{
    return( AddToUsageCount( path, -1 ) );
}

#endif

#if defined( __WINDOWS__ ) || defined( __NT__ )

static bool ZapKey( const VBUF *app_name, const char *old, const char *new,
                                const VBUF *file, const VBUF *hive, int pos )
/***************************************************************************/
{
    FILE        *io;
    char        buff[MAXVALUE];
    size_t      app_len;
    size_t      old_len;
    int         num = 0;
    bool        in_sect = false;

    /* invalidate cache copy of INI file */
    app_len = VbufLen( app_name );
    old_len = strlen( old );
    WritePrivateProfileString( NULL, NULL, NULL, VbufString( file ) );
    io = fopen_vbuf( hive, "r+t" );
    if( io == NULL )
        return( false );
    while( fgets( buff, sizeof( buff ), io ) != NULL ) {
        if( buff[0] == '[' ) {
            if( in_sect )
                break;
            if( buff[app_len + 1] == ']' ) {
                if( strncmp( app_name->buf, buff + 1, app_len ) == 0 ) {
                    in_sect = true;
                }
            }
        } else if( in_sect ) {
            if( buff[old_len] == '=' ) {
                if( strncmp( old, buff, old_len ) == 0 ) {
                    if( num++ == pos ) {
                        memcpy( buff, new, old_len );
                        fseek( io, -(long)(strlen( buff ) + 1), SEEK_CUR );
                        fputs( buff, io );
                        fclose( io );
                        return( true );
                    }
                }
            }
        }
    }
    fclose( io );
    WritePrivateProfileString( NULL, NULL, NULL, VbufString( file ) );
    return( false );
}

#define DEVICE_STRING "device"
#define ALT_DEVICE    "ecived"

static void AddDevice( const VBUF *app_name, const VBUF *value, const VBUF *file,
                                                        const VBUF *hive, bool add )
/**********************************************************************************/
{
    int         i;
    VBUF        old_name;
    VBUF        new_name;
    VBUF        old_ext;
    VBUF        new_ext;
    VBUF        old_value;
    bool        done = false;

    VbufInit( &new_name );
    VbufInit( &new_ext );
    VbufInit( &old_value );
    VbufInit( &old_name );
    VbufInit( &old_ext );

    VbufSplitpath( value, NULL, NULL, &new_name, &new_ext );
    for( i = 0; ZapKey( app_name, DEVICE_STRING, ALT_DEVICE, file, hive, i ); ++i ) {
        char    buffer[_MAX_PATH];

        GetPrivateProfileString( VbufString( app_name ), ALT_DEVICE, "", buffer, sizeof( buffer ), VbufString( file ) );
        VbufSetStr( &old_value, buffer );
        VbufSplitpath( &old_value, NULL, NULL, &old_name, &old_ext );
        if( VbufCompVbuf( &old_name, &new_name, true ) == 0 && VbufCompVbuf( &old_ext, &new_ext, true ) == 0 ) {
            WritePrivateProfileString( VbufString( app_name ), ALT_DEVICE, add ? VbufString( value ) : NULL, VbufString( file ) );
            done = true;
        }
        ZapKey( app_name, ALT_DEVICE, DEVICE_STRING, file, hive, 0 );
        if( done ) {
            break;
        }
    }
    if( !done && add ) {
        WritePrivateProfileString( VbufString( app_name ), ALT_DEVICE, VbufString( value ), VbufString( file ) );
        ZapKey( app_name, ALT_DEVICE, DEVICE_STRING, file, hive, 0 );
    }

    VbufFree( &old_ext );
    VbufFree( &old_name );
    VbufFree( &old_value );
    VbufFree( &new_ext );
    VbufFree( &new_name );
}

static void WindowsWriteProfile( const VBUF *app_name, const VBUF *key_name,
                            const VBUF *value, const VBUF *file_name, bool add )
/******************************************************************************/
{
    char                *substr;
    size_t              len;

    switch( VbufString( key_name )[0] ) {
    case '+':
      {
        char    value_buf[VALUE_SIZE];
        VBUF    vbuf;

        VbufInit( &vbuf );
        GetPrivateProfileString( VbufString( app_name ), VbufString( key_name ) + 1, "", value_buf, sizeof( value_buf ), VbufString( file_name ) );
        VbufConcStr( &vbuf, value_buf );
        len = VbufLen( value );
        substr = stristr( VbufString( &vbuf ), VbufString( value ), len );
        if( substr != NULL ) {
            if( !add ) {
                VBUF    tmp;

                VbufInit( &tmp );
                VbufConcStr( &tmp, substr + len );
                VbufSetVbufAt( &vbuf, &tmp, substr - VbufString( &vbuf ) );
                VbufFree( &tmp );
            }
        } else {
            if( add ) {
                if( VbufLen( &vbuf ) > 0 )
                    VbufConcChr( &vbuf, ' ' );
                VbufConcVbuf( &vbuf, value );
            }
        }
        WritePrivateProfileString( VbufString( app_name ), VbufString( key_name ) + 1, VbufString( &vbuf ), VbufString( file_name ) );
        VbufFree( &vbuf );
        break;
      }
    case '*':
      {
        VBUF    hive;

        VbufInit( &hive );
        if( strpbrk( VbufString( file_name ), "\\/:" ) == NULL ) {
            GetWindowsDirectoryVbuf( &hive );
            VbufConcChr( &hive, '\\' );
        }
        VbufConcVbuf( &hive, file_name );
        AddDevice( app_name, value, file_name, &hive, add );
        VbufFree( &hive );
        break;
      }
    default:
        if( add ) {
            WritePrivateProfileString( VbufString( app_name ), VbufString( key_name ), VbufString( value ), VbufString( file_name ) );
        } else {
            // if file doesn't exist, Windows creates 0-length file
            if( access_vbuf( file_name, F_OK ) == 0 ) {
                WritePrivateProfileString( VbufString( app_name ), VbufString( key_name ), NULL, VbufString( file_name ) );
            }
        }
        break;
    }
}

void    GetWindowsDirectoryVbuf( VBUF *windir )
{
    char    temp[_MAX_PATH];

    GetWindowsDirectory( temp, sizeof( temp ) );
    VbufSetStr( windir, temp );
}

#endif

#if defined( __OS2__ )

static void OS2WriteProfile( const VBUF *app_name, const VBUF *key_name,
                      const VBUF *value, const VBUF *file_name, bool add )
{
    HAB                 hab;
    HINI                hini;
    PRFPROFILE          profile;
    char                userfname[1];
    VBUF                inifile;
    VBUF                drive;
    VBUF                dir;
    char                buffer[_MAX_PATH];

    // get an anchor block
    hab = WinQueryAnchorBlock( HWND_DESKTOP );
    // find location of os2.ini the file we want should be there too
    profile.cchUserName = 1;
    profile.pszUserName = userfname;
    profile.cchSysName = sizeof( buffer ) - 1;
    profile.pszSysName = buffer;
    if( !PrfQueryProfile( hab, &profile ) ) {
        return;
    }
    VbufInit( &inifile );
    VbufInit( &drive );
    VbufInit( &dir );
    VbufConcStr( &inifile, buffer );
    // replace os2.ini with filename
    VbufSplitpath( &inifile, &drive, &dir, NULL, NULL );
    VbufMakepath( &inifile, &drive, &dir, file_name, NULL );
    // now we can open the correct ini file
    hini = PrfOpenProfile( hab, VbufString( &inifile ) );
    if( hini != NULLHANDLE ) {
        PrfWriteProfileString( hini, VbufString( app_name ), VbufString( key_name ), add ? VbufString( value ) : NULL );
        PrfCloseProfile( hini );
    }
    VbufFree( &dir );
    VbufFree( &drive );
    VbufFree( &inifile );
}

#endif


void WriteProfileStrings( bool uninstall )
/****************************************/
{
    int                 num;
    int                 i;
    int                 sign;
    int                 end;
    VBUF                app_name;
    VBUF                key_name;
    VBUF                file_name;
    VBUF                hive_name;
    VBUF                value;
    bool                add;

    add = false;
    num = SimNumProfile();
    if( uninstall ) {
        i = num - 1;
        sign = -1;
        end = -1;
    } else {
        i = 0;
        sign = 1;
        end = num;
    }
    VbufInit( &app_name );
    VbufInit( &key_name );
    VbufInit( &hive_name );
    VbufInit( &value );
    VbufInit( &file_name );
    for( ; i != end; i += sign ) {
        SimProfInfo( i, &app_name, &key_name, &value, &file_name, &hive_name );
        ReplaceVars1( &value );
        ReplaceVars1( &file_name );
        if( !uninstall ) {
            add = SimCheckProfCondition( i );
            if( !add ) {
                continue;
            }
        }
#if defined( __WINDOWS__ )
        WindowsWriteProfile( &app_name, &key_name, &value, &file_name, add );
#elif defined( __NT__ )
        if( VbufLen( &hive_name ) > 0 ) {
            CreateRegEntry( &hive_name, &app_name, &key_name, &value, &file_name, add );
        } else {
            WindowsWriteProfile( &app_name, &key_name, &value, &file_name, add );
        }
#elif defined( __OS2__ )
        OS2WriteProfile( &app_name, &key_name, &value, &file_name, add );
#endif
    }
    VbufFree( &file_name );
    VbufFree( &value );
    VbufFree( &hive_name );
    VbufFree( &key_name );
    VbufFree( &app_name );
}

void SetDialogFont()
{
#if (defined( __NT__ ) || defined( __WINDOWS__ )) && defined( GUI_IS_GUI )

    char            *fontstr;
    LOGFONT         lf;
    char            dlgfont[100];
  #if defined( __NT__ ) && !defined( _M_X64 )
    DWORD   ver;
  #endif

    if( !GetVariableBoolVal( "IsJapanese" ) ) {
        fontstr = GUIGetFontInfo( MainWnd );
        GetLogFontFromString( &lf, fontstr );
//      following line removed - has no effect on line spacing, it only
//      causes the dialog boxes to be too narrow in Win 4.0
//      lf.lfHeight = (lf.lfHeight * 8)/12;
  #if defined( __NT__ ) && defined( _M_X64 )
        lf.lfWeight = FW_NORMAL;
  #elif defined( __NT__ )
        ver = GetVersion();
        if( ver < 0x80000000 && LOBYTE( LOWORD( ver ) ) >= 4 ) {
            lf.lfWeight = FW_NORMAL;
        } else {
            lf.lfWeight = FW_BOLD;
        }
  #else
        lf.lfWeight = FW_BOLD;
  #endif
        strcpy(lf.lfFaceName, "MS Sans Serif");
        GetFontFormatString( &lf, dlgfont );
        GUISetFontInfo( MainWnd, dlgfont );
        GUIMemFree( fontstr );
    }
#endif
}

#if defined( __NT__ ) && !defined( _M_X64 )
typedef BOOL (WINAPI *ISWOW64PROCESS_FN)( HANDLE, PBOOL );

bool IsWOW64( void )
{
    ISWOW64PROCESS_FN   fn;
    BOOL                retval;
    HANDLE              h;
    bool                rc;

    rc = false;
    h = GetModuleHandle( "kernel32" );
    fn = (ISWOW64PROCESS_FN)GetProcAddress( h, "IsWow64Process" );
    if( fn != NULL ) {
        if( fn( GetCurrentProcess(), &retval ) && retval ) {
            rc = true;
        }
    }
    return( rc );
}
#endif
