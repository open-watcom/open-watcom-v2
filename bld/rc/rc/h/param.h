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
* Description:  command line parameter handling interfaces (OS/2)
*
****************************************************************************/

#ifndef PARAM_INCLUDED
#define PARAM_INCLUDED

#include "rctypes.h"

#define DB_CHAR                 1

#define MB_NONE                 0
#define DB_KANJI                1
#define DB_TRADITIONAL_CHINESE  2
#define DB_WANSUNG_KOREAN       3
#define DB_SIMPLIFIED_CHINESE   4
#define MB_UTF8                 5

typedef struct FRStrings {
    struct FRStrings    *next;
    char                *findString; /* points to the string to find */
    char                *replaceString; /* points to the string to replace */
    char                buf[1]; /* dynamic buffer contains both strings */
}FRStrings;

struct RCParams {
#ifdef SCANDEBUG
    unsigned    DebugScanner    : 1;
#endif
#ifdef YYDEBUG
    unsigned    DebugParser     : 1;
#endif
    unsigned    PrintHelp       : 1;
    unsigned    Quiet           : 1;
    unsigned    Pass1Only       : 1;
    unsigned    Pass2Only       : 1;
    unsigned    NoResFile       : 1;    /* no RES file to merge in pass2 */
    unsigned    IgnoreINCLUDE   : 1;
    unsigned    IgnoreCWD       : 1;
    unsigned    NoTargetDefine  : 1;
    unsigned    MSResFormat     : 1;
    unsigned    PrivateDLL      : 1;    /* the next 5 option are use to set bits */
    unsigned    GlobalMemEMS    : 1;    /* in the os2_exe_header.info field */
    unsigned    EMSInstance     : 1;
    unsigned    EMSDirect       : 1;
    unsigned    ProtModeOnly    : 1;
    unsigned    PreprocessOnly  : 1;
    unsigned    WritableRes     : 1;
    unsigned    VersionStamp30  : 1;
    unsigned    NoProtectCC     : 1;    /* if set, don't invoke prot. mode comp */
    unsigned    NoPreprocess    : 1;    /* if set won't attemp any preprocessing */
    unsigned    GenAutoDep      : 1;    /* generate autodependency info for wmake */
    unsigned    FindAndReplace  : 1;    /* a check to see whether for this option */
    unsigned    Prepend         : 1;
    unsigned    MBCharSupport   : 3;    /* which of the zk switches is set */
    unsigned    SegmentSorting  : 2;    /* which segment sorting method to use */
    unsigned    TargetOS        : 2;
    char        InFileName[_MAX_PATH];
    char        InExeFileName[_MAX_PATH];
    char        OutResFileName[_MAX_PATH];
    char        OutExeFileName[_MAX_PATH];
    char        CodePageFile[_MAX_PATH];
    char        PrependString[_MAX_PATH];
    char        **CPPArgs;    /* temporary until preprocessing done inline */
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
    RC_TARGET_OS_WIN32,
    RC_TARGET_OS_OS2,
};

extern bool ScanParams( int argc, char * argv[] );
extern void RcAddCPPArg( char * newarg );
extern void ScanParamShutdown( void );
extern int  ParseEnvVar( const char *env, char **argv, char *buf );
extern void SetMBRange( unsigned from, unsigned to, char data );
extern char *FindAndReplace( char *stringFromFile, FRStrings *frStrings );

#endif
