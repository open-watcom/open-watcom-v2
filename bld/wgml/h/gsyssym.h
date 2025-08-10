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
* Description:  WGML predefined system symbols.
*
****************************************************************************/


/* Included exclusively from gsyssym.c. See gsyssym.c for the pick macros. */

//      symbol   flags
//      name
  pickl( ad,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( adeven,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( adodd,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( amp,     SF_no_free | SF_ro | SF_predefined | SF_is_AMP     )
  pickl( apage,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( bc,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( be,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( bfonts,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( bm,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( bo,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( bs,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( bx,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( bxchar,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( cc,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ccc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( cd,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( cdcount, SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( chars,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( cl,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( co,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( cont,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( cp,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( cpages,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( cpc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( cpi,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( cw,      SF_no_free | SF_ro | SF_predefined                 )
  picka( date,    SF_no_free | SF_ro | SF_predefined                 )
  picka( dayofm,  SF_no_free | SF_ro | SF_predefined                 )
  picka( dayofw,  SF_no_free | SF_ro | SF_predefined                 )
  picka( dayofy,  SF_no_free | SF_ro | SF_predefined                 )
  pickk( dfonts,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( dhset,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( dpage,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( duplex,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( env,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( fb,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fbc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fbf,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( file,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( fk,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fkc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( flnest,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fm,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( fn,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( fnam,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fnc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fnum,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( fonts,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( fs,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( gml,     SF_no_free | SF_ro | SF_predefined                 )
  pickl( gutter,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( hi,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( hm,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( hn,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( hnc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( hour,    SF_no_free | SF_ro | SF_predefined                 )
  pickl( hs,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( hy,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( hyc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( hyph,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( in,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( inr,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ir,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( ixj,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ixref,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( ju,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( layout,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( lc,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( li,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( linb,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( line,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ll,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( lnum,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ls,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( lst,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( mc,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( mcs,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( member,  SF_no_free | SF_ro | SF_predefined                 )
  picka( minute,  SF_no_free | SF_ro | SF_predefined                 )
  picka( month,   SF_no_free | SF_ro | SF_predefined                 )
  pickk( nodeid,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( of,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( offline, SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( online,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ooc,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( out,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( page,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( paged,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pagelm,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pagerm,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( parm,    SF_no_free | SF_ro | SF_predefined                 )
  pickl( passno,  SF_no_free | SF_ro | SF_predefined                 )
  pickl( passof,  SF_no_free | SF_ro | SF_predefined                 )
  picka( pdayofw, SF_no_free | SF_ro | SF_predefined                 )
  picka( pdev,    SF_no_free | SF_ro | SF_predefined                 )
  pickl( pgnuma,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pgnumad, SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pgnumc,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pgnumcd, SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pgnumr,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pgnumrd, SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( pi,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( pix,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pl,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( pls,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( pmonth,  SF_no_free | SF_ro | SF_predefined                 )
  pickl( pn,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ppage,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( prs,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( prt,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( ps,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( pw,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( pyear,   SF_no_free | SF_ro | SF_predefined                 )
  pickk( quiet,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( rb,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( recno,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( resh,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( resv,    SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( ret,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( rmnest,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( sc,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( screen,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( second,  SF_no_free | SF_ro | SF_predefined                 )
  pickl( seqno,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( skcond,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( sl,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( spcond,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( su,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( sys,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( tab,     SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickc( tb,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( termt,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( time,    SF_no_free | SF_ro | SF_predefined                 )
  pickc( tiset,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickl( tm,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( userid,  SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( version, SF_no_free | SF_ro | SF_predefined                 )
  pickc( wd,      SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  pickk( xtext,   SF_no_free | SF_ro | SF_predefined | SF_access_fun )
  picka( year,    SF_no_free | SF_ro | SF_predefined                 )
