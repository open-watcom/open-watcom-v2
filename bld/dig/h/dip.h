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


#ifndef DIP_H
#include "digpck.h"
#include "diptypes.h"

/*
 *      Control Routines
 */

extern char     DIPDefaults[];

dip_status      DIPInit( void );
dip_status      DIPLoad( char *path );
dip_status      DIPRegister( dip_imp_routines * );
void            DIPCancel( void );
void            DIPFini( void );
void            DIPFiniLatest( void );

unsigned        DIPHandleSize( handle_kind h );

dip_status      DIPMoreMem( unsigned amount );

process_info    *DIPCreateProcess( void );
process_info    *DIPSetProcess( process_info * );
void            DIPDestroyProcess( process_info * );

unsigned        DIPPriority( unsigned );
mod_handle      DIPLoadInfo( dig_fhandle, unsigned extra, unsigned prio );
void            DIPMapInfo( mod_handle, void * );
void            DIPUnloadInfo( mod_handle );
unsigned        DIPImagePriority( mod_handle );

/*
 *      Information Walkers
 */
typedef         walk_result (IMAGE_WALKER)( mod_handle, void * );
walk_result     WalkImageList( IMAGE_WALKER *, void * );

typedef         walk_result (MOD_WALKER)( mod_handle, void * );
walk_result     WalkModList( mod_handle, MOD_WALKER *, void * );

typedef         walk_result (TYPE_WALKER)( type_handle *, void * );
walk_result     WalkTypeList( mod_handle, TYPE_WALKER *, void * );

typedef         walk_result (SYM_WALKER)( sym_walk_info, sym_handle *, void * );
walk_result     WalkSymList( symbol_source, void *, SYM_WALKER *, void * );
walk_result     WalkSymListEx( symbol_source, void *, SYM_WALKER *, location_context *, void * );

typedef         walk_result (CUE_WALKER)( cue_handle *, void * );
walk_result     WalkFileList( mod_handle, CUE_WALKER *, void * );


/*
 * Image Information
 */
void            *ImageExtra( mod_handle );
const char      *ImageDIP( mod_handle );

/*
 * Module Information
 */
unsigned        ModName( mod_handle, char *result, unsigned max );
char            *ModSrcLang( mod_handle );
dip_status      ModHasInfo( mod_handle, handle_kind );
search_result   AddrMod( address, mod_handle * );
address         ModAddr( mod_handle );
dip_status      ModDefault( mod_handle, default_kind, type_info * );

/*
 * Type Information
 */
mod_handle      TypeMod( type_handle * );
dip_status      TypeInfo( type_handle *, location_context *lc, type_info * );
dip_status      TypeBase( type_handle *t, type_handle *b, location_context *lc, location_list * );
dip_status      TypeArrayInfo( type_handle *, location_context *, array_info *, type_handle * );
dip_status      TypeProcInfo( type_handle *t, type_handle *r, unsigned p );
dip_status      TypePtrAddrSpace( type_handle *, location_context *, address * );
dip_status      TypeThunkAdjust( type_handle *, type_handle *, location_context *, address * );
dip_status      TypePointer( type_handle *, type_modifier, unsigned, type_handle * );
void            *TypeCreate( type_handle *, mod_handle );
int             TypeCmp( type_handle *, type_handle * );
unsigned        TypeName( type_handle *, unsigned, symbol_type *, char *, unsigned );
dip_status      TypeAddRef( type_handle * );
dip_status      TypeRelease( type_handle * );
dip_status      TypeFreeAll();

/*
 * Symbol Information
 */
mod_handle      SymMod( sym_handle * );
unsigned        SymName( sym_handle *, location_context *, symbol_name, char *name, unsigned max );
dip_status      SymType( sym_handle *, type_handle * );
dip_status      SymLocation( sym_handle *, location_context *, location_list * );
dip_status      SymValue( sym_handle *, location_context *, void * );
dip_status      SymInfo( sym_handle *, location_context *, sym_info * );
dip_status      SymParmLocation( sym_handle *, location_context *, location_list *, unsigned p );
dip_status      SymObjType( sym_handle *, type_handle *, type_info * );
dip_status      SymObjLocation( sym_handle *, location_context *, location_list * );
search_result   AddrSym( mod_handle, address, sym_handle * );
search_result   LookupSym( symbol_source, void *, lookup_item *, void * );
search_result   LookupSymEx( symbol_source, void *, lookup_item *, location_context *, void * );
search_result   AddrScope( mod_handle, address, scope_block * );
search_result   ScopeOuter( mod_handle, scope_block *, scope_block * );
int             SymCmp( sym_handle *, sym_handle * );
dip_status      SymAddRef( sym_handle * );
dip_status      SymRelease( sym_handle * );
dip_status      SymFreeAll();


/*
 * Source Cue Information
 */
mod_handle      CueMod( cue_handle * );
unsigned        CueFile( cue_handle *, char *file, unsigned max );
cue_file_id     CueFileId( cue_handle * );
dip_status      CueAdjust( cue_handle *, int adj, cue_handle * );
unsigned long   CueLine( cue_handle * );
unsigned        CueColumn( cue_handle * );
address         CueAddr( cue_handle * );
search_result   LineCue( mod_handle, cue_file_id, unsigned long line,
                        unsigned column, cue_handle * );
search_result   AddrCue( mod_handle, address, cue_handle * );
int             CueCmp( cue_handle *, cue_handle * );

#define DIP_H

#include "digunpck.h"
#endif
