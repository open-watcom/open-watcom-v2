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

//       tagname  tagname  routinename     gmlflags                     locflags
// upper case     length

  pick( ABSTRACT, 8,      gml_abstract,   0,                           0 )

  pick( ADDRESS,  7,      gml_address,    0,                           titlep_tag )
  pick( EADDRESS, 8,      gml_eaddress,   0,                           titlep_tag | address_tag )

  pick( ALINE,    5,      gml_aline,      tag_out_txt,                 address_tag )
  pick( APPENDIX, 8,      gml_appendix,   0,                           0 )
  pick( AUTHOR,   6,      gml_author,     tag_out_txt,                 titlep_tag  )
  pick( BACKM,    5,      gml_backm,      0,                           0 )
  pick( BINCLUDE, 8,      gml_binclude,   0,                           titlep_tag | address_tag )
  pick( BODY,     4,      gml_body,       0,                           0 )

  pick( CIT,      3,      gml_dummy,      tag_out_txt,                 0 )
  pick( ECIT,     4,      gml_dummy,      tag_out_txt,                 0 )

  pick( CMT,      3,      gml_cmt,        tag_only | tag_is_general,   0 )
  pick( DATE,     4,      gml_date,       tag_out_txt,                 titlep_tag )
  pick( DD,       2,      gml_dummy,      tag_out_txt,                 0 )
  pick( DDHD,     4,      gml_dummy,      tag_out_txt,                 0 )

  pick( DL,       2,      gml_dl,         tag_out_txt,                 0 )
  pick( EDL,      3,      gml_edl,        tag_out_txt,                 0 )

  pick( DOCNUM,   6,      gml_docnum,     tag_out_txt,                 titlep_tag )
  pick( DT,       2,      gml_dummy,      tag_out_txt,                 0 )
  pick( DTHD,     4,      gml_dummy,      tag_out_txt,                 0 )

  pick( FIG,      3,      gml_dummy,      tag_out_txt,                 0 )
  pick( EFIG,     4,      gml_dummy,      tag_out_txt,                 0 )

  pick( FIGCAP,   6,      gml_dummy,      tag_out_txt,                 0 )
  pick( FIGDESC,  7,      gml_dummy,      tag_out_txt,                 0 )
  pick( FIGLIST,  7,      gml_figlist,    tag_out_txt,                 0 )
  pick( FIGREF,   6,      gml_dummy,      tag_out_txt,                 0 )

  pick( FN,       2,      gml_dummy,      tag_out_txt,                 0 )
  pick( EFN,      3,      gml_dummy,      tag_out_txt,                 0 )

  pick( FNREF,    5,      gml_dummy,      tag_out_txt,                 0 )
  pick( FRONTM,   6,      gml_frontm,     0,                           0 )
  pick( GDOC,     4,      gml_gdoc,       0,                           0 )
  pick( EGDOC,    5,      gml_egdoc,      tag_out_txt,                 0 )

  pick( GL,       2,      gml_gl,         tag_out_txt,                 0 )
  pick( EGL,      3,      gml_egl,        tag_out_txt,                 0 )

  pick( GD,       2,      gml_dummy,      tag_out_txt,                 0 )
  pick( GRAPHIC,  7,      gml_graphic,    tag_out_txt,                 titlep_tag | address_tag )
  pick( GT,       2,      gml_dummy,      tag_out_txt,                 0 )
  pick( H0,       2,      gml_h0,         0,                           0 )
  pick( H1,       2,      gml_h1,         0,                           0 )
  pick( H2,       2,      gml_h2,         0,                           0 )
  pick( H3,       2,      gml_h3,         0,                           0 )
  pick( H4,       2,      gml_h4,         0,                           0 )
  pick( H5,       2,      gml_h5,         0,                           0 )
  pick( H6,       2,      gml_h6,         0,                           0 )
  pick( HDREF,    5,      gml_hdref,      tag_out_txt,                 0 )

  pick( HP0,      3,      gml_hp0,        tag_out_txt,                 0 )
  pick( EHP0,     4,      gml_ehp0,       tag_out_txt,                 0 )

  pick( HP1,      3,      gml_hp1,        tag_out_txt,                 0 )
  pick( EHP1,     4,      gml_ehp1,       tag_out_txt,                 0 )

  pick( HP2,      3,      gml_hp2,        tag_out_txt,                 0 )
  pick( EHP2,     4,      gml_ehp2,       tag_out_txt,                 0 )

  pick( HP3,      3,      gml_hp3,        tag_out_txt,                 0 )
  pick( EHP3,     4,      gml_ehp3,       tag_out_txt,                 0 )

  pick( I1,       2,      gml_i1,         tag_out_txt,                 0 )
  pick( I2,       2,      gml_i2,         tag_out_txt,                 0 )
  pick( I3,       2,      gml_i3,         tag_out_txt,                 0 )
  pick( IH1,      3,      gml_ih1,        tag_out_txt,                 0 )
  pick( IH2,      3,      gml_ih2,        tag_out_txt,                 0 )
  pick( IH3,      3,      gml_ih3,        tag_out_txt,                 0 )
  pick( IMBED,    5,      gml_include,    tag_is_general,              0 )
  pick( INCLUDE,  7,      gml_include,    tag_is_general,              0 )
  pick( INDEX,    5,      gml_index,      tag_out_txt,                 0 )
  pick( IREF,     4,      gml_iref,       tag_out_txt,                 0 )
  pick( LAYOUT,   6,      gml_layout,     0,                           0 )
  pick( LI,       2,      gml_li,         tag_out_txt,                 li_lp_tag )
  pick( LIREF,    5,      gml_dummy,      tag_out_txt,                 0 )
  pick( LP,       2,      gml_lp,         tag_out_txt,                 li_lp_tag )

  pick( LQ,       2,      gml_dummy,      tag_out_txt,                 0 )
  pick( ELQ,      3,      gml_dummy,      tag_out_txt,                 0 )

  pick( NOTE,     4,      gml_note,       tag_out_txt,                 0 )

  pick( OL,       2,      gml_ol,         tag_out_txt,                 0 )
  pick( EOL,      3,      gml_eol,        tag_out_txt,                 0 )

  pick( P,        1,      gml_p,          tag_out_txt,                 0 )
//pick( PB,       2,      gml_lp,         tag_out_txt,                 li_lp_tag ) // this is a hack TBD
  pick( PB,       2,      gml_pb,         tag_out_txt,                 0 )
  pick( PC,       2,      gml_pc,         tag_out_txt,                 0 )
  pick( PREFACE,  7,      gml_preface,    tag_out_txt,                 0 )

  pick( PSC,      3,      gml_dummy,      tag_out_txt,                 0 )
  pick( EPSC,     4,      gml_dummy,      tag_out_txt,                 0 )

  pick( Q,        1,      gml_dummy,      tag_out_txt,                 0 )
  pick( EQ,       2,      gml_dummy,      tag_out_txt,                 0 )

  pick( SET,      3,      gml_set,        tag_is_general,              0 )

  pick( SF,       2,      gml_sf,         tag_out_txt,                 0 )
  pick( ESF,      3,      gml_esf,        tag_out_txt,                 0 )

  pick( SL,       2,      gml_sl,         tag_out_txt,                 0 )
  pick( ESL,      3,      gml_esl,        tag_out_txt,                 0 )

  pick( TITLE,    5,      gml_title,      tag_out_txt,                 titlep_tag )

  pick( TITLEP,   6,      gml_titlep,     0,                           0 )
  pick( ETITLEP,  7,      gml_etitlep,    tag_out_txt,                 titlep_tag )

  pick( TOC,      3,      gml_toc,        tag_out_txt,                 0  )

  pick( UL,       2,      gml_ul,         tag_out_txt,                 0  )
  pick( EUL,      3,      gml_eul,        tag_out_txt,                 0  )

  pick( XMP,      3,      gml_xmp,        tag_out_txt,                 0  )
  pick( EXMP,     4,      gml_exmp,       tag_out_txt,                 0  )
