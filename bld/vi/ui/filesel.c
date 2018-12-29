/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


typedef enum {
    MS_NONE,
    MS_PAGEDOWN,
    MS_PAGEUP,
    MS_DOWN,
    MS_UP,
    MS_EXPOSEDOWN,
    MS_EXPOSEUP
} ms_type;

extern int      CurrentMenuNumber;

static window_id        fs_select_window_id;
static window_id        fs_event_window_id;
static window_id        fs_mouse_window_id;
static list_linenum     mouseLine = -1;
static ms_type          mouseScroll;
static bool             rlMenu;
static int              rlMenuNum;
static bool             isMenu;

/*
 * SelectFileOpen - select file from specified directory
 */
vi_rc SelectFileOpen( const char *dir, char **result_ptr, const char *mask, bool want_all_dirs )
{
    char                dd[FILENAME_MAX];
    char                cdir[FILENAME_MAX];
    list_linenum        j;
    file                *cfile;
    fcb                 *cfcb;
    line                *cline;
    selflinedata        sfd;
    bool                need_entire_path;
    char                *result = *result_ptr;
    vi_rc               rc;
    char                *p;

    /*
     * get current directory
     */
    strcpy( dd, dir );
    strcpy( cdir, dir );
    SetCWD( dir );
    need_entire_path = false;

    /*
     * work through all files
     */
    for( ;; ) {
        p = dd + strlen( dd );
        if( *(p - 1) != FILE_SEP ) {
            *p++ = FILE_SEP;
        }
        strcpy( p, mask );
        rc = GetSortDir( dd, want_all_dirs );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }

        /*
         * allocate temporary file structure
         */
        cfile = FileAlloc( NULL );

        FormatDirToFile( cfile, true );

        /*
         * go get selected line
         */
        memset( &sfd, 0, sizeof( sfd ) );
        sfd.f = cfile;
        sfd.wi = &dirw_info;
        sfd.title = CurrentDirectory;
        sfd.show_lineno = true;
        sfd.cln = 1;
        sfd.event_wid = NO_WINDOW;
        rc = SelectLineInFile( &sfd );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        if( sfd.sl == -1 ) {
            result[0] = '\0';
            break;
        }
        j = sfd.sl - 1;
        if( j < DirFileCount && !IS_SUBDIR( DirFiles[j] ) ) {
            /* file entry */
            if( need_entire_path ) {
                strcpy( result, CurrentDirectory );
                p = result + strlen( result );
                if( *(p - 1) != FILE_SEP ) {
                    *p++ = FILE_SEP;
                }
                strcpy( p, DirFiles[j]->name );
            } else {
                strcpy( result, DirFiles[j]->name );
            }
            break;
        }
        if( j < DirFileCount ) {
            /* sub-directory entry */
            strcpy( dd, cdir );
            p = dd + strlen( dd );
            if( *(p - 1) != FILE_SEP ) {
                *p++ = FILE_SEP;
            }
            strcpy( p, DirFiles[j]->name );
        } else {
            /* drive entry */
            GimmeLinePtr( j + 1, cfile, &cfcb, &cline );
            dd[0] = cline->data[3];
            dd[1] = ':';
            dd[2] = '\0';
        }
        rc = SetCWD( dd );
        if( rc != ERR_NO_ERR ) {
            break;
        }
        FreeEntireFile( cfile );
        need_entire_path = true;
        strcpy( cdir, CurrentDirectory );
        strcpy( dd, CurrentDirectory );
    }

    /*
     * done, free memory
     */
    FreeEntireFile( cfile );
    DCDisplayAllLines();
    return( rc );

} /* SelectFileOpen */

/*
 * displayGenericLines - display all lines in a window
 */
static vi_rc displayGenericLines( file *f, list_linenum pagetop, int leftcol,
                                list_linenum hilite, type_style *style, hilst *hilist,
                                char **vals, int valoff )
{
    int             j;
    int             k;
    list_linenum    text_lines;
    list_linenum    cl;
    fcb             *cfcb;
    fcb             *tfcb;
    line            *cline;
    hilst           *ptr;
    type_style      *text_style;
    type_style      *hot_key_style;
    window_info     *wi;
    type_style      base_style;
    char            tmp[MAX_STR];
//    bool            disabled;
    vi_rc           rc;

    /*
     * get pointer to first line on page, and window info
     */
    rc = GimmeLinePtr( pagetop, f, &cfcb, &cline );
    if( rc == ERR_NO_ERR ) {
        base_style.foreground = WindowAuxInfo( fs_select_window_id, WIND_INFO_TEXT_COLOR );
        base_style.background = WindowAuxInfo( fs_select_window_id, WIND_INFO_BACKGROUND_COLOR );
        base_style.font = WindowAuxInfo( fs_select_window_id, WIND_INFO_TEXT_FONT );
        text_lines = WindowAuxInfo( fs_select_window_id, WIND_INFO_TEXT_LINES );

        /*
         * mark all fcb's as being not in display
         */
        for( tfcb = f->fcbs.head; tfcb != NULL; tfcb = tfcb->next ) {
            tfcb->on_display = false;
        }
        cfcb->on_display = true;

        /*
         * run through each line in the window
         */
        ptr = hilist;
        if( ptr != NULL ) {
            ptr += pagetop - 1;
        }
        cl = pagetop;
        for( j = 1; j <= text_lines; j++ ) {
            if( cline == NULL ) {
                DisplayLineInWindow( fs_select_window_id, j, "~" );
            } else {
                if( isMenu ) {
                    if( InvokeMenuHook( CurrentMenuNumber, cl ) == -1 ) {
//                        disabled = true;
                        if( cl == hilite ) {
                            wi = &activegreyedmenu_info;
                        } else {
                            wi = &greyedmenu_info;
                        }
                    } else {
//                        disabled = false;
                        if( cl == hilite ) {
                            wi = &activemenu_info;
                        } else {
                            wi = &menuw_info;
                        }
                    }
                    text_style = &wi->text_style;
                    hot_key_style = &wi->hilight_style;
                } else {
                    text_style = &base_style;
                    if( cl == hilite ) {
                        text_style = style;
                    }
                    hot_key_style = text_style;
                }

                /*
                 * now, display what we can of the line on the window
                 */
                if( cline->len == 0 ) {
                    DisplayCrossLineInWindow( fs_select_window_id, j );
                } else {
                    if( cline->len > leftcol ) {
                        if( vals != NULL ) {
                            strncpy( tmp, &(cline->data[leftcol]), EditVars.WindMaxWidth + 5 );
                            for( k = cline->len - leftcol; k < valoff; k++ ) {
                                tmp[k] = ' ';
                            }
                            tmp[k] = '\0';
                            strcat( tmp, vals[j + pagetop - 2] );
                            DisplayLineInWindowWithColor( fs_select_window_id, j, tmp, text_style, 0 );
                        } else {
                            DisplayLineInWindowWithColor( fs_select_window_id, j, cline->data, text_style, leftcol );
                        }
                    } else {
                        DisplayLineInWindowWithColor( fs_select_window_id, j, SingleBlank, text_style, 0 );
                    }
                    if( ptr != NULL ) {
                        SetCharInWindowWithColor( fs_select_window_id, j, 1 + ptr->_offs, ptr->_char, hot_key_style );
                    }
                }
                if( ptr != NULL ) {
                    ptr += 1;
                }
                rc = GimmeNextLinePtr( f, &cfcb, &cline );
                if( rc == ERR_NO_ERR ) {
                    cl++;
                    cfcb->on_display = true;
                } else if( rc != ERR_NO_MORE_LINES ) {
                    break;
                }
            }
        }
    }
    return( rc );

} /* displayGenericLines */

/*
 * SelectLineMouseHandler - handle mouse events for line selector
 */
static bool SelectLineMouseHandler( window_id wid, int win_x, int win_y )
{
    int     x;
    int     y;
    int     i;

    if( LastMouseEvent != VI_MOUSE_DRAG && LastMouseEvent != VI_MOUSE_PRESS &&
        LastMouseEvent != VI_MOUSE_DCLICK && LastMouseEvent != VI_MOUSE_RELEASE &&
        LastMouseEvent != VI_MOUSE_REPEAT && LastMouseEvent != VI_MOUSE_PRESS_R ) {
        return( false );
    }
    fs_mouse_window_id = wid;
    mouseScroll = MS_NONE;

    if( !isMenu && ( wid == fs_select_window_id ) && (LastMouseEvent == VI_MOUSE_REPEAT ||
                                    LastMouseEvent == VI_MOUSE_PRESS ||
                                    LastMouseEvent == VI_MOUSE_DCLICK ) ) {
        x = WindowAuxInfo( fs_select_window_id, WIND_INFO_WIDTH );
        y = WindowAuxInfo( fs_select_window_id, WIND_INFO_HEIGHT );
        if( win_x == x - 1 ) {
            if( win_y == 1 ) {
                mouseScroll = MS_EXPOSEUP;
                return( true );
            } else if( win_y == y - 2 ) {
                mouseScroll = MS_EXPOSEDOWN;
                return( true );
            } else if( win_y > 1 && win_y < y / 2 ) {
                mouseScroll = MS_PAGEUP;
                return( true );
            } else if( win_y >= y / 2 && win_y < y - 1 ) {
                mouseScroll = MS_PAGEDOWN;
                return( true );
            }
        }
    }
    if( LastMouseEvent == VI_MOUSE_REPEAT ) {
        if( wid != fs_select_window_id && !isMenu ) {
            y = WindowAuxInfo( fs_select_window_id, WIND_INFO_Y1 );
            if( MouseRow < y ) {
                mouseScroll = MS_UP;
                return( true );
            }
            y = WindowAuxInfo( fs_select_window_id, WIND_INFO_Y2 );
            if( MouseRow > y ) {
                mouseScroll = MS_DOWN;
                return( true );
            }
        }
        return( false );
    }
    if( isMenu && EditFlags.Menus && wid == menu_window_id &&
        LastMouseEvent != VI_MOUSE_PRESS_R ) {
        i = GetMenuIdFromCoord( win_x );
        if( i >= 0 ) {
            rlMenuNum = i - GetCurrentMenuId();
            if( rlMenuNum != 0 ) {
                rlMenu = true;
            }
        }
        return( true );
    }
    if( wid != fs_select_window_id && wid != fs_event_window_id ) {
        return( true );
    }

    if( !InsideWindow( wid, win_x, win_y ) ) {
        return( false );
    }
    mouseLine = win_y - 1;
    return( true );

} /* SelectLineMouseHandler */

/*
 * adjustCLN - adjust current line number and pagetop
 */
static bool adjustCLN( list_linenum *cln, list_linenum *pagetop, list_linenum amt,
                       list_linenum endline, list_linenum text_lines )
{
    bool        drawbord = false;

    if( !isMenu ) {
        if( amt < 0 ) {
            if( *cln + amt > 1 ) {
                *cln += amt;
                if( *cln < *pagetop ) {
                    *pagetop += amt;
                    drawbord = true;
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
                    drawbord = true;
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
                drawbord = true;
            } else if( *cln < *pagetop ) {
                *pagetop += amt;
                drawbord = true;
            }
        } else {
            if( *cln <= endline ) {
                if( *cln >= *pagetop + text_lines ) {
                    *pagetop += amt;
                    drawbord = true;
                }
            } else {
                while( *cln > endline ) {
                    *cln -= endline;
                }
                *pagetop = *cln - text_lines + 1;
                drawbord = true;
            }
        }
    }
    if( *pagetop < 1 ) {
        *pagetop = 1;
    }
    if( *pagetop > endline - text_lines + 1 ) {
        *pagetop = endline - text_lines + 1;
        drawbord = true;
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
    int             i;
    int             winflag;
    int             leftcol = 0;
    int             key2;
    bool            done;
    bool            redraw;
    bool            hiflag;
    bool            drawbord;
    int             farx;
    list_linenum    text_lines;
    list_linenum    pagetop;
    list_linenum    endline;
    list_linenum    lln;
    list_linenum    cln;
    list_linenum    ln;
    char            tmp[MAX_STR];
    hilst           *ptr;
    vi_rc           rc;
    vi_key          key;

    /*
     * create the window
     */
    cln = sfd->cln;
    endline = (list_linenum)sfd->f->fcbs.tail->end_line;
    farx = sfd->wi->area.x2;
    if( sfd->show_lineno ) {
        farx++;
    }
    hiflag = ( sfd->hilite != NULL );
    rc = NewWindow2( &fs_select_window_id, sfd->wi );
    if( rc == ERR_NO_ERR ) {
        if( !sfd->is_menu ) {
            WindowAuxUpdate( fs_select_window_id, WIND_INFO_HAS_SCROLL_GADGETS, true );
            DrawBorder( fs_select_window_id );
        }
        fs_event_window_id = sfd->event_wid;
        isMenu = sfd->is_menu;
        PushMouseEventHandler( SelectLineMouseHandler );
        KillCursor();
        text_lines = WindowAuxInfo( fs_select_window_id, WIND_INFO_TEXT_LINES );
        sfd->sl = -1;
        if( sfd->title != NULL ) {
            WindowTitle( fs_select_window_id, sfd->title );
        }
        pagetop = text_lines * ( cln / text_lines );
        if( ( cln % text_lines ) != 0 ) {
            pagetop++;
        }
        key = 0;
        if( LastEvent == VI_KEY( MOUSEEVENT ) ) {
            DisplayMouse( true );
        }

        /*
         * now, allow free scrolling and selection
         */
        lln = 1;
        redraw = true;
        drawbord = false;
        done = false;
        while( !done ) {
            if( redraw ) {
                if( sfd->show_lineno ) {
                    MySprintf(tmp, "%l/%l", (long)cln, (long)endline );
                    WindowBorderData( fs_select_window_id, tmp, sfd->wi->area.x2 - sfd->wi->area.x1 - (int)strlen( tmp ) );
                    drawbord = true;
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
                    DrawBorder( fs_select_window_id );
                }
                displayGenericLines( sfd->f, pagetop, leftcol, cln, &(sfd->wi->hilight_style), sfd->hilite, sfd->vals, sfd->valoff );
            }
            lln = cln;
            redraw = true;
            drawbord = false;
            mouseLine = -1;
            rlMenu = false;
            if( key == VI_KEY( MOUSEEVENT ) ) {
                DisplayMouse( true );
            }
            key = GetNextEvent( true );
            if( hiflag && ((key >= VI_KEY( ALT_A ) && key <= VI_KEY( ALT_Z )) ||
                           (key >= VI_KEY( a ) && key <= VI_KEY( z )) || (key >= VI_KEY( A ) && key <= VI_KEY( Z )) ||
                           (key >= VI_KEY( 1 ) && key <= VI_KEY( 9 ))) ) {
                if( key >= VI_KEY( ALT_A ) && key <= VI_KEY( ALT_Z ) ) {
                    key2 = key - VI_KEY( ALT_A ) + 'A';
                } else if( key >= VI_KEY( a ) && key <= VI_KEY( z ) ) {
                    key2 = key - VI_KEY( a ) + 'A';
                } else {
                    key2 = key;
                }
                ln = 1;
                for( ptr = sfd->hilite; ptr->_char != '\0'; ptr++ ) {
                    if( toupper( ptr->_char ) == key2 ) {
                        cln = ln;
                        key = VI_KEY( ENTER );
                        break;
                    }
                    ln++;
                }
            }

            /*
             * check if a return-event has been selected
             */
            if( sfd->retevents != NULL ) {
                if( key == VI_KEY( MOUSEEVENT ) ) {
                    if( fs_mouse_window_id == fs_event_window_id && LastMouseEvent == VI_MOUSE_PRESS ) {
                        DisplayMouse( false );
                        sfd->event = sfd->retevents[mouseLine];
                        key = VI_KEY( ENTER );
                    }
                } else {
                    for( i = 0; sfd->retevents[i] != VI_KEY( DUMMY ); i++ ) {
                        if( key == sfd->retevents[i] ) {
                            sfd->event = key;
                            key = VI_KEY( ENTER );
                            break;
                        }
                    }
                }
            }

            /*
             * pre-process mouse events, remap to key stroke if possible
             */
            switch( key ) {
            case VI_KEY( MOUSEEVENT ):
                DisplayMouse( false );
                if( hiflag ) {
                    ptr = sfd->hilite;
                    ptr += mouseLine;
                    if( ptr->_char == (char)-1 ) {
                        break;
                    }
                }
                if( rlMenu && sfd->allowrl != NULL ) {
                    *(sfd->allowrl) = rlMenuNum;
                    done = true;
                    break;
                }
                if( mouseScroll != MS_NONE ) {
                    switch( mouseScroll ) {
                    case MS_UP:
                        key = VI_KEY( UP );
                        break;
                    case MS_DOWN:
                        key = VI_KEY( DOWN );
                        break;
                    case MS_PAGEUP:
                        key = VI_KEY( PAGEUP );
                        break;
                    case MS_PAGEDOWN:
                        key = VI_KEY( PAGEDOWN );
                        break;
                    case MS_EXPOSEDOWN:
                        adjustCLN( &cln, &pagetop, pagetop + text_lines - cln - 1, endline, text_lines );
                        adjustCLN( &cln, &pagetop, 1, endline, text_lines );
                        drawbord = true;
                        break;
                    case MS_EXPOSEUP:
                        adjustCLN( &cln, &pagetop, pagetop - cln, endline, text_lines );
                        adjustCLN( &cln, &pagetop, -1, endline, text_lines );
                        drawbord = true;
                        break;

                    }
                    break;
                }
                switch( LastMouseEvent ) {
                case VI_MOUSE_DRAG:
                    if( fs_mouse_window_id != fs_select_window_id ) {
                        break;
                    }
                    cln = mouseLine + pagetop;
                    break;
                case VI_MOUSE_RELEASE:
                    if( !sfd->is_menu ) {
                        break;
                    }
                    if( fs_mouse_window_id == fs_select_window_id ) {
                        cln = mouseLine + pagetop;
                        if( cln <= endline ) {
                            key = VI_KEY( ENTER );
                        }
                    }
                    break;
                case VI_MOUSE_DCLICK:
                    if( fs_mouse_window_id != fs_select_window_id ) {
                        AddCurrentMouseEvent();
                        done = true;
                    } else {
                        cln = mouseLine + pagetop;
                        if( cln <= endline ) {
                            key = VI_KEY( ENTER );
                        }
                    }
                    break;
                case VI_MOUSE_PRESS_R:
                    if( fs_mouse_window_id != fs_select_window_id ) {
                        AddCurrentMouseEvent();
                        done = true;
                    }
                    break;
                case VI_MOUSE_PRESS:
                    if( fs_mouse_window_id != fs_select_window_id ) {
                        AddCurrentMouseEvent();
                        done = true;
                    } else {
                        cln = mouseLine + pagetop;
                    }
                    break;
                }
                break;
            }

            /*
             * process key stroke
             */
            switch( key ) {
            case VI_KEY( MOUSEEVENT ):
                /* nothing to do */
                /* already pre-processed */
                break;
            case VI_KEY( ESC ):
                done = true;
                break;
            case VI_KEY( ENTER ):
            case VI_KEY( SPACE ):
                /*
                 * see if we need to do a callback for this
                 */
                if( sfd->checkres != NULL ) {
                    line    *cline;
                    fcb     *cfcb;
                    char    *ptr;

                    GimmeLinePtr( cln, sfd->f, &cfcb, &cline );
                    ptr = SkipLeadingSpaces( cline->data );
                    strcpy( tmp, sfd->vals[cln - 1] );
                    rc = sfd->checkres( ptr, tmp, &winflag );
                    if( winflag == 2 ) {
                        MoveWindowToFront( fs_select_window_id );
                    }
                    if( rc == ERR_NO_ERR ) {
                        ReplaceString( &(sfd->vals[cln - 1]), tmp );
                        redraw = true;
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
                    done = true;
                }
                break;
            case VI_KEY( LEFT ):
            case VI_KEY( h ):
                if( sfd->allowrl != NULL ) {
                    *(sfd->allowrl) = -1;
                    done = true;
                }
                break;
            case VI_KEY( RIGHT ):
            case VI_KEY( l ):
                if( sfd->allowrl != NULL ) {
                    *(sfd->allowrl) = 1;
                    done = true;
                }
                break;
            case VI_KEY( UP ):
            case VI_KEY( k ):
                drawbord = adjustCLN( &cln, &pagetop, -1, endline, text_lines );
                break;
            case VI_KEY( DOWN ):
            case VI_KEY( j ):
                drawbord = adjustCLN( &cln, &pagetop, 1, endline, text_lines );
                break;
            case VI_KEY( CTRL_PAGEUP ):
                drawbord = adjustCLN( &cln, &pagetop, -cln + 1, endline, text_lines );
                break;
            case VI_KEY( CTRL_PAGEDOWN ):
                drawbord = adjustCLN( &cln, &pagetop, endline - cln, endline, text_lines );
                break;
            case VI_KEY( PAGEUP ):
            case VI_KEY( CTRL_B ):
                drawbord = adjustCLN( &cln, &pagetop, -text_lines, endline, text_lines );
                break;
            case VI_KEY( PAGEDOWN ):
            case VI_KEY( CTRL_F ):
                drawbord = adjustCLN( &cln, &pagetop, text_lines, endline, text_lines );
                break;
            case VI_KEY( HOME ):
                drawbord = true;
                cln = 1;
                pagetop = 1;
                break;
            case VI_KEY( END ):
                drawbord = true;
                cln = endline;
                pagetop = endline - text_lines + 1;
                if( pagetop < 1 ) {
                    pagetop = 1;
                }
                break;
            default:
                redraw = false;
                break;
            }
        }
        PopMouseEventHandler();
        CloseAWindow( fs_select_window_id );
        RestoreCursor();
        SetWindowCursor();
    }
    return( rc );

} /* SelectLineInFile */
