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
* Description: WGML implement several sub tags for :LAYOUT processing
*                   all those with only font attribute
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*                     several  attributes  with only font as value        */
/*                                                                         */
/* :DT :GT :DTHD :CIT :GD :DDHD :IXPGNUM :IXMAJOR                          */
/*                                                                         */
/***************************************************************************/

static const lay_att    xx_att[] = {
    e_font
};


void    lay_xx( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    font_number         *fontptr;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    l_tags              ltag;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    font            :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;
    ltag = entry->u.layid;
    if( ltag == TL_CIT ) {
        fontptr = &layout_work.cit.font;
    } else if( ltag == TL_DTHD ) {
        fontptr = &layout_work.dthd.font;
    } else if( ltag == TL_DT ) {
        fontptr = &layout_work.dt.font;
    } else if( ltag == TL_GT ) {
        fontptr = &layout_work.gt.font;
    } else if( ltag == TL_GD ) {
        fontptr = &layout_work.gd.font;
    } else if( ltag == TL_DDHD ) {
        fontptr = &layout_work.ddhd.font;
    } else if( ltag == TL_IXPGNUM ) {
        fontptr = &layout_work.ixpgnum.font;
    } else if( ltag == TL_IXMAJOR ) {
        fontptr = &layout_work.ixmajor.font;
    } else {
        internal_err_exit( __FILE__, __LINE__ );
        /* never return */
    }
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != ltag ) {
        ProcFlags.lay_xxx = ltag;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( xx_att ); k++ ) {
            curr = xx_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, fontptr );
                    AttrFlags.font = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                if( cvterr ) {          // there was an error
                    xx_err_exit( ERR_ATT_VAL_INV );
                    /* never return */
                }
                break;                  // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err_exit( ERR_ATT_NAME_INV );
            /* never return */
        }
    }
    g_scandata.s = g_scandata.e;
    return;
}



/***************************************************************************/
/*   :xx        output for font only value                                 */
/***************************************************************************/
static  void    put_lay_xx( FILE *fp, font_number *font, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":%s\n", name );

    for( k = 0; k < TABLE_SIZE( xx_att ); k++ ) {
        curr = xx_att[k];
        switch( curr ) {
        case e_font:
            o_font_number( fp, curr, font );
            break;
        default:
            out_msg( "WGML logic error glconvrt.c.\n");
            err_count++;
            break;
        }
    }
}

void    put_lay_dt( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->dt.font), "DT" );
}

void    put_lay_gt( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->gt.font), "GT" );
}

void    put_lay_dthd( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->dthd.font), "DTHD" );
}

void    put_lay_cit( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->cit.font), "CIT" );
}

void    put_lay_gd( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->gd.font), "GD" );
}

void    put_lay_ddhd( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->ddhd.font), "DDHD" );
}

void    put_lay_ixpgnum( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->ixpgnum.font), "IXPGNUM" );
}

void    put_lay_ixmajor( FILE *fp, layout_data * lay )
{
    put_lay_xx( fp, &(lay->ixmajor.font), "IXMAJOR" );
}
