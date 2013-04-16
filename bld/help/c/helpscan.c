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
* Description:  Hyperlink scanning.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "uidef.h"
#include "help.h"
#include "helpscan.h"
#include "helpchar.h"
#include "helpmem.h"

static  unsigned char   specialChars[] = {
    HELP_ESCAPE,
    GOOFY_HYPERLINK,
    FAKE_RIGHT_ARROW,
    FAKE_LEFT_ARROW,
    '<',
    '>',
    '{',
    '}',
    '\0'
};

static  unsigned char   specialHyperChars[] = {
    HELP_ESCAPE,
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
    unsigned char       endchar;
    char                *cur;
    char                *topic;
    char                *hfname;
    int                 cnt;
    TextInfoBlock       *block;
    bool                chktopic;
    bool                chkhfname;

    block = &(info->block1);
    block->next = NULL;
    cur = line;
    cnt = 0;
    chktopic = FALSE;
    chkhfname = FALSE;
    if( *(unsigned char *)cur == GOOFY_HYPERLINK ) {
        *type = TK_GOOFY_LINK;
        endchar = GOOFY_HYPERLINK;
    } else {
        *type = TK_PLAIN_LINK;
        endchar = '>';
    }
    ++cur;
    topic = cur;
    for( ;; ) {
        switch( *(unsigned char *)cur ) {
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
                continue;               // let the HELP_ESCAPE case handle this
            }
            /* fall through */
        case GOOFY_HYPERLINK:
            if( *(unsigned char *)cur == endchar ) {
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
            while( (*(unsigned char *)cur != endchar)
              && (*(unsigned char *)cur != HYPER_TOPIC) ) {
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
            block->info[cnt].len = strcspn( cur, (char *)specialHyperChars );
            cur += block->info[cnt].len;
            cnt ++;
            break;
        }
    }
}

void freeHyperlinkInfo( TextInfoBlock *cur )
{
    TextInfoBlock       *tmp;

    while( cur != NULL ) {
        tmp = cur;
        cur = cur->next;
        HelpMemFree( tmp );
    }
}

bool ScanLine( char *line, ScanCBfunc *cb, void *info )
{
    char                *cur;
    Info                tinfo;
    TokenType           type;
    bool                newfile;

    cur = line;
    newfile = FALSE;
    for( ;; ) {
        switch( *(unsigned char *)cur ) {
        case HELP_ESCAPE:
            tinfo.u.text.str = cur;
            cur ++;
            switch( *cur ) {
            case H_UNDERLINE:
            case H_UNDERLINE_END:
            case H_BOLD:
            case H_BOLD_END:
                cur++;
                tinfo.u.text.type = TT_CTRL_SEQ;
                tinfo.u.text.len = 2;
                break;
            default:
                cur++;
                tinfo.u.text.type = TT_ESC_SEQ;
                tinfo.u.text.len = 2;
                break;
            }
            cb( TK_TEXT, &tinfo, info );
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
            cur += scanHyperLink( cur, &type, &tinfo.u.link );
            if( tinfo.u.link.hfname_len != 0 ) {
                newfile = TRUE;
            }
            cb( type, &tinfo, info );
            if( tinfo.u.link.block1.next != NULL ) {
                freeHyperlinkInfo( tinfo.u.link.block1.next );
            }
            break;
        case FAKE_RIGHT_ARROW:
            tinfo.u.text.str = cur;
            tinfo.u.text.type = TT_RIGHT_ARROW;
            tinfo.u.text.len = 1;
            cb( TK_TEXT, &tinfo, info );
            cur++;
            break;
        case FAKE_LEFT_ARROW:
            tinfo.u.text.str = cur;
            tinfo.u.text.type = TT_LEFT_ARROW;
            tinfo.u.text.len = 1;
            cb( TK_TEXT, &tinfo, info );
            cur++;
            break;
        case '\0':
            tinfo.u.text.str = cur;
            tinfo.u.text.type = TT_END_OF_LINE;
            tinfo.u.text.len = 1;
            cb( TK_TEXT, &tinfo, info );
            return( newfile );
            break;
        default:
            tinfo.u.text.str = cur;
            tinfo.u.text.type = TT_PLAIN;
            tinfo.u.text.len = strcspn( cur, (char *)specialChars );
            cur += tinfo.u.text.len;
            cb( TK_TEXT, &tinfo, info );
            break;
        }
    }
}
