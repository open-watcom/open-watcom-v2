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
#include <setjmp.h>
#include "parsecl.h"
#include "expr.h"
#include "fcbmem.h"
#include "win.h"
#include "menu.h"
#include "tempio.h"

#include "clibext.h"


#ifndef VICOMP

static bool msgFlag;
static bool needsRedisplay = false;
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
        needsRedisplay = false;
    }
    Message1( msgString );
}

/*
 * getOneSetVal - get a single set value
 */
static const char *getOneSetVal( int token, bool isbool, char *tmpstr, bool want_boolstr )
{
    const char  *str;
    char        *fign;
    cursor_type ct;
    int         i, j;
    bool        bln;

    *tmpstr = '\0';
    str = tmpstr;
    if( isbool ) {
        switch( token ) {
        #define PICK( a,b,c,d,e )   case SETFLAG_T_ ## e: bln = EditFlags.c; break;
            #include "setb.h"
        #undef PICK
        default:
            return( "" );
        }
        return( ( bln ) ? (( want_boolstr ) ? "TRUE" : "1") : (( want_boolstr ) ? "FALSE" : "0") );
    } else {
        switch( token ) {
        case SETVAR_T_STATUSSECTIONS:
            for( i = 0; i < EditVars.NumStatusSections; i++ ) {
                char        buff[16];
                sprintf( buff, "%d ", EditVars.StatusSections[i] );
                strcat( tmpstr, buff );
            }
            break;
        case SETVAR_T_FILEENDSTRING:
            str = EditVars.FileEndString;
            break;
        case SETVAR_T_STATUSSTRING:
            str = EditVars.StatusString;
            break;
        case SETVAR_T_TILECOLOR:
            break;
        case SETVAR_T_FIGNORE:
            fign = EditVars.FIgnore;
            for( j = 0; j < EditVars.CurrFIgnore; j++ ) {
                strcat( tmpstr, fign );
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
                tmpstr = "";
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
                    j = VI_LANG_NONE;
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
#if defined( USE_EMS )
                j = EditVars.MaxEMSBlocks;
                j *= (MAX_IO_BUFFER / 1024);
#endif
                break;
            case SETVAR_T_MAXXMSK:
#if defined( USE_XMS )
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
                j = EditVars.Hist[HIST_CMD].max;
                break;
            case SETVAR_T_MAXFILTERHISTORY:
                j = EditVars.Hist[HIST_FILTER].max;
                break;
            case SETVAR_T_MAXFINDHISTORY:
                j = EditVars.Hist[HIST_FIND].max;
                break;
            case SETVAR_T_MAXLASTFILESHISTORY:
                j = EditVars.Hist[HIST_LASTFILES].max;
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
static vi_rc GetNewValueDialog( char *value )
{
    bool        ret;
    vi_rc       rc;
    char        st[MAX_STR];
    window_id   wid;
    static char prompt[] = "New:";

    rc = NewWindow2( &wid, &setvalw_info );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    WPrintfLine( wid, 1, "Old: %s", value );
    ret = ReadStringInWindow( wid, 2, prompt, st, sizeof( st ), NULL );
    CloseAWindow( wid );
    SetWindowCursor();
    KillCursor();
    if( !ret ) {
        return( NO_VALUE_ENTERED );
    }
    if( *st == '\0' ) {
        return( NO_VALUE_ENTERED );
    }
    Expand( value, st, NULL );
    return( ERR_NO_ERR );

} /* GetNewValueDialog */
#endif
#endif /* VICOMP */


/*
 * processSetToken - set value for set token
 */
static vi_rc processSetToken( int j, char *new, const char **pvalue, int *winflag, bool isbool )
{
    char        fn[MAX_STR], str[MAX_STR];
#ifndef VICOMP
    char        settokstr[TOK_MAX_LEN + 1];
    vi_rc       rc = ERR_NO_ERR;
    int         clr;
    bool        newset;
    bool        toggle;
    jmp_buf     jmpaddr;
    cursor_type ct;
    char        *name;
    command_rtn fptr;
    event_bits  eb;
    bool        redisplay = false;
    int         i;
    long        lval;
#endif
    bool        bvalue;
    int         k;
    const char  *value;

#ifdef VICOMP
    /* unused parameters */ (void)winflag; (void)isbool; (void)new;
#endif
    /*
     * set up value for boolean set commands
     */
    if( j < 0 ) {
        j *= -1;
        bvalue = false;
    } else {
        bvalue = true;
    }
#ifndef VICOMP
    if( new != NULL ) {
        toggle = true;
    } else {
        toggle = false;
#endif
#ifdef VICOMP
        if( j >= SETVAR_T_ ) {
#else
        if( isbool || j >= SETVAR_T_ ) {
#endif
#ifndef VICOMP
            if( isbool ) {
                j += SETVAR_T_;
            } else {
                isbool = true;
            }
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
            j -= SETVAR_T_;
#endif
        }
#ifndef VICOMP
    }

    /*
     * process boolean settings
     */
    *winflag = 0;
    if( isbool ) {
        bool    flag_value;
        bool    post;

        if( j >= SETFLAG_T_ ) {
            return( ERR_INVALID_SET_COMMAND );
        }
        flag_value = false;
        switch( j ) {
        #define PICK( a,b,c,d,e )   case SETFLAG_T_ ## e: flag_value = EditFlags.c; break;
            #include "setb.h"
        #undef PICK
        }
        newset = bvalue;
        if( toggle ) {
            newset = !flag_value;
        }
        /* processing before change */
        switch( j ) {
        case SETFLAG_T_MODELESS:
            if( (newset && !flag_value) || (!newset && flag_value) ) {
                for( k = 0; k < MAX_EVENTS; k++ ) {
                    fptr = EventList[k].rtn;
                    eb = EventList[k].b;
                    EventList[k].rtn = EventList[k].alt_rtn;
                    EventList[k].alt_rtn = fptr;
                    EventList[k].b = EventList[k].alt_b;
                    EventList[k].alt_b = eb;
                }
                if( !flag_value ) {
                    if( !BAD_ID( menu_window_id ) ) {
                        UpdateCurrentStatus( CSTATUS_INSERT );
                    }
                    EditFlags.WasOverstrike = false;
                    NewCursor( current_window_id, EditVars.InsertCursorType );
                } else {
                    if( !BAD_ID( menu_window_id ) ) {
                        UpdateCurrentStatus( CSTATUS_COMMAND );
                    }
                    NewCursor( current_window_id, EditVars.NormalCursorType );
                    // nomodeless must be line based or it dies!
                    EditFlags.LineBased = true;
                }
                /* re-position cursor in window
                */
                SetWindowCursor();
            }
            break;
        case SETFLAG_T_UNDO:
            if( flag_value && !newset ) {
                FreeAllUndos();
            }
            break;
        case SETFLAG_T_COLORBAR:
  #ifdef __WIN__
            post = true;
            if( BAD_ID( root_window_id ) ) {
                newset = false;
                post = false;
            }
  #endif
            break;
        case SETFLAG_T_SSBAR:
  #ifdef __WIN__
            post = true;
            if( BAD_ID( root_window_id ) ) {
                newset = false;
                post = false;
            }
  #endif
            break;
        case SETFLAG_T_FONTBAR:
  #ifdef __WIN__
            post = true;
            if( BAD_ID( root_window_id ) ) {
                newset = false;
                post = false;
            }
  #endif
            break;
        case SETFLAG_T_LINENUMBERS:
            post = false;
            if( toggle ) {
                newset = !flag_value;
            }
            if( flag_value != newset ) {
                post = true;
            }
            break;
        case SETFLAG_T_LASTEOL:
  #ifndef __WIN__
            newset = true;
            toggle = false;
  #endif
            break;
        }
        /* change to new value */
        switch( j ) {
        #define PICK( a,b,c,d,e )   case SETFLAG_T_ ## e: EditFlags.c = newset; break;
            #include "setb.h"
        #undef PICK
        }
        /* post-processing, after change */
        switch( j ) {
        case SETFLAG_T_STATUSINFO:
  #ifdef __WIN__
            ResizeRoot();
  #endif
            rc = NewStatusWindow();
            break;
        case SETFLAG_T_WINDOWGADGETS:
            ResetAllWindows();
            *winflag = 1;
            redisplay = true;
            break;
        case SETFLAG_T_REALTABS:
            redisplay = true;
            break;
        case SETFLAG_T_CLOCK:
            redisplay = true;
            break;
        case SETFLAG_T_TOOLBAR:
  #ifdef __WIN__
            ResizeRoot();
  #endif
            break;
        case SETFLAG_T_COLORBAR:
  #ifdef __WIN__
            if( post ) {
                RefreshColorbar();
            }
  #endif
            break;
        case SETFLAG_T_SSBAR:
  #ifdef __WIN__
            if( post ) {
                RefreshSSbar();
            }
  #endif
            break;
        case SETFLAG_T_FONTBAR:
  #ifdef __WIN__
            if( post ) {
                RefreshFontbar();
            }
  #endif
            break;
        case SETFLAG_T_MENUS:
            InitMenu();
            break;
        case SETFLAG_T_LINENUMBERS:
            if( post ) {
                rc = LineNumbersSetup();
                *winflag = 1;
            }
            break;
        case SETFLAG_T_CURRENTSTATUS:
            InitMenu();
            break;
        case SETFLAG_T_DISPLAYSECONDS:
            redisplay = true;
            break;
        case SETFLAG_T_PPKEYWORDONLY:
            redisplay = true;
            break;
        }
        if( msgFlag ) {
            MySprintf( fn, "%s%s set", (newset) ? "" : "no", GetTokenStringCVT( SetFlagTokens, j, settokstr, true ) );
        }
        if( toggle ) {
            strcpy( new, (( newset ) ? "TRUE" : "FALSE") );
            (*winflag) += 1;
        }
        /* end */

    /*
     * process value settings
     */
    } else {
#endif /* VICOMP */
        value = *pvalue;
#ifndef VICOMP
        if( toggle ) {
            strcpy( new, value );
            rc = GetNewValueDialog( new );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            value = new;
        }
#endif /* VICOMP */
        SKIP_SPACES( value );
        if( *value == '"' ) {
            value = GetNextWord( value, fn, SingleDQuote );
            if( *value == '"' ) {
                SKIP_CHAR_SPACES( value );
            }
            if( *value == ',' ) {
                SKIP_CHAR_SPACES( value );
            }
        } else {
            value = GetNextWord2( value, fn, ',' );
        }
#ifndef VICOMP
        if( EditFlags.CompileScript ) {
#endif
            sprintf( str, "%d", j );
            strcat( WorkLine->data, str );
            if( *fn != '\0' ) {
                switch( j ) {
                case SETVAR_T_STATUSSTRING:
                case SETVAR_T_FILEENDSTRING:
                case SETVAR_T_HISTORYFILE:
                case SETVAR_T_TMPDIR:
                case SETVAR_T_TAGFILENAME:
                    StrMerge( 4, WorkLine->data, SingleBlank, SingleDQuote, fn, SingleDQuote );
                    break;
                case SETVAR_T_COMMANDCURSORTYPE:
                case SETVAR_T_OVERSTRIKECURSORTYPE:
                case SETVAR_T_INSERTCURSORTYPE:
                    StrMerge( 2, WorkLine->data, SingleBlank, fn );
                    value = GetNextWord1( value, fn );
                    if( *fn != '\0' ) {
                        StrMerge( 2, WorkLine->data, SingleBlank, fn );
                    }
                    break;
                case SETVAR_T_TILECOLOR:
                    StrMerge( 2, WorkLine->data, SingleBlank, fn );
                    value = GetNextWord2( value, fn, ',' );
                    if( *fn == '\0' ) {
                        return( ERR_INVALID_SET_COMMAND );
                    }
                    value = GetNextWord1( value, str );
                    if( *str == '\0' ) {
                        return( ERR_INVALID_SET_COMMAND );
                    }
                    StrMerge( 4, WorkLine->data, fn, SingleBlank, str, SingleBlank );
                    break;
                case SETVAR_T_STATUSSECTIONS:
                    StrMerge( 2, WorkLine->data, SingleBlank, fn );
                    for( value = GetNextWord2( value, fn, ',' ); *fn != '\0'; value = GetNextWord2( value, fn, ',' ) ) {
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
            }
            *pvalue = value;
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
                EditVars.StatusSections = _MemReallocArray( EditVars.StatusSections, unsigned short, EditVars.NumStatusSections + 1 );
                EditVars.StatusSections[EditVars.NumStatusSections] = (unsigned short)k;
                EditVars.NumStatusSections++;
                value = GetNextWord2( value, fn, ',' );
                if( *fn == '\0' ) {
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
            ReplaceString( &EditVars.FileEndString, fn );
            ResetAllWindows();
            redisplay = true;
            break;
        case SETVAR_T_STATUSSTRING:
            ReplaceString( &EditVars.StatusString, fn );
            if( !BAD_ID( status_window_id ) ) {
                ClearWindow( status_window_id );
                UpdateStatusWindow();
            }
            if( msgFlag ) {
                MySprintf( fn, "statusstring set to %s", EditVars.StatusString );
            }
            break;
        case SETVAR_T_GREPDEFAULT:
            ReplaceString( &EditVars.GrepDefault, fn );
            break;
        case SETVAR_T_TILECOLOR:
            if( EditVars.TileColors == NULL ) {
                EditVars.TileColors = _MemAllocArray( type_style, EditVars.MaxTileColors + 1 );
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
            value = GetNextWord2( value, fn, ',' );
            if( *fn == '\0' ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            EditVars.TileColors[clr].foreground = atoi( fn );
            value = GetNextWord1( value, fn );
            if( *fn == '\0' ) {
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
            ReplaceString( &EditVars.SpawnPrompt, fn );
            if( msgFlag ) {
                MySprintf( fn, "prompt string set to %s", EditVars.SpawnPrompt );
            }
            break;
        case SETVAR_T_FIGNORE:
            if( *fn == '\0' ) {
                MemFreePtr( (void **)&EditVars.FIgnore );
                EditVars.CurrFIgnore = 0;
                if( msgFlag ) {
                    MySprintf( fn, "fignore reset" );
                }
            } else {
                EditVars.FIgnore = MemRealloc( EditVars.FIgnore, EXTENSION_LENGTH * (EditVars.CurrFIgnore + 1) );
                str[0] = '.';
                str[1] = '\0';
                strcat( str, fn );
                str[EXTENSION_LENGTH - 1] = '\0';
                strcpy( &EditVars.FIgnore[EXTENSION_LENGTH * EditVars.CurrFIgnore], str );
                EditVars.CurrFIgnore++;
                if( msgFlag ) {
                    MySprintf( fn, "%s added to fignore", str );
                }
            }
            break;
        case SETVAR_T_HISTORYFILE:
            ReplaceString( &EditVars.HistoryFile, fn );
            if( msgFlag ) {
                MySprintf( fn, "history file set to %s", EditVars.HistoryFile );
            }
            break;

        case SETVAR_T_TAGFILENAME:
            ReplaceString( &EditVars.TagFileName, fn );
            if( msgFlag ) {
                MySprintf( fn, "tag file name set to %s", EditVars.TagFileName );
            }
            break;

        case SETVAR_T_FILENAME:
            if( CurrentFile != NULL ) {
                ReplaceString( &(CurrentFile->name), fn );
                SetFileWindowTitle( current_window_id, CurrentInfo, true );
                if( msgFlag ) {
                    MySprintf( fn, "filename set to %s", CurrentFile->name );
                }
                FileSPVAR();
            }
            break;
        case SETVAR_T_TMPDIR:
            ReplaceString( &EditVars.TmpDir, fn );
            VerifyTmpDir();
            if( msgFlag ) {
                MySprintf( fn, "tmpdir set to %s", EditVars.TmpDir );
            }
            break;
        case SETVAR_T_WORD:
            ReplaceString( &EditVars.WordDefn, fn );
            InitWordSearch( EditVars.WordDefn );
            if( msgFlag ) {
                MySprintf( fn, "word set to %s", EditVars.WordDefn );
            }
            break;
        case SETVAR_T_WORDALT:
            ReplaceString( &EditVars.WordAltDefn, fn );
            if( msgFlag ) {
                MySprintf( fn, "wordalt set to %s", EditVars.WordAltDefn );
            }
            break;
        case SETVAR_T_MAGICSTRING:
            ReplaceString( &EditVars.Majick, fn );
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
            lval = GetConstExpr();
            if( lval < 0 )
                lval = 0;
            if( lval > 100 )
                lval = 100;
            ct.height = (unsigned char)lval;
            value = GetNextWord2( value, fn, ',' );
            if( *fn == '\0' ) {
                ct.width = 100;
            } else {
                i = setjmp( jmpaddr );
                if( i != 0 ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                StartExprParse( fn, jmpaddr );
                lval = GetConstExpr();
                if( lval < 0 )
                    lval = 0;
                if( lval > 100 )
                    lval = 100;
                ct.width = (unsigned char)lval;
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
                MySprintf( fn, "%s cursor type set to %d,%d", name, ct.height, ct.width );
            }
            break;
        default:
            i = setjmp( jmpaddr );
            if( i != 0 ) {
                return( ERR_INVALID_SET_COMMAND );
            }
            StartExprParse( fn, jmpaddr );
            lval = GetConstExpr();
            if( lval < 0 ) {
                lval = 0;
            }
            switch( j ) {
            case SETVAR_T_WRAPMARGIN:
                EditVars.WrapMargin = lval;
                break;
            case SETVAR_T_CURSORBLINKRATE:
                SetCursorBlinkRate( lval );
                break;
            case SETVAR_T_MAXPUSH:
                EditVars.MaxPush = lval;
                if( EditVars.MaxPush < 1 ) {
                    EditVars.MaxPush = 1;
                }
                InitFileStack();
                break;
            case SETVAR_T_RADIX:
                EditVars.Radix = lval;
                break;
            case SETVAR_T_AUTOSAVEINTERVAL:
                EditVars.AutoSaveInterval = lval;
                SetNextAutoSaveTime();
                break;
            case SETVAR_T_LANGUAGE:
                if( lval < 0 || lval >= VI_LANG_MAX ) {
                    return( ERR_INVALID_SET_COMMAND );
                }
                if( CurrentInfo != NULL ) {
                    LangFini( CurrentInfo->fsi.Language );
                    LangInit( lval );
                    redisplay = true;
                }
                break;
            case SETVAR_T_MOVECOLOR:
                EditVars.MoveColor = lval;
                break;
            case SETVAR_T_RESIZECOLOR:
                EditVars.ResizeColor = lval;
                break;
            case SETVAR_T_MOUSEDCLICKSPEED:
                EditVars.MouseDoubleClickSpeed = lval;
                break;
            case SETVAR_T_MOUSESPEED:
                SetMouseSpeed( lval );
                break;
            case SETVAR_T_MOUSEREPEATDELAY:
                EditVars.MouseRepeatDelay = lval;
                break;
            case SETVAR_T_CURRENTSTATUSCOLUMN:
                EditVars.CurrentStatusColumn = lval;
                InitMenu();
                break;
            case SETVAR_T_ENDOFLINECHAR:
                EditVars.EndOfLineChar = (unsigned char)lval;
                break;
            case SETVAR_T_EXITATTR:
                EditVars.ExitAttr = (viattr_t)lval;
                break;
            case SETVAR_T_INACTIVEWINDOWCOLOR:
                EditVars.InactiveWindowColor = lval;
                break;
            case SETVAR_T_TABAMOUNT:
                EditVars.TabAmount = lval;
                break;
            case SETVAR_T_SHIFTWIDTH:
                EditVars.ShiftWidth = lval;
                break;
            case SETVAR_T_PAGELINESEXPOSED:
                EditVars.PageLinesExposed = lval;
                break;
            case SETVAR_T_HARDTAB:
                EditVars.HardTab = lval;
                redisplay = true;
                break;
            case SETVAR_T_STACKK:
                if( EditFlags.Starting ) {
                    EditVars.StackK = lval;
                }
                break;
            case SETVAR_T_LINENUMWINWIDTH:
                EditVars.LineNumWinWidth = lval;
                break;
            case SETVAR_T_MAXWINDOWTILEX:
                EditVars.MaxWindowTileX = lval;
                break;
            case SETVAR_T_MAXWINDOWTILEY:
                EditVars.MaxWindowTileY = lval;
                break;
            case SETVAR_T_MAXSWAPK:
                SwapBlockInit( lval );
                break;
            case SETVAR_T_MAXEMSK:
  #if defined( USE_EMS )
                EMSBlockInit( lval );
  #endif
                break;
            case SETVAR_T_MAXXMSK:
  #if defined( USE_XMS )
                XMSBlockInit( lval );
  #endif
                break;
            case SETVAR_T_MAXFILTERHISTORY:
                HistInit( &EditVars.Hist[HIST_FILTER], lval );
                break;
            case SETVAR_T_MAXCLHISTORY:
                HistInit( &EditVars.Hist[HIST_CMD], lval );
                break;
            case SETVAR_T_MAXFINDHISTORY:
                HistInit( &EditVars.Hist[HIST_FIND], lval );
                break;
            case SETVAR_T_MAXLASTFILESHISTORY:
                HistInit( &EditVars.Hist[HIST_LASTFILES], lval );
                break;
            case SETVAR_T_MAXTILECOLORS:
                k = (EditVars.TileColors == NULL) ? 0 : EditVars.MaxTileColors + 1;
                EditVars.MaxTileColors = lval;
                EditVars.TileColors = _MemReallocArray( EditVars.TileColors, type_style, EditVars.MaxTileColors + 1 );
                for( ; k <= EditVars.MaxTileColors; ++k ) {
                    EditVars.TileColors[k].foreground = -1;
                    EditVars.TileColors[k].background = -1;
                    EditVars.TileColors[k].font = -1;
                }
                break;
            case SETVAR_T_CLOCKX:
                EditVars.ClockX = lval;
                GetClockStart();
                break;
            case SETVAR_T_CLOCKY:
                EditVars.ClockY = lval;
                GetClockStart();
                break;
            case SETVAR_T_SPINX:
                EditVars.SpinX = lval;
                GetSpinStart();
                break;
            case SETVAR_T_SPINY:
                EditVars.SpinY = lval;
                GetSpinStart();
                break;
            case SETVAR_T_MAXLINELEN:
                /* file save fails if 1 line is > MAX_IO_BUFFER */
                if( lval > MAX_IO_BUFFER )
                    lval = MAX_IO_BUFFER;
                EditVars.MaxLine = lval;
                StaticStart();
                WorkLine = MemRealloc( WorkLine, sizeof( line ) + EditVars.MaxLine + 2 );
                break;
            case SETVAR_T_TOOLBARBUTTONHEIGHT:
                EditVars.ToolBarButtonHeight = lval;
  #ifdef __WIN__
                ResizeRoot();
  #endif
                break;
            case SETVAR_T_TOOLBARBUTTONWIDTH:
                EditVars.ToolBarButtonWidth = lval;
  #ifdef __WIN__
                ResizeRoot();
  #endif
                break;
            case SETVAR_T_TOOLBARCOLOR:
                EditVars.ToolBarColor = lval;
  #ifdef __WIN__
                {
                    window_id   toolbar_wid;

                    toolbar_wid = GetToolbarWindow();
                    if( toolbar_wid != NULL ) {
                        InvalidateRect( toolbar_wid, NULL, TRUE );
                        UpdateWindow( toolbar_wid );
                    }
                }
  #endif
                break;
            default:
                return( ERR_INVALID_SET_COMMAND );
            }

            if( msgFlag ) {
                MySprintf( fn, "%s set to %d", GetTokenStringCVT( SetVarTokens, j, settokstr, true ), lval );
            }
            break;
        }
        *pvalue = value;
    }

    if( msgFlag && rc == ERR_NO_ERR && !EditFlags.Quiet ) {
        setMessage( fn, redisplay );
        rc = DO_NOT_CLEAR_MESSAGE_WINDOW;
    }
    return( rc );
#endif /* VICOMP */

} /* processSetToken */

#ifndef VICOMP
/*
 * SettingSelected - a setting was selected from the dialog
 */
vi_rc SettingSelected( const char *item, char *value, int *winflag )
{
    int         id;
    bool        isbool;

    id = Tokenize( SetVarTokens, item, false );
    if( id != TOK_INVALID ) {
        isbool = false;
    } else {
        id = Tokenize( SetFlagTokens, item, false );
        if( id == TOK_INVALID ) {
            return( ERR_INVALID_SET_COMMAND );
        }
        isbool = true;
    }
    return( processSetToken( id, value, (const char **)&value, winflag, isbool ) );

} /* SettingSelected */


typedef struct {
    char        *setting;
    char        *val;
} set_data;

  #ifndef __WIN__

/*
 * compareString - quicksort comparison
 */
static int compareString( void const *p1, void const *p2 )
{
    return( stricmp( (*(const set_data **)p1)->setting,(*(const set_data **)p2)->setting ) );

} /* compareString */

/*
 * getSetInfo - build string of values
 */
static list_linenum getSetInfo( char ***vals, char ***list, size_t *longest )
{
    list_linenum    i;
    size_t          i1;
    size_t          i2;
    char            settokstr[TOK_MAX_LEN + 1];
    char            tmpstr[MAX_STR];
    set_data        **sdata;
    list_linenum    tc;
    int             tc1;
    int             tc2;

    tc1 = GetNumberOfTokens( SetVarTokens );
    tc2 = GetNumberOfTokens( SetFlagTokens );
    tc = tc1 + tc2;
    sdata = _MemAllocArray( set_data *, tc );
    *list = _MemAllocList( tc );
    *vals = _MemAllocList( tc );

    for( i1 = 0; i1 < tc1; i1++ ) {
        sdata[i1] = MemAlloc( sizeof( set_data ) );
        sdata[i1]->setting = DupString( GetTokenStringCVT( SetVarTokens, i1, settokstr, true ) );
        sdata[i1]->val = DupString( getOneSetVal( i1, false, tmpstr, true ) );
    }
    for( i2 = 0; i2 < tc2; i2++ ) {
        sdata[tc1 + i2] = MemAlloc( sizeof( set_data ) );
        sdata[tc1 + i2]->setting = DupString( GetTokenStringCVT( SetFlagTokens, i2, settokstr, true ) );
        sdata[tc1 + i2]->val = DupString( getOneSetVal( i2, true, tmpstr, true ) );
    }
    qsort( sdata, tc, sizeof( set_data * ), compareString );
    for( i = 0; i < tc; i++ ) {
        (*list)[i] = sdata[i]->setting;
        (*vals)[i] = sdata[i]->val;
        MemFree( sdata[i] );
    }
    MemFree( sdata );
    i1 = GetLongestTokenLength( SetVarTokens );
    i2 = GetLongestTokenLength( SetFlagTokens );
    if( i1 > i2 ) {
        *longest = i1;
    } else {
        *longest = i2;
    }
    return( tc );

} /* getSetInfo */
  #endif
#endif /* VICOMP */

/*
 * Set - set editor control variable
 */
vi_rc Set( const char *name )
{
    char            fn[MAX_STR];
    vi_rc           rc = ERR_NO_ERR;
    int             j;
    int             k;
    int             winflag;
    const char      *pfn;
#ifndef VICOMP
#ifndef __WIN__
    short           tmp;
    list_linenum    tc;
    list_linenum    i;
    char            **vals = NULL;
    char            **list;
    size_t          longest;
#endif
#endif

    /*
     * get item to set
     */
#ifndef VICOMP
    msgFlag = false;
    if( !EditFlags.ScriptIsCompiled ) {
#endif
        name = GetNextWord2( name, fn, '=' );
#ifndef VICOMP
    } else {
        name = GetNextWord1( name, fn );
    }
#endif

    if( *fn == '\0' ) {
#ifndef VICOMP
        if( !EditFlags.WindowsStarted ) {
            return( ERR_NO_ERR );
        }
  #ifndef __WIN__
        tc = getSetInfo( &vals, &list, &longest );
        tmp = setw_info.area.y2;
        i = setw_info.area.y2 - setw_info.area.y1 + BORDERDIFF( setw_info );
        if( tc < i ) {
            setw_info.area.y2 -= (windim)( i - tc );
        }
        rc = SelectItemAndValue( &setw_info, "Settings", list, tc, SettingSelected, 1, vals, longest + 3 );
        setw_info.area.y2 = tmp;
        MemFreeList( tc, vals );
        MemFreeList( tc, list );
        ReDisplayScreen();
  #endif
#endif /* VICOMP */
        return( rc );
    } else {
#ifndef VICOMP
        if( !EditFlags.Starting) {
            msgFlag = true;
        }
#endif
        do {
#ifndef VICOMP
            if( !EditFlags.ScriptIsCompiled ) {
#endif
                j = Tokenize( SetVarTokens, fn, false );
                if( j == TOK_INVALID ) {
                    pfn = fn;
                    k = 1;
                    if( tolower( pfn[0] ) == 'n' && tolower( pfn[1] ) == 'o' ) {
                        pfn += 2;
                        k = -1;
                    }
                    j = Tokenize( SetFlagShortTokens, pfn, false );
                    if( j == TOK_INVALID ) {
                        j = Tokenize( SetFlagTokens, pfn, false );
                        if( j == TOK_INVALID ) {
                            return( ERR_INVALID_SET_COMMAND );
                        }
                    }
                    j += SETVAR_T_;
                    j *= k;
                }
#ifndef VICOMP
            } else {
                j = atoi( fn );
            }
#endif
            rc = processSetToken( j, NULL, &name, &winflag, false );
            if( rc > ERR_NO_ERR ) {
                break;
            }
            name = GetNextWord2( name, fn, ',' );
        } while( *fn != '\0' );
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
const char *GetASetVal( const char *token, char *tmpstr )
{
    int         j;

    j = Tokenize( SetVarTokens, token, false );
    if( j != TOK_INVALID ) {
        return( getOneSetVal( j, false, tmpstr, false ) );
    }
    j = Tokenize( SetFlagShortTokens, token, false );
    if( j == TOK_INVALID ) {
        j = Tokenize( SetFlagTokens, token, false );
    }
    if( j != TOK_INVALID ) {
        return( getOneSetVal( j, true, tmpstr, false ) );
    }
    return( "" );

} /* GetASetVal */

char *ExpandTokenSet( char *token_no, char *buff )
{
    bool        val;
    int         tok;
    char        settokstr[TOK_MAX_LEN + 1];

    tok = atoi( token_no );
    val = true;
    if( tok < 0 ) {
        tok *= -1;
        val = false;
    }
    if( tok >= SETVAR_T_ ) {
        sprintf( buff, "%s%s", GET_BOOL_PREFIX( val ), GetTokenStringCVT( SetFlagTokens, tok - SETVAR_T_, settokstr, true ) );
    } else {
        sprintf( buff, "%s" CFG_SET_SEPARATOR, GetTokenStringCVT( SetVarTokens, tok, settokstr, true ) );
    }
    return( buff );
}
#endif
