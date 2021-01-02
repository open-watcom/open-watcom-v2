/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "win.h"
#include "menu.h"
#include "parsecl.h"
#include "sstyle.h"
#include "banner.h"
#include "fts.h"

#define VBUF_SIZE       8192

static bool     isCmdLine;

/*
 * writeTitle - write a title for each section
 */
static void writeTitle( FILE *fp, const char *str )
{
    if( isCmdLine ) {
        MyFprintf( fp, "\n#\n# %s\n#\n", str );
    }

} /* writeTitle */

/*
 * doMaps - write map commands
 */
static void doMaps( FILE *fp, key_map *maps, const char *extra_str )
{
    const char  *map;
    vi_key      *keymap;
    int         i;
    int         j;

    for( i = 0; i < MAX_EVENTS; i++ ) {
        if( maps[i].data != NULL ) {
            MyFprintf( fp, "map%s ", extra_str );
            map = LookUpCharToken( i, false );
            if( map == NULL ) {
                MyFprintf( fp, "%c ", i );
            } else {
                MyFprintf( fp, "%s ", map );
            }
            if( maps[i].no_input_window ) {
                MyFprintf( fp, "\\x" );
            }
            keymap = maps[i].data;
            for( j = 0; keymap[j] != VI_KEY( NULL ); j++ ) {
                map = LookUpCharToken( keymap[j], true );
                if( map == NULL ) {
                    MyFprintf( fp, "%c", (char)keymap[j] );
                } else {
                    if( map[1] == '\0' ) {
                        MyFprintf( fp, "\\%s", map );
                    } else {
                        MyFprintf( fp, "\\<%s>", map );
                    }
                }
            }
            MyFprintf( fp, "\n" );
        }
    }

} /* doMaps */

/*
 * doWindow - output settings for a window
 */
static void doWindow( FILE *fp, int id, window_info *wi, bool colour_only )
{
    char        token[64];

    MyFprintf( fp, "%s\n", GetTokenStringCVT( CmdLineTokens, id, token, true ) );
    if( !colour_only ) {
        MyFprintf( fp, "    dimension %d %d %d %d\n", wi->area.x1, wi->area.y1, wi->area.x2, wi->area.y2 );
        if( wi->has_border ) {
            MyFprintf( fp, "    border 1 %d %d\n", wi->border_color1, wi->border_color2 );
        } else {
            MyFprintf( fp, "    border -1\n" );
        }
    }
    MyFprintf( fp, "    text %d %d %d\n", wi->text_style.foreground, wi->text_style.background,
               wi->text_style.font );
    MyFprintf( fp, "    hilight %d %d %d\n", wi->hilight_style.foreground,
               wi->hilight_style.background, wi->hilight_style.font );
    if( wi == &editw_info ) {
        MyFprintf( fp, "    whitespace %d %d %d\n", SEType[SE_WHITESPACE].foreground,
                   SEType[SE_WHITESPACE].background, SEType[SE_WHITESPACE].font );
        MyFprintf( fp, "    selection %d %d %d\n", SEType[SE_SELECTION].foreground,
                   SEType[SE_SELECTION].background, SEType[SE_SELECTION].font );
        MyFprintf( fp, "    eoftext %d %d %d\n", SEType[SE_EOFTEXT].foreground,
                   SEType[SE_EOFTEXT].background, SEType[SE_EOFTEXT].font );
        MyFprintf( fp, "    keyword %d %d %d\n", SEType[SE_KEYWORD].foreground,
                   SEType[SE_KEYWORD].background, SEType[SE_KEYWORD].font );
        MyFprintf( fp, "    octal %d %d %d\n", SEType[SE_OCTAL].foreground,
                   SEType[SE_OCTAL].background, SEType[SE_OCTAL].font );
        MyFprintf( fp, "    hex %d %d %d\n", SEType[SE_HEX].foreground,
                   SEType[SE_HEX].background, SEType[SE_HEX].font );
        MyFprintf( fp, "    integer %d %d %d\n", SEType[SE_INTEGER].foreground,
                   SEType[SE_INTEGER].background, SEType[SE_INTEGER].font );
        MyFprintf( fp, "    char %d %d %d\n", SEType[SE_CHAR].foreground,
                   SEType[SE_CHAR].background, SEType[SE_CHAR].font );
        MyFprintf( fp, "    preprocessor %d %d %d\n", SEType[SE_PREPROCESSOR].foreground,
                   SEType[SE_PREPROCESSOR].background, SEType[SE_PREPROCESSOR].font );
        MyFprintf( fp, "    symbol %d %d %d\n", SEType[SE_SYMBOL].foreground,
                   SEType[SE_SYMBOL].background, SEType[SE_SYMBOL].font );
        MyFprintf( fp, "    invalidtext %d %d %d\n", SEType[SE_INVALIDTEXT].foreground,
                   SEType[SE_INVALIDTEXT].background, SEType[SE_INVALIDTEXT].font );
        MyFprintf( fp, "    identifier %d %d %d\n", SEType[SE_IDENTIFIER].foreground,
                   SEType[SE_IDENTIFIER].background, SEType[SE_IDENTIFIER].font );
        MyFprintf( fp, "    jumplabel %d %d %d\n", SEType[SE_JUMPLABEL].foreground,
                   SEType[SE_JUMPLABEL].background, SEType[SE_JUMPLABEL].font );
        MyFprintf( fp, "    comment %d %d %d\n", SEType[SE_COMMENT].foreground,
                   SEType[SE_COMMENT].background, SEType[SE_COMMENT].font );
        MyFprintf( fp, "    float %d %d %d\n", SEType[SE_FLOAT].foreground,
                   SEType[SE_FLOAT].background, SEType[SE_FLOAT].font );
        MyFprintf( fp, "    string %d %d %d\n", SEType[SE_STRING].foreground,
                   SEType[SE_STRING].background, SEType[SE_STRING].font );
        MyFprintf( fp, "    variable %d %d %d\n", SEType[SE_VARIABLE].foreground,
                   SEType[SE_VARIABLE].background, SEType[SE_VARIABLE].font );
        MyFprintf( fp, "    regexp %d %d %d\n", SEType[SE_REGEXP].foreground,
                   SEType[SE_VARIABLE].background, SEType[SE_VARIABLE].font );
    }
    MyFprintf( fp, "endwindow\n" );

} /* doWindow */

/*
 * outputMatchData - write out data associated with a match string
 */
static void outputMatchData( FILE *fp, const char *str )
{
    for( ; *str != '\0'; ++str ) {
        if( *str == '/' ) {
            fputc( '\\', fp );
        }
        fputc( *str, fp );
    }
    fputc( '/', fp );

} /* outputMatchData */


/*
 * doHookAssign - do an assignment for a hook variable
 */
static void doHookAssign( FILE *fp, hooktype num )
{
    vars        *v;

    v = GetHookVar( num );
    if( v == NULL ) {
        return;
    }
    MyFprintf( fp, "assign %%(%s) = %s\n", v->name, v->value );
    MyFprintf( fp, "load %s\n", v->value );

} /* doHookAssign */

/*
 * GenerateConfiguration - write out a config file
 */
vi_rc GenerateConfiguration( const char *fname, bool is_cmdline )
{
    FILE        *fp;
    int         i;
    char        token[128];
    const char  *str;
    char        *buff;
    int         num;
    rgb         c;
    const char  *res;
    char        tmpstr[MAX_STR];

    if( fname == NULL ) {
        fname = CFG_NAME;
    }
    fp = fopen( fname, "w" );
    if( fp == NULL ) {
        return( ERR_FILE_OPEN );
    }
    isCmdLine = is_cmdline;
    buff = MemAllocUnsafe( VBUF_SIZE );
    if( buff != NULL ) {
        setvbuf( fp, buff, _IOFBF, VBUF_SIZE );
    }
    MyFprintf( fp, "#\n# %s configuration file\n# %s\n#\n",
#if defined( __WIN__ )
    banner1w1( "Text Editor for Windows" ), banner1w2( _VI_VERSION_ ) );
#else
    banner1w1( "Vi Text Editor" ), banner1w2( _VI_VERSION_ ) );
#endif
    if( is_cmdline ) {
        GetDateTimeString( token );
        MyFprintf( fp, "# File generated on %s\n#\n", token );
    }

    writeTitle( fp, "Hook script assignments" );
    doHookAssign( fp, SRC_HOOK_WRITE );
    doHookAssign( fp, SRC_HOOK_READ );
    doHookAssign( fp, SRC_HOOK_BUFFIN );
    doHookAssign( fp, SRC_HOOK_BUFFOUT );
    doHookAssign( fp, SRC_HOOK_COMMAND );
    doHookAssign( fp, SRC_HOOK_MODIFIED );
    doHookAssign( fp, SRC_HOOK_MENU );
    doHookAssign( fp, SRC_HOOK_MOUSE_LINESEL );
    doHookAssign( fp, SRC_HOOK_MOUSE_CHARSEL );

    writeTitle( fp, "General Settings" );
    num = GetNumberOfTokens( SetVarTokens );
    for( i = 0; i < num; i++ ) {
        if( i == SETVAR_T_TILECOLOR || i == SETVAR_T_FIGNORE || i == SETVAR_T_FILENAME ) {
            continue;
        }
        res = GetASetVal( GetTokenStringCVT( SetVarTokens, i, token, true ), tmpstr );
        switch( i ) {
        case SETVAR_T_STATUSSTRING:
        case SETVAR_T_FILEENDSTRING:
        case SETVAR_T_HISTORYFILE:
        case SETVAR_T_TMPDIR:
            /* strings with possible spaces */
            MyFprintf( fp, "set %s" CFG_SET_SEPARATOR "\"%s\"\n", token, res );
            break;
        case SETVAR_T_GADGETSTRING:
            if( !IsGadgetStringChanged( res ) )
                break;
            // fall through
        default:
            MyFprintf( fp, "set %s" CFG_SET_SEPARATOR "%s\n", token, res );
            break;
        }
    }

    writeTitle( fp, "Boolean Settings" );
    num = GetNumberOfTokens( SetFlagTokens );
    for( i = 0; i < num; i++ ) {
        str = GetASetVal( GetTokenStringCVT( SetFlagTokens, i, token, true ), tmpstr );
        MyFprintf( fp, "set %s%s\n", (*str == '0') ? "no" : "", token );
    }
    writeTitle( fp, "Match pairs" );
    for( i = INITIAL_MATCH_COUNT; i < MatchCount; i += 2 ) {
        MyFprintf( fp, "match /" );
        outputMatchData( fp, MatchData[i] );
        outputMatchData( fp, MatchData[i + 1] );
        MyFprintf( fp, "\n" );
    }

    writeTitle( fp, "Command Mode Mappings" );
    doMaps( fp, KeyMaps, "" );
    writeTitle( fp, "Insert Mode Mappings" );
    doMaps( fp, InputKeyMaps, "!" );

    writeTitle( fp, "Color Settings" );
    for( i = 0; i < GetNumColors(); i++ ) {
        if( GetColorSetting( i, &c ) ) {
            MyFprintf( fp, "setcolor %d %d %d %d\n", i, c.red, c.green, c.blue );
        }
    }

#ifdef __WIN__
    writeTitle( fp, "Font Settings" );
    BarfFontData( fp );
#endif

    writeTitle( fp, "Window Configuration" );
    doWindow( fp, PCL_T_COMMANDWINDOW, &cmdlinew_info, false );
    doWindow( fp, PCL_T_STATUSWINDOW, &statusw_info, false );
    doWindow( fp, PCL_T_COUNTWINDOW, &repcntw_info, false );
    doWindow( fp, PCL_T_EDITWINDOW, &editw_info, false );
    doWindow( fp, PCL_T_FILECWINDOW, &filecw_info, false );
    doWindow( fp, PCL_T_DIRWINDOW, &dirw_info, false );
    doWindow( fp, PCL_T_FILEWINDOW, &filelistw_info, false );
    doWindow( fp, PCL_T_MESSAGEWINDOW, &messagew_info, false );
#ifndef __WIN__
    doWindow( fp, PCL_T_SETWINDOW, &setw_info, false );
    doWindow( fp, PCL_T_LINENUMBERWINDOW, &linenumw_info, false );
    doWindow( fp, PCL_T_EXTRAINFOWINDOW, &extraw_info, false );
    doWindow( fp, PCL_T_SETVALWINDOW, &setvalw_info, false );
    doWindow( fp, PCL_T_MENUWINDOW, &menuw_info, false );
    doWindow( fp, PCL_T_MENUBARWINDOW, &menubarw_info, true );
    doWindow( fp, PCL_T_ACTIVEMENUWINDOW, &activemenu_info, true );
    doWindow( fp, PCL_T_GREYEDMENUWINDOW, &greyedmenu_info, true );
    doWindow( fp, PCL_T_ACTIVEGREYEDMENUWINDOW, &activegreyedmenu_info, true );
#endif

    writeTitle( fp, "Menu Configuration" );
    BarfMenuData( fp );

#ifdef __WIN__
    writeTitle( fp, "ToolBar Configuration" );
    BarfToolBarData( fp );
#endif

    writeTitle( fp, "File Type Source" );
    FTSBarfData( fp );

    fclose( fp );
    if( is_cmdline ) {
        Message1( "Configuration file \"%s\" generated", fname );
    }

    MemFree( buff );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* GenerateConfiguration */
