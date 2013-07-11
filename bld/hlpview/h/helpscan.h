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


#define TEXT_BLOCK_SIZE 20

typedef enum {
    TK_TEXT,
    TK_PLAIN_LINK,
    TK_GOOFY_LINK,
} TokenType;

typedef enum {
    TT_PLAIN,
    TT_CTRL_SEQ,
    TT_ESC_SEQ,
    TT_LEFT_ARROW,
    TT_RIGHT_ARROW,
    TT_END_OF_LINE
} TextType;

#include "pushpck1.h"

typedef struct {
    TextType    type;
    char        *str;
    unsigned    len;
} TextInfo;

typedef struct TextInfoBlock {
    unsigned                    cnt;
    TextInfo                    info[TEXT_BLOCK_SIZE];
    struct TextInfoBlock        *next;
} TextInfoBlock;

typedef struct {
    TextInfoBlock       block1;
    char                *topic;
    unsigned            topic_len;
    char                *hfname;
    unsigned            hfname_len;
} HyperLinkInfo;

typedef struct Info {
    union {
        TextInfo        text;
        HyperLinkInfo   link;
    } u;
} Info;

#include "poppck.h"

typedef void ScanCBfunc(TokenType, Info *, void *);

extern bool ScanLine( char *, ScanCBfunc *, void * );
