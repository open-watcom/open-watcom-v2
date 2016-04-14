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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wresall.h"
#include "types.h"
#include "flag.h"
#include "fatal.h"
#include "dmpcont.h"
#include "global.h"
#include "dmpfile.h"
#include "wresdefn.h"

static void ConvertIDToStr( WResID * id, char * str, int maxlen )
/***************************************************************/
{
    int     numchars;

    if (id->IsName) {
        numchars = id->ID.Name.NumChars;
        if( numchars > maxlen - 1 )
            numchars = maxlen - 1;
        memcpy( str, &(id->ID.Name.Name), numchars );
        str[ numchars ] = '\0';
    } else {
        itoa( id->ID.Num, str, 10 );
    }
}

static void ConvertTypeIDToStr( WResID * id, char * str, int maxlen )
/*******************************************************************/
{
    if (id->IsName) {
        ConvertIDToStr( id, str, maxlen );
    } else {
        switch (id->ID.Num) {
        case RT_CURSOR:
            strncpy( str, "cursor", maxlen );
            break;
        case RT_BITMAP:
            strncpy( str, "bitmap", maxlen );
            break;
        case RT_ICON:
            strncpy( str, "icon", maxlen );
            break;
        case RT_MENU:
            strncpy( str, "menu", maxlen );
            break;
        case RT_DIALOG:
            strncpy( str, "dialog", maxlen );
            break;
        case RT_STRING:
            strncpy( str, "string table", maxlen );
            break;
        case RT_FONTDIR:
            strncpy( str, "font directory", maxlen );
            break;
        case RT_FONT:
            strncpy( str, "font", maxlen );
            break;
        case RT_ACCELERATOR:
            strncpy( str, "accelerator table", maxlen );
            break;
        case RT_RCDATA:
            strncpy( str, "rcdata", maxlen );
            break;
        case RT_ERRTABLE:
            strncpy( str, "error table", maxlen );
            break;
        case RT_GROUP_CURSOR:
            strncpy( str, "cursor group", maxlen );
            break;
        case RT_GROUP_ICON:
            strncpy( str, "icon group", maxlen );
            break;
        case RT_NAMETABLE:
            strncpy( str, "nametable", maxlen );
            break;
        case RT_VERSIONINFO:
            strncpy( str, "versioninfo", maxlen );
            break;
        default:
            ConvertIDToStr( id, str, maxlen );
            break;
        }
    }
}

static char * MemFlagsOnList[16] = {
    NULL,               /* 0x0001 */
    NULL,
    NULL,
    NULL,
    "MOVEABLE",         /* 0x0010 */
    "PURE",             /* 0x0020 */
    "PRELOAD",          /* 0x0040 */
    NULL,
    NULL,               /* 0x0100 */
    NULL,
    NULL,
    NULL,
    "DISCARDABLE",      /* 0x1000 */
    NULL,
    NULL,
    NULL
};

static char * MemFlagsOffList[16] = {
    NULL,               /* 0x0001 */
    NULL,
    NULL,
    NULL,
    "FIXED",            /* 0x0010 */
    "IMPURE",           /* 0x0020 */
    "LOADONCALL",       /* 0x0040 */
    NULL,
    NULL,               /* 0x0100 */
    NULL,
    NULL,
    NULL,
    NULL,               /* 0x1000 */
    NULL,
    NULL,
    NULL
};

static bool DumpResource( WResDirWindow wind, WResFileID handle, uint_16 os )
/***************************************************************************/
{
    bool            error;
    char            resname[15];
    char            typename[20];
    WResTypeInfo    *type;
    WResResInfo     *res;
    WResLangInfo    *lang;

    type = WResGetTypeInfo( wind );
    res = WResGetResInfo( wind );
    lang = WResGetLangInfo( wind );

    if( CmdLineParms.Type != NULL ) {
        if( !WResIDCmp( CmdLineParms.Type, &(type->TypeName) ) ) {
            return( false );
        }
    }
    if( CmdLineParms.Name != NULL ) {
        if( !WResIDCmp( CmdLineParms.Name, &(res->ResName) ) ) {
            return( false );
        }
    }

    ConvertIDToStr( &(res->ResName), resname, 15 );
    ConvertTypeIDToStr( &(type->TypeName), typename, 20 );

    fprintf( stdout, "%-20.20s  %-15.15s  0x%04X 0x%02X  ", typename,
                resname, (int)lang->lang.lang, (int)lang->lang.sublang );
    PrintUint16Flags( lang->MemoryFlags, MemFlagsOnList, MemFlagsOffList, 53 );

    if( CmdLineParms.DumpContents ) {
        error = DumpContents( type, res, lang, handle, os );
        if( error ) {
            return( true );
        }
    }

    return( false );
}


static int DumpDir( WResDir dir, WResFileID handle )
/**************************************************/
{
    int             retcode;
    bool            error;
    WResDirWindow   wind;
    uint_16         os;

    retcode = 0;

    if( WResIsEmpty( dir ) ) {
        printf( "Directory in file %s is empty\n", CmdLineParms.FileName );
    } else {
        os = WResGetTargetOS( dir );
        wind = WResFirstResource( dir );
        while( !WResIsLastResource( wind, dir ) ) {
            error = DumpResource( wind, handle, os );
            if( error ) {
                retcode = 2;
            }
            wind = WResNextResource( wind, dir );
        }
        error = DumpResource( wind, handle, os );
        if( error ) {
            retcode = 2;
        }
    }

    return( retcode );
}

extern int DumpFile( void )
/*************************/
{
    int             error;
    int             retcode;
    WResFileID      handle;
    WResDir         dir;

    handle = ResOpenFileRO( CmdLineParms.FileName );
    if (handle == -1) {
        return( 2 );
    }

    if (WResIsWResFile( handle )) {
        puts( "WATCOM format .RES file" );
    } else {
        puts( "MS format .RES file" );
    }

    dir = WResInitDir();
    if (dir == NULL) {
        FatalError( "Out of memory" );
    }

    error = WResReadDir( handle, dir, NULL );
    if (error) {
        puts( "Unable to read directory" );
        retcode = 2;
    } else {
        if( WResGetTargetOS( dir ) == WRES_OS_WIN16 ) {
            puts( "Target OS: Win16" );
        } else {
            puts( "Target OS: Win32" );
        }
        puts( "Type                  Name             Language     Flags" );
        puts( "====                  ====             ========     =====" );
        retcode = DumpDir( dir, handle );
    }

    WResFreeDir( dir );

    ResCloseFile( handle );

    return( retcode );
}
