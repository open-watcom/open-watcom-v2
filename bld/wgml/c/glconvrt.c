/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 200--2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description: WGML implement :CONVERT LAYOUT tag
*                              and file output for all subtags
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/* Layout attributes as character strings                                  */
/***************************************************************************/

const char    * const lay_att_names[] = {
    #define pick( name, funci, funco, result ) #name,
    #include "glayutil.h"
    #undef pick
};


/***************************************************************************/
/*                                                                         */
/*  Format: :CONVERT file='file name'.                                     */
/*                                                                         */
/*  alternate, undocumented but used format:                               */
/*                                                                         */
/*          :CONVERT.FILE.EXT                                              */
/*                                                                         */
/* Convert the current layout into the specified file name. The resulting  */
/* file will contain the entire layout in a readable form.                 */
/***************************************************************************/


/***************************************************************************/
/*   :XXXXX     output for unsupported LETTER tags                         */
/***************************************************************************/
static  void    put_lay_letter_unsupported( FILE *fp )
{

    fprintf( fp, ":FROM\n");
    fprintf( fp, "        left_adjust = 0\n");
    fprintf( fp, "        page_position = right\n");
    fprintf( fp, "        pre_top_skip = 6\n");
    fprintf( fp, "        font = 0\n");

    fprintf( fp, ":TO\n");
    fprintf( fp, "        left_adjust = 0\n");
    fprintf( fp, "        page_position = left\n");
    fprintf( fp, "        pre_top_skip = 1\n");
    fprintf( fp, "        font = 0\n");

    fprintf( fp, ":ATTN\n");
    fprintf( fp, "        left_adjust = 0\n");
    fprintf( fp, "        page_position = left\n");
    fprintf( fp, "        pre_top_skip = 1\n");
    fprintf( fp, "        font = 1\n");
    fprintf( fp, "        attn_string = \"Attention: \"\n");
    fprintf( fp, "        string_font = 1\n");

    fprintf( fp, ":SUBJECT\n");
    fprintf( fp, "        left_adjust = 0\n");
    fprintf( fp, "        page_position = centre\n");
    fprintf( fp, "        pre_top_skip = 2\n");
    fprintf( fp, "        font = 1\n");

    fprintf( fp, ":LETDATE\n");
    fprintf( fp, "        date_form = \"$ml $dsn, $yl\"\n");
    fprintf( fp, "        depth = 15\n");
    fprintf( fp, "        font = 0\n");
    fprintf( fp, "        page_position = right\n");

    fprintf( fp, ":OPEN\n");
    fprintf( fp, "        pre_top_skip = 2\n");
    fprintf( fp, "        font = 0\n");
    fprintf( fp, "        delim = ':'\n");

    fprintf( fp, ":CLOSE\n");
    fprintf( fp, "        pre_skip = 2\n");
    fprintf( fp, "        depth = 6\n");
    fprintf( fp, "        font = 0\n");
    fprintf( fp, "        page_position = centre\n");
    fprintf( fp, "        delim = ','\n");
    fprintf( fp, "        extract_threshold = 2\n");

    fprintf( fp, ":ECLOSE\n");
    fprintf( fp, "        pre_skip = 1\n");
    fprintf( fp, "        font = 0\n");

    fprintf( fp, ":DISTRIB\n");
    fprintf( fp, "        pre_top_skip = 3\n");
    fprintf( fp, "        skip = 1\n");
    fprintf( fp, "        font = 0\n");
    fprintf( fp, "        indent = '0.5i'\n");
    fprintf( fp, "        page_eject = no\n");
}


/***************************************************************************/
/*   output layout data to file                                            */
/***************************************************************************/
static  void    put_layout( char * name, layout_data * lay )
{
    static  FILE    *fp;
    symsub          *   sversion;

    fp = fopen( name, "wt" );
    if( fp == NULL ) {
        out_msg( "open error %s\n", name );
        return;
    }

    fprintf( fp, ":LAYOUT\n" );
//  if( GlobalFlags.research ) {
        find_symvar( global_dict, "$version", SI_no_subscript, &sversion );
        fprintf( fp, ":cmt. Created with %s\n", sversion->value );
//  }

    put_lay_page( fp, lay );
    put_lay_default( fp, lay );
    put_lay_widow( fp, lay );
    put_lay_fn( fp, lay );
    put_lay_fnref( fp, lay );
    put_lay_p( fp, lay );
    put_lay_pc( fp, lay );
    put_lay_fig( fp, lay );
    put_lay_xmp( fp, lay );
    put_lay_note( fp, lay );
    put_lay_hx( fp, lay );
    put_lay_heading( fp, lay );
    put_lay_lq( fp, lay );
    put_lay_dt( fp, lay );
    put_lay_gt( fp, lay );
    put_lay_dthd( fp, lay );
    put_lay_cit( fp, lay );
    put_lay_figcap( fp, lay );
    put_lay_figdesc( fp, lay );
    put_lay_dd( fp, lay );
    put_lay_gd( fp, lay );
    put_lay_ddhd( fp, lay );
    put_lay_abstract( fp, lay );
    put_lay_preface( fp, lay );
    put_lay_body( fp, lay );
    put_lay_backm( fp, lay );
    put_lay_lp( fp, lay );
    put_lay_index( fp, lay );
    put_lay_ixpgnum( fp, lay );
    put_lay_ixmajor( fp, lay );
    put_lay_ixhead( fp, lay );
    put_lay_ix( fp, lay );
    put_lay_toc( fp, lay );
    put_lay_tocpgnum( fp, lay );
    put_lay_tochx( fp, lay );
    put_lay_figlist( fp, lay );
    put_lay_flpgnum( fp, lay );
    put_lay_titlep( fp, lay );
    put_lay_title( fp, lay );
    put_lay_docnum( fp, lay );
    put_lay_date( fp, lay );
    put_lay_author( fp, lay );
    put_lay_address( fp, lay );
    put_lay_aline( fp, lay );

    put_lay_letter_unsupported( fp );  // dummy output

    put_lay_appendix( fp, lay );
    put_lay_sl( fp, lay );
    put_lay_ol( fp, lay );
    put_lay_ul( fp, lay );
    put_lay_dl( fp, lay );
    put_lay_gl( fp, lay );
    put_lay_banner( fp, lay );

    fprintf( fp, ":eLAYOUT\n" );

    fclose( fp );
}


/***************************************************************************/
/*  lay_convert   process :CONVERT tag                                     */
/***************************************************************************/

void    lay_convert( const gmltag * entry )
{
    char        *   p;

    (void)entry;

    p = g_scandata.s;
    g_scandata.s = g_scandata.e;

    if( !GlobalFlags.firstpass ) {
        ProcFlags.layout = true;
        return;                         // process during first pass only
    }
    SkipSpaces( p );
    *token_buf = '\0';
    if( strnicmp( "file=", p, 5 ) == 0 ) {  // file attribute?
        char    quote;
        char    *filename_start;

        p += 5;
        if( *p == '"'
          || *p == '\'' ) {
            quote = *p;
            ++p;
        } else {
            quote = '.';                // error?? filename without quotes
        }
        filename_start = p;
        while( *p != '\0' && *p != quote ) {
            ++p;
        }
        *p = '\0';
        strcpy( token_buf, filename_start );
    } else {                            // try undocumented format
        if( *p != '\0'
          && *p == '.' ) {
            strcpy( token_buf, p + 1 );
        }
    }
    if( *token_buf == '\0' ) {           // file name missing
        xx_err_exit( ERR_ATT_MISSING );
        /* never return */
    }
    put_layout( token_buf, &layout_work );
    return;
}
