/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2019 The Open Watcom Contributors. All Rights Reserved.
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

#include "bool.h"

#define MAGIC       '\x9C'

#ifdef STANDALONE_RX

#define MAX_STR     256

#define REGEXPR_ERRORS() \
    pick( ERR_NO_ERR,                       NULL ) \
    pick( ERR_RE_INTERNAL_FOULUP,           "Internal err: Regexp foulup" ) \
    pick( ERR_RE_CORRUPTED_POINTER,         "Internal err: Regexp corrupted pointer" ) \
    pick( ERR_RE_MEMORY_CORRUPTION,         "Internal err: Regexp memory corruption" ) \
    pick( ERR_RE_TRAILING_SLASH,            "Trailing \\\\" ) \
    pick( ERR_RE_OPERAND_FOLLOWS_NOTHING,   "?+* follows nothing" ) \
    pick( ERR_RE_UNMATCHED_SQUARE_BRACKET,  "Unmatched []" ) \
    pick( ERR_RE_INVALID_SB_RANGE,          "invalid [] range" ) \
    pick( ERR_RE_NESTED_OPERAND,            "nested *?+" ) \
    pick( ERR_RE_EMPTY_OPERAND,             "*+ operand could be empty" ) \
    pick( ERR_RE_UNMATCHED_ROUND_BRACKETS,  "Unmatched ()" ) \
    pick( ERR_RE_TOO_MANY_ROUND_BRACKETS,   "Too many ()" ) \
    pick( ERR_RE_NULL_ARGUMENT,             "NULL argument" ) \
    pick( ERR_RE_INVALID_CASETOGGLE,        "Invalid case toggle" )

typedef enum regex_error {
    #define pick(e,t)       e,
        REGEXPR_ERRORS()
    #undef pick
} regex_error;

extern regex_error  RegExpError;

#endif /* STANDALONE_RX */

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

#define NSUBEXP  21
typedef struct {
    const char  *startp[NSUBEXP];
    const char  *endp[NSUBEXP];
    const char  *regmust;       /* Internal use only. */
    short       regmlen;        /* Internal use only. */
    char        regstart;       /* Internal use only. */
    char        reganch;        /* Internal use only. */
    char        program[1];     /* Unwarranted chumminess with compiler. */
} regexp;

/* regexp.c */
extern regexp   *RegComp( const char * );
extern int      RegExec( regexp *, const char *, bool );
extern void     RegAnchor( regexp * );

#ifdef __cplusplus
};
#endif
#endif
