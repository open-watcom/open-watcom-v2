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


#ifndef FNUTILS_H_INCLUDED
#define FNUTILS_H_INCLUDED

#include <ctype.h>
#include <string.h>

/*
    If this environment variable is set, then filenames will not be
    lowered.
*/
#define POSIX_PRESERVE_CASE     "POSIX_PRESERVE_CASE"

char    *FNameLower( char *__filename );
int     FNameCompare( const char *__a, const char *__b );
int     FNameCompareN( const char *__a, const char *__b, size_t __len );
int     FNameCharCmp( char __a, char __b );

#define FNameCompare                    stricmp
#define FNameCompareN                   strnicmp
#define FNameCharCmp( __a, __b )        ( tolower(__a) - tolower(__b) )

int     IsDotOrDotDot( const char *__fname );

#endif
