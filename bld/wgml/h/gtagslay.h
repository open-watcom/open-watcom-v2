/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML layout tags definition header.
*               only tags with a routinename other than lay_dummy
*               are processed so far
*
*    for normal GML tags see gtags.h
*
*    :cmt :imbed :include   are defined here and in gtags.h
*    other tags for example :abstract are defined differently
****************************************************************************/

#ifndef pick
    #error macro pick not defined
#endif
//      tagname      tagname  routinename     flags
// upper case        length

  pick( ABSTRACT,     8,      lay_abspref,    0 )
  pick( ADDRESS,      7,      lay_address,    0 )
  pick( ALINE,        5,      lay_aline,      0 )
  pick( APPENDIX,     8,      lay_appendix,   0 )
  pick( ATTN,         4,      lay_dummy,      0 )  // letter not supported
  pick( AUTHOR,       6,      lay_author,     0 )
  pick( BACKM,        5,      lay_backbod,    0 )
  pick( BANNER,       6,      lay_banner,     0 )
  pick( BANREGION,    9,      lay_banregion,  0 )
  pick( BODY,         4,      lay_backbod,    0 )
  pick( CIT,          3,      lay_xx,         0 )
  pick( CLOSE,        5,      lay_dummy,      0 )  // letter not supported
  pick( CMT,          3,      gml_cmt,        tag_only )
  pick( CONVERT,      7,      lay_convert,    0 )
  pick( DATE,         4,      lay_date,       0 )
  pick( DD,           2,      lay_dd,         0 )
  pick( DDHD,         4,      lay_xx,         0 )
  pick( DEFAULT,      7,      lay_default,    0 )
  pick( DL,           2,      lay_dl,         0 )
  pick( DISTRIB,      7,      lay_dummy,      0 )  // letter not supported
  pick( DOCNUM,       6,      lay_docnum,     0 )
  pick( DT,           2,      lay_xx,         0 )
  pick( DTHD,         4,      lay_xx,         0 )
  pick( EBANNER,      7,      lay_ebanner,    0 )
  pick( EBANREGION,  10,      lay_ebanregion, 0 )
  pick( ECLOSE,       6,      lay_dummy,      0 )  // letter not supported
  pick( ELAYOUT,      7,      lay_elayout,    0 )
  pick( FIG,          3,      lay_fig,        0 )
  pick( FIGCAP,       6,      lay_figcap,     0 )
  pick( FIGDESC,      7,      lay_figdesc,    0 )
  pick( FIGLIST,      7,      lay_figlist,    0 )
  pick( FLPGNUM,      7,      lay_flpgnum,    0 )
  pick( FN,           2,      lay_fn,         0 )
  pick( FNREF,        5,      lay_fnref,      0 )
  pick( FROM,         4,      lay_dummy,      0 )  // letter not supported
  pick( GD,           2,      lay_xx,         0 )
  pick( GL,           2,      lay_gl,         0 )
  pick( GT,           2,      lay_xx,         0 )
  pick( HEADING,      7,      lay_heading,    0 )
  pick( H0,           2,      lay_hx,         0 )
  pick( H1,           2,      lay_hx,         0 )
  pick( H2,           2,      lay_hx,         0 )
  pick( H3,           2,      lay_hx,         0 )
  pick( H4,           2,      lay_hx,         0 )
  pick( H5,           2,      lay_hx,         0 )
  pick( H6,           2,      lay_hx,         0 )
  pick( I1,           2,      lay_ix,         0 )
  pick( I2,           2,      lay_ix,         0 )
  pick( I3,           2,      lay_ix,         0 )
  pick( IMBED,        5,      gml_include,    tag_is_general )
  pick( INCLUDE,      7,      gml_include,    tag_is_general )
  pick( INDEX,        5,      lay_index,      0 )
  pick( IXHEAD,       6,      lay_ixhead,     0 )
  pick( IXMAJOR,      7,      lay_xx,         0 )
  pick( IXPGNUM,      7,      lay_xx,         0 )
  pick( LETDATE,      7,      lay_dummy,      0 )  // letter not supported
  pick( LP,           2,      lay_lp,         0 )
  pick( LQ,           2,      lay_lq,         0 )
  pick( NOTE,         4,      lay_note,       0 )
  pick( OL,           2,      lay_ol,         0 )
  pick( OPEN,         4,      lay_dummy,      0 )  // letter not supported
  pick( P,            1,      lay_p,          0 )
  pick( PAGE,         4,      lay_page,       0 )
  pick( PC,           2,      lay_pc,         0 )
  pick( PREFACE,      7,      lay_abspref,    0 )
  pick( PSC,          3,      lay_dummy,      0 )
  pick( SAVE,         4,      lay_dummy,      0 )
  pick( SL,           2,      lay_sl,         0 )
  pick( SUBJECT,      7,      lay_dummy,      0 )  // letter not supported
  pick( TITLE,        5,      lay_title,      0 )
  pick( TITLEP,       6,      lay_titlep,     0 )
  pick( TO,           2,      lay_dummy,      0 )  // letter not supported
  pick( TOC,          3,      lay_toc,        0 )
  pick( TOCH0,        5,      lay_tochx,      0 )
  pick( TOCH1,        5,      lay_tochx,      0 )
  pick( TOCH2,        5,      lay_tochx,      0 )
  pick( TOCH3,        5,      lay_tochx,      0 )
  pick( TOCH4,        5,      lay_tochx,      0 )
  pick( TOCH5,        5,      lay_tochx,      0 )
  pick( TOCH6,        5,      lay_tochx,      0 )
  pick( TOCPGNUM,     8,      lay_tocpgnum,   0 )
  pick( UL,           2,      lay_ul,         0 )
  pick( WIDOW,        5,      lay_widow,      0 )
  pick( XMP,          3,      lay_xmp,        0 )

#undef  pick
