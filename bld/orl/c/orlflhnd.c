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


#include "orllevel.h"
#include "orlflhnd.h"

static void free_orl_file_hnd( orli_file_handle orli_file_hnd )
{
    ORL_FUNCS_FREE( orli_file_hnd->orli_hnd, orli_file_hnd );
}

void ORLAddFileLinks( orli_handle orli_hnd, orli_file_handle orli_file_hnd )
{
    orli_file_hnd->next = orli_hnd->first_file_hnd;
    orli_file_hnd->orli_hnd = orli_hnd;
    orli_hnd->first_file_hnd = orli_file_hnd;
}

orl_return ORLRemoveFileLinks( orli_file_handle orli_file_hnd )
{
    orli_handle                         orli_hnd;
    orli_file_handle                    current;

    orli_hnd = orli_file_hnd->orli_hnd;

    if( orli_hnd->first_file_hnd == orli_file_hnd ) {
        orli_hnd->first_file_hnd = orli_file_hnd->next;
        free_orl_file_hnd( orli_file_hnd );
        return( ORL_OKAY );
    } else {
        current = orli_hnd->first_file_hnd;
        while( current->next != NULL ) {
            if( current->next == orli_file_hnd ) {
                current->next = orli_file_hnd->next;
                free_orl_file_hnd( orli_file_hnd );
                return( ORL_OKAY );
            } else {
                current = current->next;
            }
        }
    }
    return( ORL_ERROR );
}
