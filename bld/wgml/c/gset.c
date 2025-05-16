/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
    bool            symbol_found    = false;
    bool            value_found     = false;
    char        *   p;
    char        *   pa;
    int             rc;
    symvar          sym;
    sub_index       subscript;
    symdict_hdl     working_dict;
    att_name_type   attr_name;
    att_val_type    attr_val;
    tok_type        val;

    (void)entry;

    subscript = no_subscript;           // not subscripted
    g_scan_err = false;

    p = scandata.s;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;;) {
            p = get_att_name( p, &pa, &attr_name );
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( strcmp( "symbol", attr_name.attname.t ) == 0 ) {

                /* both get_att_value() and scan_sym() must be used */

                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                scan_sym( attr_val.name, &sym, &subscript, NULL, false );
                if( g_scan_err ) {
                    break;
                }
                symbol_found = true;
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( strcmp( "value", attr_name.attname.t ) == 0 ) {
                p = get_att_value( p, &attr_val );
                if( attr_val.name == NULL ) {
                    break;
                }
                value_found = true;
                if( strcmp( "delete", attr_val.specval ) == 0 && attr_val.quoted == ' ' ) {
                    sym.flags |= deleted;
                } else {
                    if( attr_val.len > BUF_SIZE - 1 )
                        attr_val.len = BUF_SIZE - 1;
                    strncpy( token_buf, attr_val.name, attr_val.len );
                    token_buf[attr_val.len] = '\0';
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    if( symbol_found && value_found ) {   // both attributes
        if( sym.flags & local_var ) {
            working_dict = input_cbs->local_dict;
        } else {
            working_dict = global_dict;
        }
        val.s = token_buf;
        val.e = val.s + strlen( val.s );
        rc = add_symvar( working_dict, sym.name, &val, subscript, sym.flags );
    } else {
        xx_err( err_att_missing );
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {
            post_space = 0;
            ProcFlags.ct = true;
            process_text( p, g_curr_font);  // if text follows
        }
    }
    scandata.s = scandata.e;
    return;
}

