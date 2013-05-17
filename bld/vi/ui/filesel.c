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
#include "posix.h"
#include "win.h"
#include "menu.h"
#include "source.h"

extern int      CurrentMenuNumber;

/*
 * SelectFileOpen - select file from specified directory
 */
vi_rc SelectFileOpen( char *dir, char **result_ptr, char *mask, bool want_all_dirs )
{
    char                dd[FILENAME_MAX], cdir[FILENAME_MAX];
    int                 j;
    file                *cfile;
    fcb                 *cfcb;
    line                *cline;
    selflinedata        sfd;
    bool                need_entire_path;
    char                *result = *result_ptr;
    vi_rc               rc;

    /*
     * get current directory
     */
    strcpy( dd, dir );
    strcpy( cdir, dir );
    SetCWD( dir );
    need_entire_path = FALSE;

    /*
     * work through all files
     */
    for( ;; ) {

        if( dd[strlen( dd ) - 1] != FILE_SEP ) {
            strcat( dd, FILE_SEP_STR );
        }
        strcat( dd, mask );
        rc = GetSortDir( dd, want_all_dirs );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }

        /*
         * allocate temporary file structure
         */
        cfile = FileAlloc( NULL );

        FormatDirToFile( cfile, TRUE );

        /*
         * go get selected line
         */
        memset( &sfd, 0, sizeof( sfd ) );
        sfd.f = cfile;
        sfd.wi = &dirw_info;
        sfd.title = CurrentDirectory;
        sfd.show_lineno = TRUE;
        sfd.cln = 1;
        sfd.eiw = NO_WINDOW;
        rc = SelectLineInFile( &sfd );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( sfd.sl == -1 ) {
            result[0] = 0;
            break;
        }
        j = (int) sfd.sl - 1;
        if( j >= DirFileCount || DirFiles[j]->attr & _A_SUBDIR ) {
            if( j >= DirFileCount ) {
                GimmeLinePtr( j + 1, cfile, &cfcb, &cline );
                dd[0] = cline->data[3];
                dd[1] = ':';
                dd[2] = 0;
            } else {
                strcpy( dd, cdir );
                if( dd[strlen(dd) - 1] != FILE_SEP ) {
                    strcat( dd, FILE_SEP_STR );
                }
                strcat( dd, DirFiles[j]->name );
            }
            FreeEntireFile( cfile );
            rc = SetCWD( dd );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            need_entire_path = TRUE;
            strcpy( cdir, CurrentDirectory );
            strcpy( dd, CurrentDirectory );
            continue;
        }
        if( need_entire_path ) {
            strcpy( result, CurrentDirectory );
            if( result[strlen(result) - 1] != FILE_SEP ) {
                strcat( result, FILE_SEP_STR );
            }
        } else {
            result[0] = 0;
        }
        strcat( result, DirFiles[j]->name );
        break;

    }

    /*
     * done, free memory
     */
    FreeEntireFile( cfile );
    DCDisplayAllLines();
    return( rc );

} /* SelectFileOpen */

static window_id        cWin;
static bool             isMenu;

/*
 * displayGenericLines - display all lines in a window
 */
static vi_rc displayGenericLines( file *f, linenum pagetop, int leftcol,
                                linenum hilite, type_style *style, hilst *hilist,
                                char **vals, int valoff )
{
    int         i, j, k, text_lines;
    linenum     cl = pagetop;
    fcb         *cfcb, *tfcb;
    line        *cline;
    hilst       *ptr;
    type_style  *text, *hot_key;
    window_info *info;
    type_style  base;
    char        tmp[MAX_STR];
//    bool        disabled;
    vi_rc       rc;

    /*
     * get pointer to first line on page, and window info
     */
    rc = GimmeLinePtr( pagetop, f, &cfcb, &cline );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    base.foreground = WindowAuxInfo( cWin, WIND_INFO_TEXT_COLOR );
    base.background = WindowAuxInfo( cWin, WIND_INFO_BACKGROUND_COLOR );
    base.font = WindowAuxInfo( cWin, WIND_INFO_TEXT_FONT );
    text_lines = WindowAuxInfo( cWin, WIND_INFO_TEXT_LINES );

    /*
     * mark all fcb's as being not in display
     */
    for( tfcb = f->fcbs.head; tfcb != NULL; tfcb = tfcb->next ) {
        tfcb->on_display = FALSE;
    }
    cfcb->on_display = TRUE;

    /*
     * run through each line in the window
     */
    ptr = hilist;
    if( ptr != NULL ) {
        ptr += pagetop - 1;
    }
    for( j = 1; j <= text_lines; j++ ) {
        if( cline != NULL ) {
            if( isMenu ) {
                if( InvokeMenuHook( CurrentMenuNumber, cl ) == -1 ) {
//                    disabled = TRUE;
                    if( cl == hilite ) {
                        info = &activegreyedmenu_info;
                    } else {
                        info = &greyedmenu_info;
                    }
                } else {
//                    disabled = FALSE;
                    if( cl == hilite ) {
                        info = &activemenu_info;
                    } else {
                        info = &menuw_info;
                    }
                }
                text = &info->text;
                hot_key = &info->hilight;
            } else {
                text = &base;
                if( cl == hilite ) {
                    text = style;
                }
                hot_key = text;
            }

            /*
             * now, display what we can of the line on the window
             */
            if( cline->len == 0 ) {
                DisplayCrossLineInWindow( cWin, j );
                goto evil_goto;
            } else if( cline->len > leftcol ) {
                if( vals != NULL ) {
                    i = cline->len - leftcol;
                    strncpy( tmp, &(cline->data[leftcol]), EditVars.WindMaxWidth + 5 );
                    for( k = i; k < valoff; k++ ) {
                        tmp[k] = ' ';
                    }
                    tmp[k] = 0;
                    strcat( tmp, vals[j + pagetop - 2] );
                    DisplayLineInWindowWithColor( cWin, j, tmp, text, 0 );
                } else {
                    DisplayLineInWindowWithColor( cWin, j, cline->data, text, leftcol );
                }
            } else {
                DisplayLineInWindowWithColor( cWin, j, SingleBlank, text, 0 );
            }
            if( ptr != NULL ) {
                SetCharInWindowWithColor( cWin, j, 1 + ptr->_offs, ptr->_char, hot_key );
            }
evil_goto:  if( ptr != NULL ) {
                ptr += 1;
            }
            rc = GimmeNextLinePtr( f, &cfcb, &cline );
            if( rc != ERR_NO_ERR ) {
                if( rc == ERR_NO_MORE_LINES ) {
                    continue;
                }
                return( rc );
            }
            cl++;
            cfcb->on_display = TRUE;
        } else {
            DisplayLineInWindow( cWin, j, "~" );
        }

    }
    return( ERR_NO_ERR );

} /* displayGenericLines */

typedef enum {
    MS_NONE,
    MS_PAGEDOWN,
    MS_PAGEUP,
    MS_DOWN,
    MS_UP,
    MS_EXPOSEDOWN,
    MS_EXPOSEUP
} ms_type;

static window_id        oWin, mouseWin;
static int              mouseLine = -1;
static ms_type          mouseScroll;
static bool             rlMenu;
static int              rlMenuNum;

/*
 * SelectLineMouseHandler - handle mouse events for line selector
 */
bool SelectLineMouseHandler( window_id id, int win_x, int win_y )
{
    int x, y, i;

    if( LastMouseEvent != MOUSE_DRAG && LastMouseEvent != MOUSE_PRESS &&
        LastMouseEvent != MOUSE_DCLICK && LastMouseEvent != MOUSE_RELEASE &&
        LastMouseEvent != MOUSE_REPEAT && LastMouseEvent != MOUSE_PRESS_R ) {
        return( FALSE );
    }
    mouseWin = id;
    mouseScroll = MS_NONE;

    if( !isMenu && (id == cWin) && (LastMouseEvent == MOUSE_REPEAT ||
                                    LastMouseEvent == MOUSE_PRESS ||
                                    LastMouseEvent == MOUSE_DCLICK ) ) {
        x = WindowAuxInfo( cWin, WIND_INFO_WIDTH );
        y = WindowAuxInfo( cWin, WIND_INFO_HEIGHT );
        if( win_x == x - 1 ) {
            if( win_y == 1 ) {
                mouseScroll = MS_EXPOSEUP;
                return( TRUE );
            } else if( win_y == y - 2 ) {
                mouseScroll = MS_EXPOSEDOWN;
                return( TRUE );
            } else if( win_y > 1 && win_y < y / 2 ) {
                mouseScroll = MS_PAGEUP;
                return( TRUE );
            } else if( win_y >= y / 2 && win_y < y - 1 ) {
                mouseScroll = MS_PAGEDOWN;
                return( TRUE );
            }
        }
    }
    if( LastMouseEvent == MOUSE_REPEAT ) {
        if( id != cWin && !isMenu ) {
            y = WindowAuxInfo( cWin, WIND_INFO_Y1 );
            if( MouseRow < y ) {
                mouseScroll = MS_UP;
                return( TRUE );
            }
            y = WindowAuxInfo( cWin, WIND_INFO_Y2 );
            if( MouseRow > y ) {
                mouseScroll = MS_DOWN;
                return( TRUE );
            }
        }
        return( FALSE );
    }
    if( isMenu && EditFlags.Menus && id == MenuWindow &&
        LastMouseEvent != MOUSE_PRESS_R ) {
        i = GetMenuIdFromCoord( win_x );
        if( i >= 0 ) {
            rlMenuNum = i - GetCurrentMenuId();
            if( rlMenuNum != 0 ) {
                rlMenu = TRUE;
            }
        }
        return( TRUE );
    }
    if( id != cWin && id != oWin ) {
        return( TRUE );
    }

    if( !InsideWindow( id, win_x, win_y ) ) {
        return( FALSE );
    }
    mouseLine = win_y - 1;
    return( TRUE );

} /* SelectLineMouseHandler */

/*
 * adjustCLN - adjust current line number and pagetop
 */
static bool adjustCLN( linenum *cln, linenum *pagetop, int amt,
                       linenum endline, int text_lines )
{
    bool        drawbord = FALSE;

    if( !isMenu ) {
        if( amt < 0 ) {
            if( *cln + amt > 1 ) {
                *cln += amt;
                if( *cln < *pagetop ) {
                    *pagetop += amt;
                    drawbord = TRUE;
                }
            } else {
                *cln = 1;
                *pagetop = 1;
            }
        } else {
            if( *cln + amt < endline ) {
                *cln += amt;
                if( *cln >= *pagetop + text_lines ) {
                    *pagetop += amt;
                    drawbord = TRUE;
                }
            } else {
                *cln = endline;
                *pagetop = endline - text_lines + 1;
            }
        }
    } else {
        *cln += amt;
        if( amt < 0 ) {
            if( *cln <= 0 ) {
                while( *cln <= 0 ) {
                    *cln += endline;
                }
                *pagetop = *cln - text_lines + 1;
                drawbord = TRUE;
            } else if( *cln < *pagetop ) {
                *pagetop += amt;
                drawbord = TRUE;
            }
        } else {
            if( *cln <= endline ) {
                if( *cln >= *pagetop + text_lines ) {
                    *pagetop += amt;
                    drawbord = TRUE;
                }
            } else {
                while( *cln > endline ) {
                    *cln -= endline;
                }
                *pagetop = *cln - text_lines + 1;
                drawbord = TRUE;
            }
        }
    }
    if( *pagetop < 1 ) {
        *pagetop = 1;
    }
    if( endline - *pagetop + 1 < text_lines ) {
        *pagetop = endline - text_lines + 1;
        drawbord = TRUE;
        if( *pagetop < 1 ) {
            *pagetop = 1;
        }
    }
    return( drawbord );

} /* adjustCLN */

/*
 * SelectLineInFile - select a line in a given file
 */
vi_rc SelectLineInFile( selflinedata *sfd )
{
    int         i, winflag;
    int         leftcol = 0, key2;
    bool        done = FALSE, redraw = TRUE;
    bool        hiflag = FALSE, drawbord = FALSE;
    int         farx, text_lines;
    linenum     pagetop = 1, lln = 1;
    char        tmp[MAX_STR];
    hilst       *ptr;
    linenum     cln;
    linenum     endline;
    vi_rc       rc;
    vi_key      key;

    /*
     * create the window
     */
    cln = sfd->cln;
    endline = sfd->f->fcbs.tail->end_line;
    farx = sfd->wi->x2;
    if( sfd->show_lineno ) {
        farx++;
    }
    if( sfd->hilite != NULL ) {
        hiflag = TRUE;
    }
    rc = NewWindow2( &cWin, sfd->wi );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    if( !sfd->is_menu ) {
        WindowAuxUpdate( cWin, WIND_INFO_HAS_SCROLL_GADGETS, TRUE );
        DrawBorder( cWin );
    }
    oWin = sfd->eiw;
    isMenu = sfd->is_menu;
    PushMouseEventHandler( SelectLineMouseHandler );
    KillCursor();
    text_lines = WindowAuxInfo( cWin, WIND_INFO_TEXT_LINES );
    sfd->sl = -1;
    if( sfd->title != NULL ) {
        WindowTitle( cWin, sfd->title );
    }
    pagetop = text_lines * (cln / text_lines);
    if( cln % text_lines != 0 ) {
        pagetop++;
    }
    key = 0;
    if( LastEvent == VI_KEY( MOUSEEVENT ) ) {
        DisplayMouse( TRUE );
    }

    /*
     * now, allow free scrolling and selection
     */
    while( !done ) {

        if( redraw ) {
            if( sfd->show_lineno ) {
                MySprintf(tmp, "%l/%l", cln, endline );
                i = sfd->wi->x2 - sfd->wi->x1;
                WindowBorderData( cWin, tmp, i - strlen( tmp ) );
                drawbord = TRUE;
            }
            if( hiflag ) {
                ptr = sfd->hilite;
                ptr += cln - 1;
                if( ptr->_char == (char)-1 ) {
                    if( cln > lln ) {
                        cln++;
                    } else if( cln < lln ) {
                        cln--;
                    }
                }
            }
            if( drawbord ) {
                DrawBorder( cWin );
            }
            displayGenericLines( sfd->f, pagetop, leftcol, cln, &(sfd->wi->hilight), sfd->hilite, sfd->vals, sfd->valoff );
        }
        lln = cln;
        redraw = TRUE;
        drawbord = FALSE;
        mouseLine = -1;
        rlMenu = FALSE;
        if( key == VI_KEY( MOUSEEVENT ) ) {
            DisplayMouse( TRUE );
        }
        key = GetNextEvent( TRUE );
        if( hiflag && ((key >= VI_KEY( ALT_A ) && key <= VI_KEY( ALT_Z )) ||
                       (key >='a' && key <= 'z') || (key >= 'A' && key <= 'Z') ||
                       (key >= '1' && key <= '9')) ) {
            i = 0;
            if( key >= VI_KEY( ALT_A ) && key <= VI_KEY( ALT_Z ) ) {
                key2 = key - VI_KEY( ALT_A ) + 'A';
            } else if( key >= 'a' && key <= 'z' ) {
                key2 = key - 'a' + 'A';
            } else {
                key2 = key;
            }
            ptr = sfd->hilite;
            while( ptr->_char != '\0' ) {
                if( toupper( ptr->_char ) == key2 ) {
                    cln = i + 1;
                    key = VI_KEY( ENTER );
                    break;
                }
                ++i;
                ++ptr;
            }
        }

        /*
         * check if a return-event has been selected
         */
        if( sfd->retevents != NULL ) {
            i = 0;
            if( key == VI_KEY( MOUSEEVENT ) ) {
                if( mouseWin == oWin && LastMouseEvent == MOUSE_PRESS ) {
                    DisplayMouse( FALSE );
                    sfd->event = sfd->retevents[mouseLine];
                    key = VI_KEY( ENTER );
                }
            } else {
                while( sfd->retevents[i] != 0 ) {
                    if( key == sfd->retevents[i] ) {
                        sfd->event = key;
                        key = VI_KEY( ENTER );
                        break;
                    }
                    i++;
                }
            }
        }

        /*
         * process key stroke
         */
        switch( key ) {
        case VI_KEY( MOUSEEVENT ):
            DisplayMouse( FALSE );
            if( hiflag ) {
                ptr = sfd->hilite;
                ptr += mouseLine;
                if( ptr->_char == (char) -1 ) {
                    break;
                }
            }
            if( rlMenu && sfd->allow_rl != NULL ) {
                *(sfd->allow_rl) = rlMenuNum;
                done = TRUE;
                break;
            }
            if( mouseScroll != MS_NONE ) {
                switch( mouseScroll ) {
                case MS_UP: goto evil_up;
                case MS_DOWN: goto evil_down;
                case MS_PAGEUP: goto evil_pageup;
                case MS_PAGEDOWN: goto evil_pagedown;
                case MS_EXPOSEDOWN:
                    adjustCLN( &cln, &pagetop, pagetop + text_lines - cln - 1, endline, text_lines );
                    adjustCLN( &cln, &pagetop, 1, endline, text_lines );
                    drawbord = TRUE;
                    break;
                case MS_EXPOSEUP:
                    adjustCLN( &cln, &pagetop, pagetop - cln, endline, text_lines );
                    adjustCLN( &cln, &pagetop, -1, endline, text_lines );
                    drawbord = TRUE;
                    break;

                }
                break;
            }
            switch( LastMouseEvent ) {
            case MOUSE_DRAG:
                if( mouseWin != cWin ) {
                    break;
                }
                cln = mouseLine + pagetop;
                break;
            case MOUSE_RELEASE:
                if( !sfd->is_menu ) {
                    break;
                }
                if( mouseWin == cWin ) {
                    cln = mouseLine + pagetop;
                    if( cln <= endline ) {
                        goto evil_enter;
                    }
                }
                break;
            case MOUSE_DCLICK:
                if( mouseWin != cWin ) {
                    AddCurrentMouseEvent();
                    done = TRUE;
                } else {
                    cln = mouseLine + pagetop;
                    if( cln <= endline ) {
                        goto evil_enter;
                    }
                }
                break;
            case MOUSE_PRESS_R:
                if( mouseWin != cWin ) {
                    AddCurrentMouseEvent();
                    done = TRUE;
                }
                break;
            case MOUSE_PRESS:
                if( mouseWin != cWin ) {
                    AddCurrentMouseEvent();
                    done = TRUE;
                } else {
                    cln = mouseLine + pagetop;
                }
                break;
            }
            break;

        case VI_KEY( ESC ):
            done = TRUE;
            break;

        evil_enter:
        case VI_KEY( ENTER ):
        case ' ':
            /*
             * see if we need to do a callback for this
             */
            if( sfd->checkres != NULL ) {
                line    *cline;
                fcb     *cfcb;
                char    data[64];

                i = cln - 1;
                GimmeLinePtr( cln, sfd->f, &cfcb, &cline );
                strcpy( data, cline->data );
                RemoveLeadingSpaces( data );
                winflag = FALSE;
                strcpy( tmp, sfd->vals[i] );
                rc = sfd->checkres( data, tmp, &winflag );
                if( winflag ) {
                    if( winflag == 2 ) {
                        winflag = TRUE;
                    } else {
                        winflag = FALSE;
                    }
                }
                if( winflag ) {
                    MoveWindowToFront( cWin );
                }
                if( rc == ERR_NO_ERR ) {
                    AddString2( &(sfd->vals[i]), tmp );
                    redraw = TRUE;
                }
                break;

            /*
             * no value window, so just return line selected
             */
            } else {
                if( isMenu && InvokeMenuHook( CurrentMenuNumber, cln ) == -1 ) {
                    break;
                }
                sfd->sl = cln;
                done = TRUE;
            }
            break;

        case VI_KEY( LEFT ):
        case 'h':
            if( sfd->allow_rl != NULL ) {
                *(sfd->allow_rl) = -1;
                done = TRUE;
            }
            break;

        case VI_KEY( RIGHT ):
        case 'l':
            if( sfd->allow_rl != NULL ) {
                *(sfd->allow_rl) = 1;
                done = TRUE;
            }
            break;

        evil_up:
        case VI_KEY( UP ):
        case 'k':
            drawbord = adjustCLN( &cln, &pagetop, -1, endline, text_lines );
            break;

        evil_down:
        case VI_KEY( DOWN ):
        case 'j':
            drawbord = adjustCLN( &cln, &pagetop, 1, endline, text_lines );
            break;

        case VI_KEY( CTRL_PAGEUP ):
            drawbord = adjustCLN( &cln, &pagetop, -cln + 1, endline, text_lines );
            break;

        case VI_KEY( CTRL_PAGEDOWN ):
            drawbord = adjustCLN( &cln, &pagetop, endline - cln, endline, text_lines );
            break;

        evil_pageup:
        case VI_KEY( PAGEUP ):
        case VI_KEY( CTRL_B ):
            drawbord = adjustCLN( &cln, &pagetop, -text_lines, endline, text_lines );
            break;

        evil_pagedown:
        case VI_KEY( PAGEDOWN ):
        case VI_KEY( CTRL_F ):
            drawbord = adjustCLN( &cln, &pagetop, text_lines, endline, text_lines );
            break;

        case VI_KEY( HOME ):
            drawbord = TRUE;
            cln = 1;
            pagetop = 1;
            break;

        case VI_KEY( END ):
            drawbord = TRUE;
            cln = endline;
            pagetop = endline - text_lines + 1;
            if( pagetop < 1 ) {
                pagetop = 1;
            }
            break;

        default:
            redraw = FALSE;
            break;

        }

    }
    PopMouseEventHandler();
    CloseAWindow( cWin );
    RestoreCursor();
    SetWindowCursor();
    return( rc );

} /* SelectLineInFile */
