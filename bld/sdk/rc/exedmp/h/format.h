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


#ifndef FORMAT_H
#define FORMAT_H

#include "common.h"
#include "strings.h"
#include "param.h"

#define SCREENWIDTH     78
#define RULERSTRING     "-"

const unsigned_32 cpu_masks_table[] = {
    7,          /* number of masks in table */
    PE_CPU_UNKNOWN,
    PE_CPU_386,
    PE_CPU_I860,
    PE_CPU_MIPS_R3000,
    PE_CPU_MIPS_R4000,
    PE_CPU_ALPHA,
    PE_CPU_POWERPC
};

const unsigned_32 hdr_masks_table[] = {
    15,         /* number of masks in table */
    PE_FLG_PROGRAM,
    PE_FLG_RELOCS_STRIPPED,
    PE_FLG_IS_EXECUTABLE,
    PE_FLG_LINNUM_STRIPPED,
    PE_FLG_LOCALS_STRIPPED,
    PE_FLG_MINIMAL_OBJ,
    PE_FLG_UPDATE_OBJ,
    PE_FLG_16BIT_MACHINE,
    PE_FLG_REVERSE_BYTE_LO,
    PE_FLG_32BIT_MACHINE,
    PE_FLG_FIXED,
    PE_FLG_FILE_PATCH,
    PE_FLG_FILE_SYSTEM,
    PE_FLG_LIBRARY,
    PE_FLG_REVERSE_BYTE_HI
};

const unsigned_32 ss_masks_table[] = {
    7,          /* number of masks in table */
    PE_SS_UNKNOWN,
    PE_SS_NATIVE,
    PE_SS_WINDOWS_GUI,
    PE_SS_WINDOWS_CHAR,
    PE_SS_OS2_CHAR,
    PE_SS_POSIX_CHAR,
    PE_SS_PL_DOSSTYLE
};

const unsigned_32 dll_masks_table[] = {
    4,          /* number of masks in table */
    PE_DLL_PERPROC_INIT,
    PE_DLL_PERPROC_TERM,
    PE_DLL_PERTHRD_INIT,
    PE_DLL_PERTHRD_TERM
};

const unsigned_32 obj_masks_table[] = {
    29,         /* number of masks in table */
    PE_OBJ_DUMMY,
    PE_OBJ_NOLOAD,
    PE_OBJ_GROUPED,
    PE_OBJ_NOPAD,
    PE_OBJ_TYPE_COPY,
    PE_OBJ_CODE,
    PE_OBJ_INIT_DATA,
    PE_OBJ_UNINIT_DATA,
    PE_OBJ_OTHER,
    PE_OBJ_LINK_INFO,
    PE_OBJ_OVERLAY,
    PE_OBJ_REMOVE,
    PE_OBJ_COMDAT,
    PE_OBJ_ALIGN_1,
    PE_OBJ_ALIGN_2,
    PE_OBJ_ALIGN_4,
    PE_OBJ_ALIGN_8,
    PE_OBJ_ALIGN_16,
    PE_OBJ_ALIGN_32,
    PE_OBJ_ALIGN_64,
    PE_OBJ_DISCARDABLE,
    PE_OBJ_NOT_CACHED,
    PE_OBJ_NOT_PAGABLE,
    PE_OBJ_SHARED,
    PE_OBJ_EXECUTABLE,
    PE_OBJ_READABLE,
    PE_OBJ_WRITABLE,
    PE_OBJ_ALIGN_MASK,
    PE_OBJ_ALIGN_SHIFT
};


void indentMore( int level );
void indentLess( int level );
void printIndent( bool newline, Parameters *param );
void printRuler( bool indent, bool newline, Parameters *param );
void printParameters( Parameters *param );
void printDefaultParameters( void );
void printBanner( void );
void printHelp( void );
void printDosHeader( ExeFile *exeFile, Parameters *param );
void printPeHeader( ExeFile *exeFile, Parameters *param );
void printResObject( ExeFile *exeFile, Parameters *param );
void printTableContents( ResTableEntry *table, ExeFile *exeFile,
                         Parameters *param, long int addr, int depth );
void printDirContents( ResDirEntry *dir, ExeFile *exeFile,
                       Parameters  *param, long int addr, int depth );
void printDataContents( ResDataEntry *data, ExeFile *exeFile,
                        Parameters   *param, long int addr );
void printHexHeader( long int addr, size_t length, ExeFile *exeFile );

#endif
