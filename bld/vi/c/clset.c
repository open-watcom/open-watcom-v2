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
            itoa( j, tmpstr, 10 );
        }
    } else {
        switch( token ) {
        case SET1_T_STATUSSECTIONS:
            str = tmpstr;
            *str = 0;
            for( i = 0; i < NumStatusSections; i++ ) {
                char        buff[16];
                itoa( StatusSections[i], buff, 10 );
                strcat( str, buff );
                strcat( str, " " );
            }
            break;
        case SET1_T_FILEENDSTRING:
            str = FileEndString;
            break;
        case SET1_T_STATUSSTRING:
            str = StatusString;
            break;
        case SET1_T_TILECOLOR:
            str = tmpstr;
            break;
        case SET1_T_FIGNORE:
            fign = FIgnore;
            str = tmpstr;
            for( j = 0; j < CurrFIgnore; j++ ) {
                strcat( str, fign );
                fign += EXTENSION_LENGTH;
            }
            break;
        case SET1_T_GADGETSTRING:
            str = GadgetString;
            break;
        case SET1_T_SHELLPROMPT:
            str = SpawnPrompt;
            break;
        case SET1_T_GREPDEFAULT:
            str = GrepDefault;
            break;
        case SET1_T_TMPDIR:
            str = TmpDir;
            break;
        case SET1_T_WORD:
            str = WordDefn;
            break;
        case SET1_T_WORDALT:
            str = WordAltDefn;
            break;
        case SET1_T_FILENAME:
            if( CurrentFile == NULL ) {
                str = "";
            } else {
                str = CurrentFile->name;
            }
            break;
        case SET1_T_HISTORYFILE:
            str = HistoryFile;
            break;
        case SET1_T_TAGFILENAME:
            str = TagFileName;
            break;
        case SET1_T_MAGICSTRING:
            str = Majick;
            break;
        case SET1_T_COMMANDCURSORTYPE:
        case SET1_T_OVERSTRIKECURSORTYPE:
        case SET1_T_INSERTCURSORTYPE:
            if( token == SET1_T_COMMANDCURSORTYPE ) {
                ct = NormalCursorType;
            } else if( token == SET1_T_OVERSTRIKECURSORTYPE ) {
                ct = OverstrikeCursorType;
            } else {
                ct = InsertCursorType;
            }
            str = tmpstr;
            MySprintf( tmpstr, "%d %d", ct.height, ct.width );
            break;
        default:
            switch( token ) {
            case SET1_T_WRAPMARGIN:
                j = WrapMargin;
                break;
            case SET1_T_CURSORBLINKRATE:
                j = CursorBlinkRate;
                break;
            case SET1_T_MAXPUSH:
                j = MaxPush;
                break;
            case SET1_T_RADIX:
                j = Radix;
                break;
            case SET1_T_AUTOSAVEINTERVAL:
                j = AutoSaveInterval;
                break;
            case SET1_T_LANGUAGE:
                if( CurrentInfo == NULL ) {
                    j = LANG_NONE;
                } else {
                    j = CurrentInfo->Language;
                }
                break;
            case SET1_T_MOUSEDCLICKSPEED:
                j = MouseDoubleClickSpeed;
                break;
            case SET1_T_MOUSESPEED:
                j = MouseSpeed;
                break;
            case SET1_T_MOUSEREPEATDELAY:
                j = MouseRepeatDelay;
                break;
            case SET1_T_CURRENTSTATUSCOLUMN:
                j = CurrentStatusColumn;
                break;
            case SET1_T_ENDOFLINECHAR:
                j = EndOfLineChar;
                break;
            case SET1_T_EXITATTR:
                j = ExitAttr;
                break;
            case SET1_T_MAXSWAPK:
                j = MaxSwapBlocks;
                j *= (MAX_IO_BUFFER / 1024);
                break;
            case SET1_T_MAXEMSK:
#ifndef NOEMS
                j = MaxEMSBlocks;
                j *= (MAX_IO_BUFFER / 1024);
#else
                j = 0;
#endif
                break;
            case SET1_T_MAXXMSK:
#ifndef NOXMS
                j = MaxXMSBlocks;
                j *= (MAX_IO_BUFFER / 1024);
#else
                j = 0;
#endif
                break;
            case SET1_T_RESIZECOLOR:
                j = ResizeColor;
                break;
            case SET1_T_MOVECOLOR:
                j = MoveColor;
                break;
            case SET1_T_INACTIVEWINDOWCOLOR:
                j = InactiveWindowColor;
                break;
            case SET1_T_MAXTILECOLORS:
                j = MaxTileColors;
                break;
            case SET1_T_MAXWINDOWTILEX:
                j = MaxWindowTileX;
                break;
            case SET1_T_MAXWINDOWTILEY:
                j = MaxWindowTileY;
                break;
            case SET1_T_HARDTAB:
                j = HardTab;
                break;
            case SET1_T_TABAMOUNT:
                j = TabAmount;
                break;
            case SET1_T_SHIFTWIDTH:
                j = ShiftWidth;
                break;
            case SET1_T_STACKK:
                j = StackK;
                break;
            case SET1_T_LINENUMWINWIDTH:
                j = LineNumWinWidth;
                break;
            case SET1_T_CLOCKX:
                j = ClockX;
                break;
            case SET1_T_CLOCKY:
                j = ClockY;
                break;
            case SET1_T_SPINX:
                j = SpinX;
                break;
            case SET1_T_SPINY:
                j = SpinY;
                break;
            case SET1_T_MAXCLHISTORY:
                j = CLHist.max;
                break;
            case SET1_T_MAXFILTERHISTORY:
                j = FilterHist.max;
                break;
            case SET1_T_MAXFINDHISTORY:
                j = FindHist.max;
                break;
            case SET1_T_MAXLINELEN:
                j = MaxLine;
                break;
            case SET1_T_PAGELINESEXPOSED:
                j = PageLinesExposed;
                break;
            case SET1_T_TOOLBARBUTTONHEIGHT:
                j = ToolBarButtonHeight;
                break;
            case SET1_T_TOOLBARBUTTONWIDTH:
                j = ToolBarButtonWidth;
                break;
            case SET1_T_TOOLBARCOLOR:
                j = ToolBarColor;
                break;
            }
            itoa( j, tmpstr, 10 );
            str = tmpstr;
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
#else
extern vi_rc GetNewValueDialog( char * );
#endif

/*
 * processSetToken - set value for set token
 */
static vi_rc processSetToken( int j, char *value, int *winflag, bool isnonbool )
{
    char        fn[MAX_STR], str[MAX_STR], tmp[3];
    char        tokstr[MAX_STR];
    char        save[MAX_STR];
    vi_rc       rc = ERR_NO_ERR;
    int         i, clr, k;
    bool        newset;
    bool        set1, toggle, *ptr;
    jmp_buf     jmpaddr;
    cursor_type ct;
    char        *name;
    command_rtn fptr;
    event_bits  eb;
    bool        redisplay = FALSE;

    /*
     * set up value for boolean set commands
     */
    if( j < 0 ) {
        j *= -1;
        i = FALSE;
    } else {
        i = TRUE;
    }
    if( !(*winflag) ) {
        toggle = TRUE;
        set1 = isnonbool;
    } else {
        toggle = FALSE;
        if( j >= SET1_T_ ) {
            if( EditFlags.CompileScript ) {
                if( !i ) {
                    j *= -1;
                }
                itoa( j, str, 10 );
                StrMerge( 2, WorkLine->data, str, SingleBlank );
                return( ERR_NO_ERR );
            }
            set1 = FALSE;
            j -= SET1_T_;
        } else {
            set1 = TRUE;
        }
    }
    *winflag = FALSE;

    /*
     * process boolean settings
     */
    if( !set1 ) {
        if( j >= SET2_T_ ) {
            return( ERR_INVALID_SET_COMMAND );
        }
        ptr = &(((bool *)&EditFlags)[j]);
        newset = (bool) i;
        if( toggle ) {
            newset = !(*ptr);
        }
        switch( j ) {
        case SET2_T_MODELESS:
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
                    if( MenuWindow != (window_id)-1 ) {
                        UpdateCurrentStatus( CSTATUS_INSERT );
                    }
                    EditFlags.WasOverstrike = FALSE;
                    NewCursor( CurrentWindow, InsertCursorType );
                } else {
                    if( MenuWindow != (window_id)-1 ) {
                        UpdateCurrentStatus( CSTATUS_COMMAND );
                    }
                    NewCursor( CurrentWindow, NormalCursorType );
                    // nomodeless must be line based or it dies!
                    EditFlags.LineBased = TRUE;
                }
                /* re-position cursor in window
                */
                SetWindowCursor();
            }
            EditFlags.Modeless = newset;
            break;
        case SET2_T_UNDO:
            if( EditFlags.Undo && !newset ) {
                FreeAllUndos();
            }
            EditFlags.Undo = newset;
            break;
        case SET2_T_STATUSINFO:
            EditFlags.StatusInfo = newset;
#ifdef __WIN__
            ResizeRoot();
#endif
            rc = NewStatusWindow();
            break;
        case SET2_T_WINDOWGADGETS:
            EditFlags.WindowGadgets = newset;
            ResetAllWindows();
            *winflag = TRUE;
            redisplay = TRUE;
            break;
        case SET2_T_REALTABS:
            EditFlags.RealTabs = newset;
            redisplay = TRUE;
            break;
        case SET2_T_CLOCK:
            EditFlags.Clock = newset;
            redisplay = TRUE;
            break;
        case SET2_T_WRITECRLF:
            if( CurrentFile ) {
                CurrentFile->check_for_crlf = FALSE;
            }
            EditFlags.WriteCRLF = newset;
            break;
        case SET2_T_TOOLBAR:
            EditFlags.Toolbar = newset;
#ifdef __WIN__
            ResizeRoot();
#endif
            break;
        case SET2_T_COLORBAR:
            EditFlags.Colorbar = newset;
#ifdef __WIN__
            if( Root == NULL ) {
                EditFlags.Colorbar = FALSE;
            } else {
                RefreshColorbar();
            }
#endif
            break;
        case SET2_T_SSBAR:
            EditFlags.SSbar = newset;
#ifdef __WIN__
            if( Root == NULL ) {
                EditFlags.SSbar = FALSE;
            } else {
                RefreshSSbar();
            }
#endif
            break;
        case SET2_T_FONTBAR:
            EditFlags.Fontbar = newset;
#ifdef __WIN__
            if( Root == NULL ) {
                EditFlags.Fontbar = FALSE;
            } else {
                RefreshFontbar();
            }
#endif
            break;
        case SET2_T_MARKLONGLINES:
            EditFlags.MarkLongLines = newset;
            break;
        case SET2_T_MENUS:
            EditFlags.Menus = newset;
            InitMenu();
            break;
        case SET2_T_LINENUMBERS:
            if( toggle ) {
                newset = !EditFlags.LineNumbers;
            }
            if( newset != EditFlags.LineNumbers ) {
                EditFlags.LineNumbers = newset;
                rc = LineNumbersSetup();
                *winflag = TRUE;
            }
            break;
        case SET2_T_CURRENTSTATUS:
            EditFlags.CurrentStatus = newset;
            InitMenu();
            break;
        case SET2_T_DISPLAYSECONDS:
            EditFlags.DisplaySeconds = newset;
            redisplay = TRUE;
            break;
        case SET2_T_PPKEYWORDONLY:
            EditFlags.PPKeywordOnly = newset;
            redisplay = TRUE;
            break;
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
            strcpy( tokstr, GetTokenString( SetTokens2, j ) );
            strlwr( tokstr );
            MySprintf( fn, "%s%s set", tmp, tokstr );
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
            if( GetNewValueDialog( value ) ) {
                return( NO_VALUE_ENTERED );
            }
            strcpy( save, value );
        }
        RemoveLeadingSpaces( value );
        if( value[0] == '"' ) {
            NextWord( value, fn, "\"" );
            EliminateFirstN( value, 1 );
        } else {
            NextWord1( value, fn );
        }
        if( EditFlags.CompileScript ) {
            itoa( j, str, 10 );
            StrMerge( 4, WorkLine->data, str, SingleBlank, fn, SingleBlank );
            switch( j ) {
            case SET1_T_COMMANDCURSORTYPE:
            case SET1_T_OVERSTRIKECURSORTYPE:
            case SET1_T_INSERTCURSORTYPE:
                if( NextWord1( value, fn ) <= 0 ) {
                    break;
                }
                strcat( WorkLine->data, fn );
                break;
            case SET1_T_TILECOLOR:
                if( NextWord1( value, fn ) <= 0 ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                if( NextWord1( value, str ) <= 0 ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                StrMerge( 4, WorkLine->data, fn, SingleBlank, str, SingleBlank );
                break;
            }
            return( ERR_NO_ERR );
        }
        switch( j ) {
        case SET1_T_STATUSSECTIONS:
            if( StatusSections != NULL ) {
                MemFree( StatusSections );
                StatusSections = NULL;
                NumStatusSections = 0;
            }
            while( 1 ) {
                k = atoi( fn );
                if( k <= 0 ) {
                    break;
                }
                StatusSections = MemReAlloc( StatusSections,
                                    sizeof( short ) * (NumStatusSections + 1) );
                StatusSections[NumStatusSections] = k;
                NumStatusSections++;
                if( NextWord1( value, fn ) <= 0 ) {
                    break;
                }
            }
            if( StatusSections == NULL ) {
                MySprintf( fn, "statussections turned off" );
            } else {
                MySprintf( fn, "statussections set" );
            }
            break;
        case SET1_T_FILEENDSTRING:
            AddString2( &FileEndString, fn );
            ResetAllWindows();
            redisplay = TRUE;
            break;
        case SET1_T_STATUSSTRING:
            AddString2( &StatusString, fn );
            if( StatusWindow != (window_id)-1 ) {
                ClearWindow( StatusWindow );
                UpdateStatusWindow();
            }
            if( msgFlag ) {
                MySprintf( fn, "statusstring set to %s", StatusString );
            }
            break;
        case SET1_T_GREPDEFAULT:
            AddString2( &GrepDefault, fn );
            break;
        case SET1_T_TILECOLOR:
            if( TileColors == NULL ) {
                TileColors = (type_style *) MemAlloc( sizeof( type_style ) * ( MaxTileColors + 1 ) );
                for( i = 0; i <= MaxTileColors; ++i ) {
                    TileColors[i].foreground = -1;
                    TileColors[i].background = -1;
                    TileColors[i].font = -1;
                }
            }
            clr = atoi( fn );
            if( clr > MaxTileColors ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            if( NextWord1( name, fn ) <= 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            TileColors[clr].foreground = atoi( fn );
            if( NextWord1( name, fn ) <= 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            TileColors[clr].background = atoi( fn );
            TileColors[clr].font = FONT_DEFAULT;
            if( msgFlag ) {
                MySprintf( fn, "tilecolor %d set", clr );
            }
            break;
        case SET1_T_GADGETSTRING:
            SetGadgetString( fn );
            if( msgFlag ) {
                MySprintf( fn, "gadget string set to %s", GadgetString );
            }
            ResetAllWindows();
            break;
        case SET1_T_SHELLPROMPT:
            strcpy( SpawnPrompt, fn );
            if( msgFlag ) {
                MySprintf( fn, "prompt string set to %s", SpawnPrompt );
            }
            break;
        case SET1_T_FIGNORE:
            if( fn[0] == 0 ) {
                MemFree2( &FIgnore );
                CurrFIgnore = 0;
                if( msgFlag ) {
                    MySprintf( fn, "fignore reset" );
                }
            } else {
                FIgnore = MemReAlloc( FIgnore, EXTENSION_LENGTH * (CurrFIgnore + 1) );
                str[0] = '.';
                str[1] = 0;
                strcat( str, fn );
                str[EXTENSION_LENGTH - 1] = 0;
                strcpy( &FIgnore[EXTENSION_LENGTH * CurrFIgnore], str );
                CurrFIgnore++;
                if( msgFlag ) {
                    MySprintf( fn, "%s added to fignore", str );
                }
            }
            break;
        case SET1_T_HISTORYFILE:
            AddString2( &HistoryFile, fn );
            if( msgFlag ) {
                MySprintf( fn, "history file set to %s", HistoryFile );
            }
            break;

        case SET1_T_TAGFILENAME:
            AddString2( &TagFileName, fn );
            if( msgFlag ) {
                MySprintf( fn, "tag file name set to %s", TagFileName );
            }
            break;

        case SET1_T_FILENAME:
            if( CurrentFile != NULL ) {
                AddString2( &(CurrentFile->name), fn );
                SetFileWindowTitle( CurrentWindow, CurrentInfo, TRUE );
                if( msgFlag ) {
                    MySprintf( fn, "filename set to %s", CurrentFile->name );
                }
                FileSPVAR();
            }
            break;
        case SET1_T_TMPDIR:
            AddString2( &TmpDir, fn );
            VerifyTmpDir();
            if( msgFlag ) {
                MySprintf( fn, "tmpdir set to %s", TmpDir );
            }
            break;
        case SET1_T_WORD:
            AddString2( &WordDefn, fn );
            InitWordSearch( WordDefn );
            if( msgFlag ) {
                MySprintf( fn, "word set to %s", WordDefn );
            }
            break;
        case SET1_T_WORDALT:
            AddString2( &WordAltDefn, fn );
            if( msgFlag ) {
                MySprintf( fn, "wordalt set to %s", WordAltDefn );
            }
            break;
        case SET1_T_MAGICSTRING:
            AddString2( &Majick, fn );
            if( msgFlag ) {
                MySprintf( fn, "magicstring set to %s", Majick );
            }
            break;
        case SET1_T_COMMANDCURSORTYPE:
        case SET1_T_OVERSTRIKECURSORTYPE:
        case SET1_T_INSERTCURSORTYPE:
            i = setjmp( jmpaddr );
            if( i != 0 ) {
                return( (vi_rc)i );
            }
            StartExprParse( fn, jmpaddr );
            ct.height = GetConstExpr();
            if( NextWord1( value, fn ) <= 0 ) {
                ct.width = 100;
            } else {
                i = setjmp( jmpaddr );
                if( i != 0 ) {
                    return( (vi_rc)i );
                }
                StartExprParse( fn, jmpaddr );
                ct.width = GetConstExpr();
            }
            if( j == SET1_T_COMMANDCURSORTYPE ) {
                NormalCursorType = ct;
                name = "command";
            } else if( j == SET1_T_OVERSTRIKECURSORTYPE ) {
                OverstrikeCursorType = ct;
                name = "overstrike";
            } else {
                InsertCursorType = ct;
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
                return( (vi_rc)i );
            }
            StartExprParse( fn, jmpaddr );
            i = GetConstExpr();
            if( i < 0 ) {
                i = 0;
            }
            switch( j ) {
            case SET1_T_WRAPMARGIN:
                WrapMargin = i;
                break;
            case SET1_T_CURSORBLINKRATE:
                SetCursorBlinkRate( i );
                break;
            case SET1_T_MAXPUSH:
                MaxPush = i;
                if( MaxPush < 1 ) {
                    MaxPush = 1;
                }
                InitFileStack();
                break;
            case SET1_T_RADIX:
                Radix = i;
                break;
            case SET1_T_AUTOSAVEINTERVAL:
                AutoSaveInterval = i;
                SetNextAutoSaveTime();
                break;
            case SET1_T_LANGUAGE:
                if( i < LANG_NONE || i >= LANG_MAX ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                if( CurrentInfo != NULL ) {
                    LangFini( CurrentInfo->Language );
                    LangInit( i );
                    redisplay = TRUE;
                }
                break;
            case SET1_T_MOVECOLOR:
                MoveColor = i;
                break;
            case SET1_T_RESIZECOLOR:
                ResizeColor = i;
                break;
            case SET1_T_MOUSEDCLICKSPEED:
                MouseDoubleClickSpeed = i;
                break;
            case SET1_T_MOUSESPEED:
                SetMouseSpeed( i );
                break;
            case SET1_T_MOUSEREPEATDELAY:
                MouseRepeatDelay = i;
                break;
            case SET1_T_CURRENTSTATUSCOLUMN:
                CurrentStatusColumn = i;
                InitMenu();
                break;
            case SET1_T_ENDOFLINECHAR:
                EndOfLineChar = i;
                break;
            case SET1_T_EXITATTR:
                ExitAttr = (char) i;
                break;
            case SET1_T_INACTIVEWINDOWCOLOR:
                InactiveWindowColor = i;
                break;
            case SET1_T_TABAMOUNT:
                TabAmount = i;
                break;
            case SET1_T_SHIFTWIDTH:
                ShiftWidth = i;
                break;
            case SET1_T_PAGELINESEXPOSED:
                PageLinesExposed = i;
                break;
            case SET1_T_HARDTAB:
                HardTab = i;
                redisplay = TRUE;
                break;
            case SET1_T_STACKK:
                if( EditFlags.Starting ) {
                    StackK = i;
                }
                break;
            case SET1_T_LINENUMWINWIDTH:
                LineNumWinWidth = i;
                break;
            case SET1_T_MAXWINDOWTILEX:
                MaxWindowTileX = i;
                break;
            case SET1_T_MAXWINDOWTILEY:
                MaxWindowTileY = i;
                break;
            case SET1_T_MAXSWAPK:
                SwapBlockInit( i );
                break;
            case SET1_T_MAXEMSK:
#ifndef NOEMS
                EMSBlockInit( i );
#endif
                break;
            case SET1_T_MAXXMSK:
#ifndef NOXMS
                XMSBlockInit( i );
#endif
                break;
            case SET1_T_MAXFILTERHISTORY:
                FilterHistInit( i );
                break;
            case SET1_T_MAXCLHISTORY:
                CLHistInit( i );
                break;
            case SET1_T_MAXFINDHISTORY:
                FindHistInit( i );
                break;
            case SET1_T_MAXTILECOLORS:
                k = (TileColors == NULL) ? 0 : MaxTileColors + 1;
                MaxTileColors = i;
                TileColors = MemReAlloc( TileColors, sizeof( type_style ) * ( MaxTileColors + 1 ) );
                for( ; k <= MaxTileColors; ++k ) {
                    TileColors[k].foreground = -1;
                    TileColors[k].background = -1;
                    TileColors[k].font = -1;
                }
                break;
            case SET1_T_CLOCKX:
                ClockX = i;
                GetClockStart();
                break;
            case SET1_T_CLOCKY:
                ClockY = i;
                GetClockStart();
                break;
            case SET1_T_SPINX:
                SpinX = i;
                GetSpinStart();
                break;
            case SET1_T_SPINY:
                SpinY = i;
                GetSpinStart();
                break;
            case SET1_T_MAXLINELEN:
                /* file save fails if 1 line is > MAX_IO_BUFFER */
                i = min( i, MAX_IO_BUFFER );
                MaxLine = i;
                MaxLinem1 = MaxLine - 1;
                StaticStart();
                /* 94/05/11 -- WorkLine was not realloced - thus too short */
                WorkLine = MemReAlloc( WorkLine, sizeof( line ) + MaxLine + 2 );
                break;
            case SET1_T_TOOLBARBUTTONHEIGHT:
                ToolBarButtonHeight = i;
#ifdef __WIN__
                ResizeRoot();
#endif
                break;
            case SET1_T_TOOLBARBUTTONWIDTH:
                ToolBarButtonWidth = i;
#ifdef __WIN__
                ResizeRoot();
#endif
                break;
            case SET1_T_TOOLBARCOLOR:
                ToolBarColor = i;
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
                strcpy( tokstr, GetTokenString( SetTokens1, j ) );
                strlwr( tokstr );
                MySprintf( fn, "%s set to %d", tokstr, i );
            }
            break;
        }
    }

    if( msgFlag && !rc && !EditFlags.Quiet ) {
        setMessage( fn, redisplay );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    if( rc == ERR_NO_ERR && toggle ) {
        strcpy( value, save );
    }

    return( rc );

} /* processSetToken */

/*
 * SettingSelected - a setting was selected from the dialog
 */
vi_rc SettingSelected( char *item, char *value, int *winflag )
{
    int         id;
    bool        isnonbool;

    id = Tokenize( SetTokens1, item, FALSE );
    if( id >= 0 ) {
        isnonbool = TRUE;
    } else {
        id = Tokenize( SetTokens2, item, FALSE );
        if( id < 0 ) {
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
    set_data * const *p1 = _p1;
    set_data * const *p2 = _p2;

    return( stricmp( (*p1)->setting,(*p2)->setting ) );

} /* compareString */

/*
 * getSetInfo - build string of values
 */
static int getSetInfo( char ***vals, char ***list, int *longest )
{
    int         i, j;
    char        tmpstr[MAX_STR];
    set_data    **sdata;
    int         tc, tc1, tc2;

    tc1 = GetNumberOfTokens( SetTokens1 );
    tc2 = GetNumberOfTokens( SetTokens2 );
    tc = tc1 + tc2;
    sdata = MemAlloc( tc * sizeof( set_data * ) );
    *list = MemAlloc( tc * sizeof( char * ) );
    *vals = MemAlloc( tc * sizeof( char * ) );

    for( i = 0; i < tc1; i++ ) {
        sdata[i] = MemAlloc( sizeof( set_data ) );
        AddString( &(sdata[i]->setting), GetTokenString( SetTokens1, i ) );
        AddString( &(sdata[i]->val), getOneSetVal( i, TRUE, tmpstr, TRUE ) );
    }
    for( i = 0; i < tc2; i++ ) {
        sdata[tc1 + i] = MemAlloc( sizeof( set_data ) );
        AddString( &(sdata[tc1 + i]->setting), GetTokenString( SetTokens2, i ) );
        AddString( &(sdata[tc1 + i]->val), getOneSetVal( i, FALSE, tmpstr, TRUE ) );
    }
    qsort( sdata, tc, sizeof( set_data * ), compareString );
    for( i = 0; i < tc; i++ ) {
        (*list)[i] = sdata[i]->setting;
        (*vals)[i] = sdata[i]->val;
        MemFree( sdata[i] );
    }
    MemFree( sdata );
    i = GetLongestTokenLength( SetTokens1 );
    j = GetLongestTokenLength( SetTokens2 );
    if( i > j ) {
        *longest = i;
    } else {
        *longest = j;
    }
    return( tc );

} /* getSetInfo */
#endif

/*
 * Set - set editor control variable
 */
vi_rc Set( char *name )
{
    char        fn[MAX_STR];
    vi_rc       rc = ERR_NO_ERR;
    int         j, i;
#ifndef __WIN__
    int         tmp, tc;
    char        **vals = NULL;
    char        **list;
    int         longest;
#endif

    /*
     * get item to set
     */
    msgFlag = FALSE;
    if( !EditFlags.ScriptIsCompiled ) {
        RemoveLeadingSpaces( name );
        j = strlen( name );
        for( i = 0; i < j; i++ ) {
            if( name[i] == '=' || name[i] == ',' ) {
                name[i] = ' ';
            }
        }
    }

    if( NextWord1( name, fn ) <=0 ) {
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
        return( rc );
    } else {
        if( !EditFlags.Starting) {
            msgFlag = TRUE;
        }
        do {
            if( !EditFlags.ScriptIsCompiled ) {
                if( !strnicmp( fn, "no", 2 ) ) {
                    EliminateFirstN( fn, 2 );
                    i = -1;
                } else {
                    i = 1;
                }
                j = Tokenize( SetTokens1, fn, FALSE );
                if( j < 0 ) {
                    j = Tokenize( SetTokens2a, fn, FALSE );
                    if( j < 0 ) {
                        if( j < 0 ) {
                            j = Tokenize( SetTokens2, fn, FALSE );
                        }
                        if( j < 0 ) {
                            return( ERR_INVALID_SET_COMMAND );
                        }
                    }
                    j += SET1_T_;
                }
                j *= i;
            } else {
                j = atoi( fn );
            }
            i = TRUE;
            rc = processSetToken( j, name, &i, FALSE );
            if( rc > ERR_NO_ERR ) {
                break;
            }
            RemoveLeadingSpaces( name );
        } while( NextWord1( name, fn ) > 0 );
        if( msgFlag ) {
            putMessage();
        }
        return( rc );
    }

} /* Set */

/*
 * GetASetVal - get set val data
 */
char *GetASetVal( char *token )
{
    int         j;
    char        tmpstr[MAX_STR];

    j = Tokenize( SetTokens1, token, FALSE );
    if( j >= 0 ) {
        return( getOneSetVal( j, TRUE, tmpstr, FALSE ) );
    }
    j = Tokenize( SetTokens2a, token, FALSE );
    if( j < 0 ) {
        j = Tokenize( SetTokens2, token, FALSE );
    }
    if( j >= 0 ) {
        return( getOneSetVal( j, FALSE, tmpstr, FALSE ) );
    }
    return( "" );

} /* GetASetVal */
