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


#include "coffflhn.h"
#include "orlhash.h"

static void free_coff_file_hnd( coff_file_handle coff_file_hnd )
{
    int                 loop;
    coff_sec_handle     coff_sec_hnd;

    if( coff_file_hnd->coff_sec_hnd ) {
        for( loop = 0; loop < coff_file_hnd->num_sections; loop++ ) {
            coff_sec_hnd = coff_file_hnd->coff_sec_hnd[loop];
            if( coff_sec_hnd->type == ORL_SEC_TYPE_RELOCS ) {
                if( coff_sec_hnd->assoc.reloc.relocs ) {
                    _ClientFree( coff_file_hnd, coff_sec_hnd->assoc.reloc.relocs );
                }
            }
            if( coff_sec_hnd->name_alloced ) {
                _ClientFree( coff_file_hnd, coff_sec_hnd->name );
            }
            _ClientFree( coff_file_hnd, coff_file_hnd->coff_sec_hnd[loop] );
        }
        _ClientFree( coff_file_hnd, coff_file_hnd->coff_sec_hnd );
    }
    _ClientFree( coff_file_hnd, coff_file_hnd->orig_sec_hnd );
    if( coff_file_hnd->symbol_handles ) {
        for( loop = 0; loop < coff_file_hnd->num_symbols; loop++ ) {
            if( coff_file_hnd->symbol_handles[loop].name_alloced ) {
                _ClientFree( coff_file_hnd, coff_file_hnd->symbol_handles[loop].name );
            }
            loop += coff_file_hnd->symbol_handles[loop].symbol->num_aux;
        }
        _ClientFree( coff_file_hnd, coff_file_hnd->symbol_handles );
    }
    if( coff_file_hnd->sec_name_hash_table ) {
        ORLHashTableFree( coff_file_hnd->sec_name_hash_table );
    }
    if( coff_file_hnd->implib_data != NULL ) {
        _ClientFree( coff_file_hnd, coff_file_hnd->implib_data );
    }
    _ClientFree( coff_file_hnd, coff_file_hnd );
}

void CoffAddFileLinks( coff_handle coff_hnd, coff_file_handle coff_file_hnd )
{
    coff_file_hnd->next = coff_hnd->first_file_hnd;
    coff_file_hnd->coff_hnd = coff_hnd;
    coff_hnd->first_file_hnd = coff_file_hnd;
}

orl_return CoffRemoveFileLinks( coff_file_handle coff_file_hnd )
{
    coff_handle                         coff_hnd;
    coff_file_handle                    current;

    coff_hnd = coff_file_hnd->coff_hnd;

    if( coff_hnd->first_file_hnd == coff_file_hnd ) {
        coff_hnd->first_file_hnd = coff_file_hnd->next;
        free_coff_file_hnd( coff_file_hnd );
        return( ORL_OKAY );
    } else {
        current = coff_hnd->first_file_hnd;
        while( current->next != NULL ) {
            if( current->next == coff_file_hnd ) {
                current->next = coff_file_hnd->next;
                free_coff_file_hnd( coff_file_hnd );
                return( ORL_OKAY );
            } else {
                current = current->next;
            }
        }
    }
    return( ORL_ERROR );
}
