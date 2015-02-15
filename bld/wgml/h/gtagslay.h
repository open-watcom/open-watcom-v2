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
//      tagname      tagname  routinename
// upper case        length

  pick( ABSTRACT,     8,      lay_abspref     )
  pick( ADDRESS,      7,      lay_address     )
  pick( ALINE,        5,      lay_aline       )
  pick( APPENDIX,     8,      lay_appendix    )
  pick( ATTN,         4,      lay_dummy       )  // letter not supported
  pick( AUTHOR,       6,      lay_author      )
  pick( BACKM,        5,      lay_backbod     )
  pick( BANNER,       6,      lay_banner      )
  pick( BANREGION,    9,      lay_banregion   )
  pick( BODY,         4,      lay_backbod     )
  pick( CIT,          3,      lay_xx          )
  pick( CLOSE,        5,      lay_dummy       )  // letter not supported
  pick( CMT,          3,      lay_cmt         )
  pick( CONVERT,      7,      lay_convert     )
  pick( DATE,         4,      lay_date        )
  pick( DD,           2,      lay_dd          )
  pick( DDHD,         4,      lay_xx          )
  pick( DEFAULT,      7,      lay_default     )
  pick( DL,           2,      lay_dl          )
  pick( DISTRIB,      7,      lay_dummy       )  // letter not supported
  pick( DOCNUM,       6,      lay_docnum      )
  pick( DT,           2,      lay_xx          )
  pick( DTHD,         4,      lay_xx          )
  pick( EBANNER,      7,      lay_ebanner     )
  pick( EBANREGION,  10,      lay_ebanregion  )
  pick( ECLOSE,       6,      lay_dummy       )  // letter not supported
  pick( ELAYOUT,      7,      lay_elayout     )
  pick( FIG,          3,      lay_fig         )
  pick( FIGCAP,       6,      lay_figcap      )
  pick( FIGDESC,      7,      lay_figdesc     )
  pick( FIGLIST,      7,      lay_figlist     )
  pick( FLPGNUM,      7,      lay_flpgnum     )
  pick( FN,           2,      lay_fn          )
  pick( FNREF,        5,      lay_fnref       )
  pick( FROM,         4,      lay_dummy       )  // letter not supported
  pick( GD,           2,      lay_xx          )
  pick( GL,           2,      lay_gl          )
  pick( GT,           2,      lay_xx          )
  pick( HEADING,      7,      lay_heading     )
  pick( H0,           2,      lay_hx          )
  pick( H1,           2,      lay_hx          )
  pick( H2,           2,      lay_hx          )
  pick( H3,           2,      lay_hx          )
  pick( H4,           2,      lay_hx          )
  pick( H5,           2,      lay_hx          )
  pick( H6,           2,      lay_hx          )
  pick( I1,           2,      lay_ix          )
  pick( I2,           2,      lay_ix          )
  pick( I3,           2,      lay_ix          )
  pick( IMBED,        5,      lay_include     )
  pick( INCLUDE,      7,      lay_include     )
  pick( INDEX,        5,      lay_index       )
  pick( IXHEAD,       6,      lay_ixhead      )
  pick( IXMAJOR,      7,      lay_xx          )
  pick( IXPGNUM,      7,      lay_xx          )
  pick( LETDATE,      7,      lay_dummy       )  // letter not supported
  pick( LP,           2,      lay_lp          )
  pick( LQ,           2,      lay_lq          )
  pick( NOTE,         4,      lay_note        )
  pick( OL,           2,      lay_ol          )
  pick( OPEN,         4,      lay_dummy       )  // letter not supported
  pick( P,            1,      lay_p           )
  pick( PAGE,         4,      lay_page        )
  pick( PC,           2,      lay_pc          )
  pick( PREFACE,      7,      lay_abspref     )
  pick( PSC,          3,      lay_dummy       )
  pick( SAVE,         4,      lay_dummy       )
  pick( SL,           2,      lay_sl          )
  pick( SUBJECT,      7,      lay_dummy       )  // letter not supported
  pick( TITLE,        5,      lay_title       )
  pick( TITLEP,       6,      lay_titlep      )
  pick( TO,           2,      lay_dummy       )  // letter not supported
  pick( TOC,          3,      lay_toc         )
  pick( TOCH0,        5,      lay_tochx       )
  pick( TOCH1,        5,      lay_tochx       )
  pick( TOCH2,        5,      lay_tochx       )
  pick( TOCH3,        5,      lay_tochx       )
  pick( TOCH4,        5,      lay_tochx       )
  pick( TOCH5,        5,      lay_tochx       )
  pick( TOCH6,        5,      lay_tochx       )
  pick( TOCPGNUM,     8,      lay_tocpgnum    )
  pick( UL,           2,      lay_ul          )
  pick( WIDOW,        5,      lay_widow       )
  pick( XMP,          3,      lay_xmp         )

#undef  pick
