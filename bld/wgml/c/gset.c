/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
 
#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */
 
#include "wgml.h"
#include "gvars.h"
 
 
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
    char        *   p;
    char        *   symstart;
    char        *   valstart;
    char            c;
    bool            symbolthere = false;
    bool            valuethere = false;
    symvar          sym;
    sub_index       subscript;
    int             rc;
    symvar      * * working_dict;
 
    entry = entry;
    subscript = no_subscript;           // not subscripted
    scan_err = false;
 
    p = scan_start;
    p++;
 
    for( ;;) {
        while( *p == ' ' ) {            // over WS to attribute
            p++;
        }
 
        if( !strnicmp( "symbol", p, 6 ) ) {
 
            p += 6;
            while( *p == ' ' ) {        // over WS to attribute
                p++;
            }
            if( *p == '=' ) {
                p++;
                while( *p == ' ' ) {    // over WS to attribute
                    p++;
                }
            } else {
                continue;
            }
            symstart = p;
 
            p = scan_sym( symstart, &sym, &subscript );
            if( scan_err ) {
                return;
            }
            if( *p == '"' || *p == '\'' ) {
                p++;                    // skip terminating quote
            }
            if( sym.flags & local_var ) {
                working_dict = &input_cbs->local_dict;
            } else {
                working_dict = &global_dict;
            }
            symbolthere = true;
 
            while( *p == ' ' ) {
                p++;
            }
        } else {
 
            if( !strnicmp( "value", p, 5 ) ) {
                char    quote;
 
                p += 5;
                while( *p == ' ' ) {    // over WS to attribute
                    p++;
                }
                if( *p == '=' ) {
                    p++;
                    while( *p == ' ' ) {// over WS to attribute
                        p++;
                    }
                } else {
                    continue;
                }
                if( *p == '"' || *p == '\'' ) {
                    quote = *p;
                    ++p;
                } else {
                    quote = ' ';
                }
                valstart = p;
                while( *p && *p != quote ) {
                    ++p;
                }
                c = *p;
                *p = '\0';
                strcpy_s( token_buf, buf_size, valstart );
                *p = c;
                if( c == '"' || c == '\'' ) {
                    p++;
                }
                valuethere = true;
            } else {
                char    linestr[MAX_L_AS_STR];
 
                err_count++;
 
                g_err( err_att_name_inv );
                if( input_cbs->fmflags & II_macro ) {
                    ultoa( input_cbs->s.m->lineno, linestr, 10 );
                    g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
                } else {
                    ultoa( input_cbs->s.f->lineno, linestr, 10 );
                    g_info( inf_file_line, linestr, input_cbs->s.f->filename );
                }
                if( inc_level > 1 ) {
                    show_include_stack();
                }
                break;
 
            }
        }
        if( symbolthere && valuethere ) {   // both attributes
 
            if( !strnicmp( token_buf, "delete", 6 ) ) {
                sym.flags |= deleted;
            }
            rc = add_symvar( working_dict, sym.name, token_buf, subscript,
                             sym.flags );
            break;                          // tag complete with attributes
        }
 
        c = *p;
        if( p >= scan_stop ) {
            c = '.';                    // simulate end of tag if EOF
 
            if( !(input_cbs->fmflags & II_eof) ) {
                if( get_line( true ) ) {      // next line for missing attribute
 
                    process_line();
                    scan_start = buff2;
                    scan_stop  = buff2 + buff2_lg - 1;
                    if( (*scan_start == SCR_char) ||
                        (*scan_start == GML_char) ) {
                                        //  missing attribute not supplied error
 
                    } else {
                        p = scan_start;
                        continue;       // scanning
                    }
                }
            }
        }
        if( c == '.' ) {                // end of tag found
            char    linestr[MAX_L_AS_STR];
 
            err_count++;
            // AT-001 Required attribute not found
 
            g_err( err_att_missing );
            if( input_cbs->fmflags & II_macro ) {
                ultoa( input_cbs->s.m->lineno, linestr, 10 );
                g_info( inf_mac_line, linestr, input_cbs->s.m->mac->name );
            } else {
                ultoa( input_cbs->s.f->lineno, linestr, 10 );
                g_info( inf_file_line, linestr, input_cbs->s.f->filename );
            }
            if( inc_level > 1 ) {
                show_include_stack();
            }
            break;
        }
    }
    scan_start = scan_stop + 1;
    return;
}
 
