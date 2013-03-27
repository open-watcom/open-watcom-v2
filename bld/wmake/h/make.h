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
* Description:  Global wmake definitions.
*
****************************************************************************/


#ifndef _MAKE_H
#define _MAKE_H     1
#include "msysdep.h"
#include "mtypes.h"

#define DOLLAR          '$'     /* macro introducer                          */
// #define DOLLAR_S        "$"
#define LINECONT        '&'     /* line continuation                         */
#define UNIX_LINECONT   '\\'    /* UNIX line continuation                    */
#define MS_LINECONT     '\\'    /* MS line continuation                      */
#define COMMENT         '#'     /* beginning of comment                      */
#define COLON           ':'     /* target, dependants seperator              */
#define DOT             '.'     /* beginning of a suffix/extension           */
#define BANG            '!'     /* preprocessor introducer                   */
#define ENVVAR          '%'     /* %environment-var                          */
#define SEMI            ';'     /* dependent/cmd seperator                   */
#define L_CURL_PAREN    '{'
#define R_CURL_PAREN    '}'

#define NULLCHAR        '\0'
#define TAB             '\t'
#define EOL             '\n'
#define SPACE           ' '

/*
 * When we initialize ourselves, this is how many objects we will preallocate
 * and chain into the free lists.
 */
#define STREAM_ALLOC_SENT       10
#define VECSTR_ALLOC_VECSTR     5

struct Glob {
    char    swchar;     /* the 'switch' character (ie: '-' or '/' )          */

    BIT     all         : 1;/* make all targets                              */
    BIT     block       : 1;/* block the use of implicit rules               */
    BIT     nocheck     : 1;/* don't check for existence of files made       */
    BIT     debug       : 1;/* print debugging info                          */
    BIT     erase       : 1;/* on error delete the file without asking       */
    BIT     noheader    : 1;/* do not display the header                     */
    BIT     ignore      : 1;/* ignore error returns from commands            */
    BIT     cont        : 1;/* continue with other targets after error       */

    BIT     noexec      : 1;/* don't execute any commands                    */
    BIT     optimize    : 1;/* use circular path feature                     */
    BIT     print       : 1;/* print the dependency tree                     */
    BIT     query       : 1;/* query mode - check if target up to date       */
    BIT     overide     : 1;/* do not use any default rules                  */
    BIT     silent      : 1;/* do not print cmds                             */
    BIT     silentno    : 1;/* print cmds - overrules silent                 */
    BIT     touch       : 1;/* just touch the files, don't execute anything  */

    BIT     hold        : 1;/* on error don't erase (and don't ask)          */
    BIT     shell       : 1;/* execute commands through the shell            */
    BIT     preproc     : 1;/* TRUE - preprocessor on, FALSE - off           */
    BIT     erroryet    : 1;/* has PrtMsg printed an error yet?              */
    BIT     cachedir    : 1;/* TRUE if dir caching turned on                 */
    BIT     nomakeinit  : 1;/* TRUE if we are to suppress parsing makeinit   */
    BIT     macreadonly : 1;/* should macro definitions be read only?        */
    BIT     headerout   : 1;/* has the header been printed out yet?          */

    BIT     rcs_make    : 1;/* TRUE if new target's date is max of dep dates */
    BIT     fuzzy       : 1;/* TRUE .AUTODEPEND times can be off by 1 minute */
    BIT     keep_spaces : 1;/* TRUE keep spaces in macro defns               */
    BIT     compat_unix : 1;/* TRUE if UNIX compatibility desired            */
    BIT     compat_posix: 1;/* TRUE if POSIX conformance desired             */
    BIT     compat_nmake: 1;/* Microsoft nmake Optioning-Compatability switch*/
    BIT     verbose     : 1;/* Bit to list out the contents of a tmp file    */
    BIT     auto_depends: 1;/* force autodepends info to be used             */

    BIT     show_offenders: 1;   /* display the out-of-date file             */
#ifdef CACHE_STATS
    BIT     cachestat   : 1;/* cache status report                           */
#endif
};

#ifdef DEVELOPMENT
#define STATIC
#else
#define STATIC static
#endif

extern struct Glob      Glob;
extern const char FAR   *BuiltIns;
extern const char FAR   *MSBuiltIn;
extern const char FAR   *UNIXBuiltIn;
extern const char FAR   *POSIXBuiltIn;
extern const char FAR   *SuffixList;
extern const char FAR   *MSSuffixList;
extern const char FAR   *UNIXSuffixList;
extern const char FAR   *POSIXSuffixList;

extern int  ExitSafe( int rc );
extern void Header( void );

#endif
