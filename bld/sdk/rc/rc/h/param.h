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


#ifndef PARAM_INCLUDED
#define PARAM_INCLUDED

#include <stdlib.h>
#include "types.h"
#ifdef UNIX
    #include "clibext.h"
#endif

#define DB_CHAR         1

#define DB_NONE                 0
#define DB_KANJI                1
#define DB_TRADITIONAL_CHINESE  2
#define DB_WANSUNG_KOREAN       3
#define DB_SIMPLIFIED_CHINESE   4

typedef struct ExtraRes {
    struct ExtraRes     *next;
    char                 name[_MAX_PATH];
}ExtraRes;

typedef struct EnvVarInfo {
    struct EnvVarInfo   *next;
    char                **argv; /* points into buf */
    char                buf[1]; /* dynamic array */
} EnvVarInfo;

typedef struct FRStrings {
    struct FRStrings    *next;
    char                *findString; /* points to the string to find */
    char                *replaceString; /* points to the string to replace */
    char                buf[1]; /* dynamic buffer contains both strings */
}FRStrings;

struct RCParams {
#ifdef SCANDEBUG
    int     DebugScanner : 1;
#endif
#ifdef YYDEBUG
    int     DebugParser     : 1;
#endif
    int     PrintHelp       : 1;
    int     Quiet           : 1;
    int     Pass1Only       : 1;
    int     Pass2Only       : 1;
    int     NoResFile       : 1;    /* no RES file to merge in pass2 */
    int     IgnoreINCLUDE   : 1;
    int     IgnoreCWD       : 1;
    int     MSResFormat     : 1;
    int     PrivateDLL      : 1;    /* the next 5 option are use to set bits */
    int     GlobalMemEMS    : 1;    /* in the os2_exe_header.info field */
    int     EMSInstance     : 1;
    int     EMSDirect       : 1;
    int     ProtModeOnly    : 1;
    int     PreprocessOnly  : 1;
    int     WritableRes     : 1;
    int     NoProtectCC     : 1;    /* if set, don't invoke prot. mode comp */
    int     NoPreprocess    : 1;    /* if set won't attemp any preprocessing */
    int     GenAutoDep      : 1;    /* generate autodependency info for wmake */
    int     FindAndReplace;         /* a check to see whether for this option */
    int     Prepend;
    unsigned DBCharSupport  : 3;    /* which of the zk switches is set */
    int     SegmentSorting;         /* which segment sorting method to use */
    int     TargetOS;
    char    InFileName[ _MAX_PATH ];
    char    InExeFileName[ _MAX_PATH ];
    char    OutResFileName[ _MAX_PATH ];
    char    OutExeFileName[ _MAX_PATH ];
    char    CodePageFile[ _MAX_PATH ];
    char    PrependString[ _MAX_PATH ];
    char ** CPPArgs;    /* temporary until preprocessing done inline */
    int     VersionStamp;
    EnvVarInfo  *EnvVariables;
    ExtraRes    *ExtraResFiles;
    FRStrings   *FindReplaceStrings;
};

#define VERSION_30_STAMP 0x0300
#define VERSION_31_STAMP 0x030a     /* This is what the MS rc uses */

enum SegmentSortMethods {
    SEG_SORT_NONE,
    SEG_SORT_PRELOAD_ONLY,
    SEG_SORT_MANY           /* preload, data, and non-discardable */
};

enum RCTargetOS {
    RC_TARGET_OS_WIN16,
    RC_TARGET_OS_WIN32
};

extern bool ScanParams( int argc, char * argv[] );
extern void RcAddCPPArg( char * newarg );
extern void ScanParamShutdown( void );
extern unsigned ParseEnvVar( const char *env, char **argv, char *buf );
extern void AddNewIncludeDirs( const char * arg );

#endif
