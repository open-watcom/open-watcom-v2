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
* Description:  OMF comment parsing.
*
****************************************************************************/


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "omfdrctv.h"
#include "omfmunge.h"
#include "omfload.h"
#include "orlhash.h"
#include "pcobj.h"


orl_return      OmfParseComments( omf_sec_handle sh, orl_note_callbacks *cb,
                                  void *cookie )
{
    omf_quantity        x;
    orl_return          err = ORL_OKAY;
    omf_comment_struct  *comment;
    omf_scan_tab_struct st;
    omf_sec_handle      csh;

    assert( sh );
    assert( cb );

    for( x = 0; x < sh->assoc.comment.num; x++ ) {
        err = ORL_OKAY;
        comment = sh->assoc.comment.comments[x];
        assert( comment );

        switch( comment->class ) {
        case( CMT_DEFAULT_LIBRARY ):
            if( cb->deflib_fn ) {
                err = cb->deflib_fn( (char *)comment->data, cookie );
            }
            break;
        case( CMT_DISASM_DIRECTIVE ):
            if( !cb->scantab_fn ) continue;
            err = OmfParseScanTab( comment->data, comment->len, &st );
            if( err != ORL_OKAY ) continue;

            csh = OmfFindSegOrComdat( sh->omf_file_hnd, st.seg, st.lname );
            if( !csh ) continue;

            err = cb->scantab_fn((orl_sec_handle)csh, st.start, st.end, cookie);
        }
        if( err != ORL_OKAY ) break;
    }

    return( err );
}
