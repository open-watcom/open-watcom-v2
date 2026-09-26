/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


/*
 * NOTE: MESSAGETABLE resource uses different definition on OS/2 and on Windows
 *      need to get information about what is MS standard on Windows and what
 *      is IBM standard on OS/2
 */
#define RESOURCE_DEFAULT_FLAGS      (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE)
#define RCDATA_DEFAULT_FLAGS        (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE)
#define STRINGTABLE_DEFAULT_FLAGS   (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE)
#define ERRTABLE_DEFAULT_FLAGS      (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE)
#define MENU_DEFAULT_FLAGS          (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE)
#define DIALOG_DEFAULT_FLAGS        (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE)
#define HELPTABLE_DEFAULT_FLAGS     (RESFLAG_PURE | RESFLAG_MOVEABLE | RESFLAG_DISCARDABLE) /* OS/2 specific */
#define ACCEL_DEFAULT_FLAGS         (RESFLAG_PURE | RESFLAG_MOVEABLE)
#define VERSIONINFO_DEFAULT_FLAGS   (RESFLAG_PURE | RESFLAG_MOVEABLE)
#define TOOLBAR_DEFAULT_FLAGS       (RESFLAG_PURE | RESFLAG_MOVEABLE)                       /* Windows specific */

/**** Semantic structures ****/
typedef struct FullResFlags {
    ResMemFlags         res_flags;
    uint_32             codePage;
    boolbit             loadOptGiven        : 1;
    boolbit             memOptGiven         : 1;
    boolbit             purityOptGiven      : 1;
    boolbit             cpOptGiven          : 1;
} FullResFlags;

typedef uint_32 SemOffset;
typedef uint_32 SemLength;

typedef struct ResLocation {
    SemOffset   start;
    SemLength   len;
} ResLocation;

typedef struct IntMask {
    uint_32     Mask;
    uint_32     Value;
    boolbit     longVal :1;
    boolbit     unsgVal :1;
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
extern void                 SemAddResourceAndFree( WResID *res_id, WResID *type_id, ResMemFlags res_flags, ResLocation loc );
extern void                 SemAddResource( WResID *res_id, WResID *type_id, ResMemFlags res_flags, ResLocation );
extern void                 SemAddResource2( WResID *res_id, WResID *type_id, ResMemFlags res_flags, ResLocation loc, const char *filename );
extern void                 SemSetDefLang( void );
extern const WResLangType   *SemGetResourceLanguage( void );
extern void                 SemanticInitStatics( void );
extern void                 SemCheckResFlags( FullResFlags *fullflags, ResMemFlags loadopts, ResMemFlags memopts, ResMemFlags pureopts );

#endif
