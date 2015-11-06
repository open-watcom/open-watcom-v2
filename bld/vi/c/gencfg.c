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
static void writeTitle( FILE *f, const char *str )
{
    if( isCmdLine ) {
        MyFprintf( f,"\n#\n# %s\n#\n", str );
    }

} /* writeTitle */

/*
 * doMaps - write map commands
 */
static void doMaps( FILE *f, key_map *maps, const char *extra_str )
{
    char        *map;
    vi_key      *keymap;
    int         i;
    int         j;

    for( i = 0; i < MAX_EVENTS; i++ ) {
        if( maps[i].data != NULL ) {
            MyFprintf( f, "map%s ", extra_str );
            map = LookUpCharToken( i, false );
            if( map == NULL ) {
                MyFprintf( f, "%c ", i );
            } else {
                MyFprintf( f, "%s ", map );
            }
            if( maps[i].no_input_window ) {
                MyFprintf( f, "\\x" );
            }
            keymap = maps[i].data;
            for( j = 0; keymap[j] != VI_KEY( NULL ); j++ ) {
                map = LookUpCharToken( keymap[j], true );
                if( map == NULL ) {
                    MyFprintf( f, "%c", (char)keymap[j] );
                } else {
                    if( map[1] == '\0' ) {
                        MyFprintf( f, "\\%s", map );
                    } else {
                        MyFprintf( f, "\\<%s>", map );
                    }
                }
            }
            MyFprintf( f, "\n" );
        }
    }

} /* doMaps */

/*
 * doWindow - output settings for a window
 */
static void doWindow( FILE *f, int id, window_info *wi, bool colour_only )
{
    char        token[64];

    MyFprintf( f, "%s\n", GetTokenStringCVT( TokensCmdLine, id, token, true ) );
    if( !colour_only ) {
        MyFprintf( f, "    dimension %d %d %d %d\n", wi->x1, wi->y1,
                   wi->x2, wi->y2 );
        if( wi->has_border ) {
            MyFprintf( f, "    border 1 %d %d\n", wi->border_color1, wi->border_color2 );
        } else {
            MyFprintf( f, "    border -1\n" );
        }
    }
    MyFprintf( f, "    text %d %d %d\n", wi->text.foreground, wi->text.background,
               wi->text.font );
    MyFprintf( f, "    hilight %d %d %d\n", wi->hilight.foreground,
               wi->hilight.background, wi->hilight.font );
    if( wi == &editw_info ) {
        MyFprintf( f, "    whitespace %d %d %d\n", SEType[SE_WHITESPACE].foreground,
                   SEType[SE_WHITESPACE].background, SEType[SE_WHITESPACE].font );
        MyFprintf( f, "    selection %d %d %d\n", SEType[SE_SELECTION].foreground,
                   SEType[SE_SELECTION].background, SEType[SE_SELECTION].font );
        MyFprintf( f, "    eoftext %d %d %d\n", SEType[SE_EOFTEXT].foreground,
                   SEType[SE_EOFTEXT].background, SEType[SE_EOFTEXT].font );
        MyFprintf( f, "    keyword %d %d %d\n", SEType[SE_KEYWORD].foreground,
                   SEType[SE_KEYWORD].background, SEType[SE_KEYWORD].font );
        MyFprintf( f, "    octal %d %d %d\n", SEType[SE_OCTAL].foreground,
                   SEType[SE_OCTAL].background, SEType[SE_OCTAL].font );
        MyFprintf( f, "    hex %d %d %d\n", SEType[SE_HEX].foreground,
                   SEType[SE_HEX].background, SEType[SE_HEX].font );
        MyFprintf( f, "    integer %d %d %d\n", SEType[SE_INTEGER].foreground,
                   SEType[SE_INTEGER].background, SEType[SE_INTEGER].font );
        MyFprintf( f, "    char %d %d %d\n", SEType[SE_CHAR].foreground,
                   SEType[SE_CHAR].background, SEType[SE_CHAR].font );
        MyFprintf( f, "    preprocessor %d %d %d\n", SEType[SE_PREPROCESSOR].foreground,
                   SEType[SE_PREPROCESSOR].background, SEType[SE_PREPROCESSOR].font );
        MyFprintf( f, "    symbol %d %d %d\n", SEType[SE_SYMBOL].foreground,
                   SEType[SE_SYMBOL].background, SEType[SE_SYMBOL].font );
        MyFprintf( f, "    invalidtext %d %d %d\n", SEType[SE_INVALIDTEXT].foreground,
                   SEType[SE_INVALIDTEXT].background, SEType[SE_INVALIDTEXT].font );
        MyFprintf( f, "    identifier %d %d %d\n", SEType[SE_IDENTIFIER].foreground,
                   SEType[SE_IDENTIFIER].background, SEType[SE_IDENTIFIER].font );
        MyFprintf( f, "    jumplabel %d %d %d\n", SEType[SE_JUMPLABEL].foreground,
                   SEType[SE_JUMPLABEL].background, SEType[SE_JUMPLABEL].font );
        MyFprintf( f, "    comment %d %d %d\n", SEType[SE_COMMENT].foreground,
                   SEType[SE_COMMENT].background, SEType[SE_COMMENT].font );
        MyFprintf( f, "    float %d %d %d\n", SEType[SE_FLOAT].foreground,
                   SEType[SE_FLOAT].background, SEType[SE_FLOAT].font );
        MyFprintf( f, "    string %d %d %d\n", SEType[SE_STRING].foreground,
                   SEType[SE_STRING].background, SEType[SE_STRING].font );
        MyFprintf( f, "    variable %d %d %d\n", SEType[SE_VARIABLE].foreground,
                   SEType[SE_VARIABLE].background, SEType[SE_VARIABLE].font );
        MyFprintf( f, "    regexp %d %d %d\n", SEType[SE_REGEXP].foreground,
                   SEType[SE_VARIABLE].background, SEType[SE_VARIABLE].font );
    }
    MyFprintf( f, "endwindow\n" );

} /* doWindow */

/*
 * outputMatchData - write out data associated with a match string
 */
static void outputMatchData( FILE *f, const char *str )
{
    for( ; *str != '\0'; ++str ) {
        if( *str == '/' ) {
            fputc( '\\', f );
        }
        fputc( *str, f );
    }
    fputc( '/', f );

} /* outputMatchData */


/*
 * doHookAssign - do an assignment for a hook variable
 */
static void doHookAssign( FILE *f, hooktype num )
{
    vars        *v;

    v = GetHookVar( num );
    if( v == NULL ) {
        return;
    }
    MyFprintf( f, "assign %%(%s) = %s\n", v->name, v->value );
    MyFprintf( f, "load %s\n", v->value );

} /* doHookAssign */

/*
 * GenerateConfiguration - write out a config file
 */
vi_rc GenerateConfiguration( const char *fname, bool is_cmdline )
{
    FILE        *f;
    int         i;
    char        token[128];
    char        *str;
    char        boolstr[3];
    char        *buff;
    int         num;
    rgb         c;
    char        *res;

    if( fname == NULL ) {
        fname = CFG_NAME;
    }
    f = fopen( fname, "w" );
    if( f == NULL ) {
        return( ERR_FILE_OPEN );
    }
    isCmdLine = is_cmdline;
    buff = MemAllocUnsafe( VBUF_SIZE );
    if( buff != NULL ) {
        setvbuf( f, buff, _IOFBF, VBUF_SIZE );
    }
    MyFprintf( f, "#\n# %s configuration file\n# %s\n#\n", 
#if defined( __WIN__ )
    banner1w1( "Text Editor for Windows" ), banner1w2( _VI_VERSION_ ) );
#else
    banner1w1( "Vi Text Editor" ), banner1w2( _VI_VERSION_ ) );
#endif
    if( is_cmdline ) {
        GetDateTimeString( token );
        MyFprintf( f, "# File generated on %s\n#\n", token );
    }

    writeTitle( f, "Hook script assignments" );
    doHookAssign( f, SRC_HOOK_WRITE );
    doHookAssign( f, SRC_HOOK_READ );
    doHookAssign( f, SRC_HOOK_BUFFIN );
    doHookAssign( f, SRC_HOOK_BUFFOUT );
    doHookAssign( f, SRC_HOOK_COMMAND );
    doHookAssign( f, SRC_HOOK_MODIFIED );
    doHookAssign( f, SRC_HOOK_MENU );
    doHookAssign( f, SRC_HOOK_MOUSE_LINESEL );
    doHookAssign( f, SRC_HOOK_MOUSE_CHARSEL );

    writeTitle( f, "General Settings" );
    num = GetNumberOfTokens( TokensSetVar );
    for( i = 0; i < num; i++ ) {
        if( i == SETVAR_T_TILECOLOR || i == SETVAR_T_FIGNORE || i == SETVAR_T_FILENAME ) {
            continue;
        }
        res = GetASetVal( GetTokenStringCVT( TokensSetVar, i, token, true ) );
        switch( i ) {
        case SETVAR_T_STATUSSTRING:
        case SETVAR_T_FILEENDSTRING:
        case SETVAR_T_HISTORYFILE:
        case SETVAR_T_TMPDIR:
            /* strings with possible spaces */
            MyFprintf( f, "set %s = \"%s\"\n", token, res );
            break;
        case SETVAR_T_GADGETSTRING:
            if( !IsGadgetStringChanged( res ) )
                 break;
            // fall through
        default:
            MyFprintf( f, "set %s = %s\n", token, res );
            break;
        }
    }

    writeTitle( f, "Boolean Settings" );
    num = GetNumberOfTokens( TokensSetFlag );
    for( i = 0; i < num; i++ ) {
        str = GetASetVal( GetTokenStringCVT( TokensSetFlag, i, token, true ) );
        boolstr[0] = 0;
        if( str[0] == '0' ) {
            boolstr[0] = 'n';
            boolstr[1] = 'o';
            boolstr[2] = 0;
        }
        MyFprintf( f, "set %s%s\n", boolstr, token );
    }
    writeTitle( f, "Match pairs" );
    for( i = INITIAL_MATCH_COUNT; i < MatchCount; i += 2 ) {
        MyFprintf( f, "match /" );
        outputMatchData( f, MatchData[i] );
        outputMatchData( f, MatchData[i + 1] );
        MyFprintf( f, "\n" );
    }

    writeTitle( f, "Command Mode Mappings" );
    doMaps( f, KeyMaps, "" );
    writeTitle( f, "Insert Mode Mappings" );
    doMaps( f, InputKeyMaps, "!" );

    writeTitle( f, "Color Settings" );
    for( i = 0; i < GetNumColors(); i++ ) {
        if( GetColorSetting( i, &c ) ) {
            MyFprintf( f, "setcolor %d %d %d %d\n", i, c.red, c.green, c.blue );
        }
    }

#ifdef __WIN__
    writeTitle( f, "Font Settings" );
    BarfFontData( f );
#endif

    writeTitle( f, "Window Configuration" );
    doWindow( f, PCL_T_COMMANDWINDOW, &cmdlinew_info, false );
    doWindow( f, PCL_T_STATUSWINDOW, &statusw_info, false );
    doWindow( f, PCL_T_COUNTWINDOW, &repcntw_info, false );
    doWindow( f, PCL_T_EDITWINDOW, &editw_info, false );
    doWindow( f, PCL_T_FILECWINDOW, &filecw_info, false );
    doWindow( f, PCL_T_DIRWINDOW, &dirw_info, false );
    doWindow( f, PCL_T_FILEWINDOW, &filelistw_info, false );
    doWindow( f, PCL_T_MESSAGEWINDOW, &messagew_info, false );
#ifndef __WIN__
    doWindow( f, PCL_T_SETWINDOW, &setw_info, false );
    doWindow( f, PCL_T_LINENUMBERWINDOW, &linenumw_info, false );
    doWindow( f, PCL_T_EXTRAINFOWINDOW, &extraw_info, false );
    doWindow( f, PCL_T_SETVALWINDOW, &setvalw_info, false );
    doWindow( f, PCL_T_MENUWINDOW, &menuw_info, false );
    doWindow( f, PCL_T_MENUBARWINDOW, &menubarw_info, true );
    doWindow( f, PCL_T_ACTIVEMENUWINDOW, &activemenu_info, true );
    doWindow( f, PCL_T_GREYEDMENUWINDOW, &greyedmenu_info, true );
    doWindow( f, PCL_T_ACTIVEGREYEDMENUWINDOW, &activegreyedmenu_info, true );
#endif

    writeTitle( f, "Menu Configuration" );
    BarfMenuData( f );

#ifdef __WIN__
    writeTitle( f, "ToolBar Configuration" );
    BarfToolBarData( f );
#endif

    writeTitle( f, "File Type Source" );
    FTSBarfData( f );

    fclose( f );
    if( is_cmdline ) {
        Message1( "Configuration file \"%s\" generated", fname );
    }

    MemFree( buff );
    return( DO_NOT_CLEAR_MESSAGE_WINDOW );

} /* GenerateConfiguration */
