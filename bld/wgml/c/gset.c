/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GML :SET processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :SET symbol='symbol-name'                                             */
/*        value='character-string'                                         */
/*              delete.                                                    */
/*                                                                         */
/* This tag defines and assigns a value to a symbol name.  The symbol      */
/* attribute must be specified.  The value of this attribute is the name   */
/* of the symbol being defined, and cannot have a length greater than ten  */
/* characters.  The symbol name may only contain letters, numbers, and the */
/* characters @, #, $ and underscore(_).  The value attribute must be      */
/* specified.  The attribute value delete or a valid character string may  */
/* be assigned to the symbol name.  If the attribute value delete is used, */
/* the symbol referred to by the symbol name is deleted.                   */
/***************************************************************************/

extern  void    gml_set( const gmltag * entry )
{
    bool            symbol_found;
    bool            value_found;
    char            *p;
    char            *pa;
    int             rc;
    symvar          sym;
    sub_index       subscript;
    att_name_type   attr_name;
    att_val_type    attr_val;

    (void)entry;

    symbol_found = false;
    value_found = false;
    subscript = SI_no_subscript;           // not subscripted
    g_scan_err = false;
    sym.flags = SF_none;

    p = g_scandata.s;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;;) {
            p = get_tag_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line )
                break;
            if( ProcFlags.tag_end_found )
                break;
            if( strcmp( "symbol", attr_name.attname.t ) == 0 ) {

                /* both get_att_value() and scan_sym() must be used */

                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                if( attr_val.quoted != ' ' )
                    attr_val.tok.s--;
                scan_sym( attr_val.tok.s, &sym, &subscript, NULL, false );
                if( g_scan_err ) {
                    break;
                }
                symbol_found = true;
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "value", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.tok.s == NULL ) {
                    break;
                }
                value_found = true;
                if( strcmp( "delete", attr_val.specval ) == 0 && attr_val.quoted == ' ' ) {
                    sym.flags |= SF_deleted;
                } else {
                    sym.flags &= ~SF_deleted;
                    if( attr_val.tok.l > BUF_SIZE )
                        attr_val.tok.l = BUF_SIZE;
                    strncpy( token_buf, attr_val.tok.s, attr_val.tok.l );
                    token_buf[attr_val.tok.l] = '\0';
                }
                break;
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    if( symbol_found && value_found ) {   // both attributes
        if( (sym.flags & SF_deleted) == 0 ) {
            rc = add_symvar_sym( &sym, token_buf, strlen( token_buf ), subscript, sym.flags );
        }
    } else {
        xx_err_exit( ERR_ATT_MISSING );
        /* never return */
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {
            post_space = 0;
            ProcFlags.ct = true;
            process_text( p, g_curr_font);  // if text follows
        }
    }
    g_scandata.s = g_scandata.e;
    return;
}

