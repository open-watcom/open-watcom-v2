/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML predefined system symbols.
*
****************************************************************************/


/* Included exclusively from gsyssym.c. See gsyssym.c for the pick macros. */

//      symbol   flags
//      name
  pickl( ad,     no_free+ro+predefined+access_fun          )
  pickl( adeven, no_free+ro+predefined+access_fun          )
  pickl( adodd,  no_free+ro+predefined+access_fun          )
  pickc( amp,    no_free+ro+predefined+is_AMP              )
  pickl( apage,  no_free+ro+predefined+access_fun          )
  pickc( bc,     no_free+ro+predefined+access_fun          )
  pickl( be,     no_free+ro+predefined+access_fun          )
  pickk( bfonts, no_free+ro+predefined+access_fun          )
  pickl( bm,     no_free+ro+predefined+access_fun          )
  pickl( bo,     no_free+ro+predefined+access_fun          )
  pickc( bs,     no_free+ro+predefined+access_fun          )
  pickc( bx,     no_free+ro+predefined+access_fun          )
  pickl( bxchar, no_free+ro+predefined+access_fun          )
  pickc( cc,     no_free+ro+predefined+access_fun          )
  pickl( ccc,    no_free+ro+predefined+access_fun          )
  pickl( cd,     no_free+ro+predefined+access_fun          )
  pickl( cdcount,no_free+ro+predefined+access_fun          )
  pickk( chars,  no_free+ro+predefined+access_fun          )
  pickl( cl,     no_free+ro+predefined+access_fun          )
  pickk( co,     no_free+ro+predefined+access_fun          )
  pickc( cont,   no_free+ro+predefined+access_fun          )
  pickc( cp,     no_free+ro+predefined+access_fun          )
  pickk( cpages, no_free+ro+predefined+access_fun          )
  pickl( cpc,    no_free+ro+predefined+access_fun          )
  pickl( cpi,    no_free+ro+predefined+access_fun          )
  pickc( cw,     no_free+ro+predefined                     )
  picka( date,   no_free+ro+predefined                     )
  picka( dayofm, no_free+ro+predefined                     )
  picka( dayofw, no_free+ro+predefined                     )
  picka( dayofy, no_free+ro+predefined                     )
  pickk( dfonts, no_free+ro+predefined+access_fun          )
  pickl( dhset,  no_free+ro+predefined+access_fun          )
  pickk( dpage,  no_free+ro+predefined+access_fun          )
  pickk( duplex, no_free+ro+predefined+access_fun          )
  pickk( env,    no_free+ro+predefined+access_fun          )
  pickc( fb,     no_free+ro+predefined+access_fun          )
  pickl( fbc,    no_free+ro+predefined+access_fun          )
  pickl( fbf,    no_free+ro+predefined+access_fun          )
  pickk( file,   no_free+ro+predefined+access_fun          )
  pickc( fk,     no_free+ro+predefined+access_fun          )
  pickl( fkc,    no_free+ro+predefined+access_fun          )
  pickl( flnest, no_free+ro+predefined+access_fun          )
  pickl( fm,     no_free+ro+predefined+access_fun          )
  pickc( fn,     no_free+ro+predefined+access_fun          )
  pickk( fnam,   no_free+ro+predefined+access_fun          )
  pickl( fnc,    no_free+ro+predefined+access_fun          )
  pickl( fnum,   no_free+ro+predefined+access_fun          )
  pickk( fonts,  no_free+ro+predefined+access_fun          )
  pickl( fs,     no_free+ro+predefined+access_fun          )
  pickc( gml,    no_free+ro+predefined                     )
  pickl( gutter, no_free+ro+predefined+access_fun          )
  pickl( hi,     no_free+ro+predefined+access_fun          )
  pickl( hm,     no_free+ro+predefined+access_fun          )
  pickc( hn,     no_free+ro+predefined+access_fun          )
  pickl( hnc,    no_free+ro+predefined+access_fun          )
  picka( hour,   no_free+ro+predefined                     )
  pickl( hs,     no_free+ro+predefined+access_fun          )
  pickk( hy,     no_free+ro+predefined+access_fun          )
  pickl( hyc,    no_free+ro+predefined+access_fun          )
  pickc( hyph,   no_free+ro+predefined+access_fun          )
  pickl( in,     no_free+ro+predefined+access_fun          )
  pickl( inr,    no_free+ro+predefined+access_fun          )
  pickl( ir,     no_free+ro+predefined+access_fun          )
  pickc( ixj,    no_free+ro+predefined+access_fun          )
  pickl( ixref,  no_free+ro+predefined+access_fun          )
  pickk( ju,     no_free+ro+predefined+access_fun          )
  pickk( layout, no_free+ro+predefined+access_fun          )
  pickl( lc,     no_free+ro+predefined+access_fun          )
  pickc( li,     no_free+ro+predefined+access_fun          )
  pickc( linb,   no_free+ro+predefined+access_fun          )
  pickl( line,   no_free+ro+predefined+access_fun          )
  pickl( ll,     no_free+ro+predefined+access_fun          )
  pickl( lnum,   no_free+ro+predefined+access_fun          )
  pickl( ls,     no_free+ro+predefined+access_fun          )
  pickl( lst,    no_free+ro+predefined+access_fun          )
  pickc( mc,     no_free+ro+predefined+access_fun          )
  pickc( mcs,    no_free+ro+predefined+access_fun          )
  picka( member, no_free+ro+predefined                     )
  picka( minute, no_free+ro+predefined                     )
  picka( month,  no_free+ro+predefined                     )
  pickk( nodeid, no_free+ro+predefined+access_fun          )
  pickl( of,     no_free+ro+predefined+access_fun          )
  pickc( offline,no_free+ro+predefined+access_fun          )
  pickc( online, no_free+ro+predefined+access_fun          )
  pickl( ooc,    no_free+ro+predefined+access_fun          )
  pickk( out,    no_free+ro+predefined+access_fun          )
  pickl( page,   no_free+ro+predefined+access_fun          )
  pickl( paged,  no_free+ro+predefined+access_fun          )
  pickl( pagelm, no_free+ro+predefined+access_fun          )
  pickl( pagerm, no_free+ro+predefined+access_fun          )
  picka( parm,   no_free+ro+predefined                     )
  pickl( passno, no_free+ro+predefined                     )
  pickl( passof, no_free+ro+predefined                     )
  picka( pdayofw,no_free+ro+predefined                     )
  picka( pdev,   no_free+ro+predefined                     )
  pickl( pgnuma, no_free+ro+predefined+access_fun          )
  pickl( pgnumad,no_free+ro+predefined+access_fun          )
  pickl( pgnumc, no_free+ro+predefined+access_fun          )
  pickl( pgnumcd,no_free+ro+predefined+access_fun          )
  pickl( pgnumr, no_free+ro+predefined+access_fun          )
  pickl( pgnumrd,no_free+ro+predefined+access_fun          )
  pickc( pi,     no_free+ro+predefined+access_fun          )
  pickc( pix,    no_free+ro+predefined+access_fun          )
  pickl( pl,     no_free+ro+predefined+access_fun          )
  pickc( pls,    no_free+ro+predefined+access_fun          )
  picka( pmonth, no_free+ro+predefined                     )
  pickl( pn,     no_free+ro+predefined+access_fun          )
  pickl( ppage,  no_free+ro+predefined+access_fun          )
  pickc( prs,    no_free+ro+predefined+access_fun          )
  pickk( prt,    no_free+ro+predefined+access_fun          )
  pickc( ps,     no_free+ro+predefined+access_fun          )
  pickl( pw,     no_free+ro+predefined+access_fun          )
  picka( pyear,  no_free+ro+predefined                     )
  pickk( quiet,  no_free+ro+predefined+access_fun          )
  pickc( rb,     no_free+ro+predefined+access_fun          )
  pickl( recno,  no_free+ro+predefined+access_fun          )
  pickl( resh,   no_free+ro+predefined+access_fun          )
  pickl( resv,   no_free+ro+predefined+access_fun          )
  pickl( ret,    no_free+ro+predefined+access_fun          )
  pickl( rmnest, no_free+ro+predefined+access_fun          )
  pickc( sc,     no_free+ro+predefined+access_fun          )
  pickc( screen, no_free+ro+predefined+access_fun          )
  picka( second, no_free+ro+predefined                     )
  pickl( seqno,  no_free+ro+predefined+access_fun          )
  pickl( skcond, no_free+ro+predefined+access_fun          )
  pickl( sl,     no_free+ro+predefined+access_fun          )
  pickl( spcond, no_free+ro+predefined+access_fun          )
  pickk( su,     no_free+ro+predefined+access_fun          )
  pickk( sys,    no_free+ro+predefined+access_fun          )
  pickc( tab,    no_free+ro+predefined+access_fun          )
  pickc( tb,     no_free+ro+predefined+access_fun          )
  pickk( termt,  no_free+ro+predefined+access_fun          )
  picka( time,   no_free+ro+predefined                     )
  pickc( tiset,  no_free+ro+predefined+access_fun          )
  pickl( tm,     no_free+ro+predefined+access_fun          )
  pickk( userid, no_free+ro+predefined+access_fun          )
  picka( version,no_free+ro+predefined                     )
  pickc( wd,     no_free+ro+predefined+access_fun          )
  pickk( xtext,  no_free+ro+predefined+access_fun          )
  picka( year,   no_free+ro+predefined                     )
