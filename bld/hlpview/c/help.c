/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Help file access functions.
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "help.h"
#include "helpmem.h"
#include "uibox.h"
#include "hlpuicvr.h"
#include "hlputkys.h"
#include "uidialog.h"
#include "uimenu.h"
#include "uitab.h"
#include "helpscan.h"
#include "msgbox.h"
#include "uigchar.h"
#include "wibhelp.h"
#include "pathgrp2.h"

#include "clibext.h"


#define EV_HELP         EV_F1

enum {
    E_UP = EV_FIRST_UNUSED,
    E_DOWN
};

// In normal QNX text files, the record separator is just a LF.
// However, with the new help file format containing both binary and
// text data, we can't do a text conversion, so the record separator
// is still a CR/LF from DOS. So the RSEP_LEN is 2.

#define RSEP_LEN                2

#if defined( QNX )
    #define EV_MARK_PRESS       EV_CTRL_PRESS
    #define EV_MARK_RELEASE     EV_CTRL_RELEASE
#else
    #define EV_MARK_PRESS       EV_SHIFT_PRESS
    #define EV_MARK_RELEASE     EV_SHIFT_RELEASE
#endif

#define AT(x)                   UIData->attrs[x]
#define AT_BLINK                0x80

#define C_PLAIN                 0
#define C_ULINE                 1
#define C_BOLD                  2
#define C_ULBOLD                3

#define HELP_MIN_WIDTH          63

#define BUF_LEN                 400

enum {
    HSTCK_NAME,
    HSTCK_OFFSET
};

typedef struct hstackent {
    struct hstackent    *next;
    int                 cur;
    int                 line;
    int                 type;
    char                helpfname[_MAX_PATH];
    char                word[1];        /* dynamic array */
} a_hstackent;

typedef struct a_field {
    SAREA               area;
    struct a_field      *next;
    unsigned            key1_len;
    unsigned            key2_len;
    char                keyword[1];
} a_field;

static VSCREEN helpScreen = {
    EV_NO_EVENT,                    /* event number */
    NULL,                           /* screen title */
    {0, 0, 0, 0},                   /* location and size */
    V_DIALOGUE,                     /* flags */
    0, 0,                           /* initial cursor position */
    C_OFF                           /* cursor type */
};

static a_gadget         vGadget = {
    NULL,                       /* virtual screen */
    VERTICAL,                   /* direction */
    0,                          /* anchor position */
    0,                          /* start of bar */
    0,                          /* end of bar */
    E_DOWN,                     /* forward event */
    E_UP,                       /* backward event */
    EV_PAGE_DOWN,               /* forward page event */
    EV_PAGE_UP,                 /* backward page event */
    EV_NO_EVENT,                /* slider event */
    0,                          /* total size */
    0,                          /* page size */
    0,                          /* current position */
    GADGET_NONE,                /* flags */
    0                           /* linear */
};

static a_hot_spot       hotSpots[] = {
    { "&Back",            EV_F4,          -1,       1,     15, 0  },
    { "&Search",          EV_ALT_S,       -1,       0,     15, 0  },
    { "Cancel",           EV_ESCAPE,      -1,      -2,     15, 0  },
    { NULL,               ___ }
};

#define SEARCH_HOT_SPOT         2

static VFIELD hotSpotFields[] = {
    {{0, 0, 0, 0}, FLD_HOT, &hotSpots[0] },
    {{0, 0, 0, 0}, FLD_HOT, &hotSpots[2] },
    {{0, 0, 0, 0}, FLD_HOT, &hotSpots[1] },
    {{0, 0, 0, 0}, FLD_NONE,NULL }
};

static ui_event keyShift[] = {
    __rend__,
    EV_ALT_F12,
    EV_MARK_PRESS,
    EV_MARK_RELEASE,
    EV_MOUSE_RELEASE,
    EV_MOUSE_DRAG,
    EV_INSERT,
    EV_DELETE,
    EV_MOUSE_DCLICK,
    __end__
};

static ui_event helpEventList[] = {
    __rend__,
    EV_ALT_B,
    'b',
    'B',
    EV_ALT_S,
    's',
    'S',
    EV_CURSOR_LEFT,
    EV_CURSOR_RIGHT,
    EV_TAB_FORWARD,
    EV_TAB_BACKWARD,
    EV_ENTER,
    EV_ESCAPE,
    EV_HELP,
    EV_PAGE_UP,
    EV_PAGE_DOWN,
    '-',
    '+',
    EV_F4,
    EV_F9,
    EV_F10,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_TOP,
    EV_BOTTOM,
    EV_SCROLL_VERTICAL,
    EV_MOUSE_PRESS,
    EV_MOUSE_REPEAT,
    EV_MOUSE_RELEASE,
    EV_MOUSE_RELEASE_R,
    EV_FIELD_CHANGE,
    E_UP,
    E_DOWN,
    __end__
};

static a_hstackent      *helpStack;
static a_field          *helpTab;
static a_field          *helpCur;
static char             *helpInBuf;
static char             *helpOutBuf;
static char             helpWord[] = { "HELP" };
static int              helpLines;
static unsigned         *helpPos;
static long             topPos;
static int              maxPos;
static int              maxLine;
static int              currLine;
static int              lastHelpLine;
static ATTR             currentAttr;
static int              currentColour;
static bool             ignoreMouseRelease;

static char             curFileName[_MAX_PATH];
static FILE             *helpfile_fp;
static HelpHdl          helpSearchHdl;
static VTAB             tabFilter;
static ui_event         curEvent;
static ui_event         (*eventMapFn)( ui_event );

static int              CheckHelpBlock( FILE *fp, const char *topic, char *buffer, long start );
static void             replacetopic( const char *word );
static ScanCBfunc       scanCallBack;


static void addSearchButton( bool add )
{
    if( add ) {
        hotSpotFields[SEARCH_HOT_SPOT].typ = FLD_HOT;
        hotSpotFields[SEARCH_HOT_SPOT].u.hs = &hotSpots[1];
    } else {
        hotSpotFields[SEARCH_HOT_SPOT].typ = FLD_VOID;
        hotSpotFields[SEARCH_HOT_SPOT].u.hs = NULL;
    }
}

/*
 * helpGetString
 */
static char *helpGetString( char *buf, size_t size, FILE *fp )
{
    long            pos;
    size_t          bytesread;
    size_t          cnt;

    pos = HelpTell( fp );
    bytesread = HelpRead( fp, buf, size - 1 );
    if( bytesread != size - 1 )
        return( NULL );
    for( cnt = 0; cnt < bytesread; ++cnt ) {
        if( buf[cnt] == '\n' ) {
            cnt++;
            break;
        }
    }
    HelpSeek( fp, pos + cnt, HELP_SEEK_SET );
    buf[cnt] = '\0';
    return( buf );
}

/*
 * OpenTopicInFile - open a help file and look for a topic
 */
static int OpenTopicInFile( help_file_info *fileinfo, const char *topic, char *buffer )
{
    long                start_offset;     /* - starting offset in HELP file        */
    long                size_left;        /* - size left to search                 */
    int                 next_posn;        /* - contains indicator for next pos'n   */
    unsigned long       topic_pos;

    if( fileinfo->fp == NULL )
        return( 0 );
    if( fileinfo->searchhdl != NULL ) {
        /* search by new method */
        topic_pos = HelpFindTopicOffset( fileinfo->searchhdl, topic );
        if( topic_pos == (unsigned long)-1 ) {
            return( 0 );
        } else {
            HelpSeek( fileinfo->fp, topic_pos, HELP_SEEK_SET );
            if( helpGetString( buffer, BUF_LEN, fileinfo->fp ) == NULL ) {
                return( 0 );
            } else {
                return( 1 );
            }
        }
    } else {
        start_offset = 0L;
    }
    size_left = HelpFileLen( fileinfo->fp );
    do {
        size_left = size_left / 2;
        if( size_left <= 8 ) {
            if( CheckHelpBlock( fileinfo->fp, topic, buffer, start_offset ) != 0 ) {
                return( 0 );
            } else {
                return( 1 );
            }
        }
        next_posn = CheckHelpBlock( fileinfo->fp, topic, buffer, start_offset + size_left );
        if( next_posn > 0 ) {
            start_offset += size_left;
        }
    } while( next_posn != 0 );
    return( 1 );
}

static char *scanTopic( char *buf, char **theend )
{
    char        *topic;
    char        *end;

    if( !IS_IB_TOPIC_NAME( buf ) )
        return( NULL );
    if( buf[4] == '"' ) {
        topic = buf + 5;
        for( end = topic; *end != '"' && *end != '\0'; ++end ) {
            if( *end == IB_ESCAPE ) {
                end++;
            }
        }
    } else {
        topic = buf + 5;
        while( *topic == ' ' )
            topic++;
        end = topic;
        while( *end != ' ' && *end != '\n' && *end != '\0' ) {
            end++;
        }
    }
    if( theend != NULL )
        *theend = end;
    return( topic );
}

/*
 * CheckHelpBlock - see if a topic is in the 2nd half of a block
 */
static int CheckHelpBlock( FILE *fp, const char *topic, char *buffer, long start )
{
    int         retn;
    char        *ftopic;
    char        *end;
    unsigned    len;

    HelpSeek( fp, start, HELP_SEEK_SET );
    retn = 0;
    do {
        if( helpGetString( buffer, BUF_LEN, fp ) == NULL ) {
            retn = -1;
            break;
        }
    } while( !IS_IB_TOPIC_NAME( buffer ) );
    if( retn == 0 ) {
        ftopic = scanTopic( buffer, &end );
        len = end - ftopic;
        retn = strnicmp( topic, ftopic, len );
        if( !retn && topic[len] != '\0' ) {
            retn = 1;
        }
    }
    return( retn );
}

static void help_close( void )
{
    help_file_info  *fileinfo;

    for( fileinfo = HelpFileInfo(); fileinfo->name != NULL; ++fileinfo ) {
        if( fileinfo->fp != NULL ) {
            HelpClose( fileinfo->fp );
            FiniHelpSearch( fileinfo->searchhdl );
            fileinfo->searchhdl = NULL;
            fileinfo->fp = NULL;
        }
    }
}

/*
 * help_open - open a help file at a topic location
 */
static help_file_info *help_open( char *buffer )
{
    help_file_info  *fileinfo;
    char            *newtopic;

    for( fileinfo = HelpFileInfo(); fileinfo->name != NULL; fileinfo++ ) {
        if( fileinfo->fp == NULL ) {
            /* text files screw up ctrl z */
            fileinfo->fp = HelpOpen( fileinfo->name );
            fileinfo->searchhdl = InitHelpSearch( fileinfo->fp );
        }
        if( helpStack->word[0] == '\0' ) {
            newtopic = GetDefTopic( fileinfo->searchhdl );
            replacetopic( newtopic );
        }
        if( OpenTopicInFile( fileinfo, helpStack->word, buffer ) ) {
            break;
        }
    }
    if( fileinfo->name == NULL )
        return( NULL );
    return( fileinfo );
}


static void add_field( a_field *ht, bool changecurr )
{
    a_field             **p;
    int                 count;

    count = 0;
    for( p = &helpTab; ; p = &((*p)->next) ) {
        if( *p == NULL
        || (*p)->area.row > ht->area.row
        || ((*p)->area.row == ht->area.row && (*p)->area.col > ht->area.col )
        ) {
            ht->next = *p;
            *p = ht;
            if( changecurr && count < helpStack->cur ) {
                helpStack->cur += 1;
            }
            break;
        }
        ++count;
    }
    tabFilter.first = helpTab;
}

static int field_count( a_field *table, a_field *field )
{
    int                 i;

    if( field == NULL )
        return( 0 );
    for( i = 0; table != NULL; table = table->next, ++i ) {
        if( table == field ) {
            return( i );
        }
    }
    return( 0 );
}

static void del_field( a_field *table, a_field **field, bool changecurr )
{
    a_field             *next;
    int                 count;

    count = field_count( table, *field );
    if( *field == tabFilter.curr ) {
        tabFilter.curr = NULL;
    }
    if( changecurr && count < helpStack->cur ) {
        helpStack->cur -= 1;
    }
    next = (*field)->next;
    HelpMemFree( *field );
    *field = next;
    tabFilter.first = helpTab;
}

static void display_fields( void )
{
    a_field             *p;

    for( p = helpTab; p != NULL; p = p->next ) {
        uivattribute( &helpScreen, p->area, AT( ATTR_EDIT ) );
    }
}

static void free_fields( a_field **ht )
{
    while( *ht != NULL ) {
        del_field( *ht, ht, false );
    }
}

static a_field *field_find( a_field *table, int count )
{
    for( ; count > 0 && table != NULL; --count, table = table->next )
        ;

    return( table );
}

static void vscroll_fields( a_field **ht, SAREA use, int incr )
{
    a_field             **p;
    a_field             **next;

    if( incr == 0 )
        return;
    for( p = ht; *p && (*p)->area.row < use.row + use.height ; p = next ) {
        next = &((*p)->next);
        if( (*p)->area.row >= use.row ) {
            if((incr > 0 && (*p)->area.row < use.row + incr )
            || (incr < 0 && (*p)->area.row - incr >= use.row + use.height) ) {
                next = p;
                del_field( *ht, p, true );
            } else {
                (*p)->area.row -= incr;
            }
        }
    }
}

static a_tab_field *help_next_field( a_field *fld, a_field *table )
{
    /* unused parameters */ (void)table;

    if( fld != NULL ) {
        fld = fld->next;
    }
    return( fld );
}

static void nexttopic( const char *word )
{
    a_hstackent         *h;
    unsigned            len;

    if( word == NULL ) {
        len = 0;
    } else {
        len = strlen( word );
    }
    h = HelpMemAlloc( sizeof( a_hstackent ) + len );
    if( helpStack != NULL ) {
        helpStack->cur = field_count( helpTab, helpCur );
        helpStack->line = currLine;
    }
    h->next = helpStack;
    h->type = HSTCK_NAME;
    h->cur = 0;
    h->line = 0;
    strcpy( h->helpfname, curFileName );
    if( word != NULL ) {
        strcpy( h->word, word );
    }
    h->word[len] = '\0';
    helpStack = h;
    free_fields( &helpTab );
}

static void prevtopic( void )
{
    a_hstackent         *h;

    if( helpStack->next != NULL ) {
        h = helpStack;
        helpStack = helpStack->next;
        HelpMemFree( h );
    }
    free_fields( &helpTab );
}

static void replacetopic( const char *word )
{
    if( helpStack->next == NULL ) {
        HelpMemFree( helpStack );
        helpStack = NULL;
    } else {
        prevtopic();
    }
    nexttopic( word );
}

void Free_Stack( void )
{
    while( helpStack->next != NULL ) {
        prevtopic();
    }
    HelpMemFree( helpStack );
}

static unsigned help_in_tab( a_field *fld, void *dummy )
{
    /* unused parameters */ (void)fld; (void)dummy;

    return( true );
}

static ui_event hlpwait( VTAB *tab )
{
    bool                done;
    static ui_event     bumpev = EV_NO_EVENT;
    char                *next_name;
    unsigned            len1;
    unsigned            len2;

    helpCur = field_find( helpTab, helpStack->cur );
    if( helpTab != NULL && helpCur == NULL ) {
        helpCur = helpTab;
    }
    tab->other = helpCur;
    tab->curr = helpCur;
    if( helpCur != NULL ) {
        tab->home = helpCur->area.col;
    }
    uipushlist( helpEventList );
    if( bumpev != EV_NO_EVENT ) {
        uitabfilter( bumpev, tab );
        helpCur = tab->curr;
        bumpev = EV_NO_EVENT;
    }
    done = false;
    while( !done ) {
        if( helpTab != NULL ) {
            uivattribute( &helpScreen, helpCur->area, AT( ATTR_CURR_EDIT ) );
        }
        do {
            uipushlist( keyShift );
            curEvent = uivget( &helpScreen );
            if( curEvent == EV_MOUSE_PRESS ) {
                ignoreMouseRelease = false;
            }
            uipoplist();
            curEvent = uigadgetfilter( curEvent, &vGadget );
            curEvent = uitabfilter( curEvent, tab );
        } while( curEvent == EV_NO_EVENT );
        if( eventMapFn != NULL ) {
            curEvent = (*eventMapFn)( curEvent );
        }
        curEvent = uihotspotfilter( &helpScreen, hotSpotFields, curEvent );
        if( helpTab != NULL ) {
            uivattribute( &helpScreen, helpCur->area, AT( ATTR_EDIT ) );
        }
        switch( curEvent ) {
        case EV_HELP:
            nexttopic( helpWord );
            done = true;
            break;
        case EV_BOTTOM:
        case E_UP:
        case EV_PAGE_UP:
        case EV_PAGE_DOWN:
        case EV_CURSOR_UP:
        case EV_CURSOR_DOWN:
        case EV_TOP:
        case E_DOWN:
        case EV_SCROLL_VERTICAL:
            if( curEvent == EV_BOTTOM ) {
                bumpev = EV_CURSOR_DOWN;
            } else if( curEvent == EV_TOP ) {
                bumpev = EV_CURSOR_UP;
            }
            helpStack->cur = field_count( helpTab, helpCur );
            done = true;
            break;
        case '-':
        case EV_MOUSE_RELEASE_R:
        case EV_ALT_B:
        case 'b':
        case 'B':
        case EV_F8:
        case EV_F4:
            prevtopic();
            if( strcmp( helpStack->helpfname, curFileName ) ) {
                len1 = strlen( helpStack->word );
                len2 = strlen( helpStack->helpfname );
                helpCur = HelpMemAlloc( sizeof( a_field ) + len1 + len2 );
                memcpy( helpCur->keyword, helpStack->word, len1 );
                memcpy( helpCur->keyword + len1, helpStack->helpfname, len2 );
                helpCur->keyword[len1 + len2] = '\0';
                helpCur->key1_len = len1;
                helpCur->key2_len = len2;
                helpCur->next = NULL;
//              prevtopic();
                helpTab = helpCur;
            }
            done = true;
            break;
        case EV_ALT_S:
        case 'S':
        case 's':
            if( helpSearchHdl != NULL ) {
                uipoplist();
                next_name = HelpSearch( helpSearchHdl );
                if( next_name != NULL ) {
                    nexttopic( next_name );
                    HelpMemFree( next_name );
                    done = true;
                }
                uipushlist( helpEventList );
            }
            break;
        case EV_FIELD_CHANGE:
            helpCur = tab->curr;
            break;
        case EV_MOUSE_RELEASE:
            if( tab->other == NULL )
                break;
            if( ignoreMouseRelease ) {
                /* this mouse release is for a mouse press that occured
                 * before this help topic was opened */
                 ignoreMouseRelease = false;
                 break;
            }
            tab->other = tab->curr;
            /* fall through */
        case EV_ENTER:  /*same as page-down if there are other topics*/
        case EV_F7:
        case '+':
            // DEN 90/04/12 - next line necessary for mouse release kludge
            helpCur = tab->curr;
            if( helpTab != NULL ) {
                if( helpCur->key2_len == 0 ) {
                    nexttopic( helpCur->keyword );
                }
                done = true;
            }
            break;
        case EV_KILL_UI:
            uiforceevadd( EV_KILL_UI );
            /* fall through */
        case EV_ESCAPE:
            done = true;
            break;
        }
    }
    uipoplist();
    return( curEvent );
}

static bool mygetline( void )
{
    int                 l;

    if( helpGetString( helpInBuf, BUF_LEN, helpfile_fp ) == NULL ) {
        return( false );
    }
    l = strlen( helpInBuf );
    if( l >= RSEP_LEN ) {
        helpInBuf[l - RSEP_LEN] = '\0'; /* ignore record sep */
    }
    return( true );
}

static void clearline( void )
{
    helpInBuf[0] = '\0';
    helpOutBuf[0] = '\0';
}

static void scanCallBack( HelpTokenType type, Info *info, void *_myinfo )
{
    TextInfoBlock       *block;
    unsigned            i;
    a_field             *ht;
    ScanInfo            *myinfo = _myinfo;

    switch( type ) {
    case TK_TEXT:
        switch( info->u.text.type ) {
        case TT_LEFT_ARROW:
            myinfo->buf[0] = PC_arrowleft;
            myinfo->buf++;
            myinfo->pos++;
            break;
        case TT_RIGHT_ARROW:
            myinfo->buf[0] = PC_arrowright;
            myinfo->buf++;
            myinfo->pos++;
            break;
        case TT_END_OF_LINE:
            myinfo->buf[0] = '\0';
            myinfo->buf++;
            myinfo->pos++;
            break;
        case TT_CTRL_SEQ:
            memcpy( myinfo->buf, info->u.text.str, info->u.text.len );
            myinfo->buf += info->u.text.len;
            break;
        case TT_ESC_SEQ:
            memcpy( myinfo->buf, info->u.text.str, info->u.text.len );
            myinfo->buf += info->u.text.len;
            myinfo->pos += 1;
            break;
        default:
            memcpy( myinfo->buf, info->u.text.str, info->u.text.len );
            myinfo->buf += info->u.text.len;
            myinfo->pos += info->u.text.len;
            break;
        }
        break;
    case TK_PLAIN_LINK:
        myinfo->buf[0] = '<';
        myinfo->buf++;
        myinfo->pos++;
        /* fall through */
    case TK_GOOFY_LINK:
        ht = HelpMemAlloc( sizeof( a_field ) + info->u.link.topic_len + info->u.link.hfname_len );
        ht->area.width = 0;
        ht->area.row = myinfo->line;
        ht->area.height = 1;
        ht->area.col = myinfo->pos;
        memcpy( ht->keyword, info->u.link.topic, info->u.link.topic_len );
        if( info->u.link.hfname_len != 0 ) {
            memcpy( ht->keyword + info->u.link.topic_len, info->u.link.hfname, info->u.link.hfname_len );
            ht->keyword[info->u.link.topic_len + info->u.link.hfname_len] = '\0';
        } else {
            ht->keyword[info->u.link.topic_len] = '\0';
        }
        ht->key1_len = info->u.link.topic_len;
        ht->key2_len = info->u.link.hfname_len;
        for( block = &( info->u.link.block1 ); block != NULL; block = block->next ) {
            for( i = 0; i < block->cnt; i++ ) {
                info = (Info *)&( block->info[i] );
                switch( info->u.text.type ) {
                case TT_ESC_SEQ:
                    memcpy( myinfo->buf, info->u.text.str, info->u.text.len );
                    myinfo->buf += info->u.text.len;
                    myinfo->pos++;
                    ht->area.width++;
                    break;
                case TT_PLAIN:
                    memcpy( myinfo->buf, info->u.text.str, info->u.text.len );
                    myinfo->buf += info->u.text.len;
                    myinfo->pos += info->u.text.len;
                    ht->area.width += info->u.text.len;
                    break;
                }
            }
        }
        if( ht->area.col + ht->area.width > helpScreen.area.width ) {
            ht->area.width = helpScreen.area.width - ht->area.col;
        }
        add_field( ht, myinfo->changecurr );
        if( type == TK_PLAIN_LINK ) {
            myinfo->buf[0] = '>';
            myinfo->buf++;
            myinfo->pos++;
        }
        break;
    }
}

/*
 * processLine
 */
static bool processLine( char *bufin, char *bufout, int line, bool changecurr )
{
    ScanInfo    info;

    info.buf = bufout;
    info.line = line;
    info.changecurr = changecurr;
    info.pos = 0;
    ScanLine( bufin, scanCallBack, &info );
    return( true );
}

/*
 * helpSet - set the help window title bar and its size
 *         - assumes helpInbuf contains the topic line for this topic
 */
static void helpSet( char *str, char *helpname, unsigned buflen )
{
    int         nums[5];
    int         i;
    char        *srcptr;
    char        *dstptr;

    *helpname = '\0';
    helpLines = 0;
    if( str != NULL ) {
        srcptr = str;
        dstptr = helpname;
        while( *srcptr != '\0' ) {
            if( *srcptr == IB_ESCAPE )
                srcptr++;
            *dstptr = *srcptr;
            srcptr++;
            dstptr++;
            buflen--;
            if( buflen == 0 ) {
                dstptr--;
                *dstptr = '\0';
                break;
            }
        }
        *dstptr = '\0';
        scanTopic( helpInBuf, &str );
        str++;
        str = strtok( str, " " );
    }
    for( i = 0; i < 5; ++i ) {
        nums[i] = 0;
        if( str != NULL ) {
            nums[i] = atoi( str );
            str = strtok( NULL, " " );
        }
    }
    /* To remove help window height/width restrictions hardcoded in the
     * help file, we could zero out or ignore the 'nums' entries.
     */
#if 0
    nums[0] = nums[1] = 0;
#endif
    helpScreen.area.height = (nums[0] == 0) ? UIData->height - 3 : nums[0] + 3;
    helpScreen.area.width = (nums[1] == 0) ? UIData->width - 2 : nums[1];
    helpLines = nums[4];
    if( helpScreen.area.height > UIData->height - 2 ) {
        helpScreen.area.height = UIData->height - 2;
    }
    if( helpScreen.area.width > UIData->width - 2 ) {
        helpScreen.area.width = UIData->width - 2;
    } else if( helpScreen.area.width < HELP_MIN_WIDTH ) {
        helpScreen.area.width = HELP_MIN_WIDTH;
    }
    if( strlen( helpname ) > helpScreen.area.width ) {
        helpname[helpScreen.area.width] = '\0';
    }
    helpScreen.title = helpname;
    uiposition( &helpScreen.area, helpScreen.area.height, helpScreen.area.width, nums[2], nums[3], true );
}


static void putline( char *buffer, int line )
{
    int                 i;
    int                 start;

    helpScreen.cursor_col = 0;
    i = 0;
    while( buffer[i] && helpScreen.cursor_col < helpScreen.area.width ) {
        start = i;
        while( buffer[i] && buffer[i] != IB_ESCAPE &&
               helpScreen.cursor_col + i - start <= helpScreen.area.width ) {
            if( buffer[i] == IB_SPACE_NOBREAK )
                buffer[i] = ' ';
            ++i;
        }
        if( i - start > 0 ) {
            uivtextput( &helpScreen, line, helpScreen.cursor_col, currentAttr, &buffer[start], i - start );
        }
        helpScreen.cursor_col += i - start;
        if( buffer[i] == IB_ESCAPE ) {
            switch( buffer[i+1] ) {
            case IB_UNDERLINE_ON:
                currentColour |= C_ULINE;
                ++i;
                break;
            case IB_UNDERLINE_OFF:
                currentColour &= ~C_ULINE;
                ++i;
                break;
            case IB_BOLD_ON:
                currentColour |= C_BOLD;
                ++i;
                break;
            case IB_BOLD_OFF:
                currentColour &= ~C_BOLD;
                ++i;
                break;
            }
            ++i;
            switch( currentColour ) {
            case C_PLAIN:
                currentAttr = AT( ATTR_NORMAL );
                break;
            case C_ULINE:
                currentAttr = AT( ATTR_BRIGHT );
                break;
            case C_BOLD:
                currentAttr = AT( ATTR_BRIGHT );
                break;
            case C_ULBOLD:
                currentAttr = AT( ATTR_BRIGHT );
                break;
            }
        }
    }
}

static void save_line( int line, long offset )
/* Assumption:  line <= lastHelpLine + 1 */
{
    if( line >= maxPos ) {
        maxPos = line + 10;
        helpPos = HelpMemRealloc( helpPos, maxPos * sizeof( *helpPos ) );
    }
    if( line > lastHelpLine ) {
        lastHelpLine = line;
    }
    if( offset == -1 ) {
        helpPos[line] = (unsigned)-1;
    } else {
        helpPos[line] = offset - topPos;
    }
}

static void seek_line( int line )
{
    int                 i;

    if( line > lastHelpLine ) {
        HelpSeek( helpfile_fp, topPos + helpPos[lastHelpLine], HELP_SEEK_SET );
        for( i = lastHelpLine; ; ++i ) {
            save_line( i, HelpTell( helpfile_fp ) );
            if( i == line )
                break;
            if( !mygetline() || IS_IB_TOPIC_NAME( helpInBuf ) ) {
                maxLine = i;
                break;
            }
        }
    } else if( line >= 0 ) {
        HelpSeek( helpfile_fp, topPos + helpPos[line], HELP_SEEK_SET );
    }
}

static void set_slider( int pos )
{
    uisetgadget( &vGadget, pos );
}


/*
 * handleFooter - scan through and process footer information
 */
static void handleFooter( int *startline, SAREA *use, SAREA *line )
{
    int         start;

    start = *startline;
    if( IS_IB_TRAILER_BEG( helpInBuf ) ) {
        ++start;        /* leave room for line */
        for( ;; ) {
            if( !mygetline() )
                break;
            if( IS_IB_TRAILER_END( helpInBuf ) )
                break;
            processLine( helpInBuf, helpOutBuf, start, false );
            putline( helpOutBuf, start );
            ++start;
        }
        vscroll_fields( &helpTab, *use, start - use->row - use->height );
        vvscroll( &helpScreen, *use, start - use->row - use->height );
        use->height -= start - use->row;
        line->row = use->row + use->height;
        uivfill( &helpScreen, *line, AT( ATTR_NORMAL ), UiGChar[UI_SBOX_HORIZ_LINE] );
        topPos = HelpTell( helpfile_fp );
    }
    *startline = start;
}

/*
 * handleHeader - scan through and process header information
 */
static void handleHeader( int *start, SAREA *line )
{
    int         cur;

    if( IS_IB_HEADER_BEG( helpInBuf ) ) {
        cur = 0;
        for( ;; ) {
            if( !mygetline() )
                break;
            if( IS_IB_TRAILER_BEG( helpInBuf ) )
                break;
            if( IS_IB_HEADER_END( helpInBuf ) )
                break;
            processLine( helpInBuf, helpOutBuf, cur, false );
            putline( helpOutBuf, cur );
            cur++;
        }
        line->row = cur;
        uivfill( &helpScreen, *line, AT( ATTR_NORMAL ), UiGChar[UI_SBOX_HORIZ_LINE] );
        cur++;
        topPos = HelpTell( helpfile_fp );
        if( IS_IB_HEADER_END( helpInBuf ) ) {
            mygetline();
        }
        *start = cur;
    }
}

/*
 * setupScroolBar - setup the size and position of the vertical scrollbar
 */
static void setupScrollBar( SAREA *use )
{
    vGadget.start = use->row + 1;
    vGadget.end = vGadget.start + use->height - 1;
    vGadget.anchor = helpScreen.area.width + 1;
    vGadget.vs = &helpScreen;
    vGadget.total_size = helpLines;
    vGadget.page_size = use->height;
    vGadget.pos = 0;
    vGadget.slider = EV_SCROLL_VERTICAL;
    uiinitgadget( &vGadget );
}

/*
 * scrollHelp - refresh the displayed help after a scrolling operation
 */
static int scrollHelp( SAREA *use, int lastline, bool changecurr )
{
    int         useline;
    int         scroll;
    int         start;
    int         end;

    scroll = currLine - lastline;
    vscroll_fields( &helpTab, *use, scroll );
    vvscroll( &helpScreen, *use, scroll );
    currentAttr = AT( ATTR_NORMAL );
    if( abs(scroll) >= use->height ) {
        start = currLine;
        end = start + use->height;
    } else if( scroll < 0 ) {
        start = currLine;
        end = start - scroll;
    } else {
        start = currLine + use->height - scroll;
        end = start + scroll;
    }
    seek_line( start );
    for( ;; ++start ) {
        save_line( start, HelpTell( helpfile_fp ) );
        if( !mygetline() || IS_IB_TOPIC_NAME( helpInBuf )  ) {
            maxLine = start;
            break;
        }
        useline = start - currLine;
        if( useline >= use->height || start >= end ) {
            break;
        }
        /* if it is the first time in,
           then the old currfield is right
        */
        processLine( helpInBuf, helpOutBuf, useline + use->row, changecurr );
        putline( helpOutBuf, useline + use->row );
    }
    end = currLine + use->height;
    if( maxLine != 0 && end > maxLine ) {
        end = maxLine;
    }
    display_fields();
    hotSpots[1].startcol = ( helpScreen.area.width - hotSpots[1].length ) / 2;
    addSearchButton( helpSearchHdl != NULL );
    uiposnhotspots( &helpScreen, hotSpotFields );
    uiprinthotspots( &helpScreen, hotSpotFields );
    set_slider( currLine );
    uirefresh();
    return( currLine );
}

/*
 * dispHelp
 */
static int dispHelp( char *str, VTAB *tab )
{
    ui_event            ui_ev;
    bool                done;
    int                 lastline;
    int                 start;
    SAREA               use;
    SAREA               line;
    char                helpname[81];

    ignoreMouseRelease = true;
    helpSet( str, helpname, sizeof( helpname ) );
    if( uivopen( &helpScreen ) == NULL )
        return( HELP_NO_VOPEN );

    use.height = helpScreen.area.height - 3;
    use.width = helpScreen.area.width;
    use.col = 0;

    line.height = 1;
    line.width = helpScreen.area.width;
    line.row = 0;
    line.col = 0;
    start = 0;

    topPos = HelpTell( helpfile_fp );
    mygetline();

    handleHeader( &start, &line );
    use.row = start;
    use.height -= start;
    handleFooter( &start, &use, &line );
    setupScrollBar( &use );

    if( helpLines > 0 ) {
        maxPos = helpLines + 1;
        helpPos = HelpMemAlloc( maxPos * sizeof( *helpPos ) );
    } else {
        maxPos = 0;
        helpPos = NULL;
    }
    maxLine = 0;
    lastHelpLine = 0;
    save_line( 0, topPos );
    currLine = helpStack->line;
    seek_line( currLine );
    lastline = currLine + use.height;
    done = false;
    ui_ev = EV_NO_EVENT;
    while( !done ) {
        currentColour = C_PLAIN;
        currentAttr = AT( ATTR_NORMAL );
        if( lastline != currLine ) {
            lastline = scrollHelp( &use, lastline, ( ui_ev != EV_NO_EVENT ) );
        }
        ui_ev = hlpwait( tab );
        switch( ui_ev ) {
        case E_UP:
        case EV_CURSOR_UP:
        case EV_TOP:
            if( currLine > 0 ) {
                --currLine;
            }
            break;
        case E_DOWN:
        case EV_CURSOR_DOWN:
        case EV_BOTTOM:
            ++currLine;
            if( maxLine != 0 && currLine+use.height > maxLine ) {
                --currLine;
            }
            break;
        case EV_SCROLL_VERTICAL:
            currLine = vGadget.pos;
            break;
        case EV_PAGE_UP:
            currLine -= use.height;
            if( currLine < 0 ) {
                currLine = 0;
            }
            break;
        case EV_PAGE_DOWN:
            currLine += use.height;
            if( maxLine != 0 && currLine >= maxLine ) {
                currLine -= use.height;
            }
            break;
        default:
            done = true;
            break;
        }
    }
    clearline();
    uivclose( &helpScreen );
    if( helpPos != NULL ) {
        HelpMemFree( helpPos );
        helpPos = NULL;
    }
    return( HELP_OK );
}

static int findhelp( VTAB *tab )
{
    help_file_info  *fileinfo;
    int             ret;

    fileinfo = help_open( helpInBuf );
    if( fileinfo == NULL ) {
        return( HELP_NO_SUBJECT );
    }
    helpfile_fp = fileinfo->fp;
    helpSearchHdl = fileinfo->searchhdl;
    ret = dispHelp( helpStack->word, tab );
    /* if a new help file name exists,
       then link to the new file
    */
    if( helpTab != NULL && helpCur->key2_len != 0 ) {
        curEvent = EV_ESCAPE;
    }
    return( ret );
}

/*
 * fixHelpTopic - escape any special characters in topics that comes from
 *                      the outside world
 */
static char *fixHelpTopic( const char *topic )
{
    const char  *ptr;
    char        *retptr;
    unsigned    cnt;
    char        *ret;
    char        c;

    cnt = 0;
    for( ptr = topic; (c = *ptr) != '\0'; ptr++ ) {
        if( c == '<' || c == '>' || c == '"' || c == '{' || c == '}' ) {
            cnt++;
        }
        cnt++;
    }
    retptr = ret = HelpMemAlloc( cnt + 1 );
    for( ptr = topic; (c = *ptr) != '\0'; ptr++ ) {
        if( c == '<' || c == '>' || c == '"' || c == '{' || c == '}' ) {
            *retptr++ = IB_ESCAPE;
        }
        *retptr++ = c;
    }
    *retptr = '\0';
    return( ret );
}

static int do_showhelp( char **helptopic, char *filename, ui_event (*rtn)( ui_event ), bool first )
{
    int         err;
    char        *ptr;
    unsigned    len;
    char        *htopic;

    eventMapFn = rtn;
    helpTab = NULL;
    helpCur = helpTab;
    strcpy( curFileName, filename );
    helpInBuf = HelpMemAlloc( BUF_LEN );
    if( helpInBuf == NULL ) {
        HelpMemFree( helpStack );
        return( HELP_NO_MEM );
    }
    helpOutBuf = HelpMemAlloc( BUF_LEN );
    if( helpOutBuf == NULL ) {
        HelpMemFree( helpStack );
        return( HELP_NO_MEM );
    }
    // don't fix hyperlink topics
    if( *helptopic != NULL && first ) {
        htopic = fixHelpTopic( *helptopic );
    } else if( *helptopic == NULL ) {
        htopic = HelpDupStr( "" );
    } else {
        htopic = HelpDupStr( *helptopic );
    }
    nexttopic( htopic );
    for( ;; ) {
        err = findhelp( &tabFilter );
        if( err != HELP_OK )
            break;
        if( curEvent == EV_ESCAPE )
            break;
        if( curEvent == EV_KILL_UI ) {
            break;
        }
    }
    SearchDlgFini();
    help_close();
    // This is Not Nice - we're freeing memory that
    // someone else allocated! Just don't do it.
    if( err != HELP_NO_SUBJECT ) {
        if( *helptopic != NULL ) {
            HelpMemFree( *helptopic );
            *helptopic = NULL;
        }
        *filename = '\0';
    }
    HelpMemFree( helpInBuf );
    HelpMemFree( helpOutBuf );
    HelpMemFree( htopic );
    if( helpTab != NULL && helpCur->key2_len != 0 ) { // cross file link
        len = helpCur->key1_len;
        *helptopic = ptr = HelpMemAlloc( len + 1 );
        strncpy( ptr, helpCur->keyword, len );
        ptr[len] = '\0';
        ptr = helpCur->keyword + len;
        len = helpCur->key2_len;
        strncpy( filename, ptr, len );
        filename[len] = '\0';
        if( helpCur != tabFilter.curr ) { // backwards through cross file link
            prevtopic();
        }
    } else {
        Free_Stack();
    }
    free_fields( &helpTab );
    return( err );
}

int showhelp( const char *topic, ui_event (*rtn)( ui_event ), HelpLangType lang )
{
    bool            first;
    int             err;
    char            filename[_MAX_PATH];
    const char      *hfiles[] = { NULL, NULL };
    pgroup2         pg;
    char            *buffer;
    char            *helptopic;
    help_file_info  *fileinfo;

    fileinfo = HelpFileInfo();
    if( fileinfo->name == NULL ) {
        return( HELP_NO_FILE );
    }
    switch( lang ) {
    case HELPLANG_FRENCH:
        hotSpots[0].str = "F4=Sujet pr�c�dent";
        hotSpots[1].str = "Sortir";
        break;
    case HELPLANG_ENGLISH:
        break;
    }
    helpStack = NULL;
    currentColour = C_PLAIN;
    currentAttr = AT( ATTR_NORMAL );
    /* initialize the tab filter */
    tabFilter.tab = (unsigned (*)(void *,void *))help_in_tab;
    tabFilter.next = (a_tab_field *(*)(void *,void *))help_next_field;
    tabFilter.parm = helpTab;
    tabFilter.mousepos = (void *(*)(void *,ORD *, ORD *))uivmousepos;
    tabFilter.mouseparm = &helpScreen;
    tabFilter.first = helpTab;
    tabFilter.wrap = false;
    tabFilter.enter = false;
    _splitpath2( fileinfo->name, pg.buffer, NULL, NULL, &pg.fname, &pg.ext );
    _makepath( filename, NULL, NULL, pg.fname, pg.ext );
    hfiles[0] = filename;
    helptopic = HelpDupStr( topic );
    err = HELP_OK;
    first = true;
    while( helptopic != NULL || first ) {
        if( first || help_reinit( hfiles ) ) {
            err = do_showhelp( &helptopic, filename, rtn, first );
            if( err == HELP_NO_SUBJECT ) {
                break;
            }
        } else {        // cannot open help file for hyperlink
            buffer = HelpMemAlloc( 28 + strlen( filename ) );
            sprintf( buffer, "Unable to open helpfile \"%s\".", filename );
            ShowMsgBox( "Error", buffer );
            HelpMemFree( buffer );
            HelpMemFree( helptopic );
            helptopic = HelpDupStr( helpStack->word );
            strcpy( filename, helpStack->helpfname );
            prevtopic();
        }
        first = false;
    }
    if( helptopic != NULL )
        HelpMemFree( helptopic );
    return( err );
}

char *HelpDupStr( const char *str )
{
    size_t  len;
    char    *ptr;

    ptr = NULL;
    if( str != NULL ) {
        len = strlen( str ) + 1;
        ptr = HelpMemAlloc( len );
        if( ptr != NULL ) {
            strcpy( ptr, str );
        }
    }
    return( ptr );
}
