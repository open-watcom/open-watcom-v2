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


#include "helpio.h"
#include "search.h"
#include "helpchar.h"

#define HELP_NO_MEM      (-4)
#define HELP_ERROR       (-3)
#define HELP_NO_VOPEN    (-2)
#define HELP_NO_FILE     (-1)
#define HELP_OK          (0)
#define HELP_NO_SUBJECT  (1)

#define H_MAX_CHARS  35
#define H_MAX_WORDS  80

#define ishelpchr( ch )         (isalnum(ch) || ch=='-')

#define EV_HELP EV_FUNC( 1 )

enum {
        E_UP = EV_FIRST_UNUSED,
        E_DOWN
};

typedef enum {
    HELPLANG_FRENCH,
    HELPLANG_ENGLISH
}HelpLangType;

typedef enum {
    SRCHTYPE_ENV,
    SRCHTYPE_PATH,
    SRCHTYPE_EOL
} HelpSrchItemType;

typedef struct {
    char        *buf;
    int         pos;
    bool        changecurr;
    int         line;
} ScanInfo;

#include "pushpck1.h"

typedef struct helpsrch {
    HelpSrchItemType    type;
    char                *info;
}HelpSrchPathItem;

typedef struct help_file {
    char                *name;
    HelpFp              f;
    HelpHdl             searchhdl;
}help_file;

#include "poppck.h"

#define MAX_HELP_FILES  10
extern help_file HelpFiles[];

int helpinit( char **helpfilenames, HelpSrchPathItem *srchlist );
int help_reinit( char **helpfilenames );
void helpfini( void );
void Free_Stack( void );
//int showhelp( char **helptopic, char *filename, EVENT (*rtn)( EVENT ),
//            HelpLangType lang, bool first );
int showhelp( char *helptopic, EVENT (*rtn)( EVENT ), HelpLangType lang );
