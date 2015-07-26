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

#include "exepe.h"

#define PE_DEFAULT_BASE     (0x400000UL)
#define PE_DEF_STACK_SIZE   (1024 * 1024UL)
#define PE_DEF_STACK_COMMIT (64 * 1024UL)
#define PE_DEF_HEAP_SIZE    (8 * 1024UL)
#define PE_DEF_HEAP_COMMIT  (4 * 1024UL)
#define DEF_VALUE           (0xFFFFFFFFUL)

extern void             DoAddResource( char * );
extern void             FiniPELoadFile( void );
extern void             ReadPEExportTable( f_handle, pe_hdr_table_entry * );
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
    name_list           *imp;
};

typedef struct module_import {
    struct module_import        *next;
    struct name_list            *mod;
    struct import_name          *imports;
    unsigned                    num_entries;
} module_import;

extern unsigned_32 StackSizePE( void );
