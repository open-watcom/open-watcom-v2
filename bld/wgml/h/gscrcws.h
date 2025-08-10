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
* Description:  WGML script control words definition header.
*               only control words with a routine name other than scr_dummy
*               are processed so far
****************************************************************************/


//  control  routine    flags
//     word  name
// lower case

  pick( ad, scr_dummy, CW_break                )// adjust
  pick( ap, scr_ap,    CW_none                 )// append
  pick( bc, scr_dummy, CW_out_text             )// balance columns
  pick( bd, scr_bd,    CW_out_text             )// bold
  pick( bf, scr_dummy, CW_out_text             )// begin font
  pick( bi, scr_bi,    CW_out_text             )// bold italic
  pick( bl, scr_dummy, CW_break+CW_out_text    )// blank line
  pick( bm, scr_dummy, CW_break+CW_out_text    )// bottom margin
  pick( br, scr_br,    CW_break+CW_out_text    )// break
  pick( bs, scr_dummy, CW_out_text             )// backspace
  pick( bt, scr_dummy, CW_none                 )// bottom title
  pick( bx, scr_bx,    CW_break+CW_out_text    )// box
  pick( cb, scr_dummy, CW_break+CW_out_text    )// column begin
  pick( cc, scr_cc,    CW_break+CW_out_text    )// conditional column
  pick( cd, scr_cd,    CW_break+CW_out_text    )// column definition
  pick( ce, scr_dummy, CW_break+CW_out_text    )// center
  pick( ch, scr_dummy, CW_none                 )// change
  pick( cl, scr_dummy, CW_break+CW_out_text    )// column length
  pick( cm, scr_cm,    CW_none                 )// comment
  pick( co, scr_co,    CW_break+CW_out_text    )// concatenate
  pick( cp, scr_cp,    CW_break+CW_out_text    )// conditional page
  pick( cs, scr_cs,    CW_out_text             )// conditional section
  pick( ct, scr_ct,    CW_out_text             )// continued text
  pick( cw, scr_cw,    CW_none                 )// control word separator
  pick( dc, scr_dc,    CW_none                 )// define character
  pick( df, scr_dummy, CW_none                 )// define font
  pick( dh, scr_dummy, CW_out_text             )// define heading
  pick( dm, scr_dm,    CW_none                 )// define macro
  pick( do, scr_do,    CW_none                 )// do
  pick( ds, scr_dummy, CW_break+CW_out_text    )// double spacing
  pick( du, scr_dummy, CW_none                 )// dictionary update
  pick( ec, scr_dummy, CW_none                 )// execute control
  pick( ef, scr_dummy, CW_none                 )// end of file
  pick( el, scr_el,    CW_none                 )// else
  pick( em, scr_em,    CW_none                 )// execute macro
  pick( eq, scr_dummy, CW_out_text             )// equation
  pick( er, scr_dummy, CW_none                 )// error
  pick( fb, scr_fb,    CW_out_text             )// floating block
//pick( fi, scr_dummy, CW_none                 )// old
  pick( fk, scr_fk,    CW_out_text             )// floating keep
  pick( fm, scr_dummy, CW_break+CW_out_text    )// footing margin
  pick( fn, scr_dummy, CW_out_text             )// footnote
  pick( fo, scr_fo,    CW_break+CW_out_text    )// format
  pick( fs, scr_dummy, CW_break+CW_out_text    )// footing space
  pick( ga, scr_ga,    CW_none                 )// GML attribute
  pick( go, scr_go,    CW_none                 )// go to
  pick( gt, scr_gt,    CW_none                 )// GML tag
  pick( h1, scr_dummy, CW_break+CW_out_text    )// Heading level 1
  pick( h2, scr_dummy, CW_break+CW_out_text    )// heading level 2
  pick( h3, scr_dummy, CW_break+CW_out_text    )// heading level 3
  pick( hi, scr_dummy, CW_break+CW_out_text    )// hanging indent
  pick( hl, scr_dummy, CW_break+CW_out_text    )// heading level
  pick( hm, scr_dummy, CW_break+CW_out_text    )// heading margin
  pick( hn, scr_dummy, CW_none                 )// head note
  pick( hs, scr_dummy, CW_break+CW_out_text    )// heading space
  pick( hw, scr_dummy, CW_none                 )// hyphenate word
  pick( hy, scr_hy,    CW_none                 )// hyphenate
  pick( ie, scr_dummy, CW_none                 )// index entry
  pick( if, scr_if,    CW_none                 )// if
  pick( il, scr_dummy, CW_break+CW_out_text    )// indent line
  pick( im, scr_im,    CW_none                 )// imbed
  pick( in, scr_in,    CW_break+CW_out_text    )// indent
  pick( ir, scr_ir,    CW_break+CW_out_text    )// indent right
  pick( it, scr_dummy, CW_break+CW_out_text    )// input trace
  pick( ix, scr_ix,    CW_none                 )// index
  pick( ju, scr_ju,    CW_break+CW_out_text    )// justify
  pick( la, scr_dummy, CW_break+CW_out_text    )// left adjust
  pick( lb, scr_dummy, CW_break+CW_out_text    )// leading blank
  pick( le, scr_dummy, CW_out_text             )// leading space
  pick( li, scr_li,    CW_out_text             )// literal
  pick( ll, scr_dummy, CW_break+CW_out_text    )// line length
  pick( ln, scr_dummy, CW_break+CW_out_text    )// line immediate
  pick( ls, scr_dummy, CW_break+CW_out_text    )// line spacing
  pick( lt, scr_dummy, CW_break+CW_out_text    )// leading tab
  pick( mc, scr_dummy, CW_break+CW_out_text    )// multiple column
  pick( me, scr_me,    CW_none                 )// macro exit
  pick( ms, scr_dummy, CW_none                 )// macro substitution
  pick( nl, scr_nl,    CW_none                 )// null line
  pick( oc, scr_oc,    CW_none                 )// output comment
  pick( of, scr_dummy, CW_break+CW_out_text    )// offset
  pick( oj, scr_dummy, CW_break+CW_out_text    )// out justify
  pick( oo, scr_dummy, CW_out_text             )// output overlay
  pick( ov, scr_dummy, CW_out_text             )// ovrlay
  pick( pa, scr_pa,    CW_break+CW_out_text    )// page eject
  pick( pe, scr_pe,    CW_none                 )// perform
  pick( pf, scr_dummy, CW_none                 )// previous font
  pick( ph, scr_dummy, CW_none                 )// photo font
  pick( pl, scr_dummy, CW_break+CW_out_text    )// page length
  pick( pm, scr_dummy, CW_none                 )// page margin
  pick( pn, scr_dummy, CW_none                 )// page number
  pick( pp, scr_pp,    CW_break+CW_out_text    )// paragraph
  pick( ps, scr_dummy, CW_none                 )// page naumber symbol
  pick( pt, scr_dummy, CW_none                 )// put table of contents
  pick( pu, scr_pu,    CW_none                 )// put workfile
  pick( pw, scr_dummy, CW_none                 )// page width
  pick( ra, scr_dummy, CW_none                 )// old CW use .ri
  pick( rc, scr_dummy, CW_none                 )// revision code
  pick( rd, scr_dummy, CW_none                 )// read terminal
  pick( re, scr_dummy, CW_none                 )// restore environment
  pick( ri, scr_dummy, CW_break+CW_out_text    )// right adjust
  pick( rm, scr_dummy, CW_out_text             )// remote
  pick( rt, scr_dummy, CW_break                )// running title
  pick( rv, scr_dummy, CW_none                 )// read variable
  pick( sa, scr_dummy, CW_none                 )// save environment
  pick( sc, scr_dummy, CW_break+CW_out_text    )// single column
  pick( se, scr_se,    CW_none                 )// set symbol
  pick( sk, scr_sk,    CW_break                )// skip (generate blank line(s)
  pick( sl, scr_dummy, CW_break+CW_out_text    )// set leading
  pick( sp, scr_sp,    CW_break+CW_out_text    )// space
  pick( sr, scr_se,    CW_none                 )// treat as .se as substitute is always on
  pick( ss, scr_dummy, CW_break+CW_out_text    )// single space
  pick( su, scr_dummy, CW_none                 )// substitute symbol
  pick( sv, scr_dummy, CW_none                 )// spelling verification
  pick( sx, scr_dummy, CW_break+CW_out_text    )// split text
  pick( sy, scr_dummy, CW_none                 )// system command
  pick( tb, scr_tb,    CW_break+CW_out_text    )// define tab stops
  pick( tc, scr_dummy, CW_break                )// table of contents
  pick( te, scr_dummy, CW_none                 )// terminal input
  pick( th, scr_th,    CW_none                 )// then
  pick( ti, scr_ti,    CW_none                 )// translate on input
  pick( tm, scr_dummy, CW_break+CW_out_text    )// top margin
  pick( tp, scr_dummy, CW_break+CW_out_text    )// tab position
  pick( tr, scr_tr,    CW_none                 )// translate table for output
  pick( tt, scr_dummy, CW_none                 )// top title
  pick( tu, scr_dummy, CW_out_text             )// translate uppercase
  pick( ty, scr_ty,    CW_none                 )// type
  pick( uc, scr_dummy, CW_out_text             )// underscore and capitalize
  pick( ud, scr_dummy, CW_out_text             )// underscore definition
  pick( ul, scr_us,    CW_out_text             )// underline (same as us)
  pick( un, scr_dummy, CW_break+CW_out_text    )// undent
  pick( up, scr_dummy, CW_out_text             )// uppercase
  pick( us, scr_us,    CW_out_text             )// underscore
  pick( wd, scr_dummy, CW_none                 )// widow
  pick( .., scr_label, CW_none                 )// ... label
