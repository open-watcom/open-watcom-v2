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


#ifndef OMF_ORL_INCLUDED
#define OMF_ORL_INCLUDED

#include "omftype.h"
#include "pcobj.h"

omf_handle              OMFENTRY OmfInit( orl_funcs * );
orl_return              OMFENTRY OmfFini( omf_handle );

orl_return              OMFENTRY OmfFileInit( omf_handle, void *, omf_file_handle * );
orl_return              OMFENTRY OmfFileFini( omf_file_handle );
orl_return              OMFENTRY OmfFileScan( omf_file_handle, const char *, orl_sec_return_func );
orl_machine_type        OMFENTRY OmfFileGetMachineType( omf_file_handle );
orl_file_flags          OMFENTRY OmfFileGetFlags( omf_file_handle );
orl_file_type           OMFENTRY OmfFileGetType( omf_file_handle );
orl_file_size           OMFENTRY OmfFileGetSize( omf_file_handle );
omf_sec_handle          OMFENTRY OmfFileGetSymbolTable( omf_file_handle );

char *                  OMFENTRY OmfSecGetName( omf_sec_handle );
orl_sec_offset          OMFENTRY OmfSecGetBase( omf_sec_handle );
orl_sec_size            OMFENTRY OmfSecGetSize( omf_sec_handle );
orl_sec_type            OMFENTRY OmfSecGetType( omf_sec_handle );
orl_sec_flags           OMFENTRY OmfSecGetFlags( omf_sec_handle );
orl_sec_alignment       OMFENTRY OmfSecGetAlignment( omf_sec_handle );
omf_sec_handle          OMFENTRY OmfSecGetStringTable( omf_sec_handle );
omf_sec_handle          OMFENTRY OmfSecGetSymbolTable( omf_sec_handle );
omf_sec_handle          OMFENTRY OmfSecGetRelocTable( omf_sec_handle );
orl_sec_offset          OMFENTRY OmfSecGetNonLocalSymbolLoc( omf_sec_handle );
orl_return              OMFENTRY OmfSecGetContents( omf_sec_handle, unsigned char ** );
orl_return              OMFENTRY OmfSecQueryReloc( omf_sec_handle, omf_sec_offset, orl_reloc_return_func );
orl_return              OMFENTRY OmfSecScanReloc( omf_sec_handle, orl_reloc_return_func );
char *                  OMFENTRY OmfSecGetClassName( omf_sec_handle );
orl_sec_combine         OMFENTRY OmfSecGetCombine( omf_sec_handle );
orl_sec_frame           OMFENTRY OmfSecGetAbsFrame( omf_sec_handle );
orl_sec_handle          OMFENTRY OmfSecGetAssociated( omf_sec_handle );
orl_group_handle        OMFENTRY OmfSecGetGroup( omf_sec_handle );
orl_table_index         OMFENTRY OmfSecGetNumLines( omf_sec_handle );
orl_linnum *            OMFENTRY OmfSecGetLines( omf_sec_handle );

orl_table_index         OMFENTRY OmfCvtSecHdlToIdx( omf_sec_handle );
omf_sec_handle          OMFENTRY OmfCvtIdxToSecHdl( omf_file_handle, orl_table_index );

orl_return              OMFENTRY OmfRelocSecScan( omf_sec_handle, orl_reloc_return_func );
orl_return              OMFENTRY OmfSymbolSecScan( omf_sec_handle, orl_symbol_return_func );

char *                  OMFENTRY OmfSymbolGetName( omf_symbol_handle );
orl_symbol_value        OMFENTRY OmfSymbolGetValue( omf_symbol_handle );
orl_symbol_binding      OMFENTRY OmfSymbolGetBinding( omf_symbol_handle );
orl_symbol_type         OMFENTRY OmfSymbolGetType( omf_symbol_handle );
unsigned char           OMFENTRY OmfSymbolGetRawInfo( omf_symbol_handle );
omf_sec_handle          OMFENTRY OmfSymbolGetSecHandle( omf_symbol_handle );

orl_return              OMFENTRY OmfNoteSecScan( omf_sec_handle, orl_note_callbacks *, void * );

orl_return              OMFENTRY OmfGroupsScan( omf_file_handle, orl_group_return_func );
orl_table_index         OMFENTRY OmfGroupSize( omf_grp_handle );
char *                  OMFENTRY OmfGroupName( omf_grp_handle );
char *                  OMFENTRY OmfGroupMember( omf_grp_handle, orl_table_index );

#endif
