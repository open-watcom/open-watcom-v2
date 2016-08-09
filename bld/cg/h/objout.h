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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "import.h"

extern void         ObjInit( void );
extern void         ObjFini( void );
extern void         InitSegDefs( void );
extern void         DefSegment( segment_id id, seg_attr attr, const char *str, uint align, bool use_16 );
extern bool         HaveCodeSeg( void );
extern segment_id   AskCodeSeg( void );
extern segment_id   AskAltCodeSeg( void );
extern segment_id   AskBackSeg( void );
extern segment_id   AskOP( void );
extern segment_id   AskSegID( void *hdl, cg_class class );
extern bool         AskSegBlank( segment_id id );
extern segment_id   SetOP( segment_id seg );
extern void         FlushOP( segment_id id );
extern bool         NeedBaseSet( void );
extern offset       AskLocation( void );
extern long_offset  AskBigLocation( void );
extern offset       AskMaxSize( void );
extern long_offset  AskBigMaxSize( void );
extern void         SetLocation( offset loc );
extern void         SetBigLocation( long_offset loc );
extern void         OutLabel( label_handle label );
extern void         *InitPatch( void );
extern void         AbsPatch( abspatch_handle patch, offset lc );
extern void         TellObjNewProc( cg_sym_handle proc );
extern void         IncLocation( offset by );
extern bool         AskNameROM( pointer h, cg_class c );
extern void         OutLineNum( cg_linenum line, bool label_line );
extern void         BackPtr( back_handle bck, segment_id seg, offset plus, type_def *tipe );
extern void         BackPtrBase( back_handle bck, segment_id seg );
extern void         BackBigOffset( back_handle bck, segment_id seg, offset plus );
extern void         FEPtr( cg_sym_handle sym, type_def *tipe, offset plus );
extern void         FEPtrBaseOffset( cg_sym_handle sym, offset plus );
extern void         FEPtrBase( cg_sym_handle sym );
extern bool         FreeObjCache( void );
extern char         GetMemModel( void );
extern void         ChkDbgSegSize( offset max, bool typing );
extern bool         AskSegNear( segment_id id );
extern void         DoOutObjectName( cg_sym_handle sym, void (*outputter)( const char *, void * ), void *data, import_type kind );
extern void         DoEmptyQueue( void );
extern void         DoAlignment( int len );
extern bool         CodeHasAbsPatch( oc_entry *code );
