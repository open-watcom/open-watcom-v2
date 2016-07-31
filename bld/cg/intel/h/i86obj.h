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
* Description:  Emit OMF object records.
*
****************************************************************************/


#include "fppatch.h"


extern bool         UseImportForm( fe_attr attr );
extern bool         AskSegPrivate( segment_id id );
extern bool         AskSegROM( segment_id id );
extern segment_id   AskCode16Seg( void );
extern void         OutSelect( bool starts );
extern void         SetUpObj( bool is_data );
extern void         OutDLLExport( uint words, cg_sym_handle sym );
extern void         OutFPPatch( fp_patches i );
extern void         OutPatch( label_handle lbl, patch_attr attr );
extern abspatch     *NewAbsPatch( void );
extern void         OutDataByte( byte value );
extern void         OutDataShort( unsigned_16 value );
extern void         OutDataLong( unsigned_32 value );
extern void         OutAbsPatch( abspatch *patch, patch_attr attr );
extern void         OutReloc( segment_id seg, fix_class class, bool rel );
extern void         OutSpecialCommon( import_handle imphdl, fix_class class, bool rel );
extern void         OutImport( cg_sym_handle sym, fix_class class, bool rel );
extern void         OutRTImportRel( rt_class rtindex, fix_class class, bool rel );
extern void         OutRTImport( rt_class rtindex, fix_class class );
extern void         OutBckExport( const char *name, bool is_export );
extern void         OutBckImport( const char *name, back_handle bck, fix_class class );
extern unsigned     SavePendingLine( unsigned new );
extern void         OutDBytes( unsigned len, const byte *src );
extern void         OutIBytes( byte pat, offset len );
extern void         TellObjVirtFuncRef( void *cookie );
extern bool         AskNameCode( pointer hdl, cg_class class );
extern segment_id   DbgSegDef( const char *seg_name, const char *seg_class, int seg_modifier );

extern bool         UseRepForm( unsigned size );
extern void         TellObjNewLabel( cg_sym_handle lbl );

extern fp_patches   FPPatchType;
