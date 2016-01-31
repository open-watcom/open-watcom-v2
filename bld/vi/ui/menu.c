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
* Description:  Menu processing routines for vi.
*
****************************************************************************/


#include "vi.h"
#include <stddef.h>
#include <time.h>
#include "menu.h"
#include "win.h"

#include "clibext.h"


typedef struct menu_item {
    struct menu_item    *next, *prev;
    hilst               hi;
    unsigned char       slen;
    char                *cmd;
    char                str[1];
} menu_item;

typedef struct menu {
    struct menu     *next, *prev;
    menu_item       *itemhead, *itemtail;
    int             itemcnt;
    int             orig_itemcnt;
    char            **list;
    hilst           *hilist;
    bool            has_file_list   : 1;
    bool            need_hook       : 1;
    bool            has_last_files  : 1;
    unsigned char   maxwidth;
    unsigned char   orig_maxwidth;
    hilst           hi;
    unsigned char   slen;
    char            str[1];
} menu;

int CurrentMenuNumber;

static int      menuCnt;

static menu     *menuHead, *menuTail, *currMenu;
static menu     *windowGadgetMenu;
static menu     *floatMenus[MAX_FLOAT_MENUS] = { NULL, NULL, NULL, NULL };

/*
 * getMenuName - get name of menu, including '&' character
 */
static void getMenuName( char *res, const char *str, int slen, int hioff )
{
    int i;

    for( i = 0; i <= slen; i++ ) {
        if( i == hioff ) {
            *res++ = '&';
        }
        *res++ = str[i];
    }

} /* getMenuName */

/*
 * dumpMenu - dump contents of a menu
 */
static void dumpMenu( FILE *f, menu *cmenu )
{
    menu_item   *citem;
    char        str[MAX_STR];

    citem = cmenu->itemhead;
    while( citem != NULL ) {
        if( citem->slen == 0 ) {
            MyFprintf( f, "    menuitem \"\"\n" );
        } else {
            getMenuName( str, citem->str, citem->slen, citem->hi._offs );
            MyFprintf( f, "    menuitem \"%s\" %s\n", str, citem->cmd );
        }
        citem = citem->next;
    }
    if( cmenu->has_file_list ) {
        MyFprintf( f, "    menuwindowlist\n" );
    }

} /* dumpMenu */

/*
 * BarfMenuData - write out menu data to a specified file handle
 */
void BarfMenuData( FILE *f )
{
    menu        *cmenu;
    char        str[MAX_STR];

    if( windowGadgetMenu != NULL ) {
        MyFprintf( f, "menu windowgadget\n" );
        dumpMenu( f, windowGadgetMenu );
        MyFprintf( f, "endmenu\n" );
    }
    for( cmenu = menuHead; cmenu != NULL; cmenu = cmenu->next ) {
        getMenuName( str, cmenu->str, cmenu->slen, cmenu->hi._offs );
        if( cmenu->need_hook ) {
            MyFprintf( f, "menu %s 1\n", str );
        } else {
            MyFprintf( f, "menu %s\n", str );
        }
        dumpMenu( f, cmenu );
        MyFprintf( f, "endmenu\n" );
    }

} /* BarfMenuData */

/*
 * extractMenuStr - get the string and activation char for a menu item
 */
static char extractMenuStr( char *str, int *hioff )
{
    int         len, i, j;
    char        ch;

    len = strlen( str );
    *hioff = 0;
    for( i = 0; i < len; i++ ) {
        if( str[i] == '&' ) {
            ch = str[i + 1];
            *hioff = i;
            for( j = i + 1; j <= len; j++ ) {
                str[j - 1] = str[j];
            }
            return( ch );
        }
    }
    return( -1 );

} /* extractMenuStr */

/*
 * freeMenuData - release data associated with a menu
 */
static void freeMenuData( menu *cmenu )
{
    menu_item   *curr, *next;
    menu        *tmp;

    if( cmenu == NULL ) {
        return;
    }
    MemFree( cmenu->list );
    MemFree( cmenu->hilist );
    curr = cmenu->itemhead;
    while( curr != NULL ) {
        next = curr->next;
        MemFree( curr );
        curr = next;
    }
    tmp = cmenu->next;
    memset( cmenu, 0, sizeof( menu ) );
    cmenu->next = tmp;

} /* freeMenuData */

/*
 * freeMenu - free up a given menu
 */
static void freeMenu( menu *menu )
{
    freeMenuData( menu );
    MemFree( menu );

} /* freeMenu */

/*
 * findMenu - locate pointer to a given menu
 */
static menu *findMenu( const char *str, menu ***predef_menu )
{
//    int         len;
    menu        *res;
    int         num;

//    len = strlen( str );
    *predef_menu = NULL;
    res = NULL;
    if( str[0] == 'f' || str[0] == 'w' ) {
        if( !strnicmp( str, "float", 5 ) ) {
            num = str[5] - '0';
            if( num >= 0 && num < MAX_FLOAT_MENUS ) {
                res = floatMenus[num];
                *predef_menu = &floatMenus[num];
            }
        } else if( !stricmp( str, "windowgadget" ) ) {
            res = windowGadgetMenu;
            *predef_menu = &windowGadgetMenu;
        }
    }

    if( res == NULL ) {
        for( res = menuHead; res != NULL; res = res->next ) {
            if( !stricmp( str, res->str ) ) {
                break;
            }
        }
    }
    return( res );

} /* findMenu */

/*
 * StartMenu - start a new top level menu
 */
vi_rc StartMenu( const char *data )
{
    char        str[MAX_STR];
    menu        *tmp;
    char        ch;
    int         hioff;
    size_t      len;
    bool        new;
    bool        need_hook;
    menu        **predef_menu;

    GetStringWithPossibleQuote( &data, str );
    data = SkipLeadingSpaces( data );
    need_hook = false;
    if( data[0] != 0 ) {
        need_hook = true;
    }

    if( currMenu != NULL ) {
        return( ERR_INVALID_MENU );
    }
    new = false;

    ch = extractMenuStr( str, &hioff );
    len = strlen( str );

    tmp = findMenu( str, &predef_menu );
    if( predef_menu != NULL ) {
        freeMenuData( tmp );
    }

    if( tmp == NULL ) {
        tmp = MemAlloc( offsetof( menu, str ) + len + 1 );
        new = true;
    }
    if( predef_menu == NULL ) {
        if( new ) {
            AddLLItemAtEnd( (ss **)&menuHead, (ss **)&menuTail, (ss *)tmp );
        } else {
            freeMenuData( tmp );
        }
    } else {
        *predef_menu = tmp;
    }
    strcpy( tmp->str, str );
    tmp->hi._char = ch;
    tmp->hi._offs = hioff;
    tmp->slen = (unsigned char)len;
    tmp->need_hook = need_hook;
    currMenu = tmp;
    return( ERR_NO_ERR );

} /* StartMenu */

/*
 * initMenuList - set up lists for current menu
 */
static void initMenuList( menu *cmenu )
{
    menu_item   *cmi;
    int         i;

    MemFree( cmenu->list );
    MemFree( cmenu->hilist );
    cmenu->list = MemAlloc( sizeof( char * ) * cmenu->itemcnt );
    cmenu->hilist = MemAlloc( sizeof( hilst ) * (cmenu->itemcnt + 1) );

    cmi = cmenu->itemhead;
    for( i = 0; i < cmenu->itemcnt; i++ ) {
        cmenu->list[i] = cmi->str;
        cmenu->hilist[i]._char = cmi->hi._char;
        cmenu->hilist[i]._offs = cmi->hi._offs;
        cmi = cmi->next;
    }
    cmenu->hilist[i]._char = 0;
    cmenu->hilist[i]._offs = 0;

} /* initMenuList */

/*
 * ViEndMenu - terminate new menu
 */
vi_rc ViEndMenu( void )
{
    char        ch;
    vi_key      key;

    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    if( currMenu == menuTail ) {
        menuCnt++;
    }
    ch = toupper( currMenu->hi._char );
    if( ch >= 'A' && ch <='Z' ) {
        key = ch - 'A' + VI_KEY( ALT_A );
        EventList[key].rtn.old = DoMenu;
        EventList[key].b.keep_selection = true;
        EventList[key].alt_rtn.old = DoMenu;
        EventList[key].alt_b.keep_selection = true;
    }

    initMenuList( currMenu );
    currMenu = NULL;
    InitMenu();

    return( ERR_NO_ERR );

} /* ViEndMenu */

/*
 * MenuItem - add new item current menu
 */
vi_rc MenuItem( const char *data )
{
    char        str[MAX_STR];
    size_t      len;
    size_t      size;
    char        ch;
    menu_item   *tmp;
    int         hioff;

    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    GetStringWithPossibleQuote( &data, str );
    data = SkipLeadingSpaces( data );
    ch = extractMenuStr( str, &hioff );
    len = strlen( str );
    size = sizeof( menu_item ) + len + strlen( data ) + 2;
    tmp = MemAlloc( size );
    tmp->slen = (unsigned char)len;
    tmp->hi._char = ch;
    tmp->hi._offs = hioff;
    strcpy( tmp->str, str );
    tmp->cmd = &(tmp->str[len + 1]);
    strcpy( tmp->cmd, data );
    if( currMenu->maxwidth < len ) {
        currMenu->maxwidth = len;
    }

    AddLLItemAtEnd( (ss **)&currMenu->itemhead, (ss **)&currMenu->itemtail, (ss *)tmp );

    currMenu->itemcnt++;
    return( ERR_NO_ERR );

} /* MenuItem */

/*
 * DoItemDelete - delete an item from a menu
 */
vi_rc DoItemDelete( const char *data )
{
    menu        *cmenu, **predef_menu;
    char        mname[MAX_STR];
    char        str[MAX_STR];
    menu_item   *cmi, *dmi;
    int         i, id;
    windim      maxwidth;

    if( currMenu != NULL ) {
        return( ERR_INVALID_MENU );
    }
    data = GetNextWord1( data, mname );
    cmenu = findMenu( mname, &predef_menu );
    if( cmenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    data = GetNextWord1( data, str );
    id = atoi( str );
    if( id < 0 ) {
        id = cmenu->itemcnt - 1;
    }
    if( id >= cmenu->itemcnt ) {
        return( ERR_INVALID_MENU );
    }
    i = 0;
    maxwidth = 0;
    dmi = NULL;
    for( cmi = cmenu->itemhead; cmi != NULL; cmi = cmi->next ) {
        if( i == id ) {
            dmi = cmi;
        } else {
            if( maxwidth < cmi->slen ) {
                maxwidth = cmi->slen;
            }
        }
        i++;
    }
    if( dmi == NULL ) {
        return( ERR_INVALID_MENU );
    }

    cmenu->itemcnt--;
    cmenu->maxwidth = maxwidth;
    DeleteLLItem( (ss **)&cmenu->itemhead, (ss **)&cmenu->itemtail, (ss *)dmi );
    MemFree( dmi );
    initMenuList( cmenu );
    return( ERR_NO_ERR );

} /* DoItemDelete */

/*
 * AddMenuItem - add a menu item to an already created menu
 */
vi_rc AddMenuItem( const char *data )
{
    menu        *cmenu, **predef_menu;
    char        mname[MAX_STR];
    vi_rc       rc;

    if( currMenu != NULL ) {
        return( ERR_INVALID_MENU );
    }
    data = GetNextWord1( data, mname );
    cmenu = findMenu( mname, &predef_menu );
    if( cmenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    currMenu = cmenu;
    rc = MenuItem( data );
    initMenuList( currMenu );
    currMenu = NULL;
    return( rc );

} /* AddMenuItem */

/*
 * DoMenuDelete - delete an existing menu
 */
vi_rc DoMenuDelete( const char *data )
{
    menu        *cmenu, **predef_menu;
    char        mname[MAX_STR];

    if( currMenu != NULL ) {
        return( ERR_INVALID_MENU );
    }
    data = GetNextWord1( data, mname );
    cmenu = findMenu( mname, &predef_menu );
    if( cmenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    if( predef_menu != NULL ) {
        freeMenu( cmenu );
        *predef_menu = NULL;
        return( ERR_NO_ERR );
    }
    DeleteLLItem( (ss **)&menuHead, (ss **)&menuTail, (ss *)cmenu );
    freeMenu( cmenu );
    menuCnt--;
    InitMenu();
    return( ERR_NO_ERR );

} /* DeleteMenu */

/*
 * addFileList - add a list of files to the current menu
 */
static void addFileList( menu *cmenu )
{
    char        buff[MAX_STR];
    info        *cinfo;
    int         cnt;

    currMenu = cmenu;

    cmenu->orig_itemcnt = cmenu->itemcnt;
    cmenu->orig_maxwidth = cmenu->maxwidth;
    buff[0] = 0;
    MenuItem( buff );

    for( cnt = 1, cinfo = InfoHead; cinfo != NULL && cnt < 10; cinfo = cinfo->next, ++cnt ) {
        MySprintf( buff, "\"&%d %s\" edit %s", cnt, cinfo->CurrentFile->name, cinfo->CurrentFile->name );
        MenuItem( buff );
    }
    if( cinfo != NULL ) {
        strcpy( buff, "\"&More Windows ...\" files" );
        MenuItem( buff );
    }
    initMenuList( cmenu );
    currMenu = NULL;

} /* addFileList */

/*
 * removeFileList - remove files from a menu
 */
static void removeFileList( menu *cmenu )
{
    menu_item   *citem, *nitem;
    int         i;

    i = 0;
    citem = cmenu->itemhead;
    while( i < cmenu->orig_itemcnt ) {
        citem = citem->next;
        i++;
    }
    while( citem != NULL ) {
        nitem = citem->next;
        DeleteLLItem( (ss **)&cmenu->itemhead, (ss **)&cmenu->itemtail, (ss *)citem );
        MemFree( citem );
        citem = nitem;
        cmenu->itemcnt--;
    }
    cmenu->maxwidth = cmenu->orig_maxwidth;
    initMenuList( cmenu );

} /* removeFileList */

#define START_OFFSET  1

/*
 * InitMenu - initialize control bar window
 */
vi_rc InitMenu( void )
{
    int         ws;
    char        disp[MAX_STR];
    char        tmp[MAX_STR];
    menu        *cmenu;
    vi_rc       rc;

    if( !EditFlags.WindowsStarted ) {
        return( ERR_NO_ERR );
    }
    if( !BAD_ID( menu_window_id ) ) {
        CloseAWindow( menu_window_id );
        menu_window_id = NO_WINDOW;
    }
    if( !EditFlags.Menus ) {
        return( ERR_NO_ERR );
    }
    menubarw_info.area.y1 = 0;
    menubarw_info.area.y2 = 0;
    menubarw_info.area.x1 = 0;
    menubarw_info.area.x2 = EditVars.WindMaxWidth - 1;
    rc = NewWindow2( &menu_window_id, &menubarw_info );
    if( rc != ERR_NO_ERR ) {
        EditFlags.Menus = false;
        return( rc );
    }

    memset( disp, ' ', sizeof( disp ) - 1 );
    disp[START_OFFSET] = 0;
    for( cmenu = menuHead; cmenu != NULL; cmenu = cmenu->next ) {
        MySprintf( tmp, "%s  ", cmenu->str );
        strcat( disp, tmp );
    }
    disp[strlen( disp )] = ' ';
    if( EditFlags.CurrentStatus ) {
        disp[EditVars.CurrentStatusColumn - 1] = 0;
        // disp[CurrentStatusColumn - 7] = 0;
        // strcat( disp, "Mode:" );
    }
    DisplayLineInWindow( menu_window_id, 1, disp );

    ws = 0;
    for( cmenu = menuHead; cmenu != NULL; cmenu = cmenu->next ) {
        SetCharInWindowWithColor( menu_window_id, 1, ws + START_OFFSET + 1 + cmenu->hi._offs, cmenu->hi._char, &menubarw_info.hilight_style );
        ws += cmenu->slen + 2;
    }

    return( ERR_NO_ERR );

} /* InitMenu */

void FiniMenu( void )
{
    menu        *oldmenu;
    int         i;

    while( menuHead != NULL ) {
        oldmenu = menuHead;
        menuHead = menuHead->next;
        freeMenu( oldmenu );
    }
    for( i = 0; i < MAX_FLOAT_MENUS; i++ ) {
        if( floatMenus[i] != NULL ) {
            freeMenu( floatMenus[i] );
        }
    }
    freeMenu( windowGadgetMenu );
}

/*
 * lightMenu - light up control name
 */
static void lightMenu( int sel, int ws, bool on )
{
    char        ch;
    int         i;
    menu        *cmenu;
    type_style  style;

    if( sel >= menuCnt ) {
        return;
    }

    ws++;

    cmenu = menuHead;
    for( i = 0; i < sel; i++ ) {
        cmenu = cmenu->next;
    }

    for( i = 0; i < cmenu->slen; i++ ) {
        if( i == cmenu->hi._offs && !on ) {
            ch = cmenu->hi._char;
            style = menubarw_info.hilight_style;
        } else {
            ch = cmenu->str[i];
            style = menubarw_info.text_style;
            if( on ) {
                style.foreground = menubarw_info.hilight_style.foreground;
            }
        }
        SetCharInWindowWithColor( menu_window_id, 1, ws + i, ch, &style );
    }

} /* lightMenu */

/*
 * getMenuPtrFromId - given an id, find the menu in the list
 */
static menu *getMenuPtrFromId( int id )
{
    int         i = 0;
    menu        *cmenu;
    for( cmenu = menuHead; cmenu != NULL; cmenu = cmenu->next ) {
        if( id == i ) {
            break;
        }
        i++;
    }
    return( cmenu );

} /* getMenuPtrFromId */

static int currentID;

/*
 * processMenu - process selected menu
 */
static vi_rc processMenu( int sel, menu *cmenu, int xpos, int ypos, windim rmaxwidth )
{
    int         i, ws;
    char        result[80];
    int         resint, allowrl, *arl;
    selectitem  si;
    menu        *tmenu;
    menu_item   *cmi;
    windim      x1, y1, x2, y2;
    windim      diff;
    windim      maxwidth;
    vi_rc       rc;

    maxwidth = rmaxwidth;
    if( maxwidth < 0 ) {
        maxwidth = 0;
    }

    for( ;; ) {

        if( cmenu->has_file_list ) {
            addFileList( cmenu );
        }

        /*
         * get coordinates of menu
         */
        currentID = sel;
        if( xpos < 0 ) {
            ws = START_OFFSET;
            for( tmenu = menuHead; tmenu != cmenu; tmenu = tmenu->next ) {
                ws += tmenu->slen + 2;
            }
            x1 = ws;
            arl = &allowrl;
        } else {
            ws = xpos;
            x1 = ws;
            arl = NULL;
        }
        y1 = ypos;
        if( menuw_info.has_border ) {
            x2 = x1 + cmenu->maxwidth + 1;
            y2 = y1 + (windim)cmenu->itemcnt + 1;
        } else {
            x2 = x1 + cmenu->maxwidth - 1;
            y2 = y1 + (windim)cmenu->itemcnt - 1;
        }

        /*
         * make sure menu will be valid!
         */
        if( x2 - x1 + 1 > EditVars.WindMaxWidth || y2 - y1 + 1 > EditVars.WindMaxHeight ) {
            return( ERR_WIND_INVALID );
        }
        if( xpos < 0 ) {
            if( x2 >= EditVars.WindMaxWidth ) {
                diff = x2 - EditVars.WindMaxWidth;
                x2 -= diff;
                x1 -= diff;
            }
        } else {
            if( y2 >= EditVars.WindMaxHeight ) {
                diff = y2 - y1;
                y2 = y1;
                y1 -= diff;
                if( maxwidth > 0 || rmaxwidth == -1 ) {
                    y1 -= 2;
                    y2 -= 2;
                }
            }
            if( x2 >= EditVars.WindMaxWidth ) {
                diff = x2 - x1;
                x2 = x1 - maxwidth;
                x1 -= (diff + maxwidth);
            }
        }
        menuw_info.area.x1 = x1;
        menuw_info.area.x2 = x2;
        menuw_info.area.y1 = y1;
        menuw_info.area.y2 = y2;

        /*
         * go get a selected item from the menu
         */
        memset( &si, 0, sizeof( si ) );
        allowrl = 0;
        si.is_menu = true;
        si.wi = &menuw_info;
        si.list = cmenu->list;
        si.maxlist = (int) cmenu->itemcnt;
        si.result = result;
        si.allowrl = arl;
        si.hilite = cmenu->hilist;
        si.cln = 1;
        si.eiw = NO_WINDOW;

        if( xpos < 0 ) {
            lightMenu( sel, ws, true );
        }
        CurrentMenuNumber = sel + 1;
        rc = SelectItem( &si );
        if( xpos < 0 ) {
            lightMenu( sel, ws, false );
        }
        if( rc != ERR_NO_ERR ) {
            if( cmenu->has_file_list ) {
                removeFileList( cmenu );
            }
            return( rc );
        }
        if( !allowrl ) {
            break;
        }

        sel += allowrl;
        if( sel < 0 ) {
            sel = menuCnt - 1;
        }
        if( sel >= menuCnt ) {
            sel = 0;
        }
        if( cmenu->has_file_list ) {
            removeFileList( cmenu );
        }
        cmenu = getMenuPtrFromId( sel );

    }

    resint = si.num;
    if( resint < 0 ) {
        if( cmenu->has_file_list ) {
            removeFileList( cmenu );
        }
        return( ERR_NO_ERR );
    }

    cmi = cmenu->itemhead;
    for( i = 0; i < resint; i++ ) {
        cmi = cmi->next;
    }
    rc = RunCommandLine( cmi->cmd );
    if( cmenu->has_file_list ) {
        removeFileList( cmenu );
    }
    return( rc );

} /* processMenu */

/*
 * DoMenu - process some kind of control request
 */
vi_rc DoMenu( void )
{
    int         i;
    int         sel = -1;
    char        ch;
    menu        *cmenu;

    /*
     * get which command to run
     */
    if( !EditFlags.Menus ) {
        return( ERR_NO_ERR );
    }
    ch = LastEvent - VI_KEY( ALT_A ) + 'A';
    i = 0;
    for( cmenu = menuHead; cmenu != NULL; cmenu = cmenu->next ) {
        if( ch == cmenu->hi._char ) {
            sel = i;
            break;
        }
        i++;
    }
    if( sel < 0 ) {
        return( ERR_NO_ERR );
    }
    return( processMenu( sel, cmenu, -1, 1, 0 ) );

} /* DoMenu */

/*
 * DoWindowGadgetMenu - handle menu for each file
 */
vi_rc DoWindowGadgetMenu( void )
{
    vi_rc       rc;

    if( windowGadgetMenu == NULL ) {
        return( ERR_NO_ERR );
    }
    rc = processMenu( -1, windowGadgetMenu,
                      WindowAuxInfo( current_window_id, WIND_INFO_X1 ),
                      WindowAuxInfo( current_window_id, WIND_INFO_Y1 ) + 1, -1 );
    return( rc );

} /* DoWindowGadgetMenu */

/*
 * DoFloatMenu - handle floating menus
 */
vi_rc DoFloatMenu( int id, int slen, int x1, int y1 )
{
    vi_rc       rc;

    if( id < 0 || id >= MAX_FLOAT_MENUS ) {
        return( ERR_INVALID_MENU );
    }
    if( floatMenus[id] == NULL ) {
        return( ERR_INVALID_MENU );
    }
    rc = processMenu( -1, floatMenus[id], x1, y1, slen );
    return( rc );

} /* DoFloatMenu */

/*
 * ActivateFloatMenu - activate floating menu
 */
vi_rc ActivateFloatMenu( const char *data )
{
    char        str[MAX_STR];
    int         id, slen, x1, y1;

    /*
     * get input syntax :
     * FLOATMENU id slen x1 y1
     */
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_MENU );
    }
    id = atoi( str );
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_MENU );
    }
    slen = atoi( str );
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_MENU );
    }
    x1 = atoi( str );
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_MENU );
    }
    y1 = atoi( str );
    return( DoFloatMenu( id, slen, x1, y1 ) );

} /* ActivateFloatMenu */

/*
 * GetCurrentMenuId - get id of currently displayed menu
 */
int GetCurrentMenuId( void )
{
    return( currentID );

} /* GetCurrentMenuId */

/*
 * SetToMenuId - set to specified menu id (mouse did it)
 */
vi_rc SetToMenuId( int id )
{
    menu        *cmenu;

    cmenu = getMenuPtrFromId( id );
    if( cmenu != NULL ) {
        return( processMenu( id, cmenu, -1, 1, 0 ) );
    }
    return( ERR_NO_ERR );

} /* SetToMenuId */

/*
 * GetMenuIdFromCoord - given x coordinate, determine menu item
 */
int GetMenuIdFromCoord( int x )
{
    int         i;
    int         ws;
    menu        *cmenu;

    ws = START_OFFSET;
    i = 0;
    for( cmenu = menuHead; cmenu != NULL; cmenu = cmenu->next ) {
        if( x >= ws && x < ws + cmenu->slen ) {
            return( i );
        }
        ws += cmenu->slen + 2;
        i++;
    }
    return( -1 );

} /* GetMenuIdFromCoord */

/*
 * IsMenuHotKey - test if a specified character is a main menu hot key
 */
bool IsMenuHotKey( vi_key key )
{
    menu        *curr;
    char        ch;

    ch = key - VI_KEY(ALT_A ) + 'A';
    for( curr = menuHead; curr != NULL; curr = curr->next ) {
        if( curr->hi._char == ch ) {
            return( true );
        }
    }
    return( false );

} /* IsMenuHotKey */

/*
 * MenuItemFileList - add the Window List menu item
 */
vi_rc MenuItemFileList( void )
{
    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    currMenu->has_file_list = true;
    return( ERR_NO_ERR );

} /* MenuItemFileList */

/*
 * MenuItemLastFiles - add the Last File List menu item
 */
vi_rc MenuItemLastFiles( void )
{
    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    currMenu->has_last_files = true;
    return( ERR_NO_ERR );

} /* MenuItemLastFiles */
