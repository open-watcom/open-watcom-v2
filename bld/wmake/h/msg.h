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


/*
 *  This file uses some pre-processor magic using the pick() macro.  If
 *  a file just #include "msg.h", then it gets the enumerated type for
 *  messages.  It can also get the text by redefining pick and #including
 *  again.  See msg.c for example.
 */
#if defined(pick)
#include "banner.h"
#endif

#if !defined(pick)

#define YES_CHAR    'Y' /* must be upper case */

#include "mtypes.h"

enum MsgClass {
    NUM_MSK     = 0x00ff,   /* these are valid msg numbers          */

    INF         = 0x0000,   /* an inform message - always printed   */
    WRN         = 0x0100,   /* Warning(Wnn): msg                    */
    ERR         = 0x0200,   /* Error(Enn): msg                      */
    FTL         = 0x0300,   /* Error(Fnn): msg - aborts execution   */
    CLASS_MSK   = 0x0f00,


    PRNTSTR     = 0x1000,   /* print first arg as a string. used for*/
                                /* printing ANYTHING greater than 256   */
    LOC         = 0x2000,   /* print file and line info if possible */
    NEOL        = 0x4000,   /* suppress the automatic end of line   */
    FLAG_MSG    = 0xf000,

    DBG         = 0x8000,   /* only if Glob.debug                   */
};


extern size_t FmtStr( char *buf, const char *fmt, ... );
extern void PrtMsg( enum MsgClass num, ... );
extern void Usage( void );
extern BOOLEAN GetYes( enum MsgClass querymsg );
extern void LogInit( const char *logname );
extern void LogFini( void );

#define pick( name, string ) name

enum {
#endif  /* !defined(pick) */

/*
 * See msg.c for a description of the % options.
 *
 * Be sure to keep these strings, including the substitutions under 250 bytes
 *
 * These strings are the banner and messages for the debugging version, so
 * they are here and not in the resource file.
 */

/* banner */

pick( BANNER = END_OF_RESOURCE_MSG,
    banner1("Watcom Make",_WMAKE_VERSION_) "\n" banner2("1988") "\n" banner3 ),

/*
 * All messages beyond here appear only under certain conditions in debugging
 * versions of WMake...
 */

#ifndef NDEBUG
pick( ASSERTION_FAILED,         "Assertion %E in %s(%d) failed" ),
#endif

#ifdef TRACK
pick( UNABLE_TO_TRACK,          "Unable to track memory!" ),
pick( HEAP_IS_DAMAGED,          "%s heap is damaged" ),
pick( BAD_NODE_IN_HEAP,         "Bad node in %s heap" ),
#endif

#ifdef DEVELOPMENT
pick( INVALID_TOKEN_IN,         "Invalid token 0x%x in %s" ),
pick( INTERPRETING,             "Interpreting %s" ),
pick( CHUNKS_UNFREED,           "%d chunks unfreed" ),
#endif

#ifdef CACHE_STATS
/* note %l is only available if CACHE_STATS is defined */
pick( CACHING_DIRECTORY,        "Caching Directory %E..." ),
pick( CACHERELEASE,             "Releasing Directory Cache" ),
pick( CACHE_FILES_BYTES,        "\t%l files, %l bytes, %l hits" ),
pick( HIT_ON_HASH,              "\thit %d" ),
pick( CACHE_FREED_BYTES,        "\t%l bytes freed" ),
pick( CACHE_MEM,                "\tNot enough memory to cache directory" ),
#endif

pick( MSG_MAX,                  NULL ),
};
