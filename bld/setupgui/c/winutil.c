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
* Description:  Windows specific utility routines.
*
****************************************************************************/


#if defined( __NT__ ) || defined( __WINDOWS__ )
  #if defined( __WINDOWS__ )
    #define INCLUDE_COMMDLG_H
  #endif
    #include <windows.h>
  #if defined( __NT__ )
    #include <commdlg.h>
  #endif
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#if !defined( _UI ) && ( defined( __NT__ ) || defined( __WINDOWS__ ) )
    #include "fontstr.h"
#endif
#define INCL_WINSHELLDATA
#include "gui.h"
#include "setup.h"
#include "setupinf.h"
#include "utils.h"
#include "genvbl.h"


// *********************** Function for writing to WIN.INI *******************

#if defined( __NT__ )

void CreateRegEntry( char *hive_key, char *app_name, char *key_name,
                     char *value, char *file_name, bool add )
{
    char                buf[_MAX_PATH];
    long                rc;
    HKEY                hkey1;
    DWORD               disposition;
    DWORD               type;
    DWORD               old_type;
    int                 len;
    long                dword_val;
    HKEY                key;
    unsigned char       *bin_buf;
    int                 i;

    strcpy( buf, file_name );
    len = strlen( buf );
    if( buf[len-1] != '\\' ) {
        strcat( buf, "\\" );
    }
    strcat( buf, app_name );
    if( stricmp( hive_key, "local_machine" ) == 0 ) {
        key = HKEY_LOCAL_MACHINE;
    } else if( stricmp( hive_key, "current_user" ) == 0 ) {
        key = HKEY_CURRENT_USER;
    } else {
        key = HKEY_LOCAL_MACHINE;
    }
    if( add ) {
        rc = RegCreateKeyEx( key, buf, 0, NULL, REG_OPTION_NON_VOLATILE,
                             KEY_WRITE, NULL, &hkey1, &disposition );
        if( key_name[0] != '\0' ) {
            if( value[0] == '#' ) {     // dword
                dword_val = atoi( value + 1 );
                rc = RegSetValueEx( hkey1, key_name, 0, REG_DWORD, (LPBYTE)&dword_val,
                                    sizeof( long ) );
            } else if( value[0] == '%' ) {      // binary
                ++value;
                len = strlen( value );
                bin_buf = malloc( len / 2 );
                if( bin_buf != NULL ) {
                    for( i = 0; i < len / 2; ++i ) {
                        if( tolower( value[0] ) >= 'a' ) {
                            bin_buf[i] = value[0] - 'a' + 10;
                        } else {
                            bin_buf[i] = value[0] - '0';
                        }
                        bin_buf[i] = bin_buf[i] * 16;
                        if( tolower( value[1] ) >= 'a' ) {
                            bin_buf[i] += value[1] - 'a' + 10;
                        } else {
                            bin_buf[i] += value[1] - '0';
                        }
                        value += 2;
                    }
                    rc = RegSetValueEx( hkey1, key_name, 0, REG_BINARY, (LPBYTE)bin_buf,
                                        len / 2 );
                    free( bin_buf );
                }
            } else {
                rc = RegQueryValueEx( hkey1, key_name, NULL, &old_type,
                                      NULL, NULL );
                if( rc == 0 ) {
                    type = old_type;
                } else {
                    type = REG_SZ;
                }
                rc = RegSetValueEx( hkey1, key_name, 0, type, (LPBYTE)value,
                                    strlen( value ) + 1 );
            }
        }
    } else {
        rc = RegDeleteKey( key, buf );
    }
}


bool GetRegString( HKEY hive, char *section, char *value,
                   char *buffer, DWORD buff_size )
/*******************************************************/
{
    HKEY                hkey;
    LONG                rc;
    DWORD               type;
    bool                ret;

    ret = FALSE;
    rc = RegOpenKeyEx( hive, section, 0L, KEY_ALL_ACCESS, &hkey );
    if( rc == ERROR_SUCCESS ) {
        // get the value
        rc = RegQueryValueEx( hkey, value, NULL, &type, (LPBYTE)buffer, &buff_size );
        RegCloseKey( hkey );
        ret = (rc == ERROR_SUCCESS);
    }
    return( ret );
}

DWORD ConvertDataToDWORD( BYTE *data, DWORD num_bytes, DWORD type )
/*****************************************************************/
{
    int                         i;
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

BYTE *ConvertDWORDToData( DWORD number, DWORD type )
/**************************************************/
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

signed int AddToUsageCount( char *path, signed int value )
/********************************************************/
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

    result = RegQueryValueEx( key_handle, path, 0, &value_type, buff, &buff_size );
    if( result != ERROR_SUCCESS ) {
        orig_value = 0;
        value_type = REG_DWORD;
    } else {
        orig_value = ConvertDataToDWORD( buff, buff_size, value_type );
    }

    // Don't increment if reinstalling and file already has a nonzero count
    if( GetVariableIntVal( "ReInstall" ) != 0 && orig_value != 0 && value > 0 ) {
        value = 0;
    }

    new_value = (long)orig_value + value;

    if( new_value > 0 ) {
        memcpy( buff, ConvertDWORDToData( new_value, value_type ), sizeof( new_value ) );
        result = RegSetValueEx( key_handle, path, 0, value_type, buff, sizeof( new_value ) );
    } else if( new_value == 0 ) {
        result = RegDeleteValue( key_handle, path );
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

signed int IncrementDLLUsageCount( char *path )
/*********************************************/
{
    return AddToUsageCount( path, 1 );
}

signed int DecrementDLLUsageCount( char *path )
/*********************************************/
{
    return AddToUsageCount( path, -1 );
}

#endif

#if defined( __WINDOWS__ ) || defined( __NT__ )

bool ZapKey( char *app_name, char *old, char *new, char *file, char *hive, int pos )
/**********************************************************************************/
{
    FILE        *io;
    char        buff[MAXVALUE];
    int         app_len;
    int         old_len;
    int         num = 0;
    bool        in_sect = FALSE;

    /* invalidate cache copy of INI file */
    app_len = strlen( app_name );
    old_len = strlen( old );
    WritePrivateProfileString( NULL, NULL, NULL, file );
    io = fopen( hive, "r+t" );
    if( io == NULL ) return( FALSE );
    while( fgets( buff, sizeof( buff ), io ) ) {
        if( buff[0] == '[' ) {
            if( in_sect ) break;
            if( strncmp( app_name, buff + 1, app_len ) == 0 && buff[app_len + 1] == ']' ) {
                in_sect = TRUE;
            }
        } else if( in_sect ) {
            if( strncmp( old, buff, old_len ) == 0 && buff[old_len] == '=' ) {
                if( num++ == pos ) {
                    memcpy( buff, new, old_len );
                    fseek( io, -(int)(strlen( buff ) + 1), SEEK_CUR );
                    fputs( buff, io );
                    fclose( io );
                    return( TRUE );
                }
            }
        }
    }
    fclose( io );
    WritePrivateProfileString( NULL, NULL, NULL, file );
    return( FALSE );
}

#define DEVICE_STRING "device"
#define ALT_DEVICE    "ecived"

void AddDevice( char *app_name, char *value, char *file, char *hive, char *buff,
                bool add )
/******************************************************************************/
{
    int         i;
    char        old_name[_MAX_FNAME];
    char        new_name[_MAX_FNAME];
    char        old_ext[_MAX_EXT];
    char        new_ext[_MAX_EXT];
    bool        done = FALSE;

    _splitpath( value, NULL, NULL, new_name, new_ext );
    for( i = 0; ZapKey( app_name, DEVICE_STRING, ALT_DEVICE, file, hive, i ); ++i ) {
        GetPrivateProfileString( app_name, ALT_DEVICE, "", buff, MAXVALUE, file );
        _splitpath( buff, NULL, NULL, old_name, old_ext );
        if( !stricmp( old_name, new_name ) && !stricmp( old_ext, new_ext ) ) {
            WritePrivateProfileString( app_name, ALT_DEVICE, add ? value : NULL, file );
            done = TRUE;
        }
        ZapKey( app_name, ALT_DEVICE, DEVICE_STRING, file, hive, 0 );
        if( done ) break;
    }
    if( !done && add ) {
        WritePrivateProfileString( app_name, ALT_DEVICE, value, file );
        ZapKey( app_name, ALT_DEVICE, DEVICE_STRING, file, hive, 0 );
    }
}

void WindowsWriteProfile( char *app_name, char *key_name, char *buf,
                          char *file_name, bool add, char *value, char *tmp_buff )
/********************************************************************************/
{
    char                *key;
    char                *substr;
    char                *endsub;

    key = key_name;
    switch( key[0] ) {
    case '+':
        ++key;
        GetPrivateProfileString( app_name, key, "", value, MAXVALUE, file_name );
        substr = stristr( value, buf );
        if( substr != NULL ) {
            if( !add ) {
                endsub = substr + strlen( substr );
                memmove( substr, endsub, strlen( endsub ) + 1 );
            }
        } else if( value[0] != '\0' ) {
            strcat( value, " " );
            strcat( value, buf );
        }
        WritePrivateProfileString( app_name, key, value, file_name );
        break;
    case '*':
        tmp_buff[0] = '\0';
        if( strpbrk( file_name, "\\/:" ) == NULL ) {
            GetWindowsDirectory( tmp_buff, _MAX_PATH );
            strcat( tmp_buff, "\\" );
        }
        strcat( tmp_buff, file_name );
        AddDevice( app_name, buf, file_name, tmp_buff, value, add );
        break;
    default:
        if( add ) {
            WritePrivateProfileString( app_name, key, buf, file_name );
        } else {
            // if file doesn't exist, Windows creates 0-length file
            if( access( file_name, F_OK ) == 0 ) {
                WritePrivateProfileString( app_name, key, NULL, file_name );
            }
        }
        break;
    }
}

#endif

#if defined( __OS2__ )

void OS2WriteProfile( char *app_name, char *key_name,
                      char *value, char *file_name, bool add )
{
    HAB                 hab;
    HINI                hini;
    PRFPROFILE          profile;
    char                userfname[1], drive[_MAX_DRIVE], dir[_MAX_DIR];
    char                inifile[_MAX_PATH];

    // get an anchor block
    hab = WinQueryAnchorBlock( HWND_DESKTOP );
    // find location of os2.ini the file we want should be there too
    profile.cchUserName = 1;
    profile.pszUserName = userfname;
    profile.cchSysName = _MAX_PATH - 1;
    profile.pszSysName = inifile;
    if( !PrfQueryProfile( hab, &profile ) ) {
        return;
    }
    // replace os2.ini with filename
    _splitpath( inifile, drive, dir, NULL, NULL );
    _makepath( inifile, drive, dir, file_name, NULL );
    // now we can open the correct ini file
    hini = PrfOpenProfile( hab, inifile );
    if( hini != NULLHANDLE ) {
        PrfWriteProfileString( hini, app_name, key_name, add ? value : NULL );
        PrfCloseProfile( hini );
    }
}

#endif


extern void WriteProfileStrings( bool uninstall )
/***********************************************/
{
    int                 num, i, sign, end;
    char                app_name[MAXBUF];
    char                key_name[MAXBUF];
    char                fname[_MAX_PATH];
    char                file_name[_MAX_PATH];
    char                hive_name[_MAX_PATH];
    char                value[MAXVALUE];
    char                buf[MAXVALUE];
    bool                add;


    num = SimNumProfile();
    if( uninstall ) {
        add = FALSE;
        i = num - 1;
        sign = -1;
        end = -1;
    } else {
        i = 0;
        sign = 1;
        end = num;
    }

    for( ; i != end; i += sign ) {
        SimProfInfo( i, app_name, key_name, value, fname, hive_name );
        ReplaceVars( buf, value );
        ReplaceVars( file_name, fname );
        if( !uninstall ) {
            add = SimCheckProfCondition( i );
            if( add == FALSE ) {
                continue;
            }
        }
#if defined( __WINDOWS__ )
        WindowsWriteProfile( app_name, key_name, buf, file_name, add, value, hive_name );
#elif defined( __NT__ )
        if( hive_name[0] != '\0' ) {
            CreateRegEntry( hive_name, app_name, key_name, buf, file_name, add );
        } else {
            WindowsWriteProfile( app_name, key_name, buf, file_name, add, value,
                                 hive_name );
        }
#elif defined( __OS2__ )
        OS2WriteProfile( app_name, key_name, buf, file_name, add );
#endif
    }
}

#if (defined( __NT__ ) || defined( __WINDOWS__ )) && !defined( _UI )
static bool IsWin40()
{
#if defined( __NT__ )
    OSVERSIONINFO       ver;

    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if( GetVersionEx( (OSVERSIONINFO *) &ver ) ) {
        if( ver.dwMajorVersion >= 4 ) {
            return( TRUE );
        } else {
            return( FALSE );
        }
    }
    return( FALSE );
#else
    return( FALSE );
#endif
}
#endif


void SetDialogFont()
{
#if (defined( __NT__ ) || defined( __WINDOWS__ )) && !defined( _UI )

    char            *fontstr;
    LOGFONT         lf;
    char            dlgfont[100];

    if( !GetVariableIntVal( "IsJapanese" ) ) {
        fontstr = GUIGetFontInfo( MainWnd );
        GetLogFontFromString( &lf, fontstr );
//      following line removed - has no effect on line spacing, it only
//      causes the dialog boxes to be too narrow in Win 4.0
//      lf.lfHeight = (lf.lfHeight * 8)/12;
        if( IsWin40() ) {
            lf.lfWeight = FW_NORMAL;
        } else {
            lf.lfWeight = FW_BOLD;
        }
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

    retval = FALSE;
    h = GetModuleHandle( "kernel32" );
    fn = (ISWOW64PROCESS_FN)GetProcAddress( h, "IsWow64Process" );
    if( fn != NULL ) {
        if( !fn( GetCurrentProcess(), &retval ) ) {
            retval = FALSE;
        }
    }
    return( retval );
}
#endif
