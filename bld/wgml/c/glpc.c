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
* Description: WGML implement :P and :PC tags for LAYOUT processing
*
****************************************************************************/


#include "wgml.h"

#include "clibext.h"


/***************************************************************************/
/*   :P and :PC    attributes                                              */
/***************************************************************************/
static const lay_att    p_att[] = {
    e_line_indent, e_pre_skip, e_post_skip
};


/********************************************************************************/
/*Define the characteristics of the paragraph entity.                           */
/*                                                                              */
/*:P                                                                            */
/*        line_indent = 0                                                       */
/*        pre_skip = 1                                                          */
/*        post_skip = 0                                                         */
/*                                                                              */
/*line_indent The line_indent attribute accepts any valid horizontal space unit.*/
/*This attribute specifies the amount of indentation for the first output       */
/*line of the paragraph.                                                        */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the paragraph. The pre-skip will be merged with the previous                  */
/*document entity's post-skip value. If a pre-skip occurs at the                */
/*beginning of an output page, the pre-skip value has no effect.                */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the paragraph. The post-skip will be merged with the next                     */
/*document entity's pre-skip value. If a post-skip occurs at the end of         */
/*an output page, any remaining part of the skip is not carried over to         */
/*the next output page.                                                         */
/********************************************************************************/


/********************************************************************************/
/*Define the characteristics of the paragraph continuation entity.              */
/*:PC                                                                           */
/*        line_indent = 0                                                       */
/*        pre_skip = 1                                                          */
/*        post_skip = 0                                                         */
/*                                                                              */
/*line_indent The line_indent attribute accepts any valid horizontal space unit.*/
/*This attribute specifies the amount of indentation for the first output       */
/*line of the paragraph continuation.                                           */
/*                                                                              */
/*pre_skip This attribute accepts vertical space units. A zero value means that */
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped before            */
/*the paragraph continuation. The pre-skip will be merged with the              */
/*previous document entity's post-skip value. If a pre-skip occurs at           */
/*the beginning of an output page, the pre-skip value has no effect.            */
/*                                                                              */
/*post_skip This attribute accepts vertical space units. A zero value means that*/
/*no lines are skipped. If the skip value is a line unit, it is multiplied      */
/*by the current line spacing (see "Vertical Space Unit" on page 77 for         */
/*more information). The resulting amount of space is skipped after             */
/*the paragraph continuation. The post-skip will be merged with the             */
/*next document entity's pre-skip value. If a post-skip occurs at the           */
/*end of an output page, any remaining part of the skip is not carried          */
/*over to the next output page.                                                 */
/********************************************************************************/

static void process_arg( p_lay_tag *p_or_pc )
{
    char            *p;
    int             cvterr;
    int             k;
    lay_att         curr;
    condcode        cc;
    att_name_type   attr_name;
    att_val_type    attr_val;
    struct {
        unsigned    line_indent     :1;
        unsigned    pre_skip        :1;
        unsigned    post_skip       :1;
    } AttrFlags;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags

    while( (cc = lay_attr_and_value( &attr_name, &attr_val )) == CC_pos ) {   // get att with value
        cvterr = -1;
        for( k = 0; k < TABLE_SIZE( p_att ); k++ ) {
            curr = p_att[k];
            if( strcmp( lay_att_names[curr], attr_name.attname.l ) == 0 ) {
                p = attr_val.tok.s;
                switch( curr ) {
                case e_line_indent:
                    if( AttrFlags.line_indent ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &p_or_pc->line_indent );
                    AttrFlags.line_indent = true;
                    break;
                case e_pre_skip:
                    if( AttrFlags.pre_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &p_or_pc->pre_skip );
                    AttrFlags.pre_skip = true;
                    break;
                case e_post_skip:
                    if( AttrFlags.post_skip ) {
                        xx_line_err_exit_ci( ERR_ATT_DUP, attr_name.tok.s,
                            attr_val.tok.s - attr_name.tok.s + attr_val.tok.l);
                        /* never return */
                    }
                    cvterr = i_space_unit( p, &attr_val, &p_or_pc->post_skip );
                    AttrFlags.post_skip = true;
                    break;
                default:
                    internal_err_exit( __FILE__, __LINE__ );
                    /* never return */
                }
                if( cvterr ) {              // there was an error
                    xx_err_exit( ERR_ATT_VAL_INV );
                    /* never return */
                }
                break;                      // break out of for loop
            }
        }
        if( cvterr < 0 ) {
            xx_err_exit( ERR_ATT_NAME_INV );
            /* never return */
        }
    }
}


/***************************************************************************/
/*  lay_p                                                                  */
/***************************************************************************/

void    lay_p( const gmltag * entry )
{
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    process_arg( &layout_work.p );
    g_scandata.s = g_scandata.e;
}


/***************************************************************************/
/*  lay_pc                                                                 */
/***************************************************************************/

void    lay_pc( const gmltag * entry )
{
    if( ProcFlags.lay_xxx != entry->u.layid ) {
        ProcFlags.lay_xxx = entry->u.layid;
    }
    process_arg( &layout_work.pc );
    g_scandata.s = g_scandata.e;
}


/***************************************************************************/
/*   :P         output paragraph attribute values                          */
/*   :PC        output paragraph continue attribute values                 */
/***************************************************************************/
static  void    put_lay_p_pc( FILE *fp, p_lay_tag * ap, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf( fp, ":%s\n", name );

    for( k = 0; k < TABLE_SIZE( p_att ); k++ ) {
        curr = p_att[k];
        switch( curr ) {
        case e_line_indent:
            o_space_unit( fp, curr, &ap->line_indent );
            break;
        case e_pre_skip:
            o_space_unit( fp, curr, &ap->pre_skip );
            break;
        case e_post_skip:
            o_space_unit( fp, curr, &ap->post_skip );
            break;
        default:
            internal_err_exit( __FILE__, __LINE__ );
            /* never return */
        }
    }
}

void    put_lay_p( FILE *fp, layout_data * lay )
{
    put_lay_p_pc( fp, &(lay->p), "P" );
}

void    put_lay_pc( FILE *fp, layout_data * lay )
{
    put_lay_p_pc( fp, &(lay->pc), "PC" );
}
