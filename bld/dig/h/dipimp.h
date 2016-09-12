/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Interface functions exported and imported by DIPs.
*
****************************************************************************/


#ifndef DIPIMP_H_INCLUDED
#define DIPIMP_H_INCLUDED

#define DIP_MAJOR       1
#define DIP_MINOR       3
#define DIP_MINOR_OLD   0

#define MH2IMH( mh )    ((mh)&0x0000FFFF)
#define IMH2MH( imh )   (imh)

/*
    An imp_mod_handle is defined as an unsigned_16. The value zero is
    reserved to indicate "no module".
*/
#define IMH_NOMOD       ((imp_mod_handle)0)
#define IMH_BASE        ((imp_mod_handle)1)
#define IMH_GBL         ((imp_mod_handle)-1)

#define DIPImp(n)       DIPImp ## n
#define _DIPImp(n)      _DIPImp ## n n

#define DIPIMPENTRY(n)  DIGENTRY DIPImp( n )

struct imp_image_handle;
struct imp_type_handle;
struct imp_cue_handle;
struct imp_sym_handle;

typedef struct imp_image_handle imp_image_handle;
typedef struct imp_type_handle  imp_type_handle;
typedef struct imp_cue_handle   imp_cue_handle;
typedef struct imp_sym_handle   imp_sym_handle;

#include "dipcli.h"

typedef unsigned_16     imp_mod_handle;
typedef unsigned_16     image_index;

typedef walk_result (DIGCLIENT DIP_IMP_MOD_WALKER)( imp_image_handle *, imp_mod_handle, void * );
typedef walk_result (DIGCLIENT DIP_IMP_TYPE_WALKER)( imp_image_handle *, imp_type_handle *, void * );
typedef walk_result (DIGCLIENT DIP_IMP_SYM_WALKER)( imp_image_handle *, sym_walk_info, imp_sym_handle *, void * );
typedef walk_result (DIGCLIENT DIP_IMP_CUE_WALKER)( imp_image_handle *, imp_cue_handle *, void * );

#define pick(r,n,p) typedef r (DIGENTRY *_DIPImp ## n) p;
#include "_dipimp.h"
#undef pick

#define pick(r,n,p) extern r DIPIMPENTRY( n ) p;
#include "_dipimp.h"
#undef pick

extern dip_status   DIPIMPENTRY( Startup )( void );

extern const char   DIPImp( Name )[];

#include "digpck.h"

struct dip_imp_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         dip_priority;
    const char          *dip_name;

    _DIPImp( HandleSize );
    _DIPImp( MoreMem );
    _DIPImp( Shutdown );
    _DIPImp( Cancel );
    _DIPImp( LoadInfo );
    _DIPImp( MapInfo );
    _DIPImp( UnloadInfo );
    _DIPImp( WalkModList );
    _DIPImp( ModName );
    _DIPImp( ModSrcLang );
    _DIPImp( ModInfo );
    _DIPImp( ModDefault );
    _DIPImp( AddrMod );
    _DIPImp( ModAddr );
    _DIPImp( WalkTypeList );
    _DIPImp( TypeMod );
    _DIPImp( TypeInfo );
    _DIPImp( OldTypeBase );
    _DIPImp( TypeArrayInfo );
    _DIPImp( TypeProcInfo );
    _DIPImp( TypePtrAddrSpace );
    _DIPImp( TypeThunkAdjust );
    _DIPImp( TypeCmp );
    _DIPImp( TypeName );
    _DIPImp( WalkSymList );
    _DIPImp( SymMod );
    _DIPImp( SymName );
    _DIPImp( SymType );
    _DIPImp( SymLocation );
    _DIPImp( SymValue );
    _DIPImp( SymInfo );
    _DIPImp( SymParmLocation );
    _DIPImp( SymObjType );
    _DIPImp( SymObjLocation );
    _DIPImp( AddrSym );
    _DIPImp( LookupSym );
    _DIPImp( AddrScope );
    _DIPImp( ScopeOuter );
    _DIPImp( SymCmp );
    _DIPImp( WalkFileList );
    _DIPImp( CueMod );
    _DIPImp( CueFile );
    _DIPImp( CueFileId );
    _DIPImp( CueAdjust );
    _DIPImp( CueLine );
    _DIPImp( CueColumn );
    _DIPImp( CueAddr );
    _DIPImp( LineCue );
    _DIPImp( AddrCue );
    _DIPImp( CueCmp );
    _DIPImp( TypeBase );
    _DIPImp( TypeAddRef );
    _DIPImp( TypeRelease );
    _DIPImp( TypeFreeAll );
    _DIPImp( SymAddRef );
    _DIPImp( SymRelease );
    _DIPImp( SymFreeAll );
    _DIPImp( WalkSymListEx );
    _DIPImp( LookupSymEx );
};

typedef struct dip_client_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         sizeof_struct;

    _DIGCli( Alloc );
    _DIGCli( Realloc );
    _DIGCli( Free );

    _DIPCli( MapAddr );
    _DIPCli( SymCreate );
    _DIPCli( ItemLocation );
    _DIPCli( AssignLocation );
    _DIPCli( SameAddrSpace );
    _DIPCli( AddrSection );

    _DIGCli( Open );
    _DIGCli( Seek );
    _DIGCli( Read );
    _DIGCli( Write );
    _DIGCli( Close );
    _DIGCli( Remove );

    _DIPCli( Status );
    _DIPCli( CurrMAD );

    _DIGCli( MachineData );

} dip_client_routines;

#include "digunpck.h"

typedef dip_imp_routines * DIGENTRY dip_init_func( dip_status *status, dip_client_routines *client );
#ifdef __WINDOWS__
typedef void DIGENTRY dip_fini_func( void );
#endif

DIG_DLLEXPORT dip_init_func DIPLOAD;
#ifdef __WINDOWS__
DIG_DLLEXPORT dip_fini_func DIPUNLOAD;
#endif

#define DC(n)       DC ## n

#define pick(r,n,p) extern r DC( n ) p;
#include "_digcli.h"
#include "_dipcli.h"
#undef pick

extern void         *DC( AllocZ )( size_t amount );
extern dip_status   DC( ReadAt )( dig_fhandle dfh, void *b, size_t s, unsigned long p );

#endif
