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
* Description:  WGML script control words definition header.
*               only control words with a routine name other than scr_dummy
*               are processed so far
****************************************************************************/


//  control  routine    flags
//     word  name
// lower case

  pick( ad, scr_dummy, cw_break           )// adjust
  pick( ap, scr_ap,    0                  )// append
  pick( bc, scr_dummy, cw_o_t             )// balance columns
  pick( bd, scr_bd,    cw_o_t             )// bold
  pick( bf, scr_dummy, cw_o_t             )// begin font
  pick( bi, scr_bi,    cw_o_t             )// bold italic
  pick( bl, scr_dummy, cw_break+cw_o_t    )// blank line
  pick( bm, scr_dummy, cw_break+cw_o_t    )// bottom margin
  pick( br, scr_br,    cw_break+cw_o_t    )// break
  pick( bs, scr_dummy, cw_o_t             )// backspace
  pick( bt, scr_dummy, 0                  )// bottom title
  pick( bx, scr_bx,    cw_break+cw_o_t    )// box
  pick( cb, scr_dummy, cw_break+cw_o_t    )// column begin
  pick( cc, scr_cc,    cw_break+cw_o_t    )// conditional column
  pick( cd, scr_cd,    cw_break+cw_o_t    )// column definition
  pick( ce, scr_dummy, cw_break+cw_o_t    )// center
  pick( ch, scr_dummy, 0                  )// change
  pick( cl, scr_dummy, cw_break+cw_o_t    )// column length
  pick( cm, scr_cm,    0                  )// comment
  pick( co, scr_co,    cw_break+cw_o_t    )// concatenate
  pick( cp, scr_cp,    cw_break+cw_o_t    )// conditional page
  pick( cs, scr_cs,    cw_o_t             )// conditional section
  pick( ct, scr_ct,    cw_o_t             )// continued text
  pick( cw, scr_cw,    0                  )// control word separator
  pick( dc, scr_dc,    0                  )// define character
  pick( df, scr_dummy, 0                  )// define font
  pick( dh, scr_dummy, cw_o_t             )// define heading
  pick( dm, scr_dm,    0                  )// define macro
  pick( do, scr_do,    0                  )// do
  pick( ds, scr_dummy, cw_break+cw_o_t    )// double spacing
  pick( du, scr_dummy, 0                  )// dictionary update
  pick( ec, scr_dummy, 0                  )// execute control
  pick( ef, scr_dummy, 0                  )// end of file
  pick( el, scr_el,    0                  )// else
  pick( em, scr_em,    0                  )// execute macro
  pick( eq, scr_dummy, cw_o_t             )// equation
  pick( er, scr_dummy, 0                  )// error
  pick( fb, scr_fb,    cw_o_t             )// floating block
//pick( fi, scr_dummy, 0                  )// old
  pick( fk, scr_fk,    cw_o_t             )// floating keep
  pick( fm, scr_dummy, cw_break+cw_o_t    )// footing margin
  pick( fn, scr_dummy, cw_o_t             )// footnote
  pick( fo, scr_fo,    cw_break+cw_o_t    )// format
  pick( fs, scr_dummy, cw_break+cw_o_t    )// footing space
  pick( ga, scr_ga,    0                  )// GML attribute
  pick( go, scr_go,    0                  )// go to
  pick( gt, scr_gt,    0                  )// GML tag
  pick( h1, scr_dummy, cw_break+cw_o_t    )// Heading level 1
  pick( h2, scr_dummy, cw_break+cw_o_t    )// heading level 2
  pick( h3, scr_dummy, cw_break+cw_o_t    )// heading level 3
  pick( hi, scr_dummy, cw_break+cw_o_t    )// hanging indent
  pick( hl, scr_dummy, cw_break+cw_o_t    )// heading level
  pick( hm, scr_dummy, cw_break+cw_o_t    )// heading margin
  pick( hn, scr_dummy, 0                  )// head note
  pick( hs, scr_dummy, cw_break+cw_o_t    )// heading space
  pick( hw, scr_dummy, 0                  )// hyphenate word
  pick( hy, scr_hy,    0                  )// hyphenate
  pick( ie, scr_dummy, 0                  )// index entry
  pick( if, scr_if,    0                  )// if
  pick( il, scr_dummy, cw_break+cw_o_t    )// indent line
  pick( im, scr_im,    0                  )// imbed
  pick( in, scr_in,    cw_break+cw_o_t    )// indent
  pick( ir, scr_dummy, cw_break+cw_o_t    )// indent right
  pick( it, scr_dummy, cw_break+cw_o_t    )// input trace
  pick( ix, scr_ix,    0                  )// index
  pick( ju, scr_ju,    cw_break+cw_o_t    )// justify
  pick( la, scr_dummy, cw_break+cw_o_t    )// left adjust
  pick( lb, scr_dummy, cw_break+cw_o_t    )// leading blank
  pick( le, scr_dummy, cw_o_t             )// leading space
  pick( li, scr_li,    cw_o_t             )// literal
  pick( ll, scr_dummy, cw_break+cw_o_t    )// line length
  pick( ln, scr_dummy, cw_break+cw_o_t    )// line immediate
  pick( ls, scr_dummy, cw_break+cw_o_t    )// line spacing
  pick( lt, scr_dummy, cw_break+cw_o_t    )// leading tab
  pick( mc, scr_dummy, cw_break+cw_o_t    )// multiple column
  pick( me, scr_me,    0                  )// macro exit
  pick( ms, scr_dummy, 0                  )// macro substitution
  pick( nl, scr_nl,    0                  )// null line
  pick( oc, scr_oc,    0                  )// output comment
  pick( of, scr_dummy, cw_break+cw_o_t    )// offset
  pick( oj, scr_dummy, cw_break+cw_o_t    )// out justify
  pick( oo, scr_dummy, cw_o_t             )// output overlay
  pick( ov, scr_dummy, cw_o_t             )// ovrlay
  pick( pa, scr_pa,    cw_break+cw_o_t    )// page eject
  pick( pe, scr_pe,    0                  )// perform
  pick( pf, scr_dummy, 0                  )// previous font
  pick( ph, scr_dummy, 0                  )// photo font
  pick( pl, scr_dummy, cw_break+cw_o_t    )// page length
  pick( pm, scr_dummy, 0                  )// page margin
  pick( pn, scr_dummy, 0                  )// page number
  pick( pp, scr_pp,    cw_break+cw_o_t    )// paragraph
  pick( ps, scr_dummy, 0                  )// page naumber symbol
  pick( pt, scr_dummy, 0                  )// put table of contents
  pick( pu, scr_pu,    0                  )// put workfile
  pick( pw, scr_dummy, 0                  )// page width
  pick( ra, scr_dummy, 0                  )// old CW use .ri
  pick( rc, scr_dummy, 0                  )// revision code
  pick( rd, scr_dummy, 0                  )// read terminal
  pick( re, scr_dummy, 0                  )// restore environment
  pick( ri, scr_dummy, cw_break+cw_o_t    )// right adjust
  pick( rm, scr_dummy, cw_o_t             )// remote
  pick( rt, scr_dummy, cw_break           )// running title
  pick( rv, scr_dummy, 0                  )// read variable
  pick( sa, scr_dummy, 0                  )// save environment
  pick( sc, scr_dummy, cw_break+cw_o_t    )// single column
  pick( se, scr_se,    0                  )// set symbol
  pick( sk, scr_sk,    cw_break           )// skip (generate blank line(s)
  pick( sl, scr_dummy, cw_break+cw_o_t    )// set leading
  pick( sp, scr_sp,    cw_break+cw_o_t    )// space
  pick( sr, scr_se,    0                  )// treat as .se as substitute is always on
  pick( ss, scr_dummy, cw_break+cw_o_t    )// single space
  pick( su, scr_dummy, 0                  )// substitute symbol
  pick( sv, scr_dummy, 0                  )// spelling verification
  pick( sx, scr_dummy, cw_break+cw_o_t    )// split text
  pick( sy, scr_dummy, 0                  )// system command
  pick( tb, scr_tb,    cw_break+cw_o_t    )// define tab stops
  pick( tc, scr_dummy, cw_break           )// table of contents
  pick( te, scr_dummy, 0                  )// terminal input
  pick( th, scr_th,    0                  )// then
  pick( ti, scr_ti,    0                  )// translate on input
  pick( tm, scr_dummy, cw_break+cw_o_t    )// top margin
  pick( tp, scr_dummy, cw_break+cw_o_t    )// tab position
  pick( tr, scr_tr,    0                  )// translate table for output
  pick( tt, scr_dummy, 0                  )// top title
  pick( tu, scr_dummy, cw_o_t             )// translate uppercase
  pick( ty, scr_ty,    0                  )// type
  pick( uc, scr_dummy, cw_o_t             )// underscore and capitalize
  pick( ud, scr_dummy, cw_o_t             )// underscore definition
  pick( ul, scr_us,    cw_o_t             )// underline (same as us)
  pick( un, scr_dummy, cw_break+cw_o_t    )// undent
  pick( up, scr_dummy, cw_o_t             )// uppercase
  pick( us, scr_us,    cw_o_t             )// underscore
  pick( wd, scr_dummy, 0                  )// widow
  pick( .., scr_label, 0                  )// ... label
