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
#include "omforl.h"
#include "orlhash.h"


orl_return      OmfParseComments( omf_sec_handle sh, orl_note_callbacks *cbs, void *cookie )
{
    omf_quantity                x;
    orl_return                  return_val;
    omf_comment                 comment;
    ORL_STRUCT( omf_scan_tab )  st_entry;
    orl_sec_handle              csh;

    assert( sh );
    assert( cbs );

    for( x = 0; x < sh->assoc.comment.num; x++ ) {
        comment = sh->assoc.comment.comments[x];
        assert( comment );

        switch( comment->class ) {
        case( CMT_DEFAULT_LIBRARY ):
            if( cbs->deflib_fn != NULL ) {
                return_val = cbs->deflib_fn( (char *)comment->data, cookie );
                if( return_val != ORL_OKAY ) {
                    return( return_val );
                }
            }
            break;
        case( CMT_DISASM_DIRECTIVE ):
            if( cbs->scantab_fn != NULL ) {
                return_val = OmfParseScanTab( comment->data, comment->len, &st_entry );
                if( return_val == ORL_OKAY ) {
                    csh = (orl_sec_handle)OmfFindSegOrComdat( sh->omf_file_hnd, st_entry.seg, st_entry.lname );
                    if( csh != ORL_NULL_HANDLE ) {
                        return_val = cbs->scantab_fn( csh, &st_entry.start, &st_entry.end, cookie );
                        if( return_val != ORL_OKAY ) {
                            return( return_val );
                        }
                    }
                }
            }
            break;
        }
    }

    return( ORL_OKAY );
}
