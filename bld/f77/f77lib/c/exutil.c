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
* Description:  Run-time statement processor utilities
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "rundat.h"
#include "rtutls.h"
#include "exutil.h"
#include "ioerr.h"

#include <string.h>


#define MAX_KW_LEN      15      // longest keyword in following tables

char    *SpecId[] = {
        "STATUS",
        "ACCESS",
        "FORM",
        "BLANK",
        "CARRIAGECONTROL",
        "RECORDTYPE",
        "ACTION",
        "RECL",
        "BLOCKSIZE",
        "SHARE"
};


int     FindKWord( char **table, int id, int def_id, string PGM *kw ) {
//=====================================================================

    int         index;
    int         kw_len;
    char        kword[MAX_KW_LEN+1];

    index = def_id;
    if( kw != NULL ) {
        index = 0;
        for( index = 0; ; index++ ) {
            if( table[ index ] == NULL ) {
                IOErr( IO_BAD_SPEC, SpecId[ id ] );
            }
            kw_len = StrItem( kw, kword, MAX_KW_LEN );
            if( kw_len != strlen( table[ index ] ) ) continue;
            if( memicmp( table[ index ], kword, kw_len ) == 0 ) break;
        }
        ++index;
    }
    return( index );
}


void    ClrBuff( void ) {
//=================

    IOCB->fileinfo->col = 0;
    IOCB->fileinfo->len = 0;
}
