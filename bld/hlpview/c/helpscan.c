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
* Description:  Hyperlink scanning.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "watcom.h"
#include "helpscan.h"
#include "helpmem.h"
#include "wibhelp.h"


static char     specialChars[] = {
    IB_ESCAPE,
    IB_HLINK,
    IB_RIGHT_ARROW,
    IB_LEFT_ARROW,
    IB_PLAIN_LINK_BEG,
    IB_PLAIN_LINK_END,
    '{',
    '}',
    '\0'
};

static char     specialHyperChars[] = {
    IB_ESCAPE,
    IB_HLINK,
    IB_HLINK_BREAK,
    IB_PLAIN_LINK_BEG,
    IB_PLAIN_LINK_END,
    '{',
    '}',
    '\0'
};

static unsigned scanHyperLink( char *line, HelpTokenType *hlink_type, HyperLinkInfo *info )
{
    char                endchar;
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
    chktopic = false;
    chkhfname = false;
    if( *cur == IB_HLINK ) {
        *hlink_type = TK_GOOFY_LINK;
        endchar = IB_HLINK;
    } else {
        *hlink_type = TK_PLAIN_LINK;
        endchar = IB_PLAIN_LINK_END;
    }
    hfname = NULL;
    ++cur;
    topic = cur;
    for( ;; ) {
        switch( *cur ) {
        case IB_ESCAPE:
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
            cnt++;
            break;
        case IB_PLAIN_LINK_BEG:
        case IB_PLAIN_LINK_END:
        case '{':
        case '}':
            if( cur[0] == cur[1] ) {
                cur[0] = IB_ESCAPE;
                continue;               // let the IB_ESCAPE case handle this
            }
            /* fall through */
        case IB_HLINK:
            if( *cur == endchar ) {
                info->topic = topic;
                block->cnt = cnt;
                if( chkhfname ) {
                    info->hfname = hfname;
                    info->hfname_len = cur - hfname;
                    info->topic_len = hfname - topic - 1;
                } else {
                    info->hfname = NULL;
                    info->hfname_len = 0;
                    info->topic_len = cur - topic;
                }
                return( cur - line + 1 );
            }
            break;
        case IB_HLINK_BREAK:
            cur++;
            if( !chktopic ) {
                topic = cur;
                chktopic = true;
            } else {
                hfname = cur;
                chkhfname = true;
            }
            while( (*cur != endchar) && (*cur != IB_HLINK_BREAK) ) {
                if( *cur == IB_ESCAPE )
                    cur++;
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
            cnt++;
            break;
        }
    }
}

static void freeHyperlinkInfo( TextInfoBlock *cur )
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
    HelpTokenType       hlink_type;
    bool                newfile;

    cur = line;
    newfile = false;
    for( ;; ) {
        switch( *cur ) {
        case IB_ESCAPE:
            tinfo.u.text.str = cur;
            cur++;
            switch( *cur ) {
            case IB_UNDERLINE_ON:
            case IB_UNDERLINE_OFF:
            case IB_BOLD_ON:
            case IB_BOLD_OFF:
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
        case IB_PLAIN_LINK_BEG:
        case IB_PLAIN_LINK_END:
        case '{':
        case '}':
            if( cur[0] == cur[1] ) {
                cur[0] = IB_ESCAPE;
                continue;               // let the IB_ESCAPE case handle this
            }
            if( *cur != IB_PLAIN_LINK_BEG ) {
                break;
            }
            /* fall through */
        case IB_HLINK:
            cur += scanHyperLink( cur, &hlink_type, &tinfo.u.link );
            if( tinfo.u.link.hfname_len != 0 ) {
                newfile = true;
            }
            cb( hlink_type, &tinfo, info );
            if( tinfo.u.link.block1.next != NULL ) {
                freeHyperlinkInfo( tinfo.u.link.block1.next );
            }
            break;
        case IB_RIGHT_ARROW:
            tinfo.u.text.str = cur;
            tinfo.u.text.type = TT_RIGHT_ARROW;
            tinfo.u.text.len = 1;
            cb( TK_TEXT, &tinfo, info );
            cur++;
            break;
        case IB_LEFT_ARROW:
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
            tinfo.u.text.len = strcspn( cur, specialChars );
            cur += tinfo.u.text.len;
            cb( TK_TEXT, &tinfo, info );
            break;
        }
    }
}
