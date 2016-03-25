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


typedef enum
{
/* Debugger tokens */

    /* one character command line delimiters */
    #define pick(t,c)   t,
    #include "_dbgtok.h"
    #undef pick
    /* numbers & names for expressions and command lines */
    T_INT_NUM,
    T_REAL_NUM,
    T_BAD_NUM,
    T_NAME,
    T_STRING,
    T_UNKNOWN,

/* SSL tokens */

    /* SSL delimiters */
    T_SSL_DELIMS            = 0x20,

    /* SSL escape characters */
    T_SSL_ESCAPE_CHARS      = 0x80,

    /* SSL special operators */
    T_SSL_SPEC_PAREN        = 0x1000,
    T_SSL_SPEC_POINTER_IND,
    T_SSL_SPEC_FIELD_SELECT,
    T_SSL_SPEC_POINTER_FIELD,
    T_SSL_SPEC_ARRAY,
    T_SSL_SPEC_SELF,
    T_SSL_SPEC_NULL,

    T_SSL_MAX_TOKEN         = 0xFFFF,
} tokens;

#define FIRST_CMDLN_DELIM     T_LT
#define LAST_CMDLN_DELIM      T_UNKNOWN
#define FIRST_SSL_ESCAPE_CHAR T_SSL_ESCAPE_CHARS

typedef struct {
    const char  *delims;
    const char  *keywords;
} token_table;
