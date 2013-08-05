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

#ifndef pickg
    #error macro pickg not defined
#endif
//       tagname  tagname  routinename     gmlflags locflags
// upper case     length

  pickg( ABSTRACT, 8,      gml_abstract,   0,   0 )

  pickg( ADDRESS,  7,      gml_address,    0,   titlep_tag )
  pickg( EADDRESS, 8,      gml_eaddress,   0,   titlep_tag | address_tag )

  pickg( ALINE,    5,      gml_aline,      tag_out_txt, address_tag )
  pickg( APPENDIX, 8,      gml_appendix,   0,   0 )
  pickg( AUTHOR,   6,      gml_author,     tag_out_txt, titlep_tag  )
  pickg( BACKM,    5,      gml_backm,      0,   0 )
  pickg( BINCLUDE, 8,      gml_binclude,   0,   titlep_tag | address_tag )
  pickg( BODY,     4,      gml_body,       0,   0 )

  pickg( CIT,      3,      gml_dummy,      tag_out_txt, 0 )
  pickg( ECIT,     4,      gml_dummy,      tag_out_txt, 0 )

  pickg( CMT,      3,      gml_cmt,        tag_only | tag_is_general,   0 )
  pickg( DATE,     4,      gml_date,       tag_out_txt, titlep_tag )
  pickg( DD,       2,      gml_dummy,      tag_out_txt, 0 )
  pickg( DDHD,     4,      gml_dummy,      tag_out_txt, 0 )

  pickg( DL,       2,      gml_dl,         tag_out_txt, 0 )
  pickg( EDL,      3,      gml_edl,        tag_out_txt, 0 )

  pickg( DOCNUM,   6,      gml_docnum,     tag_out_txt, titlep_tag )
  pickg( DT,       2,      gml_dummy,      tag_out_txt, 0 )
  pickg( DTHD,     4,      gml_dummy,      tag_out_txt, 0 )

  pickg( FIG,      3,      gml_dummy,      tag_out_txt, 0 )
  pickg( EFIG,     4,      gml_dummy,      tag_out_txt, 0 )

  pickg( FIGCAP,   6,      gml_dummy,      tag_out_txt, 0 )
  pickg( FIGDESC,  7,      gml_dummy,      tag_out_txt, 0 )
  pickg( FIGLIST,  7,      gml_figlist,    tag_out_txt, 0 )
  pickg( FIGREF,   6,      gml_dummy,      tag_out_txt, 0 )

  pickg( FN,       2,      gml_dummy,      tag_out_txt, 0 )
  pickg( EFN,      3,      gml_dummy,      tag_out_txt, 0 )

  pickg( FNREF,    5,      gml_dummy,      tag_out_txt, 0 )
  pickg( FRONTM,   6,      gml_frontm,     0,   0 )
  pickg( GDOC,     4,      gml_gdoc,       0,   0 )
  pickg( EGDOC,    5,      gml_egdoc,      tag_out_txt, 0 )

  pickg( GL,       2,      gml_gl,         tag_out_txt, 0 )
  pickg( EGL,      3,      gml_egl,        tag_out_txt, 0 )

  pickg( GD,       2,      gml_dummy,      tag_out_txt, 0 )
  pickg( GRAPHIC,  7,      gml_graphic,    tag_out_txt, titlep_tag | address_tag )
  pickg( GT,       2,      gml_dummy,      tag_out_txt, 0 )
  pickg( H0,       2,      gml_h0,         0,           0 )
  pickg( H1,       2,      gml_h1,         0,           0 )
  pickg( H2,       2,      gml_h2,         0,           0 )
  pickg( H3,       2,      gml_h3,         0,           0 )
  pickg( H4,       2,      gml_h4,         0,           0 )
  pickg( H5,       2,      gml_h5,         0,           0 )
  pickg( H6,       2,      gml_h6,         0,           0 )
  pickg( HDREF,    5,      gml_hdref,      tag_out_txt, 0 )

  pickg( HP0,      3,      gml_hp0,        tag_out_txt, 0 )
  pickg( EHP0,     4,      gml_ehp0,       tag_out_txt, 0 )

  pickg( HP1,      3,      gml_hp1,        tag_out_txt, 0 )
  pickg( EHP1,     4,      gml_ehp1,       tag_out_txt, 0 )

  pickg( HP2,      3,      gml_hp2,        tag_out_txt, 0 )
  pickg( EHP2,     4,      gml_ehp2,       tag_out_txt, 0 )

  pickg( HP3,      3,      gml_hp3,        tag_out_txt, 0 )
  pickg( EHP3,     4,      gml_ehp3,       tag_out_txt, 0 )

  pickg( I1,       2,      gml_i1,         tag_out_txt, 0 )
  pickg( I2,       2,      gml_i2,         tag_out_txt, 0 )
  pickg( I3,       2,      gml_i3,         tag_out_txt, 0 )
  pickg( IH1,      3,      gml_ih1,        tag_out_txt, 0 )
  pickg( IH2,      3,      gml_ih2,        tag_out_txt, 0 )
  pickg( IH3,      3,      gml_ih3,        tag_out_txt, 0 )
  pickg( IMBED,    5,      gml_include,    tag_is_general, 0 )
  pickg( INCLUDE,  7,      gml_include,    tag_is_general, 0 )
  pickg( INDEX,    5,      gml_index,      tag_out_txt, 0 )
  pickg( IREF,     4,      gml_iref,       tag_out_txt, 0 )
  pickg( LAYOUT,   6,      gml_layout,     0,   0 )
  pickg( LI,       2,      gml_li,         tag_out_txt, li_lp_tag )
  pickg( LIREF,    5,      gml_dummy,      tag_out_txt, 0 )
  pickg( LP,       2,      gml_lp,         tag_out_txt, li_lp_tag )

  pickg( LQ,       2,      gml_dummy,      tag_out_txt, 0 )
  pickg( ELQ,      3,      gml_dummy,      tag_out_txt, 0 )

  pickg( NOTE,     4,      gml_note,       tag_out_txt, 0 )

  pickg( OL,       2,      gml_ol,         tag_out_txt, 0 )
  pickg( EOL,      3,      gml_eol,        tag_out_txt, 0 )

  pickg( P,        1,      gml_p,          tag_out_txt, 0 )
//pickg( PB,       2,      gml_lp,         tag_out_txt, li_lp_tag ) // this is a hack TBD
  pickg( PB,       2,      gml_pb,         tag_out_txt, 0 )
  pickg( PC,       2,      gml_pc,         tag_out_txt, 0 )
  pickg( PREFACE,  7,      gml_preface,    tag_out_txt, 0 )

  pickg( PSC,      3,      gml_dummy,      tag_out_txt, 0 )
  pickg( EPSC,     4,      gml_dummy,      tag_out_txt, 0 )

  pickg( Q,        1,      gml_dummy,      tag_out_txt, 0 )
  pickg( EQ,       2,      gml_dummy,      tag_out_txt, 0 )

  pickg( SET,      3,      gml_set,        tag_is_general, 0 )

  pickg( SF,       2,      gml_sf,         tag_out_txt, 0 )
  pickg( ESF,      3,      gml_esf,        tag_out_txt, 0 )

  pickg( SL,       2,      gml_sl,         tag_out_txt, 0 )
  pickg( ESL,      3,      gml_esl,        tag_out_txt, 0 )

  pickg( TITLE,    5,      gml_title,      tag_out_txt, titlep_tag )

  pickg( TITLEP,   6,      gml_titlep,     0,   0 )
  pickg( ETITLEP,  7,      gml_etitlep,    tag_out_txt, titlep_tag )

  pickg( TOC,      3,      gml_toc,        tag_out_txt, 0  )

  pickg( UL,       2,      gml_ul,         tag_out_txt, 0  )
  pickg( EUL,      3,      gml_eul,        tag_out_txt, 0  )

  pickg( XMP,      3,      gml_xmp,        tag_out_txt, 0  )
  pickg( EXMP,     4,      gml_exmp,       tag_out_txt, 0  )

#undef  pickg
