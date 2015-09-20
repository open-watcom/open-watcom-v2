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
* Description:  Menu support.
*
****************************************************************************/


#include "vi.h"
#include <malloc.h>
#include "source.h"
#include "menu.h"
#include <assert.h>

/* The following value comes from Petzold - page 344 */
/* we don't want to interfere with any system id's   */
#define MAX_ID          0xf000
#define FILE_LIST_ID    0xf000 - 11
#define LAST_FILES_ID   FILE_LIST_ID - 11
#define HOT_KEY_CHAR    '&'

typedef struct item {
    struct item     *next, *prev;
    UINT            id;
    unsigned char   in_menu     : 1;
    unsigned char   is_active   : 1;
    unsigned char   is_checked  : 1;
    unsigned char   unused      : 5;
    char            *name;
    char            *help;
    char            cmd[1];
} item;

typedef struct menu {
    struct menu     *next, *prev;
    void            *item_head, *item_tail;
    unsigned char   has_file_list   : 1;
    unsigned char   need_hook       : 1;
    unsigned char   has_last_files  : 1;
    unsigned char   spare           : 5;
    int             num_items;
    int             orig_num_items;
    HMENU           menu_handle;
    char            *help;
    char            name[1];
} menu;

static menu     mainMenu = { NULL, NULL, NULL, NULL, 0, 0, 0 };
static menu     *rootMenu = &mainMenu;
static menu     *currMenu = NULL;

/* utility functions used in rest of module */

/*
 * compareName - a case insensitive strcmp which ignores the embedded '&'
 *               characters used to indicate hot keys in menu names
 */
static int compareName( const char *dst, const char *src )
{
    do {
        while( *src == HOT_KEY_CHAR ) {
            src++;
        }
        while( *dst == HOT_KEY_CHAR ) {
            dst++;
        }
        if( tolower( *dst ) != tolower( *src++ ) ) {
            return( false );
        }
    } while( *dst++ );
    return( true );

} /* compareName */

/*
 * getHotKey - get the hot key specified in a string
 */
static vi_key getHotKey( const char *str )
{
    if( str == NULL ) {
        return( 0 );
    }
    while( *str ) {
        if( *str == HOT_KEY_CHAR ) {
            return( toupper( *(str + 1) ) - 'A' + VI_KEY( ALT_A ) );
        }
        str++;
    }
    return( 0 );

} /* getHotKey */

static UINT nextAvail;

/*
 * NextMenuId - returns the next available unique idea for a menu item
 */
UINT NextMenuId( void )
{
    if( ++nextAvail == MAX_ID ) {
        /* run through all menu item lists and 'normalize' them */
        /* 0xefff menu id's should be enough so we will forget it
            for now */
        Message1( "YIKES! Menu id rollover! FIXME in file %s - line %d",
                  __FILE__, __LINE__ );
    }
    return( nextAvail );

} /* NextMenuId */

/*
 * handleMenuCommand - this routine takes a menu and an id and executes the
 *                     command associated with the menu item if there is an
 *                     item with that id.
 */
static vi_rc handleMenuCommand( menu *m, UINT id )
{
    item        *citem;
    char        *str;
    int         len;
    vi_rc       rc;

    for( citem = m->item_head; citem != NULL; citem = citem->next ) {
        if( citem->id == id ) {
            /* run this command */
            len = strlen( citem->cmd ) + 1;
            str = alloca( len );
            memcpy( str, citem->cmd, len );
            IMEsc();
            rc = RunCommandLine( str );
#ifdef __WIN__
            SetWindowCursorForReal();
#endif
            return( rc );
        }
    }
    return( MENU_COMMAND_NOT_HANDLED );

} /* handleMenuCommand */

/* special menu crap - for floaters and that guy at the corner of a window */

#define MAX_FLOAT_MENUS     4

static menu floatMenus[MAX_FLOAT_MENUS] = {
    { NULL, NULL, NULL, NULL, 0, 0, 0 },
    { NULL, NULL, NULL, NULL, 0, 0, 0 },
    { NULL, NULL, NULL, NULL, 0, 0, 0 },
    { NULL, NULL, NULL, NULL, 0, 0, 0 }
};

static menu windowGadgetMenu =
    { NULL, NULL, NULL, NULL, 0, 0, 0 };

typedef struct special_menu {
    char        *name;
    menu        *m;
} special_menu;

static special_menu specialMenus[] = {
    { "float0", &floatMenus[0] },
    { "float1", &floatMenus[1] },
    { "float2", &floatMenus[2] },
    { "float3", &floatMenus[3] },
    { "windowgadget", &windowGadgetMenu }
};

/*
 * isSpecialMenuPtr - see if a menu pointer is a special menu
 */
static bool isSpecialMenuPtr( menu *cmenu )
{
    int i;

    for( i = 0; i < sizeof( specialMenus ) / sizeof( special_menu ); i++ ) {
        if( cmenu == specialMenus[i].m ) {
            return( true );
        }
    }
    return( false );

} /* isSpecialMenuPtr */

/*
 * specialMenu - check if a name is a special menu name
 */
static menu *specialMenu( const char *name )
{
    menu            *m;
    special_menu    *s;
    int             i;

    /* this is a little gross... */
    m = NULL;
    s = &specialMenus[0];
    for( i = 0; i < sizeof( specialMenus ) / sizeof( special_menu ); i++, s++ ) {
        if( compareName( name, s->name ) ) {
            m = s->m;
            break;
        }
    }
    return( m );

} /* specialMenu */

/*
 * specialMenuCommand - run a command from a specific menu
 */
static vi_rc specialMenuCommand( UINT w )
{
    int             i;
    vi_rc           rc;
    special_menu    *s;

    s = &specialMenus[0];
    for( i = 0; i < sizeof( specialMenus ) / sizeof( special_menu ); i++, s++ ) {
        rc = handleMenuCommand( s->m, w );
        if( rc != MENU_COMMAND_NOT_HANDLED ) {
            return( rc );
        }
    }
    return( MENU_COMMAND_NOT_HANDLED );

} /* specialMenuCommand */

/*
 * Intermediate level functions used by high-level guys
 */

/*
 * addMenuToMenu - adds a main level menu item
 */
static menu *addMenuToMenu( menu *m, const char *name, const char *help )
{
    menu        *new;
    int         name_len;

    // assert( IsMenu( m->menu_handle ) );
    name_len = strlen( name );
    new = MemAlloc( sizeof( menu ) + name_len + strlen( help ) + 1 );
    new->num_items = 0;
    strcpy( &new->name[0], name );
    new->help = &new->name[name_len + 1];
    strcpy( new->help, help );
    // new->menu_handle = CreatePopupMenu();
    // AppendMenu( m->menu_handle, MF_ENABLED | MF_POPUP, new->menu_handle, name );
    new->menu_handle = (HMENU)NULLHANDLE;
    new->item_head = NULL;
    new->item_tail = NULL;
    m->num_items += 1;
    AddLLItemAtEnd( (ss **)&m->item_head, (ss **)&m->item_tail, (ss *)new );
    return( new );

} /* addMenuToMenu */

/*
 * add an item to a menu structure
 */
static item *addItemToMenu( menu *m, const char *name, const char *help, const char *cmd, bool append )
{
    item        *new;
    int         cmd_len;
    int         name_len;

    // assert( IsMenu( m->menu_handle ) );
    cmd_len = strlen( cmd );
    name_len = strlen( name );
    new = MemAlloc( sizeof( item ) + cmd_len + name_len + strlen( help ) + 2 );
    if( *name == 0 ) {
        new->id = 0;
        new->name = NULL;
        if( append ) {
            AppendMenu( m->menu_handle, MF_SEPARATOR, 0, NULL );
        }
    } else {
        strcpy( &new->cmd[0], cmd );
        new->name = &new->cmd[cmd_len + 1];
        strcpy( new->name, name );
        new->help = &new->name[name_len + 1];
        strcpy( new->help, help );
        new->id = NextMenuId();
        if( append ) {
             AppendMenu( m->menu_handle, MF_ENABLED | MF_STRING, new->id, name );
        }
    }
    new->in_menu = false;
    new->is_checked = false;
    new->is_active = true;
    m->num_items += 1;
    AddLLItemAtEnd( (ss **)&m->item_head, (ss **)&m->item_tail, (ss *)new );
    return( new );

} /* addItemToMenu */

/*
 * findItem - look for a menu item with a particular offset
 */
static item *findItem( menu *m, int offset )
{
    item    *citem;
    int     i;

    if( offset == -1 ) {
        citem = m->item_tail;
    } else {
        i = 0;
        for( citem = m->item_head; citem != NULL; citem = citem->next ) {
            if( i >= offset ) {
                break;
            }
            ++i;
        }
    }
    return( citem );

} /* findItem */

/*
 * findMenu - look for a menu with a particular name
 */
static menu *findMenu( menu *parent, const char *name )
{
    menu    *m;

    m = specialMenu( name );
    if( m == NULL ) {
        for( m = (menu *)parent->item_head; m != NULL; m = m->next ) {
            if( compareName( &m->name[0], name ) ) {
                /* yep, this is him officer */
                break;
            }
        }
    }
    return( m );

} /* findMenu */

/*
 * freeItem - free an item in a menu
 */
static bool freeItem( menu *m, int offset )
{
    item    *citem;

    if( offset == -1 ) {
        offset = m->num_items - 1;
    }
    citem = findItem( m, offset );
    if( citem != NULL ) {
        if( citem->in_menu ) {
            assert( m->menu_handle != NULL );
            DeleteMenu( m->menu_handle, offset, MF_BYPOSITION );
        }
        m->num_items -= 1;
        DeleteLLItem( (ss **)&m->item_head, (ss **)&m->item_tail, (ss *)citem );
        MemFree( citem );
        return( true );
    }
    return( false );

} /* freeItem */

/*
 * clearMenu - clear all items from a menu
 */
static void clearMenu( menu *m )
{
    assert( m != NULL );
    while( m->num_items > 0 ) {
        freeItem( m, 0 );
    }
    assert( m->item_head == NULL );

} /* clearMenu */

/*
 * burnItem - remove an item from a menu
 */
static void burnItem( menu *parent, int offset )
{
    item    *citem;

    if( offset == -1 ) {
        offset = parent->num_items - 1;
    }
    citem = findItem( parent, offset );
    if( citem != NULL ) {
        assert( parent->menu_handle != NULL );
        DeleteMenu( parent->menu_handle, offset, MF_BYPOSITION );
        citem->in_menu = false;
    }

} /* burnItem */

/*
 * burnMenu - delete all menu items associated with menu
 */
static void burnMenu( menu *parent, menu *m )
{
    int     i;
    if( m->menu_handle ) {
        for( i = 0; i < m->num_items; i++ ) {
            burnItem( m, i );
        }
        assert( parent->menu_handle != NULL );
        for( i = 0; i < parent->num_items; i++ ) {
            if( GetSubMenu( parent->menu_handle, i ) == m->menu_handle ) {
                DeleteMenu( parent->menu_handle, i, MF_BYPOSITION );
                break;
            }
        }
        m->menu_handle = (HMENU)NULLHANDLE;
    }

} /* burnMenu */

/*
 * freeMenu - destroy and free all data associated with a menu
 */
static void freeMenu( menu *parent, menu *m )
{
    int     offset;

    assert( m != NULL && parent != NULL && parent->num_items > 0 );
    clearMenu( m );
    for( offset = 0; offset < parent->num_items; offset++ ) {
        if( GetSubMenu( parent->menu_handle, offset ) == m->menu_handle ) {
            DeleteMenu( parent->menu_handle, offset, MF_BYPOSITION );
            break;
        }
    }
    if( m->menu_handle ) {
        DestroyMenu( m->menu_handle );
    }
    parent->num_items -= 1;
    DeleteLLItem( (ss **)&parent->item_head, (ss **)&parent->item_tail, (ss *)m );
    MemFree( m );

} /* freeMenu */

/*
 * makeItem - add an item to a menu
 */
static void makeItem( menu *m, item *citem )
{
    if( !citem->in_menu ) {
        assert( m->menu_handle != NULL );
        if( citem->id != 0 ) {
            AppendMenu( m->menu_handle, MF_ENABLED | MF_STRING, citem->id,
                        &citem->name[0] );
        } else {
            AppendMenu( m->menu_handle, MF_SEPARATOR, 0, NULL );
        }
        citem->in_menu = true;
    }

} /* makeItem */

/*
 * makeMenu - make a new menu
 */
static void makeMenu( menu *parent, menu *m )
{
    item    *i;

    assert( parent->menu_handle != NULL && m->menu_handle == NULL );
    m->menu_handle = CreatePopupMenu();
    AppendMenu( parent->menu_handle, MF_ENABLED | MF_POPUP,
                (UINT) m->menu_handle, &m->name[0] );
    for( i = m->item_head; i != NULL; i = i->next ) {
        i->in_menu = false;
        makeItem( m, i );
    }

} /* makeMenu */


/*
 * These are the routines which are used to create and add to menus
 */

/*
 * StartMenu - start a new menu; MenuItem will be used to add to this
 */
vi_rc StartMenu( const char *data )
{
    char        name[MAX_STR];
    char        help[MAX_STR];
    bool        need_hook;

    if( currMenu != NULL ) {
        return( ERR_INVALID_MENU );
    }

    GetStringWithPossibleQuoteC( &data, name );
    GetStringWithPossibleQuoteC( &data, help );
    data = SkipLeadingSpaces( data );
    need_hook = false;
    if( data[0] != 0 ) {
        need_hook = true;
    }
    /* check for an existing menu with the same name */
    currMenu = findMenu( rootMenu, name );
    if( currMenu == NULL ) {
        currMenu = addMenuToMenu( rootMenu, name, help );
    } else {
        clearMenu( currMenu );
    }
    currMenu->need_hook = need_hook;
    return( ERR_NO_ERR );

} /* StartMenu */

/*
 * MenuItem - add an item to the current menu
 */
vi_rc MenuItem( const char *data )
{
    char        name[MAX_STR];
    char        help[MAX_STR];

    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    GetStringWithPossibleQuoteC( &data, name );
    GetStringWithPossibleQuoteC( &data, help );
    data = SkipLeadingSpaces( data );
    TranslateTabs( name );
    addItemToMenu( currMenu, name, help, data, false );
    return( ERR_NO_ERR );

} /* MenuItem */

/*
 * AddMenuItem - add an item to a specific menu
 */
vi_rc AddMenuItem( const char *data )
{
    char        menu_name[MAX_STR];
    char        name[MAX_STR];
    char        help[MAX_STR];
    menu        *m;

    GetStringWithPossibleQuoteC( &data, menu_name );
    GetStringWithPossibleQuoteC( &data, name );
    GetStringWithPossibleQuoteC( &data, help );
    data = SkipLeadingSpaces( data );
    m = findMenu( rootMenu, menu_name );
    if( m != NULL ) {
        addItemToMenu( m, name, help, data, false );
        InitMenu();
        return( ERR_NO_ERR );
    }
    return( ERR_INVALID_MENU );

} /* AddMenuItem */

/*
 * DoMenuChar - handle a menu activated by the keyboard
 */
vi_rc DoMenuChar( void )
{
    vi_key      key;
    menu        *m;
    item        *citem;
    char        *str;
    int         len;

    key = LastEvent;
    for( m = rootMenu->item_head; m != NULL; m = m->next ) {
        if( getHotKey( m->name ) == key ) {
            key = GetNextEvent( true );
            for( citem = m->item_head; citem != NULL; citem = citem->next ) {
                if( getHotKey( citem->name ) == key ) {
                    len = strlen( citem->cmd ) + 1;
                    str = alloca( len );
                    memcpy( str, citem->cmd, len );
                    return( RunCommandLine( str ) );
                }
            }
        }
    }
    return( ERR_NO_ERR );

} /* DoMenuChar */

/*
 * ViEndMenu - stop adding a top level menu
 */
vi_rc ViEndMenu( void )
{
    vi_key  key;

    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    key = getHotKey( currMenu->name );
    if( key >= VI_KEY( ALT_A ) && key <= VI_KEY( ALT_Z ) ) {
        EventList[key].rtn.old = DoMenuChar;
        EventList[key].alt_rtn.old = DoMenuChar;
        EventList[key].ins = IMMenuKey;
        EventList[key].b.keep_selection = true;
    }
    if( !isSpecialMenuPtr( currMenu ) ) {
        InitMenu();
    }
    currMenu = NULL;
    return( ERR_NO_ERR );

} /* EndMenu */

/*
 * These are the routines which are used to Destroy and Delete menus and
 * items in menus
 */

/*
 * DoItemDelete - delete an item from a menu
 */
vi_rc DoItemDelete( const char *data )
{
    menu    *m;
    char    name[MAX_STR];
    char    parm[MAX_STR];
    int     offset;

    data = GetNextWord1( data, name );
    m = findMenu( rootMenu, name );
    if( m != NULL ) {
        data = GetNextWord1( data, parm );
        offset = atoi( parm );
        if( freeItem( m, offset ) ) {
            InitMenu();
            return( ERR_NO_ERR );
        }
    }
    return( ERR_INVALID_MENU );

} /* DoItemDelete */

/*
 * DoMenuDelete - handle deleting a menu
 */
vi_rc DoMenuDelete( const char *data )
{
    menu    *m;

    data = SkipLeadingSpaces( data );
    m = specialMenu( data );
    if( m == NULL ) {
        m = findMenu( rootMenu, data );
        if( m != NULL ) {
            freeMenu( rootMenu, m );
            InitMenu();
            return( ERR_NO_ERR );
        }
    }
    return( ERR_INVALID_MENU );

} /* DoMenuDelete */

/*
 * Initialize our menus
 */
vi_rc InitMenu( void )
{
    menu    *m;

    if( Root ) {
        if( rootMenu->menu_handle != NULL ) {
            /* pitch all Windows objects */
            for( m = (menu *)rootMenu->item_head; m != NULL; m = m->next ) {
                burnMenu( rootMenu, m );
            }
            DestroyMenu( rootMenu->menu_handle );
        }
        rootMenu->menu_handle = CreateMenu();
        if( EditFlags.Menus ) {
            for( m = (menu *)rootMenu->item_head; m != NULL; m = m->next ) {
                m->menu_handle = (HMENU)NULLHANDLE;
                makeMenu( rootMenu, m );
            }
        }
        SetMenu( Root, rootMenu->menu_handle );
    }
    return( ERR_NO_ERR );

} /* InitMenu */

void FiniMenu( void )
{
    menu    *m, *next;

    if( Root ) {
        if( rootMenu->menu_handle != NULL ) {
            for( m = (menu *)rootMenu->item_head; m != NULL; m = next ) {
                next = m->next;
                if( m->menu_handle ) {
                    clearMenu( m );
                    // DestroyMenu( m->menu_handle );
                }
                DeleteLLItem( (ss **)&rootMenu->item_head, (ss **)&rootMenu->item_tail, (ss *)m );
                MemFree( m );
            }
            // DestroyMenu( rootMenu->menu_handle );
        }
    }
}


/*
 * IsMenuHotKey - decide if a specific alt key is a menu hot key
 */
bool IsMenuHotKey( vi_key key )
{
    menu    *m;

    for( m = rootMenu->item_head; m != NULL; m = m->next ) {
        if( getHotKey( m->name ) == key ) {
            return( true );
        }
    }
    return( false );

} /* IsMenuHotKey */

/*
 * doFloatMenu - create a floating popup menu
 */
static int doFloatMenu( int id, int x, int y )
{
    menu        *m;
    HMENU       f;
    item        *citem;
    POINT       p;

    assert( id >= 0 && id < MAX_FLOAT_MENUS );
    m = &floatMenus[id];
    f = CreatePopupMenu();
    for( citem = m->item_head; citem != NULL; citem = citem->next ) {
        if( citem->name == NULL ) {
            AppendMenu( f, MF_SEPARATOR, 0, NULL );
        } else {
            AppendMenu( f, MF_ENABLED | MF_STRING, citem->id, citem->name );
        }
    }
    p.x = x;
    p.y = y;
    ClientToScreen( CurrentWindow, &p );
    TrackPopupMenu( f, 0, p.x, p.y, 0, Root, NULL );
    DestroyMenu( f );
    return( ERR_NO_ERR );

} /* doFloatMenu */

/*
 * ActivateFloatMenu - activate floating menu
 */
vi_rc ActivateFloatMenu( const char *data )
{
    char        str[MAX_STR];
    int         id, len, x, y;

    /*
     * get input syntax :
     * FLOATMENU id len x y
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
    len = atoi( str );
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_MENU );
    }
    x = atoi( str );
    data = GetNextWord1( data, str );
    if( *str == '\0' ) {
        return( ERR_INVALID_MENU );
    }
    y = atoi( str );
    return( doFloatMenu( id, x, y ) );

} /* ActivateFloatMenu */

/*
 * MenuCommand: returns true if it handles the command and
 * FALSE otherwise. Looks for a menu item with id identical to
 * the one passed in.
 */
vi_rc MenuCommand( UINT w )
{
    menu    *m;
    vi_rc   rc;

    if( !w || EditFlags.HoldEverything ) {
        return( ERR_NO_ERR );
    }
    rc = HandleToolCommand( w );
    if( rc != MENU_COMMAND_NOT_HANDLED ) {
        // SetFocus( Root ); // can't do this -- we have an ideactivate button
        return( rc );
    } else {
        rc = specialMenuCommand( w );
        if( rc != MENU_COMMAND_NOT_HANDLED ) {
            return( rc );
        } else {
            for( m = rootMenu->item_head; m != NULL; m = m->next ) {
                rc = handleMenuCommand( m, w );
                if( rc != MENU_COMMAND_NOT_HANDLED ) {
                    return( rc );
                }
            }
        }
    }
    return( MENU_COMMAND_NOT_HANDLED );

} /* MenuCommand */

static void tabs_to_slash_t( char *buffer, const char *text )
{
    while( *text != 0 ) {
        if( *text == '\t' ) {
            *buffer = '\\';
            buffer++;
            *buffer = 't';
        } else {
            *buffer = *text;
        }
        buffer++;
        text++;
    }
    *buffer = '\0';
}

/*
 * dumpMenu - dump out data for a menu to a file
 */
static void dumpMenu( FILE *f, menu *cmenu )
{
    item        *citem;
    int         i;
    int         cnt;

    citem = cmenu->item_head;
    if( cmenu->has_file_list || cmenu->has_last_files ) {
        cnt = cmenu->orig_num_items;
        if( cnt == 0 ) {
            cnt = cmenu->num_items;
        }
    } else {
        cnt = cmenu->num_items;
    }
    for( i = 0; i < cnt; i++ ) {
        if( citem != NULL ) {
            if( citem->id == 0 ) {
                MyFprintf( f, "    menuitem \"\"\n" );
            } else {
                char name[256];
                tabs_to_slash_t( name, citem->name );
                MyFprintf( f, "    menuitem \"%s\" \"%s\" %s\n", name, citem->help, citem->cmd );
            }
            citem = citem->next;
        }
    }
    if( cmenu->has_file_list ) {
        MyFprintf( f, "    menufilelist\n" );
    } else if( cmenu->has_last_files ) {
        MyFprintf( f, "    menulastfiles\n" );
    }

} /* dumpMenu */

/*
 * BarfMenuData - write out data for all menus to a file
 */
void BarfMenuData( FILE *f )
{
    menu        *cmenu;

    for( cmenu = rootMenu->item_head; cmenu != NULL; cmenu = cmenu->next ) {
        if( cmenu->menu_handle != NULL ) {
            if( cmenu->need_hook ) {
                MyFprintf( f, "menu %s \"%s\" 1\n", cmenu->name, cmenu->help );
            } else {
                MyFprintf( f, "menu %s \"%s\"\n", cmenu->name, cmenu->help );
            }
            dumpMenu( f, cmenu );
            MyFprintf( f, "endmenu\n" );
        }
    }

} /* BarfMenuData */

/*
 * purgeOldMenuBottom
 */
static void purgeOldMenuBottom( menu *cmenu )
{
    int         cnt;
    item        *citem, *nitem;

    /*
     * get rid of the old menu items
     */
    if( cmenu->orig_num_items > 0 ) {
        cnt = 0;
        citem = cmenu->item_head;
        while( cnt < cmenu->orig_num_items ) {
            citem = citem->next;
            cnt++;
        }
        while( cnt < cmenu->num_items ) {
            nitem = citem->next;
            DeleteMenu( cmenu->menu_handle, cmenu->orig_num_items, MF_BYPOSITION );
            DeleteLLItem( (ss **)&cmenu->item_head, (ss **)&cmenu->item_tail, (ss *)citem );
            MemFree( citem );
            citem = nitem;
            cnt++;
        }
        cmenu->num_items = cmenu->orig_num_items;
    }

} /* purgeOldMenuBottom */

static menu     *thisMenu;
static int      thisCount;

/*
 * initMenuBottom - prepare to add stuff to the menu bottom
 */
static void initMenuBottom( menu *cmenu, bool add_line )
{
    char        help[1];
    char        name[1];
    char        data[1];

    purgeOldMenuBottom( cmenu );

    cmenu->orig_num_items = cmenu->num_items;
    if( add_line ) {
        help[0] = name[0] = data[0] = 0;
        addItemToMenu( cmenu, name, help, data, true );
    }
    thisCount = 1;
    thisMenu = cmenu;

} /* initMenuBottom */

/*
 * addToMenuBottom - add a file to the bottom of a special menu
 */
static bool addToMenuBottom( const char *fname, bool checkit )
{
    item        *citem;
    char        data[MAX_STR];
    char        name[MAX_STR];
    char        help[MAX_STR];

    MySprintf( name, "&%d %s", thisCount, fname );
    MySprintf( data, "edit \"%s\"", fname );
    MySprintf( help, "Switches to the window containing %s", fname );
    citem = addItemToMenu( thisMenu, name, help, data, true );
    if( checkit ) {
        CheckMenuItem( thisMenu->menu_handle, citem->id, MF_BYCOMMAND | MF_CHECKED );
    }
    thisCount++;
    if( thisCount > 9 ) {
        strcpy( name, "&More Windows ..." );
        strcpy( data, "files" );
        strcpy( help, "Displays a list of all files being edited" );
        addItemToMenu( thisMenu, name, help, data, true );
        return( true );
    }
    return( false );

} /* addToMenuBottom */

/*
 * addFileList - add file list to specified menu
 */
static void addFileList( menu *cmenu )
{
    info        *cinfo;
    UINT        old_avail;

    old_avail = nextAvail;
    nextAvail = FILE_LIST_ID;
    initMenuBottom( cmenu, (InfoHead != NULL) );

    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        if( addToMenuBottom( cinfo->CurrentFile->name, cinfo == CurrentInfo ) ) {
            break;
        }
    }
    nextAvail = old_avail;

} /* addFileList */

/*
 * addLastFiles - add last files to menu
 */
static void addLastFiles( menu *cmenu )
{
    UINT                old_avail;
    history_data        *h;
    char                *menu_text;
    int                 i, j;

    h = &EditVars.LastFilesHist;
    if( h->curr == 0 ) {
        cmenu->orig_num_items = cmenu->num_items;
        return;
    }
    old_avail = nextAvail;
    nextAvail = LAST_FILES_ID;
    initMenuBottom( cmenu, true );

    // make sure j will never be negative
    j = h->curr + h->max - 1;

    for( i = 0; i < h->max; i++ ) {
        menu_text = h->data[j % h->max];
        if( menu_text != NULL ) {
            addToMenuBottom( menu_text, false );
        }
        j--;
    }

    nextAvail = old_avail;

} /* addLastFiles */

/*
 * HandleInitMenu - handle the WM_INITMENU message
 */
void HandleInitMenu( HMENU hmenu )
{
    menu        *cmenu;
    int         i, j;
    item        *citem;
    vi_rc       result;
    bool        need_gray;
    bool        need_check;

    i = 1;
    for( cmenu = rootMenu->item_head; cmenu != NULL; cmenu = cmenu->next ) {
        if( cmenu->need_hook && cmenu->menu_handle != NULL ) {
            j = 1;
            for( citem = cmenu->item_head; citem != NULL; citem = citem->next ) {
                result = InvokeMenuHook( i, j );
                need_gray = (result == -1);
                need_check = (result == -2);
                if( need_gray ) {
                    if( citem->is_active ) {
                        EnableMenuItem( hmenu, citem->id, MF_GRAYED );
                        citem->is_active = false;
                    }
                } else if( !need_gray ) {
                    if( !citem->is_active ) {
                        EnableMenuItem( hmenu, citem->id, MF_ENABLED );
                        citem->is_active = true;
                    }
                    if( need_check && !citem->is_checked ) {
                        CheckMenuItem( hmenu, citem->id, MF_BYCOMMAND | MF_CHECKED );
                        citem->is_checked = true;
                    } else if( !need_check && citem->is_checked ) {
                        CheckMenuItem( hmenu, citem->id, MF_BYCOMMAND | MF_UNCHECKED );
                        citem->is_checked = false;
                    }
                }
                j++;
            }
        }
        if( cmenu->has_file_list ) {
            addFileList( cmenu );
        } else if( cmenu->has_last_files ) {
            addLastFiles( cmenu );
        }
        i++;
    }

} /* HandleInitMenu */

/*
 * ResetMenuBits - reset menu to its initial state
 */
void ResetMenuBits( void )
{
    menu        *cmenu;
    item        *citem;

    for( cmenu = rootMenu->item_head; cmenu != NULL; cmenu = cmenu->next ) {
        if( cmenu->menu_handle != NULL ) {
            citem = cmenu->item_head;
            while( citem != NULL ) {
                citem->is_active = true;
                citem->is_checked = false;
                citem = citem->next;
            }
        }
    }

} /* ResetMenuBits */

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
 * MenuItemLastFiles - add the Last Files menu item
 */
vi_rc MenuItemLastFiles( void )
{
    if( currMenu == NULL ) {
        return( ERR_INVALID_MENU );
    }
    currMenu->has_last_files = true;
    return( ERR_NO_ERR );

} /* MenuItemLastFiles */

static char *currMenuHelpString;

/*
 * HandleMenuSelect - handle a WM_MENUSELECT message (display help)
 */
void HandleMenuSelect( WPARAM wparam, LPARAM lparam )
{
    int         flags;
    int         id;
    HMENU       hmenu;
    menu        *cmenu;
    item        *citem;
    bool        found;
    int         i;

    hmenu = (HMENU) GET_WM_MENUSELECT_HMENU( wparam, lparam );
    id = GET_WM_MENUSELECT_ITEM( wparam, lparam );
    flags = GET_WM_MENUSELECT_FLAGS( wparam, lparam );
    currMenuHelpString = NULL;
    if( flags != -1 || hmenu != 0 ) {
        found = false;
        for( cmenu = rootMenu->item_head; cmenu != NULL; cmenu = cmenu->next ) {
            if( (flags & MF_POPUP) ) {
                if( cmenu->menu_handle == (HMENU) id ) {
                    currMenuHelpString = cmenu->help;
                    found = true;
                }
            } else {
                for( citem = cmenu->item_head; citem != NULL; citem = citem->next ) {
                    if( id == citem->id ) {
                        currMenuHelpString = citem->help;
                        found = true;
                        break;
                    }
                }
            }
            if( found ) {
                break;
            }
        }
        if( !found ) {
            for( i = 0; i < sizeof( specialMenus ) / sizeof( special_menu ); i++ ) {
                for( citem = specialMenus[i].m->item_head; citem != NULL; citem = citem->next ) {
                    if( id == citem->id ) {
                        currMenuHelpString = citem->help;
                        found = true;
                        break;
                    }
                }
                if( found ) {
                    break;
                }
            }
        }
    }
    UpdateStatusWindow();

} /* HandleMenuSelect */

/*
 * GetMenuHelpString - get the current menu help string
 */
void GetMenuHelpString( char *res )
{
    if( currMenuHelpString != NULL ) {
        strcpy( res, currMenuHelpString );
    } else {
        res[0] = ' ';
        res[1] = 0;
    }

} /* GetMenuHelpString */


/*
 * SetMenuHelpString - set the current menu help string
 */
void SetMenuHelpString( char *str )
{
    currMenuHelpString = str;

} /* SetMenuHelpString */


vi_rc DoWindowGadgetMenu( void )
{
    SendMessage( CurrentWindow, WM_SYSCOMMAND, 0xF100, 0x0000002DL );
    return( ERR_NO_ERR );
}

/*
 * Stubs
 */
int GetMenuIdFromCoord( int x )
{
    x = x;
    return( -1 );
}

int GetCurrentMenuId( void )
{
    return( -1 );
}
