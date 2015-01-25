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

#ifndef REGEXP_INCLUDED
#define REGEXP_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#ifdef STANDALONE_RX
enum {
    ERR_NO_ERR,
    ERR_RE_INTERNAL_FOULUP,
    ERR_RE_CORRUPTED_POINTER,
    ERR_RE_MEMORY_CORRUPTION,
    ERR_RE_TRAILING_SLASH,
    ERR_RE_OPERAND_FOLLOWS_NOTHING,
    ERR_RE_UNMATCHED_SQUARE_BRACKET,
    ERR_RE_INVALID_SB_RANGE,
    ERR_RE_NESTED_OPERAND,
    ERR_RE_EMPTY_OPERAND,
    ERR_RE_UNMATCHED_ROUND_BRACKETS,
    ERR_RE_TOO_MANY_ROUND_BRACKETS,
    ERR_RE_NULL_ARGUMENT,
    ERR_RE_INVALID_CASETOGGLE
};

#define MAX_STR 256

#include "bool.h"

extern int      RegExpError;

#ifndef REALTABS
extern bool     RealTabs;
#endif

#ifndef CASEIGNORE
extern bool     CaseIgnore;
#endif

#ifndef MAGICFLAG
extern bool     MagicFlag;
#endif

#ifndef MAGICSTR
extern char     *MagicString;
#endif

#endif

#define NSUBEXP  21
typedef struct {
    const char  *startp[NSUBEXP];
    const char  *endp[NSUBEXP];
    char        regstart;       /* Internal use only. */
    char        reganch;        /* Internal use only. */
    const char  *regmust;       /* Internal use only. */
    short       regmlen;        /* Internal use only. */
    char        program[1];     /* Unwarranted chumminess with compiler. */
} regexp;

/* regexp.c */
extern regexp   *RegComp( const char * );
extern int      RegExec( regexp *, const char *, bool );
extern void     RegAnchor( regexp * );

#define         MAGIC   '\x9C'
#ifdef __cplusplus
};
#endif
#endif
