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
#include <setjmp.h>
#include "parsecl.h"
#include "expr.h"
#include "rxsupp.h"
#include "fcbmem.h"
#include "win.h"
#include "menu.h"
#include "source.h"

#ifndef VICOMP

static bool msgFlag;
static bool needsRedisplay = FALSE;
static char msgString[MAX_STR];

static void setMessage( char *msg, bool redisplay )
{
    strcpy( msgString, msg );
    needsRedisplay = redisplay;
}

static void putMessage( void )
{
    if( needsRedisplay ) {
        ReDisplayScreen();
        needsRedisplay = FALSE;
    }
    Message1( msgString );
}

/*
 * getOneSetVal - get a single set value
 */
static char *getOneSetVal( int token, bool isnonbool, char *tmpstr,
                           bool want_boolstr )
{
    char        *str, *fign;
    cursor_type ct;
    int         i, j;

    tmpstr[0] = 0;
    if( !isnonbool ) {
        j = (int) ((bool *)&EditFlags)[token];
        if( want_boolstr ) {
            str = BoolStr[j];
        } else {
            str = tmpstr;
            sprintf( tmpstr, "%d", j );
        }
    } else {
        switch( token ) {
        case SETVAR_T_STATUSSECTIONS:
            str = tmpstr;
            *str = 0;
            for( i = 0; i < EditVars.NumStatusSections; i++ ) {
                char        buff[16];
                sprintf( buff, "%d ", EditVars.StatusSections[i] );
                strcat( str, buff );
            }
            break;
        case SETVAR_T_FILEENDSTRING:
            str = EditVars.FileEndString;
            break;
        case SETVAR_T_STATUSSTRING:
            str = EditVars.StatusString;
            break;
        case SETVAR_T_TILECOLOR:
            str = tmpstr;
            break;
        case SETVAR_T_FIGNORE:
            fign = EditVars.FIgnore;
            str = tmpstr;
            for( j = 0; j < EditVars.CurrFIgnore; j++ ) {
                strcat( str, fign );
                fign += EXTENSION_LENGTH;
            }
            break;
        case SETVAR_T_GADGETSTRING:
            str = EditVars.GadgetString;
            break;
        case SETVAR_T_SHELLPROMPT:
            str = EditVars.SpawnPrompt;
            break;
        case SETVAR_T_GREPDEFAULT:
            str = EditVars.GrepDefault;
            break;
        case SETVAR_T_TMPDIR:
            str = EditVars.TmpDir;
            break;
        case SETVAR_T_WORD:
            str = EditVars.WordDefn;
            break;
        case SETVAR_T_WORDALT:
            str = EditVars.WordAltDefn;
            break;
        case SETVAR_T_FILENAME:
            if( CurrentFile == NULL ) {
                str = "";
            } else {
                str = CurrentFile->name;
            }
            break;
        case SETVAR_T_HISTORYFILE:
            str = EditVars.HistoryFile;
            break;
        case SETVAR_T_TAGFILENAME:
            str = EditVars.TagFileName;
            break;
        case SETVAR_T_MAGICSTRING:
            str = EditVars.Majick;
            break;
        case SETVAR_T_COMMANDCURSORTYPE:
        case SETVAR_T_OVERSTRIKECURSORTYPE:
        case SETVAR_T_INSERTCURSORTYPE:
            if( token == SETVAR_T_COMMANDCURSORTYPE ) {
                ct = EditVars.NormalCursorType;
            } else if( token == SETVAR_T_OVERSTRIKECURSORTYPE ) {
                ct = EditVars.OverstrikeCursorType;
            } else {
                ct = EditVars.InsertCursorType;
            }
            str = tmpstr;
            MySprintf( tmpstr, "%d %d", ct.height, ct.width );
            break;
        default:
            j = 0;
            switch( token ) {
            case SETVAR_T_WRAPMARGIN:
                j = EditVars.WrapMargin;
                break;
            case SETVAR_T_CURSORBLINKRATE:
                j = EditVars.CursorBlinkRate;
                break;
            case SETVAR_T_MAXPUSH:
                j = EditVars.MaxPush;
                break;
            case SETVAR_T_RADIX:
                j = EditVars.Radix;
                break;
            case SETVAR_T_AUTOSAVEINTERVAL:
                j = EditVars.AutoSaveInterval;
                break;
            case SETVAR_T_LANGUAGE:
                if( CurrentInfo == NULL ) {
                    j = LANG_NONE;
                } else {
                    j = CurrentInfo->fsi.Language;
                }
                break;
            case SETVAR_T_MOUSEDCLICKSPEED:
                j = EditVars.MouseDoubleClickSpeed;
                break;
            case SETVAR_T_MOUSESPEED:
                j = EditVars.MouseSpeed;
                break;
            case SETVAR_T_MOUSEREPEATDELAY:
                j = EditVars.MouseRepeatDelay;
                break;
            case SETVAR_T_CURRENTSTATUSCOLUMN:
                j = EditVars.CurrentStatusColumn;
                break;
            case SETVAR_T_ENDOFLINECHAR:
                j = EditVars.EndOfLineChar;
                break;
            case SETVAR_T_EXITATTR:
                j = EditVars.ExitAttr;
                break;
            case SETVAR_T_MAXSWAPK:
                j = EditVars.MaxSwapBlocks;
                j *= (MAX_IO_BUFFER / 1024);
                break;
            case SETVAR_T_MAXEMSK:
#ifndef NOEMS
                j = EditVars.MaxEMSBlocks;
                j *= (MAX_IO_BUFFER / 1024);
#endif
                break;
            case SETVAR_T_MAXXMSK:
#ifndef NOXMS
                j = EditVars.MaxXMSBlocks;
                j *= (MAX_IO_BUFFER / 1024);
#endif
                break;
            case SETVAR_T_RESIZECOLOR:
                j = EditVars.ResizeColor;
                break;
            case SETVAR_T_MOVECOLOR:
                j = EditVars.MoveColor;
                break;
            case SETVAR_T_INACTIVEWINDOWCOLOR:
                j = EditVars.InactiveWindowColor;
                break;
            case SETVAR_T_MAXTILECOLORS:
                j = EditVars.MaxTileColors;
                break;
            case SETVAR_T_MAXWINDOWTILEX:
                j = EditVars.MaxWindowTileX;
                break;
            case SETVAR_T_MAXWINDOWTILEY:
                j = EditVars.MaxWindowTileY;
                break;
            case SETVAR_T_HARDTAB:
                j = EditVars.HardTab;
                break;
            case SETVAR_T_TABAMOUNT:
                j = EditVars.TabAmount;
                break;
            case SETVAR_T_SHIFTWIDTH:
                j = EditVars.ShiftWidth;
                break;
            case SETVAR_T_STACKK:
                j = EditVars.StackK;
                break;
            case SETVAR_T_LINENUMWINWIDTH:
                j = EditVars.LineNumWinWidth;
                break;
            case SETVAR_T_CLOCKX:
                j = EditVars.ClockX;
                break;
            case SETVAR_T_CLOCKY:
                j = EditVars.ClockY;
                break;
            case SETVAR_T_SPINX:
                j = EditVars.SpinX;
                break;
            case SETVAR_T_SPINY:
                j = EditVars.SpinY;
                break;
            case SETVAR_T_MAXCLHISTORY:
                j = EditVars.CLHist.max;
                break;
            case SETVAR_T_MAXFILTERHISTORY:
                j = EditVars.FilterHist.max;
                break;
            case SETVAR_T_MAXFINDHISTORY:
                j = EditVars.FindHist.max;
                break;
            case SETVAR_T_MAXLASTFILESHISTORY:
                j = EditVars.LastFilesHist.max;
                break;
            case SETVAR_T_MAXLINELEN:
                j = EditVars.MaxLine;
                break;
            case SETVAR_T_PAGELINESEXPOSED:
                j = EditVars.PageLinesExposed;
                break;
            case SETVAR_T_TOOLBARBUTTONHEIGHT:
                j = EditVars.ToolBarButtonHeight;
                break;
            case SETVAR_T_TOOLBARBUTTONWIDTH:
                j = EditVars.ToolBarButtonWidth;
                break;
            case SETVAR_T_TOOLBARCOLOR:
                j = EditVars.ToolBarColor;
                break;
            }
            str = tmpstr;
            sprintf( tmpstr, "%d ", j );
            break;
        }
    }
    if( str == NULL ) {
        return( "" );
    }
    return( str );

} /* getOneSetVal */

/*
 * GetNewValueDialog - get a new value from the user
 */
#ifndef __WIN__
vi_rc GetNewValueDialog( char *value )
{
    bool        ret;
    vi_rc       rc;
    char        st[MAX_STR];
    window_id   clw;
    static char prompt[] = "New:";

    rc = NewWindow2( &clw, &setvalw_info );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    WPrintfLine( clw, 1, "Old: %s", value );
    ret = ReadStringInWindow( clw, 2, prompt, st, MAX_STR - 1, NULL );
    CloseAWindow( clw );
    SetWindowCursor();
    KillCursor();
    if( !ret ) {
        return( NO_VALUE_ENTERED );
    }
    if( st[0] == 0 ) {
        return( NO_VALUE_ENTERED );
    }
    Expand( st, NULL );
    strcpy( value, st );
    return( ERR_NO_ERR );

} /* GetNewValueDialog */
#endif
#endif /* VICOMP */


/*
 * processSetToken - set value for set token
 */
static vi_rc processSetToken( int j, char *value, int *winflag, bool isnonbool )
{
    char        fn[MAX_STR], str[MAX_STR];
#ifndef VICOMP
    char        tmp[3];
    char        settokstr[TOK_MAX_LEN + 1];
    char        save[MAX_STR];
    vi_rc       rc = ERR_NO_ERR;
    int         clr;
    bool        newset;
    bool        set_bool_flag, toggle, *ptr;
    jmp_buf     jmpaddr;
    cursor_type ct;
    char        *name;
    command_rtn fptr;
    event_bits  eb;
    bool        redisplay = FALSE;
#endif
    bool        bvalue;
    int         i, k;

#ifdef VICOMP
    winflag = winflag;
    isnonbool = isnonbool;
#endif
    /*
     * set up value for boolean set commands
     */
    if( j < 0 ) {
        j *= -1;
        bvalue = FALSE;
    } else {
        bvalue = TRUE;
    }
#ifndef VICOMP
    if( !(*winflag) ) {
        toggle = TRUE;
        set_bool_flag = !isnonbool;
    } else {
        toggle = FALSE;
#endif
        if( j >= SETVAR_T_ ) {
#ifndef VICOMP
            if( EditFlags.CompileScript ) {
#endif
                if( !bvalue ) {
                    j *= -1;
                }
                sprintf( str, "%d", j );
                StrMerge( 2, WorkLine->data, str, SingleBlank );
                return( ERR_NO_ERR );
#ifndef VICOMP
            }
            set_bool_flag = TRUE;
            j -= SETVAR_T_;
        } else {
            set_bool_flag = FALSE;
#endif
        }
#ifndef VICOMP
    }
    *winflag = FALSE;

    /*
     * process boolean settings
     */
    if( set_bool_flag ) {
        if( j >= SETFLAG_T_ ) {
            return( ERR_INVALID_SET_COMMAND );
        }
        ptr = &(((bool *)&EditFlags)[j]);
        newset = bvalue;
        if( toggle ) {
            newset = !(*ptr);
        }
        switch( j ) {
        case SETFLAG_T_MODELESS:
            if( (newset && !EditFlags.Modeless) ||
                (!newset && EditFlags.Modeless) ) {
                for( k = 0; k < MAX_EVENTS; k++ ) {
                    fptr = EventList[k].rtn;
                    eb = EventList[k].b;
                    EventList[k].rtn = EventList[k].alt_rtn;
                    EventList[k].alt_rtn = fptr;
                    EventList[k].b = EventList[k].alt_b;
                    EventList[k].alt_b = eb;
                }
                if( !EditFlags.Modeless ) {
                    if( MenuWindow != NO_WINDOW ) {
                        UpdateCurrentStatus( CSTATUS_INSERT );
                    }
                    EditFlags.WasOverstrike = FALSE;
                    NewCursor( CurrentWindow, EditVars.InsertCursorType );
                } else {
                    if( MenuWindow != NO_WINDOW ) {
                        UpdateCurrentStatus( CSTATUS_COMMAND );
                    }
                    NewCursor( CurrentWindow, EditVars.NormalCursorType );
                    // nomodeless must be line based or it dies!
                    EditFlags.LineBased = TRUE;
                }
                /* re-position cursor in window
                */
                SetWindowCursor();
            }
            EditFlags.Modeless = newset;
            break;
        case SETFLAG_T_UNDO:
            if( EditFlags.Undo && !newset ) {
                FreeAllUndos();
            }
            EditFlags.Undo = newset;
            break;
        case SETFLAG_T_STATUSINFO:
            EditFlags.StatusInfo = newset;
#ifdef __WIN__
            ResizeRoot();
#endif
            rc = NewStatusWindow();
            break;
        case SETFLAG_T_WINDOWGADGETS:
            EditFlags.WindowGadgets = newset;
            ResetAllWindows();
            *winflag = TRUE;
            redisplay = TRUE;
            break;
        case SETFLAG_T_REALTABS:
            EditFlags.RealTabs = newset;
            redisplay = TRUE;
            break;
        case SETFLAG_T_CLOCK:
            EditFlags.Clock = newset;
            redisplay = TRUE;
            break;
        case SETFLAG_T_TOOLBAR:
            EditFlags.Toolbar = newset;
#ifdef __WIN__
            ResizeRoot();
#endif
            break;
        case SETFLAG_T_COLORBAR:
            EditFlags.Colorbar = newset;
#ifdef __WIN__
            if( Root == NULL ) {
                EditFlags.Colorbar = FALSE;
            } else {
                RefreshColorbar();
            }
#endif
            break;
        case SETFLAG_T_SSBAR:
            EditFlags.SSbar = newset;
#ifdef __WIN__
            if( Root == NULL ) {
                EditFlags.SSbar = FALSE;
            } else {
                RefreshSSbar();
            }
#endif
            break;
        case SETFLAG_T_FONTBAR:
            EditFlags.Fontbar = newset;
#ifdef __WIN__
            if( Root == NULL ) {
                EditFlags.Fontbar = FALSE;
            } else {
                RefreshFontbar();
            }
#endif
            break;
        case SETFLAG_T_MARKLONGLINES:
            EditFlags.MarkLongLines = newset;
            break;
        case SETFLAG_T_MENUS:
            EditFlags.Menus = newset;
            InitMenu();
            break;
        case SETFLAG_T_LINENUMBERS:
            if( toggle ) {
                newset = !EditFlags.LineNumbers;
            }
            if( newset != EditFlags.LineNumbers ) {
                EditFlags.LineNumbers = newset;
                rc = LineNumbersSetup();
                *winflag = TRUE;
            }
            break;
        case SETFLAG_T_CURRENTSTATUS:
            EditFlags.CurrentStatus = newset;
            InitMenu();
            break;
        case SETFLAG_T_DISPLAYSECONDS:
            EditFlags.DisplaySeconds = newset;
            redisplay = TRUE;
            break;
        case SETFLAG_T_PPKEYWORDONLY:
            EditFlags.PPKeywordOnly = newset;
            redisplay = TRUE;
            break;
        case SETFLAG_T_LASTEOL:
#ifndef __WIN__
            *ptr = TRUE;
            toggle = FALSE;
            break;
#endif
        default:
            *ptr = newset;
            break;
        }
        if( msgFlag ) {
            if( !newset ) {
                tmp[0] = 'n';
                tmp[1] = 'o';
                tmp[2] = 0;
            } else {
                tmp[0] = 0;
            }
            MySprintf( fn, "%s%s set", tmp, GetTokenStringCVT( TokensSetFlag, j, settokstr, TRUE ) );
        }
        if( toggle ) {
            strcpy( save, BoolStr[(int) newset] );
            (*winflag) += 1;
        }

    /*
     * process value settings
     */
    } else {
        if( toggle ) {
            rc = GetNewValueDialog( value );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            strcpy( save, value );
        }
#endif /* VICOMP */
        RemoveLeadingSpaces( value );
#ifndef VICOMP
        if( !EditFlags.ScriptIsCompiled ) {
#endif
            if( value[0] == '=' ) {
                EliminateFirstN( value, 1 );
                RemoveLeadingSpaces( value );
            }
#ifndef VICOMP
        }
#endif
        if( value[0] == '"' ) {
            NextWord( value, fn, "\"" );
            EliminateFirstN( value, 1 );
        } else {
            k = strlen( value );
            for( i = 0; i < k; i++ ) {
                if( value[i] == ' ' )
                    break;
                if( value[i] == ',' ) {
                    value[i] = ' ';
                    break;
                }
            }
            NextWord1( value, fn );
        }
        k = strlen( value );
        for( i = 0; i < k; i++ ) {
            if( value[i] == ',' ) {
                value[i] = ' ';
            }
        }
#ifndef VICOMP
        if( EditFlags.CompileScript ) {
#endif
            sprintf( str, "%d", j );
            strcat( WorkLine->data, str );
            if( fn[0] == '\0' )
                return( ERR_NO_ERR );
            switch( j ) {
            case SETVAR_T_STATUSSTRING:
            case SETVAR_T_FILEENDSTRING:
            case SETVAR_T_HISTORYFILE:
            case SETVAR_T_TMPDIR:
            case SETVAR_T_TAGFILENAME:
                StrMerge( 4, WorkLine->data, SingleBlank, SingleQuote, fn, SingleQuote );
                break;
            case SETVAR_T_COMMANDCURSORTYPE:
            case SETVAR_T_OVERSTRIKECURSORTYPE:
            case SETVAR_T_INSERTCURSORTYPE:
                StrMerge( 2, WorkLine->data, SingleBlank, fn );
                if( NextWord1( value, fn ) <= 0 ) {
                    break;
                }
                StrMerge( 2, WorkLine->data, SingleBlank, fn );
                break;
            case SETVAR_T_TILECOLOR:
                StrMerge( 2, WorkLine->data, SingleBlank, fn );
                if( NextWord1( value, fn ) <= 0 ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                if( NextWord1( value, str ) <= 0 ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                StrMerge( 4, WorkLine->data, fn, SingleBlank, str, SingleBlank );
                break;
            case SETVAR_T_STATUSSECTIONS:
                StrMerge( 2, WorkLine->data, SingleBlank, fn );
                while( NextWord1( value, fn ) > 0 ) {
#ifdef VICOMP
                    int k;
#endif
                    k = atoi( fn );
                    if( k <= 0 ) {
                        break;
                    }
                    StrMerge( 2, WorkLine->data, SingleBlank, fn );
                }
                break;
            default:
                StrMerge( 2, WorkLine->data, SingleBlank, fn );
                break;
            }
            return( ERR_NO_ERR );
#ifndef VICOMP
        }
        switch( j ) {
        case SETVAR_T_STATUSSECTIONS:
            if( EditVars.StatusSections != NULL ) {
                MemFree( EditVars.StatusSections );
                EditVars.StatusSections = NULL;
                EditVars.NumStatusSections = 0;
            }
            for( ;; ) {
                k = atoi( fn );
                if( k <= 0 ) {
                    break;
                }
                EditVars.StatusSections = MemReAlloc( EditVars.StatusSections, sizeof( short ) * (EditVars.NumStatusSections + 1) );
                EditVars.StatusSections[EditVars.NumStatusSections] = k;
                EditVars.NumStatusSections++;
                if( NextWord1( value, fn ) <= 0 ) {
                    break;
                }
            }
            if( EditVars.StatusSections == NULL ) {
                MySprintf( fn, "statussections turned off" );
            } else {
                MySprintf( fn, "statussections set" );
            }
            break;
        case SETVAR_T_FILEENDSTRING:
            AddString2( &EditVars.FileEndString, fn );
            ResetAllWindows();
            redisplay = TRUE;
            break;
        case SETVAR_T_STATUSSTRING:
            AddString2( &EditVars.StatusString, fn );
            if( StatusWindow != NO_WINDOW ) {
                ClearWindow( StatusWindow );
                UpdateStatusWindow();
            }
            if( msgFlag ) {
                MySprintf( fn, "statusstring set to %s", EditVars.StatusString );
            }
            break;
        case SETVAR_T_GREPDEFAULT:
            AddString2( &EditVars.GrepDefault, fn );
            break;
        case SETVAR_T_TILECOLOR:
            if( EditVars.TileColors == NULL ) {
                EditVars.TileColors = (type_style *) MemAlloc( sizeof( type_style ) * ( EditVars.MaxTileColors + 1 ) );
                for( i = 0; i <= EditVars.MaxTileColors; ++i ) {
                    EditVars.TileColors[i].foreground = -1;
                    EditVars.TileColors[i].background = -1;
                    EditVars.TileColors[i].font = -1;
                }
            }
            clr = atoi( fn );
            if( clr > EditVars.MaxTileColors ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            if( NextWord1( value, fn ) <= 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            EditVars.TileColors[clr].foreground = atoi( fn );
            if( NextWord1( value, fn ) <= 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            EditVars.TileColors[clr].background = atoi( fn );
            EditVars.TileColors[clr].font = FONT_DEFAULT;
            if( msgFlag ) {
                MySprintf( fn, "tilecolor %d set", clr );
            }
            break;
        case SETVAR_T_GADGETSTRING:
            SetGadgetString( fn );
            if( msgFlag ) {
                MySprintf( fn, "gadget string set to %s", EditVars.GadgetString );
            }
            ResetAllWindows();
            break;
        case SETVAR_T_SHELLPROMPT:
            AddString2( &EditVars.SpawnPrompt, fn );
            if( msgFlag ) {
                MySprintf( fn, "prompt string set to %s", EditVars.SpawnPrompt );
            }
            break;
        case SETVAR_T_FIGNORE:
            if( fn[0] == 0 ) {
                MemFreePtr( (void **)&EditVars.FIgnore );
                EditVars.CurrFIgnore = 0;
                if( msgFlag ) {
                    MySprintf( fn, "fignore reset" );
                }
            } else {
                EditVars.FIgnore = MemReAlloc( EditVars.FIgnore, EXTENSION_LENGTH * (EditVars.CurrFIgnore + 1) );
                str[0] = '.';
                str[1] = 0;
                strcat( str, fn );
                str[EXTENSION_LENGTH - 1] = 0;
                strcpy( &EditVars.FIgnore[EXTENSION_LENGTH * EditVars.CurrFIgnore], str );
                EditVars.CurrFIgnore++;
                if( msgFlag ) {
                    MySprintf( fn, "%s added to fignore", str );
                }
            }
            break;
        case SETVAR_T_HISTORYFILE:
            AddString2( &EditVars.HistoryFile, fn );
            if( msgFlag ) {
                MySprintf( fn, "history file set to %s", EditVars.HistoryFile );
            }
            break;

        case SETVAR_T_TAGFILENAME:
            AddString2( &EditVars.TagFileName, fn );
            if( msgFlag ) {
                MySprintf( fn, "tag file name set to %s", EditVars.TagFileName );
            }
            break;

        case SETVAR_T_FILENAME:
            if( CurrentFile != NULL ) {
                AddString2( &(CurrentFile->name), fn );
                SetFileWindowTitle( CurrentWindow, CurrentInfo, TRUE );
                if( msgFlag ) {
                    MySprintf( fn, "filename set to %s", CurrentFile->name );
                }
                FileSPVAR();
            }
            break;
        case SETVAR_T_TMPDIR:
            AddString2( &EditVars.TmpDir, fn );
            VerifyTmpDir();
            if( msgFlag ) {
                MySprintf( fn, "tmpdir set to %s", EditVars.TmpDir );
            }
            break;
        case SETVAR_T_WORD:
            AddString2( &EditVars.WordDefn, fn );
            InitWordSearch( EditVars.WordDefn );
            if( msgFlag ) {
                MySprintf( fn, "word set to %s", EditVars.WordDefn );
            }
            break;
        case SETVAR_T_WORDALT:
            AddString2( &EditVars.WordAltDefn, fn );
            if( msgFlag ) {
                MySprintf( fn, "wordalt set to %s", EditVars.WordAltDefn );
            }
            break;
        case SETVAR_T_MAGICSTRING:
            AddString2( &EditVars.Majick, fn );
            if( msgFlag ) {
                MySprintf( fn, "magicstring set to %s", EditVars.Majick );
            }
            break;
        case SETVAR_T_COMMANDCURSORTYPE:
        case SETVAR_T_OVERSTRIKECURSORTYPE:
        case SETVAR_T_INSERTCURSORTYPE:
            i = setjmp( jmpaddr );
            if( i != 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            StartExprParse( fn, jmpaddr );
            ct.height = GetConstExpr();
            if( NextWord1( value, fn ) <= 0 ) {
                ct.width = 100;
            } else {
                i = setjmp( jmpaddr );
                if( i != 0 ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                StartExprParse( fn, jmpaddr );
                ct.width = GetConstExpr();
            }
            if( j == SETVAR_T_COMMANDCURSORTYPE ) {
                EditVars.NormalCursorType = ct;
                name = "command";
            } else if( j == SETVAR_T_OVERSTRIKECURSORTYPE ) {
                EditVars.OverstrikeCursorType = ct;
                name = "overstrike";
            } else {
                EditVars.InsertCursorType = ct;
                name = "insert";
            }
            if( msgFlag ) {
                MySprintf( fn, "%s cursor type set to %d,%d", name,
                                ct.height, ct.width );
            }
            break;
        default:
            i = setjmp( jmpaddr );
            if( i != 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            StartExprParse( fn, jmpaddr );
            i = GetConstExpr();
            if( i < 0 ) {
                i = 0;
            }
            switch( j ) {
            case SETVAR_T_WRAPMARGIN:
                EditVars.WrapMargin = i;
                break;
            case SETVAR_T_CURSORBLINKRATE:
                SetCursorBlinkRate( i );
                break;
            case SETVAR_T_MAXPUSH:
                EditVars.MaxPush = i;
                if( EditVars.MaxPush < 1 ) {
                    EditVars.MaxPush = 1;
                }
                InitFileStack();
                break;
            case SETVAR_T_RADIX:
                EditVars.Radix = i;
                break;
            case SETVAR_T_AUTOSAVEINTERVAL:
                EditVars.AutoSaveInterval = i;
                SetNextAutoSaveTime();
                break;
            case SETVAR_T_LANGUAGE:
                if( i < LANG_NONE || i >= LANG_MAX ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                if( CurrentInfo != NULL ) {
                    LangFini( CurrentInfo->fsi.Language );
                    LangInit( i );
                    redisplay = TRUE;
                }
                break;
            case SETVAR_T_MOVECOLOR:
                EditVars.MoveColor = i;
                break;
            case SETVAR_T_RESIZECOLOR:
                EditVars.ResizeColor = i;
                break;
            case SETVAR_T_MOUSEDCLICKSPEED:
                EditVars.MouseDoubleClickSpeed = i;
                break;
            case SETVAR_T_MOUSESPEED:
                SetMouseSpeed( i );
                break;
            case SETVAR_T_MOUSEREPEATDELAY:
                EditVars.MouseRepeatDelay = i;
                break;
            case SETVAR_T_CURRENTSTATUSCOLUMN:
                EditVars.CurrentStatusColumn = i;
                InitMenu();
                break;
            case SETVAR_T_ENDOFLINECHAR:
                EditVars.EndOfLineChar = (unsigned char)i;
                break;
            case SETVAR_T_EXITATTR:
                EditVars.ExitAttr = (viattr_t)i;
                break;
            case SETVAR_T_INACTIVEWINDOWCOLOR:
                EditVars.InactiveWindowColor = i;
                break;
            case SETVAR_T_TABAMOUNT:
                EditVars.TabAmount = i;
                break;
            case SETVAR_T_SHIFTWIDTH:
                EditVars.ShiftWidth = i;
                break;
            case SETVAR_T_PAGELINESEXPOSED:
                EditVars.PageLinesExposed = i;
                break;
            case SETVAR_T_HARDTAB:
                EditVars.HardTab = i;
                redisplay = TRUE;
                break;
            case SETVAR_T_STACKK:
                if( EditFlags.Starting ) {
                    EditVars.StackK = i;
                }
                break;
            case SETVAR_T_LINENUMWINWIDTH:
                EditVars.LineNumWinWidth = i;
                break;
            case SETVAR_T_MAXWINDOWTILEX:
                EditVars.MaxWindowTileX = i;
                break;
            case SETVAR_T_MAXWINDOWTILEY:
                EditVars.MaxWindowTileY = i;
                break;
            case SETVAR_T_MAXSWAPK:
                SwapBlockInit( i );
                break;
            case SETVAR_T_MAXEMSK:
#ifndef NOEMS
                EMSBlockInit( i );
#endif
                break;
            case SETVAR_T_MAXXMSK:
#ifndef NOXMS
                XMSBlockInit( i );
#endif
                break;
            case SETVAR_T_MAXFILTERHISTORY:
                FilterHistInit( i );
                break;
            case SETVAR_T_MAXCLHISTORY:
                CLHistInit( i );
                break;
            case SETVAR_T_MAXFINDHISTORY:
                FindHistInit( i );
                break;
            case SETVAR_T_MAXLASTFILESHISTORY:
                LastFilesHistInit( i );
                break;
            case SETVAR_T_MAXTILECOLORS:
                k = (EditVars.TileColors == NULL) ? 0 : EditVars.MaxTileColors + 1;
                EditVars.MaxTileColors = i;
                EditVars.TileColors = MemReAlloc( EditVars.TileColors, sizeof( type_style ) * ( EditVars.MaxTileColors + 1 ) );
                for( ; k <= EditVars.MaxTileColors; ++k ) {
                    EditVars.TileColors[k].foreground = -1;
                    EditVars.TileColors[k].background = -1;
                    EditVars.TileColors[k].font = -1;
                }
                break;
            case SETVAR_T_CLOCKX:
                EditVars.ClockX = i;
                GetClockStart();
                break;
            case SETVAR_T_CLOCKY:
                EditVars.ClockY = i;
                GetClockStart();
                break;
            case SETVAR_T_SPINX:
                EditVars.SpinX = i;
                GetSpinStart();
                break;
            case SETVAR_T_SPINY:
                EditVars.SpinY = i;
                GetSpinStart();
                break;
            case SETVAR_T_MAXLINELEN:
                /* file save fails if 1 line is > MAX_IO_BUFFER */
                if( i > MAX_IO_BUFFER )
                    i = MAX_IO_BUFFER;
                EditVars.MaxLine = i;
                StaticStart();
                WorkLine = MemReAlloc( WorkLine, sizeof( line ) + EditVars.MaxLine + 2 );
                break;
            case SETVAR_T_TOOLBARBUTTONHEIGHT:
                EditVars.ToolBarButtonHeight = i;
#ifdef __WIN__
                ResizeRoot();
#endif
                break;
            case SETVAR_T_TOOLBARBUTTONWIDTH:
                EditVars.ToolBarButtonWidth = i;
#ifdef __WIN__
                ResizeRoot();
#endif
                break;
            case SETVAR_T_TOOLBARCOLOR:
                EditVars.ToolBarColor = i;
#ifdef __WIN__
                if( GetToolbarWindow() != NULL ) {
                    InvalidateRect( GetToolbarWindow(), NULL, TRUE );
                    UpdateWindow( GetToolbarWindow() );
                }
#endif
                break;
            default:
                return( ERR_INVALID_SET_COMMAND );
            }

            if( msgFlag ) {
                MySprintf( fn, "%s set to %d", GetTokenStringCVT( TokensSetVar, j, settokstr, TRUE ), i );
            }
            break;
        }
    }

    if( msgFlag && rc == ERR_NO_ERR && !EditFlags.Quiet ) {
        setMessage( fn, redisplay );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    if( rc == ERR_NO_ERR && toggle ) {
        strcpy( value, save );
    }
    return( rc );
#endif /* VICOMP */

} /* processSetToken */

#ifndef VICOMP
/*
 * SettingSelected - a setting was selected from the dialog
 */
vi_rc SettingSelected( char *item, char *value, int *winflag )
{
    int         id;
    bool        isnonbool;

    id = Tokenize( TokensSetVar, item, FALSE );
    if( id != TOK_INVALID ) {
        isnonbool = TRUE;
    } else {
        id = Tokenize( TokensSetFlag, item, FALSE );
        if( id == TOK_INVALID ) {
            return( ERR_INVALID_SET_COMMAND );
        }
        isnonbool = FALSE;
    }
    return( processSetToken( id, value, winflag, isnonbool ) );

} /* SettingSelected */


typedef struct {
    char        *setting;
    char        *val;
} set_data;

#ifndef __WIN__
/*
 * compareString - quicksort comparison
 */
static int compareString( void const *_p1, void const *_p2 )
{
    set_data * const *p1 = (set_data * const *)_p1;
    set_data * const *p2 = (set_data * const *)_p2;

    return( stricmp( (*p1)->setting,(*p2)->setting ) );

} /* compareString */

/*
 * getSetInfo - build string of values
 */
static int getSetInfo( char ***vals, char ***list, int *longest )
{
    int         i, j;
    char        settokstr[TOK_MAX_LEN + 1];
    char        tmpstr[MAX_STR];
    set_data    **sdata;
    int         tc, tc1, tc2;

    tc1 = GetNumberOfTokens( TokensSetVar );
    tc2 = GetNumberOfTokens( TokensSetFlag );
    tc = tc1 + tc2;
    sdata = MemAlloc( tc * sizeof( set_data * ) );
    *list = MemAlloc( tc * sizeof( char * ) );
    *vals = MemAlloc( tc * sizeof( char * ) );

    for( i = 0; i < tc1; i++ ) {
        sdata[i] = MemAlloc( sizeof( set_data ) );
        AddString( &(sdata[i]->setting), GetTokenStringCVT( TokensSetVar, i, settokstr, TRUE ) );
        AddString( &(sdata[i]->val), getOneSetVal( i, TRUE, tmpstr, TRUE ) );
    }
    for( i = 0; i < tc2; i++ ) {
        sdata[tc1 + i] = MemAlloc( sizeof( set_data ) );
        AddString( &(sdata[tc1 + i]->setting), GetTokenStringCVT( TokensSetFlag, i, settokstr, TRUE ) );
        AddString( &(sdata[tc1 + i]->val), getOneSetVal( i, FALSE, tmpstr, TRUE ) );
    }
    qsort( sdata, tc, sizeof( set_data * ), compareString );
    for( i = 0; i < tc; i++ ) {
        (*list)[i] = sdata[i]->setting;
        (*vals)[i] = sdata[i]->val;
        MemFree( sdata[i] );
    }
    MemFree( sdata );
    i = GetLongestTokenLength( TokensSetVar );
    j = GetLongestTokenLength( TokensSetFlag );
    if( i > j ) {
        *longest = i;
    } else {
        *longest = j;
    }
    return( tc );

} /* getSetInfo */
#endif
#endif /* VICOMP */

/*
 * Set - set editor control variable
 */
vi_rc Set( char *name )
{
    char        fn[MAX_STR];
    vi_rc       rc = ERR_NO_ERR;
    int         j, i;
#ifndef VICOMP
#ifndef __WIN__
    short       tmp;
    int         tc;
    char        **vals = NULL;
    char        **list;
    int         longest;
#endif
#endif

    /*
     * get item to set
     */
#ifndef VICOMP
    msgFlag = FALSE;
    if( !EditFlags.ScriptIsCompiled ) {
#endif
        RemoveLeadingSpaces( name );
        j = strlen( name );
        for( i = 0; i < j; i++ ) {
            if( name[i] == ' ' )
                break;
            if( name[i] == '=' ) {
                name[i] = ' ';
                break;
            }
        }
#ifndef VICOMP
    }
#endif

    if( NextWord1( name, fn ) <=0 ) {
#ifndef VICOMP
        if( !EditFlags.WindowsStarted ) {
            return( ERR_NO_ERR );
        }
#ifndef __WIN__
        tc = getSetInfo( &vals, &list, &longest );
        tmp = setw_info.y2;
        i = setw_info.y2 - setw_info.y1 + 1;
        if( setw_info.has_border ) {
            i -= 2;
        }
        if( tc < i ) {
            setw_info.y2 -= (i - tc);
        }
        rc = SelectItemAndValue( &setw_info, "Settings", list,
                          tc, SettingSelected, 1, vals, longest + 3 );
        setw_info.y2 = tmp;
        MemFreeList( tc, vals );
        MemFreeList( tc, list );
        ReDisplayScreen();
#endif
#endif /* VICOMP */
        return( rc );
    } else {
#ifndef VICOMP
        if( !EditFlags.Starting) {
            msgFlag = TRUE;
        }
#endif
        do {
#ifndef VICOMP
            if( !EditFlags.ScriptIsCompiled ) {
#endif
                if( tolower( fn[0] ) == 'n' && tolower( fn[1] ) == 'o' ) {
                    EliminateFirstN( fn, 2 );
                    i = -1;
                } else {
                    i = 1;
                }
                j = Tokenize( TokensSetVar, fn, FALSE );
                if( j == TOK_INVALID ) {
                    j = Tokenize( TokensSetFlagShort, fn, FALSE );
                    if( j == TOK_INVALID ) {
                        j = Tokenize( TokensSetFlag, fn, FALSE );
                        if( j == TOK_INVALID ) {
                            return( ERR_INVALID_SET_COMMAND );
                        }
                    }
                    j += SETVAR_T_;
                }
                j *= i;
#ifndef VICOMP
            } else {
                j = atoi( fn );
            }
#endif
            i = TRUE;
            rc = processSetToken( j, name, &i, FALSE );
            if( rc > ERR_NO_ERR ) {
                break;
            }
            RemoveLeadingSpaces( name );
        } while( NextWord1( name, fn ) > 0 );
#ifndef VICOMP
        if( msgFlag ) {
            putMessage();
        }
#endif
        return( rc );
    }

} /* Set */

#ifndef VICOMP
/*
 * GetASetVal - get set val data
 */
char *GetASetVal( char *token )
{
    int         j;
    char        tmpstr[MAX_STR];

    j = Tokenize( TokensSetVar, token, FALSE );
    if( j != TOK_INVALID ) {
        return( getOneSetVal( j, TRUE, tmpstr, FALSE ) );
    }
    j = Tokenize( TokensSetFlagShort, token, FALSE );
    if( j == TOK_INVALID ) {
        j = Tokenize( TokensSetFlag, token, FALSE );
    }
    if( j != TOK_INVALID ) {
        return( getOneSetVal( j, FALSE, tmpstr, FALSE ) );
    }
    return( "" );

} /* GetASetVal */

char *ExpandTokenSet( char *token_no, char *buff )
{
    bool        val;
    int         tok;
    char        settokstr[TOK_MAX_LEN + 1];

    tok = atoi( token_no );
    val = TRUE;
    if( tok < 0 ) {
        tok *= -1;
        val = FALSE;
    }
    if( tok >= SETVAR_T_ ) {
        sprintf( buff, "%s%s", GET_BOOL_PREFIX( val ), GetTokenStringCVT( TokensSetFlag, tok - SETVAR_T_, settokstr, TRUE ) );
    } else {
        sprintf( buff, "%s", GetTokenStringCVT( TokensSetVar, tok, settokstr, TRUE ) );
    }
    return( buff );
}
#endif
