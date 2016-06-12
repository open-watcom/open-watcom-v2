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


extern void             MadTypeToDipTypeInfo( mad_type_handle mt, dip_type_info *ti );
extern dip_status       RegLocation( machine_state *regs, const mad_reg_info *ri, location_list *ll );
extern search_result    DeAliasAddrMod( address a, mod_handle *mh );
extern search_result    DeAliasAddrSym( mod_handle mh, address a, sym_handle *sh );
extern search_result    DeAliasAddrScope( mod_handle mh, address a, scope_block *sb );
extern search_result    DeAliasAddrCue( mod_handle mh, address a, cue_handle *ch );
extern char             *DIPMsgText( dip_status status );
extern void             InitDbgInfo( void );
extern void             FiniDbgInfo( void );
extern bool             IsInternalMod( mod_handle mod );
extern bool             IsInternalModName( const char *start, size_t len );

extern dip_status       DIGREGISTER WVTypeAddRef( imp_image_handle *ii, imp_type_handle *it );
extern dip_status       DIGREGISTER WVTypeRelease( imp_image_handle *ii, imp_type_handle *it );
extern dip_status       DIGREGISTER WVTypeFreeAll( imp_image_handle *ii );
extern dip_status       DIGREGISTER WVSymAddRef( imp_image_handle *ii, imp_sym_handle *it );
extern dip_status       DIGREGISTER WVSymRelease( imp_image_handle *ii, imp_sym_handle *it );
extern dip_status       DIGREGISTER WVSymFreeAll( imp_image_handle *ii );
