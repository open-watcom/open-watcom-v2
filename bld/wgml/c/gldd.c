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
* Description: WGML implement :DD   tag for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :DD     attributes                                                    */
/***************************************************************************/
static const lay_att    dd_att[] = {
    e_line_left, e_font
};

/*****************************************************************************/
/*Define the characteristics of the data description entity.                 */
/*                                                                           */
/*:DD                                                                        */
/*        line_left = '0.5i'                                                 */
/*        font = 0                                                           */
/*                                                                           */
/*line_left This attribute accepts any valid horizontal space unit. The      */
/*specified amount of space must be available on the output line after the   */
/*definition term which precedes the data description. If there is not       */
/*enough space, the data description will be started on the next output line.*/
/*                                                                           */
/*font This attribute accepts a non-negative integer number. If a font       */
/*number is used for which no font has been defined, WATCOM                  */
/*Script/GML will use font zero. The font numbers from zero to three         */
/*correspond directly to the highlighting levels specified by the            */
/*highlighting phrase GML tags. The font attribute defines the font of       */
/*the data description.                                                      */
/*                                                                           */
/*****************************************************************************/


/***************************************************************************/
/*  lay_dd                                                                 */
/***************************************************************************/

void    lay_dd( const gmltag * entry )
{
    char                *p;
    condcode            cc;
    int                 cvterr;
    int                 k;
    lay_att             curr;
    att_name_type       attr_name;
    att_val_type        attr_val;
    struct {
        unsigned    line_left       :1;
        unsigned    font            :1;
    } AttrFlags;

    p = g_scandata.s;
    cvterr = false;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( dd_att ); k++ ) {
            curr = dd_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_line_left:
                    if( AttrFlags.line_left ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val,
                                           &layout_work.dd.line_left );
                    AttrFlags.line_left = true;
                    break;
                case e_font:
                    if( AttrFlags.font ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_font_number( p, &attr_val, &layout_work.dd.font );
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
/*   :DD        output definition data attribute values                    */
/***************************************************************************/
void    put_lay_dd( FILE *fp, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":DD\n" );

    for( k = 0; k < TABLE_SIZE( dd_att ); k++ ) {
        curr = dd_att[k];
        switch( curr ) {
        case e_line_left:
            o_space_unit( fp, curr, &lay->dd.line_left );
            break;
        case e_font:
            o_font_number( fp, curr, &lay->dd.font );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}
