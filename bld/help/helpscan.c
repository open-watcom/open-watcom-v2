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


#include <stdio.h>
#include <string.h>
#include "uidef.h"
#include "helpscan.h"
#include "helpchar.h"
#include "helpmem.h"

static  char    specialChars[] = { HELP_ESCAPE,
                                   GOOFY_HYPERLINK,
                                   FAKE_RIGHT_ARROW,
                                   FAKE_LEFT_ARROW,
                                   '<',
                                   '>',
                                   '{',
                                   '}',
                                   '\0'
                                 };

static  char    specialHyperChars[] = { HELP_ESCAPE,
                                        GOOFY_HYPERLINK,
                                        HYPER_TOPIC,
                                        '<',
                                        '>',
                                        '{',
                                        '}',
                                        '\0'
                                 };

static unsigned scanHyperLink( char *line, TokenType *type,
                               HyperLinkInfo *info )
{
    char                endchar;
    char                *cur;
    char                *topic;
    char                *hfname;
    int                 cnt;
    TextInfoBlock       *block;
    bool                chktopic;
    bool                chkhfname;

    block = &( info->block1 );
    block->next = NULL;
    cur = line;
    cnt = 0;
    chktopic = FALSE;
    chkhfname = FALSE;
    if( *cur == GOOFY_HYPERLINK ) {
        *type = TK_GOOFY_LINK;
        endchar = GOOFY_HYPERLINK;
    } else {
        *type = TK_PLAIN_LINK;
        endchar = '>';
    }
    cur ++;
    topic = cur;
    for( ;; ) {
        switch( *cur ) {
        case HELP_ESCAPE:
            if( cnt == TEXT_BLOCK_SIZE ) {
                cnt = 0;
                block->next = HelpMemAlloc( sizeof( TextInfoBlock ) );
                block = block->next;
                block->next = NULL;
            }
            block->info[cnt].str = cur;
            block->info[cnt].type = TT_ESC_SEQ;
            block->info[cnt].len = 2;
            cur += 2;
            cnt ++;
            break;
        case '{':
        case '>':
        case '}':
        case '<':
            if( cur[0] == cur[1] ) {
                cur[0] = HELP_ESCAPE;
                continue;               // let the HELP_ESCAPE case handle
                                        // this
            }
            /* fall through */
        case GOOFY_HYPERLINK:
            if( *cur == endchar ) {
                info->topic = topic;
                block->cnt = cnt;
                if( chkhfname ) {
                    info->hfname = hfname;
                    info->hfname_len = cur - hfname;
                    info->topic_len = hfname - topic - 1;
                } else {
                    info->hfname = '\0';
                    info->hfname_len = 0;
                    info->topic_len = cur - topic;
                }
                return( cur - line + 1 );
            }
            break;
        case HYPER_TOPIC:
            cur++;
            if( !chktopic ) {
                topic = cur;
                chktopic = TRUE;
            } else {
                hfname = cur;
                chkhfname = TRUE;
            }
            while( ( *cur != endchar ) && ( *cur != HYPER_TOPIC ) ) {
                if( *cur == HELP_ESCAPE ) cur++;
                cur++;
            }
            break;
        default:
            if( cnt == TEXT_BLOCK_SIZE ) {
                cnt = 0;
                block->next = HelpMemAlloc( sizeof( TextInfoBlock ) );
                block = block->next;
                block->next = NULL;
            }
            block->info[cnt].str = cur;
            block->info[cnt].type = TT_PLAIN;
            block->info[cnt].len = strcspn( cur, specialHyperChars );
            cur += block->info[cnt].len;
            cnt ++;
            break;
        }
    }
}


void freeHyperlinkInfo( TextInfoBlock *cur ) {

    TextInfoBlock       *tmp;

    while( cur != NULL ) {
        tmp = cur;
        cur = cur->next;
        HelpMemFree( tmp );
    }
}

bool ScanLine( char *line, void (*cb)(), void *info )
{
    char                *cur;
    TextInfo            textinfo;
    HyperLinkInfo       hyperlink;
    TokenType           type;
    bool                newfile;

    cur = line;
    newfile = FALSE;
    for( ;; ) {
        switch( *cur ) {
        case HELP_ESCAPE:
            textinfo.str = cur;
            cur ++;
            switch( *cur ) {
            case H_UNDERLINE:
            case H_UNDERLINE_END:
            case H_BOLD:
            case H_BOLD_END:
                cur++;
                textinfo.type = TT_CTRL_SEQ;
                textinfo.len = 2;
                break;
            default:
                cur++;
                textinfo.type = TT_ESC_SEQ;
                textinfo.len = 2;
                break;
            }
            cb( TK_TEXT, &textinfo, info );
            break;
        case '{':
        case '>':
        case '}':
            if( cur[0] == cur[1] ) {
                cur[0] = HELP_ESCAPE;
                continue;               // let the HELP_ESCAPE case handle
                                        // this
            }
            break;
        case '<':
            if( cur[1] == '<' ) {
                cur[0] = HELP_ESCAPE;
                continue;               // let the HELP_ESCAPE case handle
                                        // this
            }
            /* fall through */
        case GOOFY_HYPERLINK:
            cur += scanHyperLink( cur, &type, &hyperlink );
            if( hyperlink.hfname_len != 0 ) {
                newfile = TRUE;
            }
            cb( type, &hyperlink, info );
            if( hyperlink.block1.next != NULL ) {
                freeHyperlinkInfo( hyperlink.block1.next );
            }
            break;
        case FAKE_RIGHT_ARROW:
            textinfo.str = cur;
            textinfo.type = TT_RIGHT_ARROW;
            textinfo.len = 1;
            cb( TK_TEXT, &textinfo, info );
            cur++;
            break;
        case FAKE_LEFT_ARROW:
            textinfo.str = cur;
            textinfo.type = TT_LEFT_ARROW;
            textinfo.len = 1;
            cb( TK_TEXT, &textinfo, info );
            cur++;
            break;
        case '\0':
            textinfo.str = cur;
            textinfo.type = TT_END_OF_LINE;
            textinfo.len = 1;
            cb( TK_TEXT, &textinfo, info );
            return( newfile );
            break;
        default:
            textinfo.str = cur;
            textinfo.type = TT_PLAIN;
            textinfo.len = strcspn( cur, specialChars );
            cur += textinfo.len;
            cb( TK_TEXT, &textinfo, info );
            break;
        }
    }
}
