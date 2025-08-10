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
* Description:  WGML layout tags definition header.
*               only tags with a routinename other than lay_dummy
*               are processed so far
*
*    for normal GML tags see gtags.h
*
*    :cmt :imbed :include   are defined here and in gtags.h
*    other tags for example :abstract are defined differently
****************************************************************************/


//      tagname      tagname  funci           funco             gmlflags          locflags
// upper case        length

  pick( ABSTRACT,     8,      lay_abspref,    put_lay_abstract, TFLG_none,      TLOC_restricted )
  pick( ADDRESS,      7,      lay_address,    put_lay_address,  TFLG_none,      TLOC_restricted )
  pick( ALINE,        5,      lay_aline,      put_lay_aline,    TFLG_none,      TLOC_restricted )
  pick( APPENDIX,     8,      lay_appendix,   put_lay_appendix, TFLG_none,      TLOC_restricted )
  pick( ATTN,         4,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( AUTHOR,       6,      lay_author,     put_lay_author,   TFLG_none,      TLOC_restricted )
  pick( BACKM,        5,      lay_backbod,    put_lay_backm,    TFLG_none,      TLOC_restricted )
  pick( BANNER,       6,      lay_banner,     put_lay_banner,   TFLG_none,      TLOC_restricted )
  pick( BANREGION,    9,      lay_banregion,  put_lay_dummy,    TFLG_none,      TLOC_banner )
  pick( BODY,         4,      lay_backbod,    put_lay_body,     TFLG_none,      TLOC_restricted )
  pick( CIT,          3,      lay_xx,         put_lay_cit,      TFLG_none,      TLOC_restricted )
  pick( CLOSE,        5,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( CMT,          3,      gml_cmt,        put_lay_dummy,    TFLG_only,      TLOC_restricted )
  pick( CONVERT,      7,      lay_convert,    put_lay_convert,  TFLG_none,      TLOC_restricted )
  pick( DATE,         4,      lay_date,       put_lay_date,     TFLG_none,      TLOC_restricted )
  pick( DD,           2,      lay_dd,         put_lay_dd,       TFLG_none,      TLOC_restricted )
  pick( DDHD,         4,      lay_xx,         put_lay_ddhd,     TFLG_none,      TLOC_restricted )
  pick( DEFAULT,      7,      lay_default,    put_lay_default,  TFLG_none,      TLOC_restricted )
  pick( DL,           2,      lay_dl,         put_lay_dl,       TFLG_none,      TLOC_restricted )
  pick( DISTRIB,      7,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( DOCNUM,       6,      lay_docnum,     put_lay_docnum,   TFLG_none,      TLOC_restricted )
  pick( DT,           2,      lay_xx,         put_lay_dt,       TFLG_none,      TLOC_restricted )
  pick( DTHD,         4,      lay_xx,         put_lay_dthd,     TFLG_none,      TLOC_restricted )
  pick( EBANNER,      7,      lay_ebanner,    put_lay_dummy,    TFLG_none,      TLOC_banner )
  pick( EBANREGION,  10,      lay_ebanregion, put_lay_dummy,    TFLG_none,      TLOC_banreg )
  pick( ECLOSE,       6,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( ELAYOUT,      7,      lay_elayout,    put_lay_dummy,    TFLG_none,      TLOC_restricted )
  pick( FIG,          3,      lay_fig,        put_lay_fig,      TFLG_none,      TLOC_restricted )
  pick( FIGCAP,       6,      lay_figcap,     put_lay_figcap,   TFLG_none,      TLOC_restricted )
  pick( FIGDESC,      7,      lay_figdesc,    put_lay_figdesc,  TFLG_none,      TLOC_restricted )
  pick( FIGLIST,      7,      lay_figlist,    put_lay_figlist,  TFLG_none,      TLOC_restricted )
  pick( FLPGNUM,      7,      lay_flpgnum,    put_lay_flpgnum,  TFLG_none,      TLOC_restricted )
  pick( FN,           2,      lay_fn,         put_lay_fn,       TFLG_none,      TLOC_restricted )
  pick( FNREF,        5,      lay_fnref,      put_lay_fnref,    TFLG_none,      TLOC_restricted )
  pick( FROM,         4,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( GD,           2,      lay_xx,         put_lay_gd,       TFLG_none,      TLOC_restricted )
  pick( GL,           2,      lay_gl,         put_lay_gl,       TFLG_none,      TLOC_restricted )
  pick( GT,           2,      lay_xx,         put_lay_gt,       TFLG_none,      TLOC_restricted )
  pick( HEADING,      7,      lay_heading,    put_lay_heading,  TFLG_none,      TLOC_restricted )
  pick( H0,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( H1,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( H2,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( H3,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( H4,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( H5,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( H6,           2,      lay_hx,         put_lay_hx,       TFLG_none,      TLOC_restricted )
  pick( I1,           2,      lay_ix,         put_lay_ix,       TFLG_none,      TLOC_restricted )
  pick( I2,           2,      lay_ix,         put_lay_ix,       TFLG_none,      TLOC_restricted )
  pick( I3,           2,      lay_ix,         put_lay_ix,       TFLG_none,      TLOC_restricted )
  pick( IMBED,        5,      gml_include,    put_lay_dummy,    TFLG_is_general,   TLOC_restricted )
  pick( INCLUDE,      7,      gml_include,    put_lay_dummy,    TFLG_is_general,   TLOC_restricted )
  pick( INDEX,        5,      lay_index,      put_lay_index,    TFLG_none,      TLOC_restricted )
  pick( IXHEAD,       6,      lay_ixhead,     put_lay_ixhead,   TFLG_none,      TLOC_restricted )
  pick( IXMAJOR,      7,      lay_xx,         put_lay_ixmajor,  TFLG_none,      TLOC_restricted )
  pick( IXPGNUM,      7,      lay_xx,         put_lay_ixpgnum,  TFLG_none,      TLOC_restricted )
  pick( LAYOUT,       6,      lay_layout,     put_lay_dummy,    TFLG_none,      TLOC_restricted )
  pick( LETDATE,      7,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( LP,           2,      lay_lp,         put_lay_lp,       TFLG_none,      TLOC_restricted )
  pick( LQ,           2,      lay_lq,         put_lay_lq,       TFLG_none,      TLOC_restricted )
  pick( NOTE,         4,      lay_note,       put_lay_note,     TFLG_none,      TLOC_restricted )
  pick( OL,           2,      lay_ol,         put_lay_ol,       TFLG_none,      TLOC_restricted )
  pick( OPEN,         4,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( P,            1,      lay_p,          put_lay_p,        TFLG_none,      TLOC_restricted )
  pick( PAGE,         4,      lay_page,       put_lay_page,     TFLG_none,      TLOC_restricted )
  pick( PC,           2,      lay_pc,         put_lay_pc,       TFLG_none,      TLOC_restricted )
  pick( PREFACE,      7,      lay_abspref,    put_lay_preface,  TFLG_none,      TLOC_restricted )
  pick( PSC,          3,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )
  pick( SAVE,         4,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )
  pick( SL,           2,      lay_sl,         put_lay_sl,       TFLG_none,      TLOC_restricted )
  pick( SUBJECT,      7,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( TITLE,        5,      lay_title,      put_lay_title,    TFLG_none,      TLOC_restricted )
  pick( TITLEP,       6,      lay_titlep,     put_lay_titlep,   TFLG_none,      TLOC_restricted )
  pick( TO,           2,      lay_dummy,      put_lay_dummy,    TFLG_none,      TLOC_restricted )    // letter not supported
  pick( TOC,          3,      lay_toc,        put_lay_toc,      TFLG_none,      TLOC_restricted )
  pick( TOCH0,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCH1,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCH2,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCH3,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCH4,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCH5,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCH6,        5,      lay_tochx,      put_lay_tochx,    TFLG_none,      TLOC_restricted )
  pick( TOCPGNUM,     8,      lay_tocpgnum,   put_lay_tocpgnum, TFLG_none,      TLOC_restricted )
  pick( UL,           2,      lay_ul,         put_lay_ul,       TFLG_none,      TLOC_restricted )
  pick( WIDOW,        5,      lay_widow,      put_lay_widow,    TFLG_none,      TLOC_restricted )
  pick( XMP,          3,      lay_xmp,        put_lay_xmp,      TFLG_none,      TLOC_restricted )

