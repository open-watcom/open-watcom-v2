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
* Description:  Global wmake definitions.
*
****************************************************************************/


#ifndef _MAKE_H
#define _MAKE_H     1

#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "wnoret.h"
#include "mtypes.h"
#include "msysdep.h"
#include "massert.h"


#if defined( __WATCOMC__ ) && !defined( __UNIX__ )
#define USE_DIR_CACHE
#endif

#define CASESENSITIVE       true    /* Is Name case sensitive                   */
#define NOCASESENSITIVE     false   /* Is not Name case sensitive               */

#define LINECONT_C          '&'     /* line continuation                        */
#define UNIX_LINECONT_C     '\\'    /* UNIX line continuation                   */
#define MS_LINECONT_C       '\\'    /* MS line continuation                     */
#define COMMENT_C           '#'     /* beginning of comment                     */
#define BANG_C              '!'     /* preprocessor introducer                  */
#define ENVVAR_C            '%'     /* %environment-var                         */

#define NULLCHAR            '\0'

/*
 * When we initialize ourselves, this is how many objects we will preallocate
 * and chain into the free lists.
 */
#define STREAM_ALLOC_SENT       10
#define VECSTR_ALLOC_VECSTR     5

struct Glob {
    char        swchar;                 /* the 'switch' character (ie: '-' or '/' )      */

    boolbit     all             : 1;    /* make all targets                              */
    boolbit     block           : 1;    /* block the use of implicit rules               */
    boolbit     nocheck         : 1;    /* don't check for existence of files made       */
    boolbit     debug           : 1;    /* print debugging info                          */
    boolbit     erase           : 1;    /* on error delete the file without asking       */
    boolbit     noheader        : 1;    /* do not display the header                     */
    boolbit     ignore          : 1;    /* ignore error returns from commands            */
    boolbit     cont            : 1;    /* continue with other targets after error       */

    boolbit     noexec          : 1;    /* don't execute any commands                    */
    boolbit     optimize        : 1;    /* use circular path feature                     */
    boolbit     print           : 1;    /* print the dependency tree                     */
    boolbit     query           : 1;    /* query mode - check if target up to date       */
    boolbit     overide         : 1;    /* do not use any default rules                  */
    boolbit     silent          : 1;    /* do not print cmds                             */
    boolbit     silentno        : 1;    /* print cmds - overrules silent                 */
    boolbit     touch           : 1;    /* just touch the files, don't execute anything  */

    boolbit     hold            : 1;    /* on error don't erase (and don't ask)          */
    boolbit     shell           : 1;    /* execute commands through the shell            */
    boolbit     preproc         : 1;    /* true - preprocessor on, false - off           */
    boolbit     erroryet        : 1;    /* has PrtMsg printed an error yet?              */
    boolbit     nomakeinit      : 1;    /* true if we are to suppress parsing makeinit   */
    boolbit     macreadonly     : 1;    /* should macro definitions be read only?        */
    boolbit     headerout       : 1;    /* has the header been printed out yet?          */
    boolbit     rcs_make        : 1;    /* true if new target's date is max of dep dates */

    boolbit     fuzzy           : 1;    /* true .AUTODEPEND times can be off by 1 minute */
    boolbit     keep_spaces     : 1;    /* true keep spaces in macro defns               */
    boolbit     compat_unix     : 1;    /* true if UNIX compatibility desired            */
    boolbit     compat_posix    : 1;    /* true if POSIX conformance desired             */
    boolbit     compat_nmake    : 1;    /* Microsoft nmake Optioning-Compatability switch*/
    boolbit     verbose         : 1;    /* Bit to list out the contents of a tmp file    */
    boolbit     auto_depends    : 1;    /* force autodepends info to be used             */
    boolbit     show_offenders  : 1;    /* display the out-of-date file                  */

#ifdef CACHE_STATS
    boolbit     cachestat       : 1;    /* cache status report                           */
#endif
#ifdef USE_DIR_CACHE
    boolbit     cachedir        : 1;    /* true if dir caching turned on                 */
#endif
};

#ifdef DEVELOPMENT
#define STATIC
#else
#define STATIC static
#endif

extern struct Glob      Glob;
extern const char FAR   BuiltIns[];
extern const char FAR   MSBuiltIn[];
extern const char FAR   UNIXBuiltIn[];
extern const char FAR   POSIXBuiltIn[];
extern const char FAR   SuffixList[];
extern const char FAR   MSSuffixList[];
extern const char FAR   UNIXSuffixList[];
extern const char FAR   POSIXSuffixList[];

NO_RETURN( extern void ExitFatal( void ) );
NO_RETURN( extern void ExitError( void ) );
NO_RETURN( extern void ExitOK( void ) );

#endif
