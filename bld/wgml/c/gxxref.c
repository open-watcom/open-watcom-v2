/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML tags :HDREF :FIGREF :FNREF processing
*
*                         :FIGREF :FNREF not yet implemented    TBD
****************************************************************************/
#include    "wgml.h"
#include    "gvars.h"


/***************************************************************************/
/*         :HDREF refid='id-name'                                          */
/*                [page=yes                                                */
/*                      no].                                               */
/*                                                                         */
/* This tag causes a heading reference to be generated.  The heading       */
/* reference tag is a paragraph element, and is used with text to create   */
/* the content of a basic document element.  The heading text from the     */
/* referenced heading is enclosed in double quotation marks and inserted   */
/* into the formatted document.  The refid attribute will determine the    */
/* heading for which the reference will be generated.  The specified       */
/* identifier name must be the value of the id attribute on the heading    */
/* tag you wish to reference.  The page attribute controls the output of   */
/* the heading page number.  If the attribute value yes is specified, the  */
/* text "on page" followed by the page number of the referenced heading is */
/* placed after the heading text.  If the attribute value no is specified, */
/* the page number of the referenced heading is not generated.  If the     */
/* page attribute is not specified, the heading page number is generated   */
/* when the heading and the reference to it are not on the same output     */
/* page.                                                                   */
/***************************************************************************/

void    gml_hdref( const gmltag * entry )
{
    char    *   p;
    char    *   pa;
    char    *   pe;
    char    *   idp;
    char        quote;
    char        c;
    bool        idseen;
    bool        pageseen;
    bool        withpage;
    size_t      len;
    char        buf64[64];
    ref_entry   *   re;
    static char undefid[] = "\"Undefined Heading\" on page XXX";


    idseen = false;
    pageseen = false;
    withpage = false;
    p = scan_start;
    re = NULL;

    /***********************************************************************/
    /*  Scan attributes  for :HDREF                                        */
    /*  id=                                                                */
    /*  page=                                                              */
    /***********************************************************************/

    for( ;; ) {
        while( *p == ' ' ) {
            p++;
        }
        if( *p == '\0' || *p == '.'  ) {
            break;                      // tag end found
        }

        if( !strnicmp( "page=", p, 5 ) ) {
            p += 5;
            while( *p == ' ' ) {
                p++;
            }
            pa = p;
            if( !strnicmp( "yes", p, 3 ) ) {
                pageseen = true;
                withpage = true;
                p += 3;
            } else {
                if( !strnicmp( "no", p, 2 ) ) {
                    pageseen = true;
                    withpage = false;
                    p += 2;
                } else {
                   g_err( err_inv_att_val );
                   file_mac_info();
                   err_count++;
                   while( *p && (*p != '.') && (*p != ' ') ) p++;
                }
            }
            scan_start = p;
            continue;
        }

        if( !strnicmp( "refid=", p, 6 ) ) {
            p += 6;
            while( *p == ' ' ) {
                p++;
            }
            if( is_quote_char( *p ) ) {
                quote = *p;
                p++;
            } else {
                quote = '\0';
            }
            pa = p;
            while( *p && is_id_char( *p ) ) {
                p++;
            }
            len = min( ID_LEN, p - pa );// restrict length as in ghx.c

            if( len > 0 ) {
                idseen = true;          // valid id attribute found
                pe = pa + len;
                c = *pe;
                *pe = '\0';
                re = find_refid( ref_dict, strlwr( pa ) );
                if( re != NULL ) {      // id found in ref dict
                    idp = mem_alloc( 4 + strlen( re->text_cap ) );
                    *idp = '"';         // decorate with quotes
                    strcpy( idp + 1, re->text_cap );
                    strcat( idp, "\"" );
                } else {
                    if( GlobalFlags.lastpass ) {
                        g_warn( wng_id_xxx, pa );
                        g_info( inf_id_unknown );
                        file_mac_info();
                        wng_count++;
                    }
                }
                *pe = c;
            }
            if( *p && (quote == *p) ) {
                p++;
            }

            scan_start = p;
            continue;
        }

        /*******************************************************************/
        /* no more valid attributes                                        */
        /*******************************************************************/
        break;
    }
    if( *p == '.' ) {                   // tag end ?
        p++;
    }
    if( idseen ) {                      // id attribute was specified
        bool concatsave = ProcFlags.concat;

        ProcFlags.concat = true;        // make process_text add to line
        if( re == NULL ) {              // undefined refid
            process_text( undefid, g_curr_font_num );
        } else {
            process_text( idp, g_curr_font_num );
            if( withpage || (!pageseen && (page != re->pageno)) ) {
                sprintf_s( buf64, sizeof( buf64 ), "on page %d", re->pageno );
                process_text( buf64, g_curr_font_num );
            }
            mem_free( idp );
        }
        ProcFlags.concat = concatsave;
    } else {
        g_err( err_att_missing );       // id attribute missing
        file_mac_info();
        err_count++;
    }

    scan_start = p;
    return;
}


