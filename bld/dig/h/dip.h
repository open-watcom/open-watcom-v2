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
* Description:  Public DIP (Debugging Information Processor) interface.
*
****************************************************************************/


#ifndef DIP_H_INCLUDED
#define DIP_H_INCLUDED

#include "diptypes.h"

/*
 *      Control Routines
 */

extern char     DIPDefaults[];

dip_status      DIPInit( void );
dip_status      DIPLoad( const char *path );
dip_status      DIPRegister( dip_imp_routines * );
void            DIPCancel( void );
void            DIPFini( void );
void            DIPFiniLatest( void );

size_t          DIPHandleSize( handle_kind hk, bool mgr_size );

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
typedef         walk_result (DIP_IMAGE_WALKER)( mod_handle, void * );
typedef         walk_result (DIP_MOD_WALKER)( mod_handle, void * );
typedef         walk_result (DIP_TYPE_WALKER)( type_handle *, void * );
typedef         walk_result (DIP_SYM_WALKER)( sym_walk_info, sym_handle *, void * );
typedef         walk_result (DIP_CUE_WALKER)( cue_handle *, void * );

walk_result     DIPWalkImageList( DIP_IMAGE_WALKER *, void * );
walk_result     DIPWalkModList( mod_handle, DIP_MOD_WALKER *, void * );
walk_result     DIPWalkTypeList( mod_handle, DIP_TYPE_WALKER *, void * );
walk_result     DIPWalkSymList( symbol_source, void *, DIP_SYM_WALKER *, void * );
walk_result     DIPWalkSymListEx( symbol_source, void *, DIP_SYM_WALKER *, location_context *, void * );
walk_result     DIPWalkFileList( mod_handle, DIP_CUE_WALKER *, void * );

/*
 * Image Information
 */
void            *DIPImageExtra( mod_handle );
const char      *DIPImageName( mod_handle );

/*
 * Module Information
 */
size_t          DIPModName( mod_handle, char *buff, size_t buff_size );
char            *DIPModSrcLang( mod_handle );
dip_status      DIPModHasInfo( mod_handle, handle_kind );
search_result   DIPAddrMod( address, mod_handle * );
address         DIPModAddr( mod_handle );
dip_status      DIPModDefault( mod_handle, default_kind, dip_type_info * );

/*
 * Type Information
 */
mod_handle      DIPTypeMod( type_handle * );
dip_status      DIPTypeInfo( type_handle *, location_context *lc, dip_type_info * );
dip_status      DIPTypeBase( type_handle *t, type_handle *b, location_context *lc, location_list * );
dip_status      DIPTypeArrayInfo( type_handle *, location_context *, array_info *, type_handle * );
dip_status      DIPTypeProcInfo( type_handle *t, type_handle *r, unsigned p );
dip_status      DIPTypePtrAddrSpace( type_handle *, location_context *, address * );
dip_status      DIPTypeThunkAdjust( type_handle *, type_handle *, location_context *, address * );
dip_status      DIPTypePointer( type_handle *, type_modifier, unsigned, type_handle * );
void            DIPTypeInit( type_handle *, mod_handle );
int             DIPTypeCmp( type_handle *, type_handle * );
size_t          DIPTypeName( type_handle *, unsigned, symbol_type *, char *buff, size_t buff_size );
dip_status      DIPTypeAddRef( type_handle * );
dip_status      DIPTypeRelease( type_handle * );
dip_status      DIPTypeFreeAll( void );

/*
 * Symbol Information
 */
mod_handle      DIPSymMod( sym_handle * );
size_t          DIPSymName( sym_handle *, location_context *, symbol_name, char *buff, size_t buff_size );
dip_status      DIPSymType( sym_handle *, type_handle * );
dip_status      DIPSymLocation( sym_handle *, location_context *, location_list * );
dip_status      DIPSymValue( sym_handle *, location_context *, void * );
dip_status      DIPSymInfo( sym_handle *, location_context *, sym_info * );
void            DIPSymInit( sym_handle *sh, image_handle *ih );
dip_status      DIPSymParmLocation( sym_handle *, location_context *, location_list *, unsigned p );
dip_status      DIPSymObjType( sym_handle *, type_handle *, dip_type_info * );
dip_status      DIPSymObjLocation( sym_handle *, location_context *, location_list * );
search_result   DIPAddrSym( mod_handle, address, sym_handle * );
search_result   DIPLookupSym( symbol_source, void *, lookup_item *, void * );
search_result   DIPLookupSymEx( symbol_source, void *, lookup_item *, location_context *, void * );
search_result   DIPAddrScope( mod_handle, address, scope_block * );
search_result   DIPScopeOuter( mod_handle, scope_block *, scope_block * );
int             DIPSymCmp( sym_handle *, sym_handle * );
dip_status      DIPSymAddRef( sym_handle * );
dip_status      DIPSymRelease( sym_handle * );
dip_status      DIPSymFreeAll( void );


/*
 * Source Cue Information
 */
mod_handle      DIPCueMod( cue_handle * );
size_t          DIPCueFile( cue_handle *, char *buff, size_t buff_size );
cue_fileid      DIPCueFileId( cue_handle * );
dip_status      DIPCueAdjust( cue_handle *, int adj, cue_handle * );
unsigned long   DIPCueLine( cue_handle * );
unsigned        DIPCueColumn( cue_handle * );
address         DIPCueAddr( cue_handle * );
search_result   DIPLineCue( mod_handle, cue_fileid, unsigned long line, unsigned column, cue_handle * );
search_result   DIPAddrCue( mod_handle, address, cue_handle * );
int             DIPCueCmp( cue_handle *, cue_handle * );

#endif
