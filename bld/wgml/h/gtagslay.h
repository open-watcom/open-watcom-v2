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


//      tagname      tagname  routinename     gmlflags          locflags
// upper case        length

  pick( ABSTRACT,     8,      lay_abspref,    tag_no_flag,      TLOC_restricted )
  pick( ADDRESS,      7,      lay_address,    tag_no_flag,      TLOC_restricted )
  pick( ALINE,        5,      lay_aline,      tag_no_flag,      TLOC_restricted )
  pick( APPENDIX,     8,      lay_appendix,   tag_no_flag,      TLOC_restricted )
  pick( ATTN,         4,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( AUTHOR,       6,      lay_author,     tag_no_flag,      TLOC_restricted )
  pick( BACKM,        5,      lay_backbod,    tag_no_flag,      TLOC_restricted )
  pick( BANNER,       6,      lay_banner,     tag_no_flag,      TLOC_restricted )
  pick( BANREGION,    9,      lay_banregion,  tag_no_flag,      TLOC_banner )
  pick( BODY,         4,      lay_backbod,    tag_no_flag,      TLOC_restricted )
  pick( CIT,          3,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( CLOSE,        5,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( CMT,          3,      gml_cmt,        tag_only,         TLOC_restricted )
  pick( CONVERT,      7,      lay_convert,    tag_no_flag,      TLOC_restricted )
  pick( DATE,         4,      lay_date,       tag_no_flag,      TLOC_restricted )
  pick( DD,           2,      lay_dd,         tag_no_flag,      TLOC_restricted )
  pick( DDHD,         4,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( DEFAULT,      7,      lay_default,    tag_no_flag,      TLOC_restricted )
  pick( DL,           2,      lay_dl,         tag_no_flag,      TLOC_restricted )
  pick( DISTRIB,      7,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( DOCNUM,       6,      lay_docnum,     tag_no_flag,      TLOC_restricted )
  pick( DT,           2,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( DTHD,         4,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( EBANNER,      7,      lay_ebanner,    tag_no_flag,      TLOC_banner )
  pick( EBANREGION,  10,      lay_ebanregion, tag_no_flag,      TLOC_banreg )
  pick( ECLOSE,       6,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( ELAYOUT,      7,      lay_elayout,    tag_no_flag,      TLOC_restricted )
  pick( FIG,          3,      lay_fig,        tag_no_flag,      TLOC_restricted )
  pick( FIGCAP,       6,      lay_figcap,     tag_no_flag,      TLOC_restricted )
  pick( FIGDESC,      7,      lay_figdesc,    tag_no_flag,      TLOC_restricted )
  pick( FIGLIST,      7,      lay_figlist,    tag_no_flag,      TLOC_restricted )
  pick( FLPGNUM,      7,      lay_flpgnum,    tag_no_flag,      TLOC_restricted )
  pick( FN,           2,      lay_fn,         tag_no_flag,      TLOC_restricted )
  pick( FNREF,        5,      lay_fnref,      tag_no_flag,      TLOC_restricted )
  pick( FROM,         4,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( GD,           2,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( GL,           2,      lay_gl,         tag_no_flag,      TLOC_restricted )
  pick( GT,           2,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( HEADING,      7,      lay_heading,    tag_no_flag,      TLOC_restricted )
  pick( H0,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( H1,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( H2,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( H3,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( H4,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( H5,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( H6,           2,      lay_hx,         tag_no_flag,      TLOC_restricted )
  pick( I1,           2,      lay_ix,         tag_no_flag,      TLOC_restricted )
  pick( I2,           2,      lay_ix,         tag_no_flag,      TLOC_restricted )
  pick( I3,           2,      lay_ix,         tag_no_flag,      TLOC_restricted )
  pick( IMBED,        5,      gml_include,    tag_is_general,   TLOC_restricted )
  pick( INCLUDE,      7,      gml_include,    tag_is_general,   TLOC_restricted )
  pick( INDEX,        5,      lay_index,      tag_no_flag,      TLOC_restricted )
  pick( IXHEAD,       6,      lay_ixhead,     tag_no_flag,      TLOC_restricted )
  pick( IXMAJOR,      7,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( IXPGNUM,      7,      lay_xx,         tag_no_flag,      TLOC_restricted )
  pick( LAYOUT,       6,      lay_layout,     tag_no_flag,      TLOC_restricted )
  pick( LETDATE,      7,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( LP,           2,      lay_lp,         tag_no_flag,      TLOC_restricted )
  pick( LQ,           2,      lay_lq,         tag_no_flag,      TLOC_restricted )
  pick( NOTE,         4,      lay_note,       tag_no_flag,      TLOC_restricted )
  pick( OL,           2,      lay_ol,         tag_no_flag,      TLOC_restricted )
  pick( OPEN,         4,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( P,            1,      lay_p,          tag_no_flag,      TLOC_restricted )
  pick( PAGE,         4,      lay_page,       tag_no_flag,      TLOC_restricted )
  pick( PC,           2,      lay_pc,         tag_no_flag,      TLOC_restricted )
  pick( PREFACE,      7,      lay_abspref,    tag_no_flag,      TLOC_restricted )
  pick( PSC,          3,      lay_dummy,      tag_no_flag,      TLOC_restricted )
  pick( SAVE,         4,      lay_dummy,      tag_no_flag,      TLOC_restricted )
  pick( SL,           2,      lay_sl,         tag_no_flag,      TLOC_restricted )
  pick( SUBJECT,      7,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( TITLE,        5,      lay_title,      tag_no_flag,      TLOC_restricted )
  pick( TITLEP,       6,      lay_titlep,     tag_no_flag,      TLOC_restricted )
  pick( TO,           2,      lay_dummy,      tag_no_flag,      TLOC_restricted )    // letter not supported
  pick( TOC,          3,      lay_toc,        tag_no_flag,      TLOC_restricted )
  pick( TOCH0,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCH1,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCH2,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCH3,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCH4,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCH5,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCH6,        5,      lay_tochx,      tag_no_flag,      TLOC_restricted )
  pick( TOCPGNUM,     8,      lay_tocpgnum,   tag_no_flag,      TLOC_restricted )
  pick( UL,           2,      lay_ul,         tag_no_flag,      TLOC_restricted )
  pick( WIDOW,        5,      lay_widow,      tag_no_flag,      TLOC_restricted )
  pick( XMP,          3,      lay_xmp,        tag_no_flag,      TLOC_restricted )
