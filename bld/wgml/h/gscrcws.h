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
  pick( ap, scr_ap,    cw_none            )// append
  pick( bc, scr_dummy, cw_o_t             )// balance columns
  pick( bd, scr_bd,    cw_o_t             )// bold
  pick( bf, scr_dummy, cw_o_t             )// begin font
  pick( bi, scr_bi,    cw_o_t             )// bold italic
  pick( bl, scr_dummy, cw_break+cw_o_t    )// blank line
  pick( bm, scr_dummy, cw_break+cw_o_t    )// bottom margin
  pick( br, scr_br,    cw_break+cw_o_t    )// break
  pick( bs, scr_dummy, cw_o_t             )// backspace
  pick( bt, scr_dummy, cw_none            )// bottom title
  pick( bx, scr_bx,    cw_break+cw_o_t    )// box
  pick( cb, scr_dummy, cw_break+cw_o_t    )// column begin
  pick( cc, scr_cc,    cw_break+cw_o_t    )// conditional column
  pick( cd, scr_cd,    cw_break+cw_o_t    )// column definition
  pick( ce, scr_dummy, cw_break+cw_o_t    )// center
  pick( ch, scr_dummy, cw_none            )// change
  pick( cl, scr_dummy, cw_break+cw_o_t    )// column length
  pick( cm, scr_cm,    cw_none            )// comment
  pick( co, scr_co,    cw_break+cw_o_t    )// concatenate
  pick( cp, scr_cp,    cw_break+cw_o_t    )// conditional page
  pick( cs, scr_cs,    cw_o_t             )// conditional section
  pick( ct, scr_ct,    cw_o_t             )// continued text
  pick( cw, scr_cw,    cw_none            )// control word separator
  pick( dc, scr_dc,    cw_none            )// define character
  pick( df, scr_dummy, cw_none            )// define font
  pick( dh, scr_dummy, cw_o_t             )// define heading
  pick( dm, scr_dm,    cw_none            )// define macro
  pick( do, scr_do,    cw_none            )// do
  pick( ds, scr_dummy, cw_break+cw_o_t    )// double spacing
  pick( du, scr_dummy, cw_none            )// dictionary update
  pick( ec, scr_dummy, cw_none            )// execute control
  pick( ef, scr_dummy, cw_none            )// end of file
  pick( el, scr_el,    cw_none            )// else
  pick( em, scr_em,    cw_none            )// execute macro
  pick( eq, scr_dummy, cw_o_t             )// equation
  pick( er, scr_dummy, cw_none            )// error
  pick( fb, scr_fb,    cw_o_t             )// floating block
//pick( fi, scr_dummy, cw_none            )// old
  pick( fk, scr_fk,    cw_o_t             )// floating keep
  pick( fm, scr_dummy, cw_break+cw_o_t    )// footing margin
  pick( fn, scr_dummy, cw_o_t             )// footnote
  pick( fo, scr_fo,    cw_break+cw_o_t    )// format
  pick( fs, scr_dummy, cw_break+cw_o_t    )// footing space
  pick( ga, scr_ga,    cw_none            )// GML attribute
  pick( go, scr_go,    cw_none            )// go to
  pick( gt, scr_gt,    cw_none            )// GML tag
  pick( h1, scr_dummy, cw_break+cw_o_t    )// Heading level 1
  pick( h2, scr_dummy, cw_break+cw_o_t    )// heading level 2
  pick( h3, scr_dummy, cw_break+cw_o_t    )// heading level 3
  pick( hi, scr_dummy, cw_break+cw_o_t    )// hanging indent
  pick( hl, scr_dummy, cw_break+cw_o_t    )// heading level
  pick( hm, scr_dummy, cw_break+cw_o_t    )// heading margin
  pick( hn, scr_dummy, cw_none            )// head note
  pick( hs, scr_dummy, cw_break+cw_o_t    )// heading space
  pick( hw, scr_dummy, cw_none            )// hyphenate word
  pick( hy, scr_hy,    cw_none            )// hyphenate
  pick( ie, scr_dummy, cw_none            )// index entry
  pick( if, scr_if,    cw_none            )// if
  pick( il, scr_dummy, cw_break+cw_o_t    )// indent line
  pick( im, scr_im,    cw_none            )// imbed
  pick( in, scr_in,    cw_break+cw_o_t    )// indent
  pick( ir, scr_dummy, cw_break+cw_o_t    )// indent right
  pick( it, scr_dummy, cw_break+cw_o_t    )// input trace
  pick( ix, scr_ix,    cw_none            )// index
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
  pick( me, scr_me,    cw_none            )// macro exit
  pick( ms, scr_dummy, cw_none            )// macro substitution
  pick( nl, scr_nl,    cw_none            )// null line
  pick( oc, scr_oc,    cw_none            )// output comment
  pick( of, scr_dummy, cw_break+cw_o_t    )// offset
  pick( oj, scr_dummy, cw_break+cw_o_t    )// out justify
  pick( oo, scr_dummy, cw_o_t             )// output overlay
  pick( ov, scr_dummy, cw_o_t             )// ovrlay
  pick( pa, scr_pa,    cw_break+cw_o_t    )// page eject
  pick( pe, scr_pe,    cw_none            )// perform
  pick( pf, scr_dummy, cw_none            )// previous font
  pick( ph, scr_dummy, cw_none            )// photo font
  pick( pl, scr_dummy, cw_break+cw_o_t    )// page length
  pick( pm, scr_dummy, cw_none            )// page margin
  pick( pn, scr_dummy, cw_none            )// page number
  pick( pp, scr_pp,    cw_break+cw_o_t    )// paragraph
  pick( ps, scr_dummy, cw_none            )// page naumber symbol
  pick( pt, scr_dummy, cw_none            )// put table of contents
  pick( pu, scr_pu,    cw_none            )// put workfile
  pick( pw, scr_dummy, cw_none            )// page width
  pick( ra, scr_dummy, cw_none            )// old CW use .ri
  pick( rc, scr_dummy, cw_none            )// revision code
  pick( rd, scr_dummy, cw_none            )// read terminal
  pick( re, scr_dummy, cw_none            )// restore environment
  pick( ri, scr_dummy, cw_break+cw_o_t    )// right adjust
  pick( rm, scr_dummy, cw_o_t             )// remote
  pick( rt, scr_dummy, cw_break           )// running title
  pick( rv, scr_dummy, cw_none            )// read variable
  pick( sa, scr_dummy, cw_none            )// save environment
  pick( sc, scr_dummy, cw_break+cw_o_t    )// single column
  pick( se, scr_se,    cw_none            )// set symbol
  pick( sk, scr_sk,    cw_break           )// skip (generate blank line(s)
  pick( sl, scr_dummy, cw_break+cw_o_t    )// set leading
  pick( sp, scr_sp,    cw_break+cw_o_t    )// space
  pick( sr, scr_se,    cw_none            )// treat as .se as substitute is always on
  pick( ss, scr_dummy, cw_break+cw_o_t    )// single space
  pick( su, scr_dummy, cw_none            )// substitute symbol
  pick( sv, scr_dummy, cw_none            )// spelling verification
  pick( sx, scr_dummy, cw_break+cw_o_t    )// split text
  pick( sy, scr_dummy, cw_none            )// system command
  pick( tb, scr_tb,    cw_break+cw_o_t    )// define tab stops
  pick( tc, scr_dummy, cw_break           )// table of contents
  pick( te, scr_dummy, cw_none            )// terminal input
  pick( th, scr_th,    cw_none            )// then
  pick( ti, scr_ti,    cw_none            )// translate on input
  pick( tm, scr_dummy, cw_break+cw_o_t    )// top margin
  pick( tp, scr_dummy, cw_break+cw_o_t    )// tab position
  pick( tr, scr_tr,    cw_none            )// translate table for output
  pick( tt, scr_dummy, cw_none            )// top title
  pick( tu, scr_dummy, cw_o_t             )// translate uppercase
  pick( ty, scr_ty,    cw_none            )// type
  pick( uc, scr_dummy, cw_o_t             )// underscore and capitalize
  pick( ud, scr_dummy, cw_o_t             )// underscore definition
  pick( ul, scr_us,    cw_o_t             )// underline (same as us)
  pick( un, scr_dummy, cw_break+cw_o_t    )// undent
  pick( up, scr_dummy, cw_o_t             )// uppercase
  pick( us, scr_us,    cw_o_t             )// underscore
  pick( wd, scr_dummy, cw_none            )// widow
  pick( .., scr_label, cw_none            )// ... label
