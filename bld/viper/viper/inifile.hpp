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
* Description:  Viper INI file function prototypes.
*
****************************************************************************/
#ifndef inifile_class
#define inifile_class

#ifdef __OS2__
    #define INCL_WINSHELLDATA
    #include <os2.h>
#endif
#include "wstring.hpp"

#define IDE_INI_IDENTIFIER      "OPEN_WATCOM_IDE"
#define IDE_INI_TOOLBAR         "toolbar"
#define IDE_INI_STATWND         "statuswnd"
#define IDE_INI_AUTOREFRESH     "refresh"
#define IDE_INI_EDITOR          "editor"
#define IDE_INI_EDITOR_ISDLL    "dll_editor"
#define IDE_INI_EDITOR_PARMS    "editor_parms"
#define IDE_INI_HEIGHT          "height"
#define IDE_INI_WIDTH           "width"
#define IDE_INI_X               "x_coord"
#define IDE_INI_Y               "y_coord"

class IniFile
{
    public:
        IniFile();
        ~IniFile();
        int read( const char *section, const char *key, const char *deflt,
                  char *buffer, int len );
        int write( const char *section, const char *key, const char *string);
    private:
#ifdef __OS2__
        HINI _handle;
#elif defined( __WINDOWS__ ) || defined( __NT__ )
        WString _path;
#else
    #error UNSUPPORTED OS
#endif
};

#endif
