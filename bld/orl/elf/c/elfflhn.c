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


#include "elfflhn.h"
#include "elforl.h"
#include "orlhash.h"

static void free_elf_file_hnd( elf_file_handle elf_file_hnd )
{
    int                         loop;
    elf_sec_handle              elf_sec_hnd;

    if( elf_file_hnd->elf_sec_hnd ) {
        for( loop = 0; loop < elf_file_hnd->num_sections; loop++ ) {
            elf_sec_hnd = elf_file_hnd->elf_sec_hnd[loop];
            switch( elf_sec_hnd->type ) {
                case ORL_SEC_TYPE_RELOCS:
                case ORL_SEC_TYPE_RELOCS_EXPADD:
                    if( elf_sec_hnd->assoc.reloc.relocs ) {
                        _ClientFree( elf_file_hnd, elf_sec_hnd->assoc.reloc.relocs );
                    }
                    break;
                case ORL_SEC_TYPE_SYM_TABLE:
                case ORL_SEC_TYPE_DYN_SYM_TABLE:
                    if( elf_sec_hnd->assoc.sym.symbols ) {
                        _ClientFree( elf_file_hnd, elf_sec_hnd->assoc.sym.symbols );
                    }
                    break;
                default:
                    break;
            }
            _ClientFree( elf_file_hnd, elf_sec_hnd );
        }
        _ClientFree( elf_file_hnd, elf_file_hnd->elf_sec_hnd );
    }
    _ClientFree( elf_file_hnd, elf_file_hnd->orig_sec_hnd );
    if( elf_file_hnd->sec_name_hash_table ) {
        ORLHashTableFree( elf_file_hnd->sec_name_hash_table );
    }
    _ClientFree( elf_file_hnd, elf_file_hnd );
}

void ElfAddFileLinks( elf_handle elf_hnd, elf_file_handle elf_file_hnd )
{
    elf_file_hnd->next = elf_hnd->first_file_hnd;
    elf_file_hnd->elf_hnd = elf_hnd;
    elf_hnd->first_file_hnd = elf_file_hnd;
}

orl_return ElfRemoveFileLinks( elf_file_handle elf_file_hnd )
{
    elf_handle                          elf_hnd;
    elf_file_handle                     current;

    elf_hnd = elf_file_hnd->elf_hnd;

    if( elf_hnd->first_file_hnd == elf_file_hnd ) {
        elf_hnd->first_file_hnd = elf_file_hnd->next;
        free_elf_file_hnd( elf_file_hnd );
        return( ORL_OKAY );
    } else {
        current = elf_hnd->first_file_hnd;
        while( current->next != NULL ) {
            if( current->next == elf_file_hnd ) {
                current->next = elf_file_hnd->next;
                free_elf_file_hnd( elf_file_hnd );
                return( ORL_OKAY );
            } else {
                current = current->next;
            }
        }
    }
    return( ORL_ERROR );
}
