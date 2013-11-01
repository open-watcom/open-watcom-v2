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
* Description:  WGML predefined system symbols
*                   for the pic. macros see gsyssym.c
****************************************************************************/

#ifndef pick
    #error macro pick not defined
#endif
//      symbol   next     flags
//      name     symbol
  picl( ad,      adeven,  no_free+ro+access_fun+predefined          )
  picl( adeven,  adodd,   no_free+ro+access_fun+predefined          )
  picl( adodd,   amp,     no_free+ro+access_fun+predefined          )
  picc( amp,     apage,   no_free+ro+predefined                     )
  picl( apage,   author,  no_free+ro+access_fun+predefined          )
  pics( author,  bc,      no_free+ro+predefined                     )
  picc( bc,      be,      no_free+ro+access_fun+predefined          )
  picl( be,      bfonts,  no_free+ro+access_fun+predefined          )
  pick( bfonts,  bm,      no_free+ro+access_fun+predefined          )
  picl( bm,      bo,      no_free+ro+access_fun+predefined          )
  picl( bo,      bs,      no_free+ro+access_fun+predefined          )
  picc( bs,      bx,      no_free+ro+access_fun+predefined          )
  picc( bx,      bxchar,  no_free+ro+access_fun+predefined          )
  picl( bxchar,  cc,      no_free+ro+access_fun+predefined          )
  picc( cc,      ccc,     no_free+ro+access_fun+predefined          )
  picl( ccc,     cd,      no_free+ro+access_fun+predefined          )
  picl( cd,      cdcount, no_free+ro+access_fun+predefined          )
  picl( cdcount, chars,   no_free+ro+access_fun+predefined          )
  pick( chars,   cl,      no_free+ro+access_fun+predefined          )
  picl( cl,      co,      no_free+ro+access_fun+predefined          )
  pick( co,      cont,    no_free+ro+access_fun+predefined          )
  picc( cont,    cp,      no_free+ro+access_fun+predefined          )
  picc( cp,      cpages,  no_free+ro+access_fun+predefined          )
  pick( cpages,  cpc,     no_free+ro+access_fun+predefined          )
  picl( cpc,     cpi,     no_free+ro+access_fun+predefined          )
  picl( cpi,     cw,      no_free+ro+access_fun+predefined          )
  picc( cw,      date,    no_free+ro+predefined                     )
  pica( date,    dayofm,  no_free+ro+predefined                     )
  pica( dayofm,  dayofw,  no_free+ro+predefined                     )
  pica( dayofw,  dayofy,  no_free+ro+predefined                     )
  pica( dayofy,  dfonts,  no_free+ro+predefined                     )
  pick( dfonts,  dhset,   no_free+ro+access_fun+predefined          )
  picl( dhset,   docnum,  no_free+ro+access_fun+predefined          )
  pics( docnum,  dpage,   no_free+ro+predefined                     )
  pick( dpage,   duplex,  no_free+ro+access_fun+predefined          )
  pick( duplex,  env,     no_free+ro+access_fun+predefined          )
  pick( env,     fb,      no_free+ro+access_fun+predefined          )
  picc( fb,      fbc,     no_free+ro+access_fun+predefined          )
  picl( fbc,     fbf,     no_free+ro+access_fun+predefined          )
  picl( fbf,     file,    no_free+ro+access_fun+predefined          )
  pick( file,    fk,      no_free+ro+access_fun+predefined          )
  picc( fk,      fkc,     no_free+ro+access_fun+predefined          )
  picl( fkc,     flnest,  no_free+ro+access_fun+predefined          )
  picl( flnest,  fm,      no_free+ro+access_fun+predefined          )
  picl( fm,      fn,      no_free+ro+access_fun+predefined          )
  picc( fn,      fnam,    no_free+ro+access_fun+predefined          )
  pick( fnam,    fnc,     no_free+ro+access_fun+predefined          )
  picl( fnc,     fnum,    no_free+ro+access_fun+predefined          )
  picl( fnum,    fonts,   no_free+ro+access_fun+predefined          )
  pick( fonts,   fs,      no_free+ro+access_fun+predefined          )
  picl( fs,      gml,     no_free+ro+access_fun+predefined          )
  picc( gml,     gutter,  no_free+ro+predefined                     )
  picl( gutter,  hi,      no_free+ro+access_fun+predefined          )
  picl( hi,      hm,      no_free+ro+access_fun+predefined          )
  picl( hm,      hn,      no_free+ro+access_fun+predefined          )
  picc( hn,      hnc,     no_free+ro+access_fun+predefined          )
  picl( hnc,     hour,    no_free+ro+access_fun+predefined          )
  pica( hour,    hs,      no_free+ro+predefined                     )
  picl( hs,      hy,      no_free+ro+access_fun+predefined          )
  pick( hy,      hyc,     no_free+ro+access_fun+predefined          )
  picl( hyc,     hyph,    no_free+ro+access_fun+predefined          )
  picc( hyph,    in,      no_free+ro+access_fun+predefined          )
  picl( in,      inr,     no_free+ro+access_fun+predefined          )
  picl( inr,     ir,      no_free+ro+access_fun+predefined          )
  picl( ir,      ixj,     no_free+ro+access_fun+predefined          )
  picc( ixj,     ixref,   no_free+ro+access_fun+predefined          )
  picl( ixref,   ju,      no_free+ro+access_fun+predefined          )
  pick( ju,      layout,  no_free+ro+access_fun+predefined          )
  pick( layout,  lc,      no_free+ro+access_fun+predefined          )
  picl( lc,      li,      no_free+ro+access_fun+predefined          )
  picc( li,      linb,    no_free+ro+access_fun+predefined          )
  picc( linb,    line,    no_free+ro+access_fun+predefined          )
  picl( line,    ll,      no_free+ro+access_fun+predefined          )
  picl( ll,      lnum,    no_free+ro+access_fun+predefined          )
  picl( lnum,    ls,      no_free+ro+access_fun+predefined          )
  picl( ls,      lst,     no_free+ro+access_fun+predefined          )
  picl( lst,     mc,      no_free+ro+access_fun+predefined          )
  picc( mc,      mcs,     no_free+ro+access_fun+predefined          )
  picc( mcs,     member,  no_free+ro+access_fun+predefined          )
  pica( member,  minute,  no_free+ro+predefined                     )
  pica( minute,  month,   no_free+ro+predefined                     )
  pica( month,   nodeid,  no_free+ro+predefined                     )
  pick( nodeid,  of,      no_free+ro+access_fun+predefined          )
  picl( of,      offline, no_free+ro+access_fun+predefined          )
  picc( offline, online,  no_free+ro+access_fun+predefined          )
  picc( online,  ooc,     no_free+ro+access_fun+predefined          )
  picl( ooc,     out,     no_free+ro+access_fun+predefined          )
  pick( out,     page,    no_free+ro+access_fun+predefined          )
  picl( page,    paged,   no_free+ro+access_fun+predefined          )
  picl( paged,   pagelm,  no_free+ro+access_fun+predefined          )
  picl( pagelm,  pagerm,  no_free+ro+access_fun+predefined          )
  picl( pagerm,  parm,    no_free+ro+access_fun+predefined          )
  pica( parm,    passno,  no_free+ro+predefined                     )
  picl( passno,  passof,  no_free+ro+predefined                     )
  picl( passof,  pdayofw, no_free+ro+predefined                     )
  pica( pdayofw, pdev,    no_free+ro+predefined                     )
  pica( pdev,    pgnuma,  no_free+ro+predefined                     )
  picl( pgnuma,  pgnumad, no_free+ro+access_fun+predefined          )
  picl( pgnumad, pgnumc,  no_free+ro+access_fun+predefined          )
  picl( pgnumc,  pgnumcd, no_free+ro+access_fun+predefined          )
  picl( pgnumcd, pgnumr,  no_free+ro+access_fun+predefined          )
  picl( pgnumr,  pgnumrd, no_free+ro+access_fun+predefined          )
  picl( pgnumrd, pi,      no_free+ro+access_fun+predefined          )
  picc( pi,      pix,     no_free+ro+access_fun+predefined          )
  picc( pix,     pl,      no_free+ro+access_fun+predefined          )
  picl( pl,      pls,     no_free+ro+access_fun+predefined          )
  picc( pls,     pmonth,  no_free+ro+access_fun+predefined          )
  pica( pmonth,  pn,      no_free+ro+predefined                     )
  picl( pn,      ppage,   no_free+ro+access_fun+predefined          )
  picl( ppage,   prs,     no_free+ro+access_fun+predefined          )
  picc( prs,     prt,     no_free+ro+access_fun+predefined          )
  pick( prt,     ps,      no_free+ro+access_fun+predefined          )
  picc( ps,      pw,      no_free+ro+access_fun+predefined          )
  picl( pw,      pyear,   no_free+ro+access_fun+predefined          )
  pica( pyear,   quiet,   no_free+ro+predefined                     )
  pick( quiet,   rb,      no_free+ro+access_fun+predefined          )
  picc( rb,      recno,   no_free+ro+access_fun+predefined          )
  picl( recno,   resh,    no_free+ro+access_fun+predefined          )
  picl( resh,    resv,    no_free+ro+access_fun+predefined          )
  picl( resv,    ret,     no_free+ro+access_fun+predefined          )
  picl( ret,     rmnest,  no_free+ro+access_fun+predefined          )
  picl( rmnest,  sc,      no_free+ro+access_fun+predefined          )
  picc( sc,      screen,  no_free+ro+access_fun+predefined          )
  picc( screen,  second,  no_free+ro+access_fun+predefined          )
  pica( second,  seqno,   no_free+ro+predefined                     )
  picl( seqno,   skcond,  no_free+ro+access_fun+predefined          )
  picl( skcond,  sl,      no_free+ro+access_fun+predefined          )
  picl( sl,      spcond,  no_free+ro+access_fun+predefined          )
  picl( spcond,  su,      no_free+ro+access_fun+predefined          )
  pick( su,      sys,     no_free+ro+access_fun+predefined          )
  pick( sys,     tab,     no_free+ro+access_fun+predefined          )
  picc( tab,     tb,      no_free+ro+access_fun+predefined          )
  picc( tb,      termt,   no_free+ro+access_fun+predefined          )
  pick( termt,   time,    no_free+ro+access_fun+predefined          )
  pica( time,    tiset,   no_free+ro+predefined                     )
  picc( tiset,   tm,      no_free+ro+access_fun+predefined          )
  picl( tm,      userid,  no_free+ro+access_fun+predefined          )
  pick( userid,  version, no_free+ro+access_fun+predefined          )
  pica( version, wd,      no_free+ro+predefined                     )
  picc( wd,      xtext,   no_free+ro+access_fun+predefined          )
  pick( xtext,   year,    no_free+ro+access_fun+predefined          )
  pica( year,    ad,      no_free+ro+predefined                     )
//               &ad will be set to NULL in init routine for end of chain
#undef pick
