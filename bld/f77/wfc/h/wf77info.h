/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Front end routines defined for optimizing code generator.
*
****************************************************************************/


extern void         InitSubSegs( void );
extern segment_id   AllocImpSegId( void );
extern void         InitSegs( void );
extern void         FiniSegs( void );
extern void         AllocSegs( void );
extern void         SubCodeSeg( void );
extern void         DtInit( segment_id segid, seg_offset offset );
extern void         DtIBytes( byte data, uint size );
extern void         DtStreamBytes( const char *data, uint size );
extern void         DtBytes( const char *data, uint size );
extern void         DtStartSequence( void );
extern void         DtFiniSequence( void );
extern segment_id   GetComSegId( sym_id sym, uint offset );
extern segment_id   GetDataSegId( sym_id sym );
extern seg_offset   GetGlobalOffset( uint g_offset );
extern seg_offset   GetComOffset( uint offset );
extern seg_offset   GetDataOffset( sym_id sym );
extern segment_id   GetGlobalSegId( uint g_offset );
extern void         DefTypes( void );
extern void         DefStructs( void );
extern void         *ConstBack( sym_id c_ptr );
extern void         FCMessage( fc_msg_class tipe, pointer x );
extern char         *GetFullSrcName( void );
