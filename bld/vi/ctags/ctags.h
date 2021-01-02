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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wio.h"
#include "bool.h"

typedef enum {
    TYPE_C,
    TYPE_FORTRAN,
    TYPE_PASCAL,
    TYPE_CPLUSPLUS,
    TYPE_ASM,
    TYPE_NONE
} file_type;

#define MAX_STR                 512
#define MAX_LINE                150
#define MAX_STRUCT_DEPTH        64

#define SKIP_DIGITS(s)          while( isdigit( *s ) ) s++
#define SKIP_SPACES(s)          while( isspace( *s ) ) s++
#define SKIP_NOSPACES(s)        while( !isspace( *s ) && *s != '\0' ) s++
#define SKIP_LIST_WS(s)         while( isspace( *s ) || *s == ',' ) s++
#define SKIP_LIST_NOWS(s)       while( !isspace( *s ) && *s != ',' && *s != '\0' ) s++
#define SKIP_CHAR_SPACES(s)     while( isspace( *++s ) )

/* global variables */
extern bool     WantTypedefs;
extern bool     WantMacros;
extern bool     WantProtos;
extern bool     WantAllDefines;
extern bool     WantUSE;
extern bool     WantEnums;
extern bool     WantClasses;
extern bool     VerboseFlag;

/*
 * function prototypes
 */
/* ctags.c */
extern bool     IsTokenChar( int ch );
extern int      MyStricmp( char **buf, char *literal );
extern void     ErrorMsgExit( const char *, ... );

/* file.c */
extern void     StartFile( const char *fname );
extern void     EndFile( void );
extern void     NewFileLine( void );
extern int      GetChar( void );
extern void     UnGetChar( int ch );
extern void     RecordCurrentLineData( void );
extern char     *GetCurrentLineDataPtr( void );
extern const char *GetCurrentFileName( void );
extern bool     GetString( char *buff, int maxbuff );

/* scanc.c */
extern void     ScanC( void );

/* scanfor.c */
extern void     ScanFortran( void );

/* scanasm.c */
extern void     ScanAsm( void );

/* taglist.c */
extern void     AddTag( const char *id );
extern void     GenerateTagsFile( const char *fname );
extern void     ReadExtraTags( const char *fname );
extern unsigned GetTagCount( void );
