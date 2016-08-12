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
* Description:  Interface functions exported and imported by DIPs.
*
****************************************************************************/


#ifndef DIPIMP_H_INCLUDED
#define DIPIMP_H_INCLUDED

#include "digpck.h"

struct imp_image_handle;
struct imp_type_handle;
struct imp_cue_handle;
struct imp_sym_handle;

typedef struct imp_image_handle imp_image_handle;
typedef struct imp_type_handle  imp_type_handle;
typedef struct imp_cue_handle   imp_cue_handle;
typedef struct imp_sym_handle   imp_sym_handle;

typedef unsigned_16     imp_mod_handle;
typedef unsigned_16     image_index;

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

typedef walk_result (DIGCLIENT IMP_MOD_WKR)( imp_image_handle *, imp_mod_handle, void * );
typedef walk_result (DIGCLIENT IMP_TYPE_WKR)( imp_image_handle *, imp_type_handle *, void * );
typedef walk_result (DIGCLIENT IMP_SYM_WKR)( imp_image_handle *, sym_walk_info, imp_sym_handle *, void * );
typedef walk_result (DIGCLIENT IMP_CUE_WKR)( imp_image_handle *, imp_cue_handle *, void * );

struct dip_imp_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         dip_priority;
    const char          *dip_name;

    unsigned            (DIGENTRY *QueryHandleSize)( handle_kind );
    dip_status          (DIGENTRY *MoreMem)( unsigned );
    void                (DIGENTRY *Shutdown)( void );
    void                (DIGENTRY *Cancel)(void);

    dip_status          (DIGENTRY *LoadInfo)( dig_fhandle, imp_image_handle * );
    void                (DIGENTRY *MapInfo)( imp_image_handle *, void * );
    void                (DIGENTRY *UnLoadInfo)( imp_image_handle * );

    walk_result         (DIGENTRY *WalkModList)( imp_image_handle *, IMP_MOD_WKR *, void * );
    size_t              (DIGENTRY *ModName)( imp_image_handle *, imp_mod_handle, char *buff, size_t buff_size );
    char                *(DIGENTRY *ModSrcLang)( imp_image_handle *, imp_mod_handle );
    dip_status          (DIGENTRY *ModInfo)( imp_image_handle *, imp_mod_handle, handle_kind );
    dip_status          (DIGENTRY *ModDefault)( imp_image_handle *, imp_mod_handle, default_kind, dip_type_info * );
    search_result       (DIGENTRY *AddrMod)( imp_image_handle *, address, imp_mod_handle * );
    address             (DIGENTRY *ModAddr)( imp_image_handle *, imp_mod_handle );

    walk_result         (DIGENTRY *WalkTypeList)( imp_image_handle *, imp_mod_handle, IMP_TYPE_WKR *, imp_type_handle *, void * );
    imp_mod_handle      (DIGENTRY *TypeMod)(imp_image_handle *, imp_type_handle * );
    dip_status          (DIGENTRY *TypeInfo)(imp_image_handle *, imp_type_handle *, location_context *, dip_type_info * );
    dip_status          (DIGENTRY *OldTypeBase)(imp_image_handle *, imp_type_handle *, imp_type_handle * );
    dip_status          (DIGENTRY *TypeArrayInfo)(imp_image_handle *, imp_type_handle *, location_context *, array_info *, imp_type_handle * );
    dip_status          (DIGENTRY *TypeProcInfo)(imp_image_handle *, imp_type_handle *, imp_type_handle *, unsigned );
    dip_status          (DIGENTRY *TypePtrAddrSpace)( imp_image_handle *, imp_type_handle *, location_context *, address * );
    dip_status          (DIGENTRY *TypeThunkAdjust)( imp_image_handle *, imp_type_handle *, imp_type_handle *, location_context *, address * );
    int                 (DIGENTRY *TypeCmp)( imp_image_handle *, imp_type_handle *, imp_type_handle * );
    size_t              (DIGENTRY *TypeName)( imp_image_handle *, imp_type_handle *, unsigned, symbol_type *, char *buff, size_t buff_size);

    walk_result         (DIGENTRY *WalkSymList)( imp_image_handle *, symbol_source, void *, IMP_SYM_WKR *, imp_sym_handle *, void * );
    imp_mod_handle      (DIGENTRY *SymMod)(imp_image_handle *, imp_sym_handle * );
    size_t              (DIGENTRY *SymName)( imp_image_handle *, imp_sym_handle *, location_context *, symbol_name, char *buff, size_t buff_size );
    dip_status          (DIGENTRY *SymType)( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
    dip_status          (DIGENTRY *SymLocation)( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
    dip_status          (DIGENTRY *SymValue)( imp_image_handle *, imp_sym_handle *, location_context *, void * );
    dip_status          (DIGENTRY *SymInfo)( imp_image_handle *, imp_sym_handle *, location_context *, sym_info * );
    dip_status          (DIGENTRY *SymParmLocation)( imp_image_handle *, imp_sym_handle *, location_context *, location_list *, unsigned );
    dip_status          (DIGENTRY *SymObjType)( imp_image_handle *, imp_sym_handle *, imp_type_handle *, dip_type_info * );
    dip_status          (DIGENTRY *SymObjLocation)( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
    search_result       (DIGENTRY *AddrSym)( imp_image_handle *, imp_mod_handle, address, imp_sym_handle * );
    search_result       (DIGENTRY *LookupSym)( imp_image_handle *, symbol_source, void *, lookup_item *, void * );
    search_result       (DIGENTRY *AddrScope)( imp_image_handle *, imp_mod_handle, address, scope_block * );
    search_result       (DIGENTRY *ScopeOuter)( imp_image_handle *, imp_mod_handle, scope_block *, scope_block * );
    int                 (DIGENTRY *SymCmp)( imp_image_handle *, imp_sym_handle *, imp_sym_handle * );

    walk_result         (DIGENTRY *WalkFileList)( imp_image_handle *, imp_mod_handle, IMP_CUE_WKR *, imp_cue_handle *, void * );
    imp_mod_handle      (DIGENTRY *CueMod)(imp_image_handle *, imp_cue_handle * );
    size_t              (DIGENTRY *CueFile)( imp_image_handle *, imp_cue_handle *, char *buff, size_t buff_size );
    cue_fileid          (DIGENTRY *CueFileId)( imp_image_handle *, imp_cue_handle * );
    dip_status          (DIGENTRY *CueAdjust)( imp_image_handle *, imp_cue_handle *, int, imp_cue_handle * );
    unsigned long       (DIGENTRY *CueLine)( imp_image_handle *, imp_cue_handle * );
    unsigned            (DIGENTRY *CueColumn)( imp_image_handle *, imp_cue_handle * );
    address             (DIGENTRY *CueAddr)( imp_image_handle *, imp_cue_handle * );
    search_result       (DIGENTRY *LineCue)( imp_image_handle *, imp_mod_handle, cue_fileid, unsigned long, unsigned, imp_cue_handle * );
    search_result       (DIGENTRY *AddrCue)( imp_image_handle *, imp_mod_handle, address, imp_cue_handle * );
    int                 (DIGENTRY *CueCmp)( imp_image_handle *, imp_cue_handle *, imp_cue_handle * );

    dip_status          (DIGENTRY *TypeBase)(imp_image_handle *, imp_type_handle *, imp_type_handle *, location_context *, location_list * );

    dip_status          (DIGENTRY *TypeAddRef)( imp_image_handle *, imp_type_handle * );
    dip_status          (DIGENTRY *TypeRelease)( imp_image_handle *, imp_type_handle * );
    dip_status          (DIGENTRY *TypeFreeAll)( imp_image_handle * );
    dip_status          (DIGENTRY *SymAddRef)( imp_image_handle *, imp_sym_handle * );
    dip_status          (DIGENTRY *SymRelease)( imp_image_handle *, imp_sym_handle * );
    dip_status          (DIGENTRY *SymFreeAll)( imp_image_handle * );

    walk_result         (DIGENTRY *WalkSymListEx)( imp_image_handle *, symbol_source, void *, IMP_SYM_WKR *, imp_sym_handle *, location_context *, void * );
    search_result       (DIGENTRY *LookupSymEx)( imp_image_handle *, symbol_source, void *, lookup_item *, location_context *, void * );
};

extern const char       DIPImpName[];

unsigned        DIGENTRY DIPImpQueryHandleSize( handle_kind );
dip_status      DIGENTRY DIPImpMoreMem( unsigned );
dip_status      DIGENTRY DIPImpStartup( void );
void            DIGENTRY DIPImpShutdown( void );
void            DIGENTRY DIPImpCancel( void );

dip_status      DIGENTRY DIPImpLoadInfo( dig_fhandle, imp_image_handle * );
void            DIGENTRY DIPImpMapInfo( imp_image_handle *, void * );
void            DIGENTRY DIPImpUnloadInfo( imp_image_handle * );

walk_result     DIGENTRY DIPImpWalkModList( imp_image_handle *, IMP_MOD_WKR *, void * );
size_t          DIGENTRY DIPImpModName( imp_image_handle *, imp_mod_handle, char *buff, size_t buff_size );
char            *DIGENTRY DIPImpModSrcLang( imp_image_handle *, imp_mod_handle );
dip_status      DIGENTRY DIPImpModInfo( imp_image_handle *, imp_mod_handle, handle_kind );
search_result   DIGENTRY DIPImpAddrMod( imp_image_handle *, address, imp_mod_handle * );
address         DIGENTRY DIPImpModAddr( imp_image_handle *, imp_mod_handle );
dip_status      DIGENTRY DIPImpModDefault( imp_image_handle *, imp_mod_handle, default_kind, dip_type_info * );

walk_result     DIGENTRY DIPImpWalkTypeList( imp_image_handle *, imp_mod_handle, IMP_TYPE_WKR *, imp_type_handle *, void * );
imp_mod_handle  DIGENTRY DIPImpTypeMod( imp_image_handle *, imp_type_handle * );
dip_status      DIGENTRY DIPImpTypeInfo( imp_image_handle *, imp_type_handle *, location_context *, dip_type_info * );
dip_status      DIGENTRY DIPImpTypeBase( imp_image_handle *, imp_type_handle *, imp_type_handle *, location_context *, location_list * );
dip_status      DIGENTRY DIPImpTypeArrayInfo( imp_image_handle *, imp_type_handle *, location_context *, array_info *, imp_type_handle * );
dip_status      DIGENTRY DIPImpTypeProcInfo( imp_image_handle *, imp_type_handle *, imp_type_handle *, unsigned );
dip_status      DIGENTRY DIPImpTypePtrAddrSpace( imp_image_handle *, imp_type_handle *, location_context *, address * );
dip_status      DIGENTRY DIPImpTypeThunkAdjust( imp_image_handle *, imp_type_handle *, imp_type_handle *, location_context *, address * );
int             DIGENTRY DIPImpTypeCmp( imp_image_handle *, imp_type_handle *, imp_type_handle * );
size_t          DIGENTRY DIPImpTypeName( imp_image_handle *, imp_type_handle *, unsigned, symbol_type *, char *buff, size_t buff_size);
dip_status      DIGENTRY DIPImpTypeAddRef( imp_image_handle *, imp_type_handle * );
dip_status      DIGENTRY DIPImpTypeRelease( imp_image_handle *, imp_type_handle * );
dip_status      DIGENTRY DIPImpTypeFreeAll( imp_image_handle * );

walk_result     DIGENTRY DIPImpWalkSymList( imp_image_handle *, symbol_source, void *, IMP_SYM_WKR *, imp_sym_handle *, void * );
walk_result     DIGENTRY DIPImpWalkSymListEx( imp_image_handle *, symbol_source, void *, IMP_SYM_WKR *, imp_sym_handle *, location_context *lc, void * );
imp_mod_handle  DIGENTRY DIPImpSymMod( imp_image_handle *, imp_sym_handle * );
size_t          DIGENTRY DIPImpSymName( imp_image_handle *, imp_sym_handle *, location_context *, symbol_name, char *buff, size_t buff_size);
dip_status      DIGENTRY DIPImpSymType( imp_image_handle *, imp_sym_handle *, imp_type_handle * );
dip_status      DIGENTRY DIPImpSymLocation( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
dip_status      DIGENTRY DIPImpSymValue( imp_image_handle *, imp_sym_handle *, location_context *, void * );
dip_status      DIGENTRY DIPImpSymInfo( imp_image_handle *, imp_sym_handle *, location_context *, sym_info * );
dip_status      DIGENTRY DIPImpSymParmLocation( imp_image_handle *, imp_sym_handle *, location_context *, location_list *, unsigned );
dip_status      DIGENTRY DIPImpSymObjType( imp_image_handle *, imp_sym_handle *, imp_type_handle *, dip_type_info * );
dip_status      DIGENTRY DIPImpSymObjLocation( imp_image_handle *, imp_sym_handle *, location_context *, location_list * );
search_result   DIGENTRY DIPImpAddrSym( imp_image_handle *, imp_mod_handle, address, imp_sym_handle * );
search_result   DIGENTRY DIPImpLookupSym( imp_image_handle *, symbol_source, void *, lookup_item *, void * );
search_result   DIGENTRY DIPImpLookupSymEx( imp_image_handle *, symbol_source, void *, lookup_item *, location_context *, void * );
search_result   DIGENTRY DIPImpAddrScope( imp_image_handle *, imp_mod_handle, address, scope_block * );
search_result   DIGENTRY DIPImpScopeOuter( imp_image_handle *, imp_mod_handle, scope_block *, scope_block * );
int             DIGENTRY DIPImpSymCmp( imp_image_handle *, imp_sym_handle *, imp_sym_handle * );
dip_status      DIGENTRY DIPImpSymAddRef( imp_image_handle *, imp_sym_handle * );
dip_status      DIGENTRY DIPImpSymRelease( imp_image_handle *, imp_sym_handle * );
dip_status      DIGENTRY DIPImpSymFreeAll( imp_image_handle * );

walk_result     DIGENTRY DIPImpWalkFileList( imp_image_handle *, imp_mod_handle, IMP_CUE_WKR *, imp_cue_handle *, void * );
imp_mod_handle  DIGENTRY DIPImpCueMod( imp_image_handle *, imp_cue_handle * );
size_t          DIGENTRY DIPImpCueFile( imp_image_handle *, imp_cue_handle *, char *buff, size_t buff_size );
cue_fileid      DIGENTRY DIPImpCueFileId( imp_image_handle *, imp_cue_handle * );
dip_status      DIGENTRY DIPImpCueAdjust( imp_image_handle *, imp_cue_handle *, int, imp_cue_handle * );
unsigned long   DIGENTRY DIPImpCueLine( imp_image_handle *, imp_cue_handle * );
unsigned        DIGENTRY DIPImpCueColumn( imp_image_handle *, imp_cue_handle * );
address         DIGENTRY DIPImpCueAddr( imp_image_handle *, imp_cue_handle * );
search_result   DIGENTRY DIPImpLineCue( imp_image_handle *, imp_mod_handle, cue_fileid, unsigned long, unsigned, imp_cue_handle * );
search_result   DIGENTRY DIPImpAddrCue( imp_image_handle *, imp_mod_handle, address, imp_cue_handle * );
int             DIGENTRY DIPImpCueCmp( imp_image_handle *, imp_cue_handle *, imp_cue_handle * );


typedef struct dip_client_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         sizeof_struct;

    void                *(DIGCLIENT *Alloc)( size_t );
    void                *(DIGCLIENT *Realloc)( void *, size_t );
    void                (DIGCLIENT *Free)( void * );

    void                (DIGCLIENT *MapAddr)( addr_ptr *, void * );
    imp_sym_handle      *(DIGCLIENT *SymCreate)( imp_image_handle *, void * );
    dip_status          (DIGCLIENT *ItemLocation)( location_context *, context_item, location_list * );
    dip_status          (DIGCLIENT *AssignLocation)( location_list *, location_list *, unsigned long );
    dip_status          (DIGCLIENT *SameAddrSpace)( address, address );
    void                (DIGCLIENT *AddrSection)( address * );

    dig_fhandle         (DIGCLIENT *Open)( char const *, dig_open );
    unsigned long       (DIGCLIENT *Seek)( dig_fhandle, unsigned long, dig_seek );
    size_t              (DIGCLIENT *Read)( dig_fhandle, void *, size_t );
    size_t              (DIGCLIENT *Write)( dig_fhandle, void const *, size_t );
    void                (DIGCLIENT *Close)( dig_fhandle );
    void                (DIGCLIENT *Remove)( char const *, dig_open );

    void                (DIGCLIENT *Status)( dip_status );

    dig_mad             (DIGCLIENT *CurrMAD)( void );
    unsigned            (DIGCLIENT *MachineData)( address, dig_info_type, dig_elen, void const*, dig_elen, void * );
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

void            *DCAlloc( size_t amount );
void            *DCAllocZ( size_t amount );
void            *DCRealloc( void *p, size_t amount );
void            DCFree( void *p );

void            DCMapAddr( addr_ptr *a, void *d );
imp_sym_handle  *DCSymCreate( imp_image_handle *iih, void *d );
dip_status      DCItemLocation( location_context *, context_item, location_list * );
dip_status      DCAssignLocation( location_list *, location_list *, unsigned long );
dip_status      DCSameAddrSpace( address, address );
void            DCAddrSection( address * );

dig_fhandle     DCOpen( const char *path, dig_open flags );
unsigned long   DCSeek( dig_fhandle h, unsigned long p, dig_seek w );
size_t          DCRead( dig_fhandle h, void *b, size_t s );
dip_status      DCReadAt( dig_fhandle h, void *b, size_t s, unsigned long p );
size_t          DCWrite( dig_fhandle h, void *b, size_t s );
void            DCClose( dig_fhandle h );
void            DCRemove( const char *path, dig_open flags );

void            DCStatus( dip_status );

dig_mad         DCCurrMAD(void);

unsigned        DCMachineData( address, dig_info_type, dig_elen, void *, dig_elen, void * );

#endif
