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
* Description:  Semantic actions interface (used by yydriver).
*
****************************************************************************/


#ifndef SEMSTRUCT_INCLUDED
#define SEMSTRUCT_INCLUDED

#include "wresall.h"

/**** Semantic structures ****/
typedef struct FullMemFlags {
    ResMemFlags         flags;
    unsigned            loadOptGiven        : 1;
    unsigned            memOptGiven         : 1;
    unsigned            purityOptGiven      : 1;
} FullMemFlags;

typedef struct FullOptFlagsOS2 {
    ResMemFlags         flags;
    uint_32             codePage;
    unsigned            loadOptGiven        : 1;
    unsigned            memOptGiven         : 1;
    unsigned            purityOptGiven      : 1;
    unsigned            cpOptGiven          : 1;
} FullOptFlagsOS2;

typedef uint_32 SemOffset;
typedef uint_32 SemLength;

typedef struct ResLocation {
    SemOffset   start;
    SemLength   len;
} ResLocation;

typedef struct IntMask {
    uint_32     Mask;
    uint_32     Value;
    unsigned    longVal         : 1;
    unsigned    unsgVal         : 1;
} IntMask;

/**** include all the semantic structures and routine prototypes for each ****/
/**** type of structure ****/

#include "semaccel.h"
#include "semmenu.h"
#include "semraw.h"
#include "semdiag.h"
#include "semsingl.h"
#include "semresfl.h"
#include "semutil.h"

/**** Semantic global routines ****/
extern SemOffset            SemStartResource( void );
extern SemLength            SemEndResource( SemOffset start );
extern void                 SemAddResourceFree( WResID * name, WResID * type,
                                        ResMemFlags flags, ResLocation loc );
extern void                 SemAddResource( WResID * name, WResID * type,
                                        ResMemFlags, ResLocation );
extern void                 SemAddResource2( WResID * name, WResID * type,
                                        ResMemFlags flags, ResLocation loc,
                                        const char *filename );
extern void                 SemSetDefLang( void );
extern const WResLangType   *SemGetResourceLanguage( void );
extern void                 SemanticInitStatics( void );

#endif
