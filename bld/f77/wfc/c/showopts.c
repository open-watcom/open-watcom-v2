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


//
// SHOWOPTS     : print option information
//

#include "ftnstd.h"
#include "optflags.h"
#include "cpopt.h"
#include "inout.h"

#include <string.h>

extern  void                    MsgBuffer(uint,char *,...);

extern  opt_entry       CompOptns[];

#define OPTION_TAB      11
#define COLUMN_TAB      41
#define CATEGORY_TAB    31


void    ShowOptions( char *buff ) {
//=================================

    opt_entry   *opt;
    char        pad[COLUMN_TAB];
    size_t      len;
    size_t      desc_len;
    int         column;

    column = 1;
    for( opt = CompOptns; opt->option != NULL; ++opt ) {

        // ignore hidden development options
        if( opt->description == 0 ) continue;

        MsgBuffer( opt->description, buff );
        if( opt->flags & CTG ) {
            len = strlen( buff );
            memset( pad, ' ', CATEGORY_TAB - len / 2 );
            pad[ CATEGORY_TAB - len / 2 ] = NULLCHAR;
            if( column == 0 ) {
                TOutNL( "" );
            }
            TOut( pad );
            TOutNL( buff );
            column = 1;
        } else {
            MsgBuffer( opt->description, buff );
            desc_len = strlen( buff );
            if( column == 0 ) {
                if( OPTION_TAB + desc_len >= COLUMN_TAB ) {
                    TOutNL( "" );
                }
            }
            len = strlen( opt->option );
            TOut( opt->option );
            memset( pad, ' ', OPTION_TAB - len - 1 );
            pad[ OPTION_TAB - len - 1 ] = NULLCHAR;
            TOut( pad );
            if( column == 1 ) {
                if( desc_len < COLUMN_TAB - OPTION_TAB ) {
                    TOut( buff );
                    memset( pad, ' ', COLUMN_TAB - OPTION_TAB - desc_len );
                    pad[ COLUMN_TAB - OPTION_TAB - desc_len ] = NULLCHAR;
                    TOut( pad );
                    column = 0;
                } else {
                    TOutNL( buff );
                    column = 1;
                }
            } else {
                TOutNL( buff );
                column = 1;
            }
        }
    }
    if( column == 0 ) {
        TOutNL( "" );
    }
}
