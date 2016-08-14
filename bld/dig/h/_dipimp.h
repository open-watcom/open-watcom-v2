/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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


pick( unsigned,         HandleSize,         ( handle_kind ) )
pick( dip_status,       MoreMem,            ( unsigned ) )
pick( void,             Shutdown,           ( void ) )
pick( void,             Cancel,             ( void ) )
pick( dip_status,       LoadInfo,           ( dig_fhandle, imp_image_handle * ) )
pick( void,             MapInfo,            ( imp_image_handle *, void * ) )
pick( void,             UnloadInfo,         ( imp_image_handle * ) )
pick( walk_result,      WalkModList,        ( imp_image_handle *, IMP_MOD_WKR *, void * ) )
pick( size_t,           ModName,            ( imp_image_handle *, imp_mod_handle, char *buff, size_t buff_size ) )
pick( char *,           ModSrcLang,         ( imp_image_handle *, imp_mod_handle ) )
pick( dip_status,       ModInfo,            ( imp_image_handle *, imp_mod_handle, handle_kind ) )
pick( dip_status,       ModDefault,         ( imp_image_handle *, imp_mod_handle, default_kind, dip_type_info * ) )
pick( search_result,    AddrMod,            ( imp_image_handle *, address, imp_mod_handle * ) )
pick( address,          ModAddr,            ( imp_image_handle *, imp_mod_handle ) )
pick( walk_result,      WalkTypeList,       ( imp_image_handle *, imp_mod_handle, IMP_TYPE_WKR *, imp_type_handle *, void * ) )
pick( imp_mod_handle,   TypeMod,            ( imp_image_handle *, imp_type_handle * ) )
pick( dip_status,       TypeInfo,           ( imp_image_handle *, imp_type_handle *, location_context *, dip_type_info * ) )
pick( dip_status,       OldTypeBase,        ( imp_image_handle *, imp_type_handle *, imp_type_handle * ) )
pick( dip_status,       TypeArrayInfo,      ( imp_image_handle *, imp_type_handle *, location_context *, array_info *, imp_type_handle * ) )
pick( dip_status,       TypeProcInfo,       ( imp_image_handle *, imp_type_handle *, imp_type_handle *, unsigned ) )
pick( dip_status,       TypePtrAddrSpace,   ( imp_image_handle *, imp_type_handle *, location_context *, address * ) )
pick( dip_status,       TypeThunkAdjust,    ( imp_image_handle *, imp_type_handle *, imp_type_handle *, location_context *, address * ) )
pick( int,              TypeCmp,            ( imp_image_handle *, imp_type_handle *, imp_type_handle * ) )
pick( size_t,           TypeName,           ( imp_image_handle *, imp_type_handle *, unsigned, symbol_type *, char *buff, size_t buff_size) )
pick( walk_result,      WalkSymList,        ( imp_image_handle *, symbol_source, void *, IMP_SYM_WKR *, imp_sym_handle *, void * ) )
pick( imp_mod_handle,   SymMod,             ( imp_image_handle *, imp_sym_handle * ) )
pick( size_t,           SymName,            ( imp_image_handle *, imp_sym_handle *, location_context *, symbol_name, char *buff, size_t buff_size) )
pick( dip_status,       SymType,            ( imp_image_handle *, imp_sym_handle *, imp_type_handle * ) )
pick( dip_status,       SymLocation,        ( imp_image_handle *, imp_sym_handle *, location_context *, location_list * ) )
pick( dip_status,       SymValue,           ( imp_image_handle *, imp_sym_handle *, location_context *, void * ) )
pick( dip_status,       SymInfo,            ( imp_image_handle *, imp_sym_handle *, location_context *, sym_info * ) )
pick( dip_status,       SymParmLocation,    ( imp_image_handle *, imp_sym_handle *, location_context *, location_list *, unsigned ) )
pick( dip_status,       SymObjType,         ( imp_image_handle *, imp_sym_handle *, imp_type_handle *, dip_type_info * ) )
pick( dip_status,       SymObjLocation,     ( imp_image_handle *, imp_sym_handle *, location_context *, location_list * ) )
pick( search_result,    AddrSym,            ( imp_image_handle *, imp_mod_handle, address, imp_sym_handle * ) )
pick( search_result,    LookupSym,          ( imp_image_handle *, symbol_source, void *, lookup_item *, void * ) )
pick( search_result,    AddrScope,          ( imp_image_handle *, imp_mod_handle, address, scope_block * ) )
pick( search_result,    ScopeOuter,         ( imp_image_handle *, imp_mod_handle, scope_block *, scope_block * ) )
pick( int,              SymCmp,             ( imp_image_handle *, imp_sym_handle *, imp_sym_handle * ) )
pick( walk_result,      WalkFileList,       ( imp_image_handle *, imp_mod_handle, IMP_CUE_WKR *, imp_cue_handle *, void * ) )
pick( imp_mod_handle,   CueMod,             ( imp_image_handle *, imp_cue_handle * ) )
pick( size_t,           CueFile,            ( imp_image_handle *, imp_cue_handle *, char *buff, size_t buff_size ) )
pick( cue_fileid,       CueFileId,          ( imp_image_handle *, imp_cue_handle * ) )
pick( dip_status,       CueAdjust,          ( imp_image_handle *, imp_cue_handle *, int, imp_cue_handle * ) )
pick( unsigned long,    CueLine,            ( imp_image_handle *, imp_cue_handle * ) )
pick( unsigned,         CueColumn,          ( imp_image_handle *, imp_cue_handle * ) )
pick( address,          CueAddr,            ( imp_image_handle *, imp_cue_handle * ) )
pick( search_result,    LineCue,            ( imp_image_handle *, imp_mod_handle, cue_fileid, unsigned long, unsigned, imp_cue_handle * ) )
pick( search_result,    AddrCue,            ( imp_image_handle *, imp_mod_handle, address, imp_cue_handle * ) )
pick( int,              CueCmp,             ( imp_image_handle *, imp_cue_handle *, imp_cue_handle * ) )
pick( dip_status,       TypeBase,           ( imp_image_handle *, imp_type_handle *, imp_type_handle *, location_context *, location_list * ) )
pick( dip_status,       TypeAddRef,         ( imp_image_handle *, imp_type_handle * ) )
pick( dip_status,       TypeRelease,        ( imp_image_handle *, imp_type_handle * ) )
pick( dip_status,       TypeFreeAll,        ( imp_image_handle * ) )
pick( dip_status,       SymAddRef,          ( imp_image_handle *, imp_sym_handle * ) )
pick( dip_status,       SymRelease,         ( imp_image_handle *, imp_sym_handle * ) )
pick( dip_status,       SymFreeAll,         ( imp_image_handle * ) )
pick( walk_result,      WalkSymListEx,      ( imp_image_handle *, symbol_source, void *, IMP_SYM_WKR *, imp_sym_handle *, location_context *lc, void * ) )
pick( search_result,    LookupSymEx,        ( imp_image_handle *, symbol_source, void *, lookup_item *, location_context *, void * ) )
