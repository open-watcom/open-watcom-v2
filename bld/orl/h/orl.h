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
* Description:  Object Reader Library public interface.
*
****************************************************************************/


#ifndef ORL_H
#define ORL_H

#include <stddef.h>
#include "orlglobl.h"

typedef struct orl_handle_struct        * orl_handle;
typedef struct orl_file_handle_struct   * orl_file_handle;
typedef struct orl_sec_handle_struct    * orl_sec_handle;
typedef struct orl_symbol_handle_struct * orl_symbol_handle;
typedef struct orl_group_handle_struct  * orl_group_handle;

#include "orlcomon.h"

orl_handle              ORLENTRY ORLInit( orl_funcs *funcs );
orl_return              ORLENTRY ORLGetError( orl_handle );
orl_return              ORLENTRY ORLFini( orl_handle );

orl_file_format         ORLENTRY ORLFileIdentify( orl_handle, void * );
orl_file_handle         ORLENTRY ORLFileInit( orl_handle, void *, orl_file_format );
orl_return              ORLENTRY ORLFileFini( orl_file_handle );
orl_return              ORLENTRY ORLFileScan( orl_file_handle, const char *, orl_sec_return_func );
orl_machine_type        ORLENTRY ORLFileGetMachineType( orl_file_handle );
orl_file_flags          ORLENTRY ORLFileGetFlags( orl_file_handle );
orl_file_type           ORLENTRY ORLFileGetType( orl_file_handle );
orl_file_format         ORLENTRY ORLFileGetFormat( orl_file_handle );
orl_file_size           ORLENTRY ORLFileGetSize( orl_file_handle );
orl_sec_handle          ORLENTRY ORLFileGetSymbolTable( orl_file_handle );

char *                  ORLENTRY ORLSecGetName( orl_sec_handle );
orl_sec_offset          ORLENTRY ORLSecGetBase( orl_sec_handle );
orl_sec_size            ORLENTRY ORLSecGetSize( orl_sec_handle );
orl_sec_type            ORLENTRY ORLSecGetType( orl_sec_handle );
orl_sec_flags           ORLENTRY ORLSecGetFlags( orl_sec_handle );
orl_sec_alignment       ORLENTRY ORLSecGetAlignment( orl_sec_handle );
orl_sec_handle          ORLENTRY ORLSecGetStringTable( orl_sec_handle );
orl_sec_handle          ORLENTRY ORLSecGetSymbolTable( orl_sec_handle );
orl_sec_handle          ORLENTRY ORLSecGetRelocTable( orl_sec_handle );
orl_linnum *            ORLENTRY ORLSecGetLines( orl_sec_handle );
orl_table_index         ORLENTRY ORLSecGetNumLines( orl_sec_handle );
orl_sec_offset          ORLENTRY ORLSecGetOffset( orl_sec_handle );
orl_return              ORLENTRY ORLSecGetContents( orl_sec_handle, unsigned char ** );
orl_return              ORLENTRY ORLSecQueryReloc( orl_sec_handle, orl_sec_offset, orl_reloc_return_func );
orl_return              ORLENTRY ORLSecScanReloc( orl_sec_handle, orl_reloc_return_func );
orl_table_index         ORLENTRY ORLCvtSecHdlToIdx( orl_sec_handle );
orl_sec_handle          ORLENTRY ORLCvtIdxToSecHdl( orl_file_handle,
                                                    orl_table_index );

char *                  ORLENTRY ORLSecGetClassName( orl_sec_handle );
orl_sec_combine         ORLENTRY ORLSecGetCombine( orl_sec_handle );
orl_sec_frame           ORLENTRY ORLSecGetAbsFrame( orl_sec_handle );
orl_sec_handle          ORLENTRY ORLSecGetAssociated( orl_sec_handle );
orl_group_handle        ORLENTRY ORLSecGetGroup( orl_sec_handle );

orl_return              ORLENTRY ORLRelocSecScan( orl_sec_handle,
                                                  orl_reloc_return_func );
orl_return              ORLENTRY ORLSymbolSecScan( orl_sec_handle,
                                                   orl_symbol_return_func );
orl_return              ORLENTRY ORLNoteSecScan( orl_sec_handle,
                                                 orl_note_callbacks *, void * );

char *                  ORLENTRY ORLSymbolGetName( orl_symbol_handle );
orl_symbol_value        ORLENTRY ORLSymbolGetValue( orl_symbol_handle );
orl_symbol_binding      ORLENTRY ORLSymbolGetBinding( orl_symbol_handle );
orl_symbol_type         ORLENTRY ORLSymbolGetType( orl_symbol_handle );
unsigned char           ORLENTRY ORLSymbolGetRawInfo( orl_symbol_handle );
orl_sec_handle          ORLENTRY ORLSymbolGetSecHandle( orl_symbol_handle );
orl_symbol_handle       ORLENTRY ORLSymbolGetAssociated( orl_symbol_handle );

orl_return              ORLENTRY ORLGroupsScan( orl_file_handle,
                                                orl_group_return_func );
char *                  ORLENTRY ORLGroupName( orl_group_handle );
orl_table_index         ORLENTRY ORLGroupSize( orl_group_handle );
char *                  ORLENTRY ORLGroupMember( orl_group_handle,
                                                 orl_table_index );

unsigned long           ORLENTRY ORLExportTableRVA( orl_file_handle);
#endif
