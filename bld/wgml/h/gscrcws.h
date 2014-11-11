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
* Description:  WGML script control words definition header.
*               only control words with a routine name other than scr_dummy
*               are processed so far
****************************************************************************/

//  control  routine    flags
//     word  name
// lower case

  picks( ad, scr_dummy, cw_break           )// adjust
  picks( ap, scr_ap,    0                  )// append
  picks( bc, scr_dummy, cw_o_t             )// balance columns
  picks( bd, scr_us,    cw_o_t             )// bold dummy treat as underscore TBD
  picks( bf, scr_dummy, cw_o_t             )// begin font
  picks( bi, scr_dummy, cw_o_t             )// bold italic
  picks( bl, scr_dummy, cw_break+cw_o_t    )// blank line
  picks( bm, scr_dummy, cw_break+cw_o_t    )// bottom margin
  picks( br, scr_br,    cw_break+cw_o_t    )// break
  picks( bs, scr_dummy, cw_o_t             )// backspace
  picks( bt, scr_dummy, 0                  )// bottom title
  picks( bx, scr_bx,    cw_break+cw_o_t    )// box
  picks( cb, scr_dummy, cw_break+cw_o_t    )// column begin
  picks( cc, scr_dummy, cw_break+cw_o_t    )// conditional column
  picks( cd, scr_dummy, cw_break+cw_o_t    )// column definition
  picks( ce, scr_dummy, cw_break+cw_o_t    )// center
  picks( ch, scr_dummy, 0                  )// change
  picks( cl, scr_dummy, cw_break+cw_o_t    )// column length
//picks( cm, scr_dummy, 0                  )// comment ----- handled differently
  picks( co, scr_co,    cw_break+cw_o_t    )// concatenate
  picks( cp, scr_cp,    cw_break+cw_o_t    )// conditional page
  picks( cs, scr_dummy, cw_o_t             )// conditional section
  picks( ct, scr_ct,    cw_o_t             )// continued text
  picks( cw, scr_cw,    0                  )// control word separator
  picks( dc, scr_dc,    0                  )// define character
  picks( df, scr_dummy, 0                  )// define font
  picks( dh, scr_dummy, cw_o_t             )// define heading
  picks( dm, scr_dm,    0                  )// define macro
  picks( do, scr_do,    0                  )// do
  picks( ds, scr_dummy, cw_break+cw_o_t    )// double spacing
  picks( du, scr_dummy, 0                  )// dictionary update
  picks( ec, scr_dummy, 0                  )// execute control
  picks( ef, scr_dummy, 0                  )// end of file
  picks( el, scr_el,    0                  )// else
  picks( em, scr_em,    0                  )// execute macro
  picks( eq, scr_dummy, cw_o_t             )// equation
  picks( er, scr_dummy, 0                  )// error
  picks( fb, scr_dummy, cw_o_t             )// floating block
//picks( fi, scr_dummy, 0                  )// old
  picks( fk, scr_dummy, cw_o_t             )// floating keep
  picks( fm, scr_dummy, cw_break+cw_o_t    )// footing margin
  picks( fn, scr_dummy, cw_o_t             )// footnote
  picks( fo, scr_fo,    cw_break+cw_o_t    )// format
  picks( fs, scr_dummy, cw_break+cw_o_t    )// footing space
  picks( ga, scr_ga,    0                  )// GML attribute
  picks( go, scr_go,    0                  )// go to
  picks( gt, scr_gt,    0                  )// GML tag
  picks( h1, scr_dummy, cw_break+cw_o_t    )// Heading level 1
  picks( h2, scr_dummy, cw_break+cw_o_t    )// heading level 2
  picks( h3, scr_dummy, cw_break+cw_o_t    )// heading level 3
  picks( hi, scr_dummy, cw_break+cw_o_t    )// hanging indent
  picks( hl, scr_dummy, cw_break+cw_o_t    )// heading level
  picks( hm, scr_dummy, cw_break+cw_o_t    )// heading margin
  picks( hn, scr_dummy, 0                  )// head note
  picks( hs, scr_dummy, cw_break+cw_o_t    )// heading space
  picks( hw, scr_dummy, 0                  )// hyphenate word
  picks( hy, scr_dummy, 0                  )// hyphenate
  picks( ie, scr_dummy, 0                  )// index entry
  picks( if, scr_if,    0                  )// if
  picks( il, scr_dummy, cw_break+cw_o_t    )// indent line
  picks( im, scr_im,    0                  )// imbed
  picks( in, scr_in,    cw_break+cw_o_t    )// indent
  picks( ir, scr_dummy, cw_break+cw_o_t    )// indent right
  picks( ix, scr_ix,    0                  )// index
  picks( ju, scr_ju,    cw_break+cw_o_t    )// justify
  picks( la, scr_dummy, cw_break+cw_o_t    )// left adjust
  picks( lb, scr_dummy, cw_break+cw_o_t    )// leading blank
  picks( le, scr_dummy, cw_o_t             )// leading space
  picks( li, scr_li,    cw_o_t             )// literal
  picks( ll, scr_dummy, cw_break+cw_o_t    )// line length
  picks( ln, scr_dummy, cw_break+cw_o_t    )// line immediate
  picks( ls, scr_dummy, cw_break+cw_o_t    )// line spacing
  picks( lt, scr_dummy, cw_break+cw_o_t    )// leading tab
  picks( mc, scr_dummy, cw_break+cw_o_t    )// multiple column
  picks( me, scr_me,    0                  )// macro exit
  picks( ms, scr_dummy, 0                  )// macro substitution
  picks( oc, scr_oc,    0                  )// output comment
  picks( of, scr_dummy, cw_break+cw_o_t    )// offset
  picks( oj, scr_dummy, cw_break+cw_o_t    )// out justify
  picks( oo, scr_dummy, cw_o_t             )// output overlay
  picks( ov, scr_dummy, cw_o_t             )// ovrlay
  picks( pa, scr_pa,    cw_break+cw_o_t    )// page eject
  picks( pe, scr_pe,    0                  )// perform
  picks( pf, scr_dummy, 0                  )// previous font
  picks( ph, scr_dummy, 0                  )// photo font
  picks( pl, scr_dummy, cw_break+cw_o_t    )// page length
  picks( pm, scr_dummy, 0                  )// page margin
  picks( pn, scr_dummy, 0                  )// page number
  picks( pp, scr_dummy, cw_break+cw_o_t    )// paragraph
  picks( ps, scr_dummy, 0                  )// page naumber symbol
  picks( pt, scr_dummy, 0                  )// put table of contents
  picks( pu, scr_pu,    0                  )// put workfile
  picks( pw, scr_dummy, 0                  )// page width
  picks( ra, scr_dummy, 0                  )// old CW use .ri
  picks( rc, scr_dummy, 0                  )// revision code
  picks( rd, scr_dummy, 0                  )// read terminal
  picks( re, scr_dummy, 0                  )// restore environment
  picks( ri, scr_dummy, cw_break+cw_o_t    )// right adjust
  picks( rm, scr_dummy, cw_o_t             )// remote
  picks( rt, scr_dummy, cw_break           )// running title
  picks( rv, scr_dummy, 0                  )// read variable
  picks( sa, scr_dummy, 0                  )// save environment
  picks( sc, scr_dummy, cw_break+cw_o_t    )// single column
  picks( se, scr_se,    0                  )// set symbol
  picks( sk, scr_sk,    cw_break           )// skip (generate blank line(s)
  picks( sl, scr_dummy, cw_break+cw_o_t    )// set leading
  picks( sp, scr_dummy, cw_break+cw_o_t    )// space
  picks( sr, scr_se,    0                  )// treat as .se as substitute is always on
  picks( ss, scr_dummy, cw_break+cw_o_t    )// single space
  picks( su, scr_dummy, 0                  )// substitute symbol
  picks( sv, scr_dummy, 0                  )// spelling verification
  picks( sx, scr_dummy, cw_break+cw_o_t    )// split text
  picks( sy, scr_dummy, 0                  )// system command
  picks( tb, scr_tb,    cw_break+cw_o_t    )// define tab stops
  picks( tc, scr_dummy, cw_break           )// table of contents
  picks( te, scr_dummy, 0                  )// terminal input
  picks( th, scr_th,    0                  )// then
  picks( ti, scr_ti,    0                  )// translate on input
  picks( tm, scr_dummy, cw_break+cw_o_t    )// top margin
  picks( tp, scr_dummy, cw_break+cw_o_t    )// tab position
  picks( tr, scr_tr,    0                  )// translate table for output
  picks( tt, scr_dummy, 0                  )// top title
  picks( tu, scr_dummy, cw_o_t             )// translate uppercase
  picks( ty, scr_ty,    0                  )// type
  picks( uc, scr_dummy, cw_o_t             )// underscore and capitalize
  picks( ud, scr_dummy, cw_o_t             )// underscore definition
  picks( ul, scr_us,    cw_o_t             )// underline (same as us)
  picks( un, scr_dummy, cw_break+cw_o_t    )// undent
  picks( up, scr_dummy, cw_o_t             )// uppercase
  picks( us, scr_us,    cw_o_t             )// underscore
  picks( wd, scr_dummy, 0                  )// widow
  picklab( .., scr_label, 0                )
