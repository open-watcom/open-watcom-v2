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


#ifndef _SSTYLE_INCLUDED
#define _SSTYLE_INCLUDED


/*----- INCLUDES -----*/
#include "source.h"
#include "limits.h"


/*----- CONSTANTS -----*/
#define BEYOND_TEXT     ( SHRT_MAX )
#define MAX_SS_BLOCKS   200

enum syntax_elements {
    SE_UNPARSED = -2,   // basically used to flag problems
    SE_UNUSED = -1,     // use to temporarily setup a style
    SE_TEXT = 0,        // always first
    SE_WHITESPACE,      // don't mess with order (fonts in .cfg parallel #s)
    SE_SELECTION,
    SE_EOFTEXT,
    SE_KEYWORD,
    SE_OCTAL,
    SE_HEX,
    SE_INTEGER,
    SE_CHAR,
    SE_PREPROCESSOR,
    SE_SYMBOL,
    SE_INVALIDTEXT,
    SE_IDENTIFIER,
    SE_JUMPLABEL,
    SE_COMMENT,
    SE_FLOAT,
    SE_STRING,
    SE_NUMTYPES,        // always last
};


/*----- STRUCTURES -----*/
typedef struct ss_block {
    signed char type;
    short       end;
    short       len;
#ifdef __WIN__
    // offset of start of following block
    int         offset;
#endif
} ss_block;

typedef struct ss_flags_c {
    char inCComment:1;
    char inCPPComment:1;
    char inString:1;
    char inPreprocessor:1;
    char spare:4;
} ss_flags_c;

typedef struct ss_flags_f {
    char inString:1;
    char spare:7;
} ss_flags_f;

typedef union ss_flags {
    ss_flags_c  c;
    ss_flags_f  f;
} ss_flags;

/*----- EXPORTS -----*/
extern type_style   SEType[ SE_NUMTYPES ];


/*----- PROTOTYPES -----*/
void        SSInitLanguageFlags( linenum );
void        SSInitLanguageFlagsGivenValues( ss_flags * );
void        SSGetLanguageFlags( ss_flags * );
bool        SSKillsFlags( char );
void        SSDifBlock( ss_block *, char *, int, line *, linenum, int * );
ss_block    *SSNewBlock( void );
void        SSKillBlock( ss_block * );
int         SSGetStyle( int, int );
void        SSInitBeforeConfig( void );
void        SSInitAfterConfig( void );
void        SSFini( void );

#endif
