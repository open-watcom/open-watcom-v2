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
* Description:  WMAKE message output related defines.
*
****************************************************************************/


/*
 * See msg.c for a description of the % options.
 *
 * Be sure to keep these strings, including the substitutions under 250 bytes
 *
 * These strings are the banner and messages for the debugging version, so
 * they are here and not in the resource file.
 */

/* banner */

pick( BANNER, banner1w( "Make", _WMAKE_VERSION_ ) "\n" banner2 "\n" banner2a( "1988" ) "\n" banner3 "\n" banner3a )

/*
 * All messages beyond here appear only under certain conditions in debugging
 * versions of WMake...
 */

#ifndef NDEBUG
pick( ASSERTION_FAILED,         "Assertion %E in %s(%d) failed" )
#endif

#ifdef DEVELOPMENT
pick( INVALID_TOKEN_IN,         "Invalid token 0x%x in %s" )
pick( INTERPRETING,             "Interpreting %s" )
pick( CHUNKS_UNFREED,           "%d chunks unfreed" )
#endif

#ifdef CACHE_STATS
/* note %l is only available if CACHE_STATS is defined */
pick( CACHING_DIRECTORY,        "Caching Directory %E..." )
pick( CACHERELEASE,             "Releasing Directory Cache" )
pick( CACHE_FILES_BYTES,        "\t%l files, %l bytes, %l hits" )
pick( HIT_ON_HASH,              "\thit %d" )
pick( CACHE_FREED_BYTES,        "\t%l bytes freed" )
pick( CACHE_MEM,                "\tNot enough memory to cache directory" )
#endif
