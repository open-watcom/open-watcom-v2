/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#ifdef _OS2

#include "exepe.h"

#define DEF_VALUE               CONSTU32( 0xFFFFFFFF )

#define PE_DEFAULT_BASE         CONSTU32( 0x400000 )
#define PE_DEF_STACK_SIZE       _1M
#define PE_DEF_STACK_COMMIT     _64K
#define PE_DEF_HEAP_SIZE        _8K
#define PE_DEF_HEAP_COMMIT      _4K

extern void             DoAddResource( char * );
extern void             FiniPELoadFile( void );
extern void             ReadPEExportTable( f_handle, pe_dir_entry * );
extern void             AllocPETransferTable( void );
extern void             GenPEToc( void );
extern void             ChkPEData( void );
extern unsigned long    GetPEHeaderSize( void );
extern void             ResetLoadPE( void );
extern void             AddPEImportLocalSym( symbol *, symbol * );
extern bool             ImportPELocalSym( symbol * );
extern void             FreePELocalImports( void );

struct import_name {
    struct import_name  *next;
    dll_sym_info        *dll;
    obj_name_list       *imp;
};

typedef struct module_import {
    struct module_import        *next;
    struct obj_name_list        *mod;
    struct import_name          *imports;
    unsigned                    num_entries;
} module_import;

extern unsigned_32 DefStackSizePE( void );

#endif
