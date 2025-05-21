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
* Description:  WGML GML tags definition header.
*               only tags with a routinename other than gml_dummy
*               are processed so far (not always complete)
*
*    layout tags are defined in gtagslay.h
*
*    :cmt :imbed :include   are defined here and in gtagslay.h identically
*    other tags for example :abstract are defined differently
*
*    If an eXXX tag exists for a XXX tag, it must follow immediately
*
****************************************************************************/


//       tagname  tagname  routinename     gmlflags locflags classflags
// upper case     length

  pick( ABSTRACT, 8,      gml_abstract,   tag_no_flag, TLOC_restricted, TCLS_no_class )

  pick( ADDRESS,  7,      gml_address,    tag_no_flag, TLOC_titlep,     TCLS_no_class )
  pick( EADDRESS, 8,      gml_eaddress,   tag_no_flag, TLOC_titlep | TLOC_address,  TCLS_no_class )

  pick( ALINE,    5,      gml_aline,      tag_out_txt, TLOC_address,    TCLS_no_class )
  pick( APPENDIX, 8,      gml_appendix,   tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( AUTHOR,   6,      gml_author,     tag_out_txt, TLOC_titlep,     TCLS_no_class )
  pick( BACKM,    5,      gml_backm,      tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( BINCLUDE, 8,      gml_binclude,   tag_no_flag, TLOC_titlep | TLOC_address,  TCLS_no_class )
  pick( BODY,     4,      gml_body,       tag_no_flag, TLOC_restricted, TCLS_no_class )

  pick( CIT,      3,      gml_cit,        tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( ECIT,     4,      gml_ecit,       tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( CMT,      3,      gml_cmt,        tag_only | tag_is_general, TLOC_restricted,  TCLS_no_class )
  pick( DATE,     4,      gml_date,       tag_out_txt, TLOC_titlep,     TCLS_no_class )
  pick( DD,       2,      gml_dd,         tag_out_txt, TLOC_restricted, TCLS_def | TCLS_list )
  pick( DDHD,     4,      gml_ddhd,       tag_out_txt, TLOC_restricted, TCLS_def | TCLS_list )

  pick( DL,       2,      gml_dl,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EDL,      3,      gml_edl,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( DOCNUM,   6,      gml_docnum,     tag_out_txt, TLOC_titlep,     TCLS_no_class )
  pick( DT,       2,      gml_dt,         tag_out_txt, TLOC_restricted, TCLS_list )
  pick( DTHD,     4,      gml_dthd,       tag_out_txt, TLOC_restricted, TCLS_list )

  pick( FIG,      3,      gml_fig,        tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EFIG,     4,      gml_efig,       tag_out_txt, TLOC_figcap,     TCLS_no_class )

  pick( FIGCAP,   6,      gml_figcap,     tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( FIGDESC,  7,      gml_figdesc,    tag_out_txt, TLOC_figcap,     TCLS_no_class )
  pick( FIGLIST,  7,      gml_figlist,    tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( FIGREF,   6,      gml_figref,     tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( FN,       2,      gml_fn,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EFN,      3,      gml_efn,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( FNREF,    5,      gml_fnref,      tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( FRONTM,   6,      gml_frontm,     tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( GDOC,     4,      gml_gdoc,       tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( EGDOC,    5,      gml_egdoc,      tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( GL,       2,      gml_gl,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EGL,      3,      gml_egl,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( GD,       2,      gml_gd,         tag_out_txt, TLOC_restricted, TCLS_def | TCLS_list )
  pick( GRAPHIC,  7,      gml_graphic,    tag_out_txt, TLOC_titlep | TLOC_address,  TCLS_no_class )
  pick( GT,       2,      gml_gt,         tag_out_txt, TLOC_restricted, TCLS_list )
  pick( H0,       2,      gml_h0,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( H1,       2,      gml_h1,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( H2,       2,      gml_h2,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( H3,       2,      gml_h3,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( H4,       2,      gml_h4,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( H5,       2,      gml_h5,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( H6,       2,      gml_h6,         tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( HDREF,    5,      gml_hdref,      tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( HP0,      3,      gml_hp0,        tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( EHP0,     4,      gml_ehp0,       tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( HP1,      3,      gml_hp1,        tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( EHP1,     4,      gml_ehp1,       tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( HP2,      3,      gml_hp2,        tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( EHP2,     4,      gml_ehp2,       tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( HP3,      3,      gml_hp3,        tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( EHP3,     4,      gml_ehp3,       tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( I1,       2,      gml_i1,         tag_out_txt, TLOC_restricted, TCLS_index )
  pick( I2,       2,      gml_i2,         tag_out_txt, TLOC_restricted, TCLS_index )
  pick( I3,       2,      gml_i3,         tag_out_txt, TLOC_restricted, TCLS_index )
  pick( IH1,      3,      gml_ih1,        tag_out_txt, TLOC_restricted, TCLS_index )
  pick( IH2,      3,      gml_ih2,        tag_out_txt, TLOC_restricted, TCLS_index )
  pick( IH3,      3,      gml_ih3,        tag_out_txt, TLOC_restricted, TCLS_index )

  pick( IMBED,    5,      gml_include,    tag_is_general, TLOC_restricted, TCLS_no_class )
  pick( INCLUDE,  7,      gml_include,    tag_is_general, TLOC_restricted, TCLS_no_class )
  pick( INDEX,    5,      gml_index,      tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( IREF,     4,      gml_iref,       tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( LI,       2,      gml_li,         tag_out_txt, TLOC_restricted, TCLS_li_lp | TCLS_list )
  pick( LIREF,    5,      gml_dummy,      tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( LP,       2,      gml_lp,         tag_out_txt, TLOC_restricted, TCLS_li_lp | TCLS_list )

  pick( LQ,       2,      gml_lq,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( ELQ,      3,      gml_elq,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( NOTE,     4,      gml_note,       tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( OL,       2,      gml_ol,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EOL,      3,      gml_eol,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( P,        1,      gml_p,          tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( PB,       2,      gml_pb,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( PC,       2,      gml_pc,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( PREFACE,  7,      gml_preface,    tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( PSC,      3,      gml_dummy,      tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EPSC,     4,      gml_dummy,      tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( Q,        1,      gml_q,          tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( EQ,       2,      gml_eq,         tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( SET,      3,      gml_set,        tag_is_general, TLOC_restricted, TCLS_li_lp )

  pick( SF,       2,      gml_sf,         tag_out_txt, TLOC_restricted, TCLS_ip_start )
  pick( ESF,      3,      gml_esf,        tag_out_txt, TLOC_restricted, TCLS_ip_end )

  pick( SL,       2,      gml_sl,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( ESL,      3,      gml_esl,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( TITLE,    5,      gml_title,      tag_out_txt, TLOC_titlep,     TCLS_no_class )

  pick( TITLEP,   6,      gml_titlep,     tag_no_flag, TLOC_restricted, TCLS_no_class )
  pick( ETITLEP,  7,      gml_etitlep,    tag_out_txt, TLOC_titlep,     TCLS_no_class )

  pick( TOC,      3,      gml_toc,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( UL,       2,      gml_ul,         tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EUL,      3,      gml_eul,        tag_out_txt, TLOC_restricted, TCLS_no_class )

  pick( XMP,      3,      gml_xmp,        tag_out_txt, TLOC_restricted, TCLS_no_class )
  pick( EXMP,     4,      gml_exmp,       tag_out_txt, TLOC_restricted, TCLS_no_class )
