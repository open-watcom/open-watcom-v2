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


#ifndef WRES_INCLUDED
#define WRES_INCLUDED

#include "bool.h"
#include "filefmt.h"
#include "wressetr.h"

/***** Private Types *****/

typedef struct WResLangNode {
    struct WResLangNode         *Next;
    struct WResLangNode         *Prev;
    void                        *data; /* pointer for use by resource editors */
    WResLangInfo                Info;
    void                        *fileInfo;
} WResLangNode;

typedef struct WResResNode {        /* linked list of WResResInfo's */
    struct WResResNode *    Next;
    struct WResResNode *    Prev;
    WResLangNode *          Head;   /* list of resources of this type */
    WResLangNode *          Tail;   /* and name */
    WResResInfo             Info;
} WResResNode;

typedef struct WResTypeNode {       /* linked list of WResTypeInfo's */
    struct WResTypeNode *   Next;
    struct WResTypeNode *   Prev;
    WResResNode *           Head;   /* list of resources of this type */
    WResResNode *           Tail;
    WResTypeInfo            Info;
} WResTypeNode;

typedef struct WResDirHead {        /* head of the directory */
    uint_16                 NumResources;
    uint_16                 NumTypes;
    WResTypeNode *          Head;
    WResTypeNode *          Tail;
    WResTargetOS            TargetOS;
} WResDirHead;

/***** Exported types *****/
typedef struct WResDirWindow {
    struct WResResNode          *CurrRes;
    struct WResTypeNode         *CurrType;
    struct WResLangNode         *CurrLang;
} WResDirWindow;

typedef struct WResDirHead      *WResDir;

/* macros to test condition for a WRes directory */
#define WResDirInitError( s )           ((s) == NULL)
#define WResIsEmptyWindow( wind )       ( (wind).CurrType == NULL ||     \
                                        (wind).CurrRes == NULL \
                                        || (wind).CurrLang == NULL )
#define WResGetNumTypes( dir )          ((dir)->NumTypes)
#define WResGetNumResources( dir )      ((dir)->NumResources)
#define WResGetTargetOS( dir )          ((dir)->TargetOS)
#define WResSetTargetOS( dir, os )      (dir)->TargetOS = (os)
#define WResIsFirstResOfType( wind )    ((wind).CurrRes == \
                                        (wind).CurrType->Head &&\
                                        WResIsFirstLangOfRes( wind ) )
#define WResIsFirstLangOfRes( wind )    ((wind).CurrLang == \
                                        (wind).CurrRes->Head )

/* function prototypes for WRes directory manipulation */
extern WResDir          WResInitDir( void );
extern bool             WResIsEmpty( WResDir );
extern void             WResFreeDir( WResDir );
extern bool             WResWriteDir( WResFileID, WResDir );
extern bool             WResReadDir( WResFileID, WResDir, bool * );
extern bool             WResReadDir2( WResFileID handle, WResDir currdir, bool *dup_discarded, void *fileinfo );
extern bool             WResAddResource( const WResID *type, const WResID *name,
                                uint_16 memflags, long offset, uint_32 length,
                                WResDir, const WResLangType *lang, bool *duplicate );
extern bool             WResAddResource2( const WResID *type, const WResID *name,
                                uint_16 memflags, long offset, uint_32 length,
                                WResDir currdir, const WResLangType *lang,
                                WResDirWindow *duplicate, void *fileinfo );
extern WResDirWindow    WResFindResource( const WResID *type, const WResID *name, WResDir, const WResLangType *lang );
extern void             WResRemoveResource( WResDir currdir, const WResID *type, const WResID *name, const WResLangType *lang );
extern WResDirWindow    WResFirstResource( WResDir );
extern void             WResSetEmptyWindow( WResDirWindow *wind );
extern void             WResMakeWindow( WResDirWindow *wind, WResTypeNode *type, WResResNode *res, WResLangNode *lang );
extern WResDirWindow    WResNextResource( WResDirWindow, WResDir );
extern bool             WResIsLastResource( WResDirWindow, WResDir );
extern bool             WResIsLastLangRes( WResDirWindow currwind, WResDir currdir );
extern WResResInfo      *WResGetResInfo( WResDirWindow );
extern WResTypeInfo     *WResGetTypeInfo( WResDirWindow );
extern WResLangInfo     *WResGetLangInfo( WResDirWindow );
extern void             *WResGetFileInfo( WResDirWindow currwind );
extern void             WResDelResource( WResDir currdir, const WResID *type, const WResID *name );

/* hidden but exported routines */

extern WResTypeNode     *__FindType( const WResID * __type, WResDir __currdir );
extern WResResNode      *__FindRes( const WResID * __name, WResTypeNode * __currtype );
extern void             __FreeTypeList( WResDirHead * __currdir );
extern void             __FreeResList( WResTypeNode *currtype );
extern void             __FreeLangList( WResResNode *curres );
extern WResLangNode     *__FindLang( const WResLangType *lang, WResResNode *curres );

#endif
