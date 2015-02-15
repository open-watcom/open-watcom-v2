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
//  upper case

  pick( AD, scr_dummy, cw_break           )// adjust
  pick( AP, scr_ap,    0                  )// append
  pick( BC, scr_dummy, cw_o_t             )// balance columns
  pick( BD, scr_us,    cw_o_t             )// bold dummy treat as underscore TBD
  pick( BF, scr_dummy, cw_o_t             )// begin font
  pick( BI, scr_dummy, cw_o_t             )// bold italic
  pick( BL, scr_dummy, cw_break+cw_o_t    )// blank line
  pick( BM, scr_dummy, cw_break+cw_o_t    )// bottom margin
  pick( BR, scr_br,    cw_break+cw_o_t    )// break
  pick( BS, scr_dummy, cw_o_t             )// backspace
  pick( BT, scr_dummy, 0                  )// bottom title
  pick( BX, scr_bx,    cw_break+cw_o_t    )// box
  pick( CB, scr_dummy, cw_break+cw_o_t    )// column begin
  pick( CC, scr_dummy, cw_break+cw_o_t    )// conditional column
  pick( CD, scr_dummy, cw_break+cw_o_t    )// column definition
  pick( CE, scr_dummy, cw_break+cw_o_t    )// center
  pick( CH, scr_dummy, 0                  )// change
  pick( CL, scr_dummy, cw_break+cw_o_t    )// column length
//pick( CM, scr_dummy, 0                  )// comment ----- handled differently
  pick( CO, scr_co,    cw_break+cw_o_t    )// concatenate
  pick( CP, scr_cp,    cw_break+cw_o_t    )// conditional page
  pick( CS, scr_dummy, cw_o_t             )// conditional section
  pick( CT, scr_ct,    cw_o_t             )// continued text
  pick( CW, scr_cw,    0                  )// control word separator
  pick( DC, scr_dc,    0                  )// define character
  pick( DF, scr_dummy, 0                  )// define font
  pick( DH, scr_dummy, cw_o_t             )// define heading
  pick( DM, scr_dm,    0                  )// define macro
  pick( DO, scr_do,    0                  )// do
  pick( DS, scr_dummy, cw_break+cw_o_t    )// double spacing
  pick( DU, scr_dummy, 0                  )// dictionary update
  pick( EC, scr_dummy, 0                  )// execute control
  pick( EF, scr_dummy, 0                  )// end of file
  pick( EL, scr_el,    0                  )// else
  pick( EM, scr_em,    0                  )// execute macro
  pick( EQ, scr_dummy, cw_o_t             )// equation
  pick( ER, scr_dummy, 0                  )// error
  pick( FB, scr_dummy, cw_o_t             )// floating block
//pick( FI, scr_dummy, 0                  )// old
  pick( FK, scr_dummy, cw_o_t             )// floating keep
  pick( FM, scr_dummy, cw_break+cw_o_t    )// footing margin
  pick( FN, scr_dummy, cw_o_t             )// footnote
  pick( FO, scr_fo,    cw_break+cw_o_t    )// format
  pick( FS, scr_dummy, cw_break+cw_o_t    )// footing space
  pick( GA, scr_ga,    0                  )// GML attribute
  pick( GO, scr_go,    0                  )// go to
  pick( GT, scr_gt,    0                  )// GML tag
  pick( H1, scr_dummy, cw_break+cw_o_t    )// Heading level 1
  pick( H2, scr_dummy, cw_break+cw_o_t    )// heading level 2
  pick( H3, scr_dummy, cw_break+cw_o_t    )// heading level 3
  pick( H4, scr_dummy, cw_break+cw_o_t    )// hanging indent
  pick( HL, scr_dummy, cw_break+cw_o_t    )// heading level
  pick( HM, scr_dummy, cw_break+cw_o_t    )// heading margin
  pick( HN, scr_dummy, 0                  )// head note
  pick( HS, scr_dummy, cw_break+cw_o_t    )// heading space
  pick( HW, scr_dummy, 0                  )// hyphenate word
  pick( HY, scr_dummy, 0                  )// hyphenate
  pick( IE, scr_dummy, 0                  )// index entry
  pick( IF, scr_if,    0                  )// if
  pick( IL, scr_dummy, cw_break+cw_o_t    )// indent line
  pick( IM, scr_im,    0                  )// imbed
  pick( IN, scr_in,    cw_break+cw_o_t    )// indent
  pick( IR, scr_dummy, cw_break+cw_o_t    )// indent right
  pick( IX, scr_ix,    0                  )// index
  pick( JU, scr_ju,    cw_break+cw_o_t    )// justify
  pick( LA, scr_dummy, cw_break+cw_o_t    )// left adjust
  pick( LB, scr_dummy, cw_break+cw_o_t    )// leading blank
  pick( LE, scr_dummy, cw_o_t             )// leading space
  pick( LI, scr_li,    cw_o_t             )// literal
  pick( LL, scr_dummy, cw_break+cw_o_t    )// line length
  pick( LN, scr_dummy, cw_break+cw_o_t    )// line immediate
  pick( LS, scr_dummy, cw_break+cw_o_t    )// line spacing
  pick( LT, scr_dummy, cw_break+cw_o_t    )// leading tab
  pick( MC, scr_dummy, cw_break+cw_o_t    )// multiple column
  pick( ME, scr_me,    0                  )// macro exit
  pick( MS, scr_dummy, 0                  )// macro substitution
  pick( OC, scr_oc,    0                  )// output comment
  pick( OF, scr_dummy, cw_break+cw_o_t    )// offset
  pick( OJ, scr_dummy, cw_break+cw_o_t    )// out justify
  pick( OO, scr_dummy, cw_o_t             )// output overlay
  pick( OV, scr_dummy, cw_o_t             )// ovrlay
  pick( PA, scr_pa,    cw_break+cw_o_t    )// page eject
  pick( PE, scr_pe,    0                  )// perform
  pick( PF, scr_dummy, 0                  )// previous font
  pick( PH, scr_dummy, 0                  )// photo font
  pick( PL, scr_dummy, cw_break+cw_o_t    )// page length
  pick( PM, scr_dummy, 0                  )// page margin
  pick( PN, scr_dummy, 0                  )// page number
  pick( PP, scr_dummy, cw_break+cw_o_t    )// paragraph
  pick( PS, scr_dummy, 0                  )// page naumber symbol
  pick( PT, scr_dummy, 0                  )// put table of contents
  pick( PU, scr_pu,    0                  )// put workfile
  pick( PW, scr_dummy, 0                  )// page width
  pick( RA, scr_dummy, 0                  )// old CW use .ri
  pick( RC, scr_dummy, 0                  )// revision code
  pick( RD, scr_dummy, 0                  )// read terminal
  pick( RE, scr_dummy, 0                  )// restore environment
  pick( RI, scr_dummy, cw_break+cw_o_t    )// right adjust
  pick( RM, scr_dummy, cw_o_t             )// remote
  pick( RT, scr_dummy, cw_break           )// running title
  pick( RV, scr_dummy, 0                  )// read variable
  pick( SA, scr_dummy, 0                  )// save environment
  pick( SC, scr_dummy, cw_break+cw_o_t    )// single column
  pick( SE, scr_se,    0                  )// set symbol
  pick( SK, scr_sk,    cw_break           )// skip (generate blank line(s)
  pick( SL, scr_dummy, cw_break+cw_o_t    )// set leading
  pick( SP, scr_dummy, cw_break+cw_o_t    )// space
  pick( SR, scr_se,    0                  )// treat as .se as substitute is always on
  pick( SS, scr_dummy, cw_break+cw_o_t    )// single space
  pick( SU, scr_dummy, 0                  )// substitute symbol
  pick( SV, scr_dummy, 0                  )// spelling verification
  pick( SX, scr_dummy, cw_break+cw_o_t    )// split text
  pick( SY, scr_dummy, 0                  )// system command
  pick( TB, scr_tb,    cw_break+cw_o_t    )// define tab stops
  pick( TC, scr_dummy, cw_break           )// table of contents
  pick( TE, scr_dummy, 0                  )// terminal input
  pick( TH, scr_th,    0                  )// then
  pick( TI, scr_ti,    0                  )// translate on input
  pick( TM, scr_dummy, cw_break+cw_o_t    )// top margin
  pick( TP, scr_dummy, cw_break+cw_o_t    )// tab position
  pick( TR, scr_tr,    0                  )// translate table for output
  pick( TT, scr_dummy, 0                  )// top title
  pick( TU, scr_dummy, cw_o_t             )// translate uppercase
  pick( TY, scr_ty,    0                  )// type
  pick( UC, scr_dummy, cw_o_t             )// underscore and capitalize
  pick( UD, scr_dummy, cw_o_t             )// underscore definition
  pick( UL, scr_us,    cw_o_t             )// underline (same as us)
  pick( UN, scr_dummy, cw_break+cw_o_t    )// undent
  pick( UP, scr_dummy, cw_o_t             )// uppercase
  pick( US, scr_us,    cw_o_t             )// underscore
  pick( WD, scr_dummy, 0                  )// widow
