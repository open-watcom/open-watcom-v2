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


#ifndef GLOBAL_CONSTANTS_INCLUDED
#define GLOBAL_CONSTANTS_INCLUDED

#if !defined(__WINDOWS__) && !defined(__NT__)
typedef enum {
    TRUE = ( 1==1 ),
    FALSE = ( 1==2 )
} BOOL;
#else
#include <windows.h>
#endif

#define WIG_HEADER "/* WATCOM Interface Generator   Version 1.0 */\n"
#define WIG_BANNER "\
WATCOM Interface Generator   Version 1.0 Limited Availability\n\
Copyright by WATCOM International Corp. 1994. All rights reserved.\n\
WATCOM is a trademark of WATCOM International Corp.\n\
"

#define HASH_PRIME              257

#endif
