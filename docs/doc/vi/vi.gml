.****************************************************************************
.*
.*                            Open Watcom Project
.*
.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
.*
.*  ========================================================================
.*
.*    This file contains Original Code and/or Modifications of Original
.*    Code as defined in and that are subject to the Sybase Open Watcom
.*    Public License version 1.0 (the 'License'). You may not use this file
.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
.*    provided with the Original Code and Modifications, and is also
.*    available at www.sybase.com/developer/opensource.
.*
.*    The Original Code and all software distributed under the License are
.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
.*    NON-INFRINGEMENT. Please see the License for the specific language
.*    governing rights and limitations under the License.
.*
.*  ========================================================================
.*
.* Description:  Root file of VI documentation.
.*
.* Date         By                      Reason
.* ----         --                      ------
.* 28-sep-05    L. Haynes               reformatted for hlp, figures
.*
.****************************************************************************/

:INCLUDE file='LYTCHG'.
.if &e'&dohelp eq 0 .do begin
.   :INCLUDE file='WNOHELP'
.do end
:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='XDEFS'.
:INCLUDE file='DEFS'.
:INCLUDE file='EXTRA'.
.*
:set symbol="edname" value="Open Watcom Vi Editor".
:set symbol="edvi" value="Vi".
:set symbol="isbn" value="".
.*
:GDOC.
.*
.if &e'&dohelp eq 0 .do begin
.   :FRONTM.
.   :TITLEP.
.   :TITLE stitle="&edvi User's Guide".&edname Reference and User's Guide
.   :INCLUDE file='DOCTITLE'.
.   :eTITLEP.
.   :ABSTRACT.
.   :INCLUDE file='COPYRITE'.
.   :INCLUDE file='DISCLAIM'.
.   :INCLUDE file='NEWSLETT'.
.   :PREFACE.
.   :INCLUDE file='vipref'.
.   .pa odd
.   :TOC.
.   :FIGLIST.
.   .pa odd
.do end
.*
:BODY.
.if &e'&dohelp ne 0 .do begin
.   :exhelp.
.   :INCLUDE file='&book..idx'
.   :INCLUDE file='&book..tbl'
.   :INCLUDE file='&book..kw'
.   .do end
:cmt. .if &e'&dohelp eq 0 .do begin
.   .sepsect The &edname User's Guide
.   :INCLUDE file='intro'.
.   :INCLUDE file='basic'.
.   :INCLUDE file='inter'.
.   :INCLUDE file='advan'.
:cmt. .   .do end
.sepsect The &edname Reference
:INCLUDE file='env'.
:INCLUDE file='modes'.
:INCLUDE file='cmds'.
:INCLUDE file='win_menu'.
:INCLUDE file='settings'.
:INCLUDE file='regexp'.
:INCLUDE file='scripts'.
.sepsect Appendices
:APPENDIX
:INCLUDE file='a_cmdmode'.
:INCLUDE file='a_errmsg'.
:INCLUDE file='a_ctags'.
:INCLUDE file='a_symkeys'.
:INCLUDE file='a_errcode'.
.if &e'&dohelp eq 0 .do begin
.   :BACKM.
.   .cd set 2
.   :INDEX.
.   .do end
.cd set 1
.cntents end_of_book
:eGDOC.
