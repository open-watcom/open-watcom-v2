/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef __RXWRAP_H__
#define __RXWRAP_H__

#define REGEX_ERRORS() \
    pick( ERR_NO_ERR,                       NULL ) \
    pick( ERR_RE_INTERNAL_FOULUP,           "internal regular expression problem" ) \
    pick( ERR_RE_CORRUPTED_POINTER,         "corrupted regular expression pointer" ) \
    pick( ERR_RE_MEMORY_CORRUPTION,         "memory corruption" ) \
    pick( ERR_RE_TRAILING_SLASH,            "trailing slash found" ) \
    pick( ERR_RE_OPERAND_FOLLOWS_NOTHING,   "operand follows nothing" ) \
    pick( ERR_RE_UNMATCHED_SQUARE_BRACKET,  "unmatched square bracket" ) \
    pick( ERR_RE_INVALID_SB_RANGE,          "invalid range" ) \
    pick( ERR_RE_NESTED_OPERAND,            "nested operand" ) \
    pick( ERR_RE_EMPTY_OPERAND,             "empty operand" ) \
    pick( ERR_RE_UNMATCHED_ROUND_BRACKETS,  "unmatched round brackets" ) \
    pick( ERR_RE_TOO_MANY_ROUND_BRACKETS,   "too many round brackets" ) \
    pick( ERR_RE_NULL_ARGUMENT,             "no regular expression specified" ) \
    pick( ERR_RE_INVALID_CASETOGGLE,        "invalid case toggle" )

typedef enum regex_error {
    #define pick(e,t)   e,
        REGEX_ERRORS()
    #undef pick
} regex_error;

#ifdef __cplusplus
extern "C" {
#endif
    extern regex_error  RegExpError;
#ifdef __cplusplus
};
#endif

#include "regexp.h"

#endif
