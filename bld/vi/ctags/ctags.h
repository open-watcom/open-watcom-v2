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


#include <unistd.h>

typedef enum {
    FALSE = 0,
    TRUE
} bool;

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

/* global variables */
extern bool     WantTypedefs;
extern bool     WantMacros;
extern bool     WantProtos;
extern bool     WantAllDefines;
extern bool     WantUSE;
extern bool     WantEnums;
extern bool     WantClasses;
extern bool     VerboseFlag;
extern unsigned TagCount;

/*
 * function prototypes
 */
/* ctags.c */
bool    IsTokenChar( char ch );
int     MyStricmp( char **buf, char *literal );
void    ErrorMsgExit( const char *, ... );

/* file.c */
void    StartFile( char *fname );
void    EndFile( void );
void    NewFileLine( void );
int     GetChar( void );
void    UnGetChar( int ch );
void    RecordCurrentLineData( void );
char    *GetCurrentLineDataPtr( void );
char    *GetCurrentFileName( void );
bool    GetString( char *buff, int maxbuff );

/* scanc.c */
void    ScanC( void );

/* scanfor.c */
void    ScanFortran( void );

/* scanasm.c */
void    ScanAsm( void );

/* taglist.c */
void    AddTag( char *id );
void    GenerateTagsFile( char *fname );
int     CompareStrings( char **p1, char **p2 );
void    ReadExtraTags( char *fname );

