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
.* Description:  Definitions of GML tags and macros for vi documentation
.*
.* Font Description
.*  0   normal
.*  1   italic
.*  2   bold
.*  3   bold italic
.*  4   fixed
.*  5   fixed 8pt
.*  6   fixed bold
.*  7   symbol
.*  8   sans-serif 14pt
.*  9   sans-serif 16pt
.* 10   sans-serif 20pt
.* 11   sans-serif 24pt
.* 12   sans-serif 32pt
.****************************************************************************/

:cmt. CTRL_A, 0x01

:cmt. .'se cmdmode=';.bd command mode;.ct '
:cmt. .'se tinsmode=';.bd text insertion mode;.ct '
:cmt. .'se cmdline=';.bd command line;.ct '
:cmt. .'se copybuffer=';.bd copy buffer;.ct '

.'se cmdmode='command mode'
.'se tinsmode='text insertion mode'
.'se cmdline='command line'
.'se copybuffer='copy buffer'

.se idcnt=0

.dm setid begin
.sr idcnt=&idcnt.+1
.se _id(&idcnt.)=&*.
.dm setid end

:INCLUDE file='ids'.

.dm getid begin
.if '&*2.' eq '1' .do begin
.   .se idnum = &'veclastpos(&*1, _id)
.   .do end
.el .do begin
.   .se idnum = &'vecpos(&*1, _id)
.   .do end
.if &idnum < 1 .do begin
.   .ty *****Bad ID value for &*1.*****
.   .do end
.dm getid end

.dm @period begin
.ct .li .
.dm @period end
.gt period add @period cont

.dm @cont begin
.ct &*.
.dm @cont end
.gt cont add @cont

.dm keyword begin
:cmt. .ix '&*'
:SF font=2.&*:eSF.
.dm keyword end

.dm keyref begin
.if &e'&dohelp eq 1 .do begin
.   .se *lc1 = &'lower(&*1.)
.   .getid &*lc1. &*2.
.   .if &*0 > 1 .do begin
.   .   .se *id=id&idnum.B
.   .   .do end
.   .el .do begin
.   .   .se *id=id&idnum.
.   .   .do end
.   :ZHDREF refid='&*id'.
.do end
.el .do begin
:SF font=2.&*1:eSF.
.do end
.dm keyref end

.dm param begin
:SF font=2.&*.:eSF.
.dm param end

.dm var begin
:SF font=1.&*.:eSF.
.dm var end

.dm fnlist begin
.if '&*1.' eq 'begin' .do begin
.   .se fn_topic='&*2.'
.   .beglevel
.   .do end
.el .if '&*1.' eq 'end' .do begin
.   .se fn_topic=''
.   .endlevel
.   .do end
.el .do begin
.   .ty *****begin or end required in fnlist*****
.   .do end
.dm fnlist end

.dm begfunc begin
.se func_name = '&*1.'
.se *lc1 = &'lower(&*1.)
.getid &*lc1. 0
.se *id=id&idnum.
.em .section *refid=&*id &func_name.
.if '&fn_topic.' ne '' .do begin
.   .ix '&fn_topic.' '&*lc1.'
.   .do end
.ix '&*lc1.'
.dm begfunc end

.dm endfunc begin
.dm endfunc end

:cmt. the macro 'syntax' is defined in GMLMACS
.dm syntx begin
.if &e'&dohelp eq 0 .do begin
.   :DL termhi=0.
.   :DT.Syntax:
.   .if '&*1.' eq '*' .do begin
.   .   :DD.&*2. &*3. &*4. &*5. &*6. &*7. &*8.
.   .   .do end
.   .el .do begin
.   .   :DD.&*.
.   .   .do end
.   :eDL
.   .do end
.el .do begin
.   :ZDL termhi=0.
.   :ZDT.Syntax:
.   .if '&*1.' eq '*' .do begin
.   .   :ZDD.&*2. &*3. &*4. &*5. &*6. &*7. &*8.
.   .   .do end
.   .el .do begin
.   .   :ZDD.&*.
.   .   .do end
.   :ZeDL
.   .do end
.'se range =';.param &*1.;.ct '
.'se parm1 =';.param &*3.;.ct '
.'se parm2 =';.param &*4.;.ct '
.'se parm3 =';.param &*5.;.ct '
.'se parm4 =';.param &*6.;.ct '
.'se parm5 =';.param &*7.;.ct '
.'se parm6 =';.param &*8.;.ct '
.dm syntx end

.dm begdescr begin
.if &e'&dohelp eq 0 .do begin
.   :DL termhi=0 break.
.   :DT.Description:
.   :DD.
.   .do end
.el .do begin
.   :ZDL termhi=0 break.
.   :ZDT.Description:
.   :ZDD.
.   .do end
.dm begdescr end

.dm enddescr begin
.if &e'&dohelp eq 0 .do begin
.   :eDL.
.   .do end
.el .do begin
.   :ZeDL.
.   .do end
.dm enddescr end

.dm xmplsect begin
.if '&*1.' eq 'begin' .do begin
.   .if &e'&dohelp eq 0 .do begin
.   .   :SF font=2.Example(s): :eSF.
.   .   .in +0.6i
.   .   .do end
.   .el .do begin
.       :ZDL termhi = 0.
.       :ZDT.Example(s):
.       :ZDD.
.   .   .do end
.   .do end
.el .if '&*1.' eq 'end' .do begin
.   .if &e'&dohelp eq 0 .do begin
.   .   .in -0.6i
.   .   .do end
.   .el .do begin
.   .   :ZeDL.
.   .   .do end
.   .do end
.el .do begin
.   .ty *****begin or end required in xmplsect*****
.   .do end
.dm xmplsect end

.dm begxmpl begin
.* courier bold
.if &e'&dohelp eq 0 .do begin
.   :SF font=4.&*.:eSF.
.   .in +0.2i
.   .do end
.el .do begin
.   .np
.   :SF font=4.&*.:eSF.
.   .np
.   .do end
.dm begxmpl end

.dm endxmpl begin
.if &e'&dohelp eq 0 .do begin
.   .in -0.2i
.   .sk
.   .do end
.dm endxmpl end

.dm alsosee begin
.if '&*.' eq 'begin' .do begin
.   .se see = 0
.   .if &e'&dohelp eq 0 .do begin
.       :DL termhi = 0.
.       :DT.See Also: 
.       :DD.
.       .do end
.   .el .do begin
.       :ZDL termhi = 0.
.       :ZDT.See Also: 
.       :ZDD.
.       .do end
.   .do end
.el .if '&*.' eq 'end' .do begin
.   .if &e'&dohelp eq 0 .do begin
.       :eDL.
.       .do end
.   .el .do begin
.       :ZeDL.
.       .do end
.   .do end
.el .do begin
.   .ty *****begin or end required in alsosee*****
.   .do end
.dm alsosee end

.dm seethis begin
.if '&see.' eq '1' .do begin
.   .ct ,
.   .do end
.se see = 1
.if '&*key.' eq '1' .do begin
.   .keyref &*1.
.   key (Command Mode)
.   .do end
.el .do begin
.   .keyref &*.
.   .do end
.dm seethis end

.dm returns begin
.if '&*.' eq 'begin' .do begin
.   .sk
.   .if &e'&dohelp eq 0 .do begin
.       :DL termhi=0 break.
.       :DT.Returns:
.       :DD.:DL break.
.       .do end
.   .el .do begin
.       :ZDL termhi=0 break.
.       :ZDT.Returns:
.       :ZDD.:ZDL break.
.       .do end
.   .do end
.el .if '&*.' eq 'end' .do begin
.   .if &e'&dohelp eq 0 .do begin
.       :eDL.
.       :eDL.
.       .do end
.   .el .do begin
.       :ZeDL.
.       :ZeDL.
.       .do end
.   .do end
.el .do begin
.   .ty *****begin or end required in returns*****
.   .do end
.dm returns end

.dm retval begin
.if &e'&dohelp eq 0 .do begin
.   :DT. &*.
.   :DD.
.   .do end
.el .do begin
.   :ZDT. &*.
.   :ZDD.
.   .do end
.dm retval end

.dm sesect begin
.if '&*1.' eq 'begin' .do begin
.   .beglevel
.   .section 'Example - &*2.'
.   .do end
.el .if '&*1.' eq 'end' .do begin
.   .endlevel
.   .do end
.el .do begin
.   .ty *****begin or end required in sesect*****
.   .do end
.dm sesect end

.dm sexmp begin
.if '&*.' eq 'begin' .do begin
.   .millust begin
.   .se currline=1
.   .do end
.el .if '&*.' eq 'end' .do begin
.   .millust end
.   .do end
.el .do begin
.   .ty *****begin or end required in sexmp*****
.   .do end
.dm sexmp end

.dm seline begin
.if &currline. le 9 .do begin
.   .se *tmp2 = ' &currline.) &*.'
.   .do end
.el .do begin
.   .se *tmp2 = '&currline.) &*.'
.   .do end
.se *tmp = '> &*.'
&*tmp2.
.se line(&currline) = &*tmp.
.sr currline = &currline + 1
.sk
.dm seline end

.dm seref begin
.if '&*.' eq 'begin' .do begin
.   .se sein = 0
.   .do end
.el .if '&*.' eq 'end' .do begin
.   .if &sein. eq 1 .do begin
.   .   .in -0.1i
.   .   .do end
.   .do end
.el .do begin
.   .ty *****begin or end required in seref*****
.   .do end
.dm seref end

.dm segrp begin
&line(&s)
.sk
.sr s = &s + 1
.dm segrp end

.dm serefer begin
.if &sein. eq 1 .do begin
.   .in -0.1i
.   .do end
.se sein = 1
.np
.se s = &*1.
.if '&*2.' ne '' .do begin
.*  italics
.   :SF font=1.Lines &*1.-&*2.:eSF.
.   .se *e = &*2.
.   .do end
.el .do begin
.*  italics
.   :SF font=1.Line &*1.:eSF.
.   .se *e = &*1.
.   .do end
.millust begin
.pe on;.segrp;.if &s gt &*e .pe delete
.millust end
.in +0.1i
.sk
.dm serefer end

.dm rxxmp begin
.if '&*1.' eq 'begin' .do begin
.   :SF font=4.&*2.:eSF.
.   .if &e'&dohelp eq 0 .do begin
.       :OL compact.
.       .do end
.   .el .do begin
        :ZOL compact.
.       .do end
.   .do end
.el .if '&*1.' eq 'end' .do begin
.   .if &e'&dohelp eq 0 .do begin
.       :eOL.
.       .do end
.   .el .do begin
.       :ZeOL.
.       .do end
.   .sk
.   .do end
.el .do begin
.   .ty *****begin or end required in rxxmp*****
.   .do end
.dm rxxmp end

.dm rxorig begin
.if &e'&dohelp eq 0 .do begin
.   :LI.
.   .se orig = '&*.'
.   :DL termhi=0 compact.
.   :DT.String:
.   :DD.
.   :SF font=0.&*.:eSF.
.   :eDL.
.   .do end
.el .do begin
.   :ZLI.
.   .'se orig = '&*.'
.   :ZDL termhi=0 compact.
.   :ZDT.String:
.   :ZDD.
.   :SF font=0.&*.:eSF.
.   :ZeDL.
.   .do end
.dm rxorig end

.dm rxres begin
.if &e'&dohelp eq 0 .do begin
.   :DL termhi=0 compact.
.   :DT.Matches:
.   :DD.
.   .do end
.el .do begin
.   :ZDL termhi=0 compact.
.   :ZDT.Matches:
.   :ZDD.
.   .do end
.if '&*3.' eq '' .do begin
.   .if '&*2.' eq '' .do begin
.   .   &*.
.   .do end
.   .el .do begin
.   .   .if '&*1.' eq '*' .do begin
.   .   .   :SF font=3.&*2.:eSF
.   .   .   .do end
.   .   .el .do begin
.   .   .   :SF font=3.&*1.:eSF
.   .   .   .ct &*2.
.   .   .   .do end
.   .   .do end
.   .do end
.el .do begin
.   .if '&*1.' eq '*' .do begin
.   .   &*2.
.   .   .ct :SF font=3.&*3.:eSF
.   .   .do end
.   .el .do begin
.   .   &*1.
.   .   .ct :SF font=3.&*2.:eSF
.   .   .ct &*3.
.   .   .do end
.   .do end
.if &e'&dohelp eq 0 .do begin
.   :eDL.
.   .do end
.el .do begin
.   :ZeDL.
.   .do end
.dm rxres end

.dm setcmd begin
.se cmd_short='&*short.'
.'se cmd_parm='<&*parm.>'
.'se nocmd_long='no&*1.'
.'se cmd_long='&*1.'
.'se parm='&*parm.'
.se *lc1 = &'lower(&*1.)
.if '&fn_topic' eq 'Boolean Settings' .do begin
.   .getid &*lc1. 1
.   .se *id=id&idnum.B
.   .do end
.el .do begin
.   .getid &*lc1. 0
.   .se *id=id&idnum.
.   .do end
.em .section *refid=&*id &*1.
.if '&fn_topic.' ne '' .do begin
.   .ix '&fn_topic.' '&*1.'
.   .do end
.if '&*short' ne '' .do begin
.   .ix '&*short.'
.   .do end
.ix '&*1.'
.dm setcmd end

.dm setsyntx begin
.if &e'&dohelp eq 0 .do begin
.   :DL termhi=0.
.   :DT.Syntax:
.   .if '&cmd_short.' <> '' .th .do begin
.       :DD.&cmd_long. [&cmd_short.]
.   .   .do end
.   .el .do begin
.   .   .if '&parm.' <> '' .th .do begin
.           :DD.&cmd_long. <&parm.>
.   .   .   .do end
.   .   .el .do begin
.           :DD.&cmd_long.
.   .   .   .do end
.   .   .do end
.   :eDL.
.   .do end
.el .do begin
.   :ZDL termhi=0.
.   :ZDT.Syntax:
.   .if '&cmd_short.' <> '' .th .do begin
.       :zDD.&cmd_long. [&cmd_short.]
.   .   .do end
.   .el .do begin
.   .   .if '&parm.' <> '' .th .do begin
.           :zDD.&cmd_long. <&parm.>
.   .   .   .do end
.   .   .el .do begin
.           :zDD.&cmd_long.
.   .   .   .do end
.   .   .do end
.   :ZeDL
.   .do end
.dm setsyntx end

.dm exmode begin
.if &e'&dohelp eq 0 .do begin
.   :DL termhi = 0.
.   :DT.Notes:
.   :DD.
.   .do end
.el .do begin
.   :ZDL termhi = 0.
.   :ZDT.Notes:
.   :ZDD.
.   .do end
Only valid in
.keyword EX mode
:period.
.if &e'&dohelp eq 0 .do begin
.   :eDL.
.   .do end
.el .do begin
.   :ZeDL.
.   .do end
.dm exmode end

.dm keylist begin
.if '&*1.' eq 'begin' .do begin
.   .if &e'&dohelp eq 0 .do begin
.   .   :DL break.
.   .   .do end
.   .el .do begin
.   .   :ZDL break.
.   .   .do end
.   .se cindex=&*2.
.   .do end
.el .if '&*1.' eq 'end' .do begin
.   .se cindex=''.
.   .if &e'&dohelp eq 0 .do begin
.   .   :eDL.
.   .   .do end
.   .el .do begin
.   .   :ZeDL.
.   .   .do end
.   .do end
.el .do begin
.   .ty *****begin or end required in keylist*****
.   .do end
.dm keylist end

.dm begkey begin
.if &e'&dohelp eq 0 .do begin
.   :DT.&*2. &*3. &*4. &*5. &*6. &*7. &*8.
.   .do end
.el .do begin
.   :ZDT.&*2. &*3. &*4. &*5. &*6. &*7. &*8.
.   .do end
.ix 'Command Mode' '&cindex.' '&*1.'
.se keysp=1
.if &e'&dohelp eq 0 .do begin
.   :DD.
.   .do end
.el .do begin
.   :ZDD.
.   .do end
.dm begkey end

.dm endkey begin
.se keysp=0
.dm endkey end

.dm seecmd begin
.sk
Also see the
:SF font=2.command line:eSF.
command
.keyref &*.
:period.
.dm seecmd end

.* Used in non-hlp portions
.dm exercises begin
.np
.if &e'&dohelp eq 0 .do begin
.* Helvetica 16pt
:SF font=9.Exercises:eSF.
.do end
.el .do begin
:SF font=2.Exercises:eSF.
.do end
.np
.dm exercises end

