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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef WRES_INCLUDED
#define WRES_INCLUDED

#include "bool.h"
#include "filefmt.h"
#include "wressetr.h"

/***** Private Types *****/

typedef uint_16             WResLen;

typedef struct WResLangNode {
    struct WResLangNode     *Next;
    struct WResLangNode     *Prev;
    char                    *data; /* pointer for use by resource editors */
    WResLangInfo            Info;
    void                    *fileInfo;
} WResLangNode;

typedef struct WResResNode {        /* linked list of WResResInfo's */
    struct WResResNode      *Next;
    struct WResResNode      *Prev;
    WResLangNode            *Head;  /* list of resources of this type */
    WResLangNode            *Tail;  /* and name */
    WResResInfo             Info;
} WResResNode;

typedef struct WResTypeNode {       /* linked list of WResTypeInfo's */
    struct WResTypeNode     *Next;
    struct WResTypeNode     *Prev;
    WResResNode             *Head;   /* list of resources of this type */
    WResResNode             *Tail;
    WResTypeInfo            Info;
} WResTypeNode;

typedef struct WResDirHead {        /* head of the directory */
    uint_16                 NumResources;
    uint_16                 NumTypes;
    WResTypeNode            *Head;
    WResTypeNode            *Tail;
    uint_16                 TargetOS;
} WResDirHead, *WResDir;

/***** Exported types *****/
typedef struct WResDirWindow {
    struct WResResNode      *CurrRes;
    struct WResTypeNode     *CurrType;
    struct WResLangNode     *CurrLang;
} WResDirWindow;

/* macros to test condition for a WRes directory */

#define WResIsEmptyWindow( wind )       ((wind).CurrType == NULL || (wind).CurrRes == NULL || (wind).CurrLang == NULL)
#define WResIsFirstResOfType( wind )    ((wind).CurrRes == (wind).CurrType->Head && WResIsFirstLangOfRes( wind ))
#define WResIsFirstLangOfRes( wind )    ((wind).CurrLang == (wind).CurrRes->Head)

/* internal global data prototypes */

extern WResDir          MainDir;

/* function prototypes for WRes directory manipulation */

extern WResDir          WResInitDir( void );
extern bool             WResIsEmpty( WResDir );
extern void             WResFreeDir( WResDir );
extern bool             WResWriteDir( FILE *fp, WResDir );
extern bool             WResReadDir( FILE *fp, WResDir *dir, bool * );
extern bool             WResReadDir2( FILE *fp, WResDir *dir, bool *dup_discarded, void *fileinfo );
extern bool             WResAddResource( const WResID *type_id, const WResID *res_id, uint_16 memflags, long offset,
                                uint_32 length, WResDir dir, const WResLangType *lang, bool *duplicate );
extern bool             WResAddResource2( const WResID *type_id, const WResID *res_id, uint_16 memflags, long offset,
                                uint_32 length, WResDir dir, const WResLangType *lang, WResDirWindow *wind_dup,
                                void *fileinfo );
extern WResDirWindow    WResFindResource( const WResID *type_id, const WResID *res_id, WResDir, const WResLangType *lang );
extern void             WResRemoveResource( WResDir dir, const WResID *type_id, const WResID *res_id, const WResLangType *lang );
extern WResDirWindow    WResFirstResource( WResDir );
extern void             WResSetEmptyWindow( WResDirWindow *wind );
extern void             WResMakeWindow( WResDirWindow *wind, WResTypeNode *typenode, WResResNode *resnode, WResLangNode *langnode );
extern WResDirWindow    WResNextResource( WResDirWindow, WResDir );
extern bool             WResIsLastResource( WResDirWindow, WResDir );
extern bool             WResIsLastLangRes( WResDirWindow wind, WResDir dir );
extern WResResInfo      *WResGetResInfo( WResDirWindow );
extern WResTypeInfo     *WResGetTypeInfo( WResDirWindow );
extern WResLangInfo     *WResGetLangInfo( WResDirWindow );
extern void             *WResGetFileInfo( WResDirWindow wind );
extern void             WResDelResource( WResDir dir, const WResID *type_id, const WResID *res_id );

/* hidden but exported routines */

extern WResTypeNode     *__FindType( const WResID *__type_id, WResDir __dir );
extern WResResNode      *__FindRes( const WResID *__res_id, WResTypeNode *__typenode );
extern void             __FreeTypeList( WResDir __dir );
extern void             __FreeResList( WResTypeNode *typenode );
extern void             __FreeLangList( WResResNode *resnode );
extern WResLangNode     *__FindLang( const WResLangType *lang, WResResNode *resnode );

/* function prototypes for WRes file initialization */

extern bool             WResFileInit( FILE *fp );

#endif
