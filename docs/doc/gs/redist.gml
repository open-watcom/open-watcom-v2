.sr licagr='Powersoft Language Products Software License Agreement'
.sr liccmp='Powersoft'
.*
.chap Additional Redistribution Rights
.*
.np
Please read carefully the information in the following sections if you
plan to distribute your application to others.
.*
.section Redistributable Components
.*
.np
Subject to the terms and conditions of the &licagr, in addition to any
Redistribution Rights granted therein, you are hereby granted a
non-exclusive, royalty-free right to reproduce and distribute the
Components specified below provided that (a) it is distributed as part
of and only with your software product; (b) you not suppress, alter or
remove proprietary rights notices contained therein; and (c) you
indemnify, hold harmless and defend &liccmp and its suppliers from
and against any claims or lawsuits, including attorney's fees, that
arise or result from the use or distribution of your software product.
.np
The Tenberry Software (formerly Rational Systems, Inc.)
and &company Components are:
.begbull
.bull
DOS4GW.EXE Copyright :SF font=7.~C:eSF. 1990-1994 Tenberry Software, Inc.
.bull
RMINFO.EXE Copyright :SF font=7.~C:eSF. 1987-1993 Tenberry Software, Inc.
.bull
PMINFO.EXE Copyright :SF font=7.~C:eSF. 1987-1993 Tenberry Software, Inc.
.bull
PRIVATXM.EXE Copyright :SF font=7.~C:eSF. 1991 Tenberry Software, Inc.
.bull
DOS4GW.DOC Copyright :SF font=7.~C:eSF. 1991-1995 Tenberry Software, Inc.
.bull
WEMU387.386 Copyright :SF font=7.~C:eSF. 1991-&cpyyear. &spcompany.
.endbull
.np
The Microsoft Components from the Microsoft Windows SDK version 3.1,
all Copyright :SF font=7.~C:eSF. 1992 Microsoft Corporation, are:
:UL compact.
:LI
COMMDLG.DLL
:LI
COMMDLG.DAN
:LI
COMMDLG.DUT
:LI
COMMDLG.FIN
:LI
COMMDLG.FRN
:LI
COMMDLG.GER
:LI
COMMDLG.ITN
:LI
COMMDLG.NOR
:LI
COMMDLG.POR
:LI
COMMDLG.SPA
:LI
COMMDLG.SWE
:LI
DDEML.DLL
:LI
LZEXPAND.DLL
:LI
OLECLI.DLL
:LI
OLESVR.DLL
:LI
PENWIN.DLL
:LI
SHELL.DLL
:LI
STRESS.DLL
:LI
TOOLHELP.DLL
:LI
VTD.386
:LI
VER.DLL
:LI
DIB.DRV
:LI
EXPAND.EXE
:LI
MARKMIDI.EXE
:LI
REGLOAD.EXE
:LI
WINHELP.EXE
:LI
WINHELP.HLP
:eUL.
.*
.section OS/2 Toolkit
.*
.np
You may distribute components from the OS/2 Toolkit included in
the package which are header files and include files and you may
modify and distribute components from the OS/2 Toolkit included in
the package which are sample programs provided that such header
files, include files and sample programs are distributed only for
the purposes of developing, using, marketing and distributing
application programs written to the OS/2 application programming
interface.  Further, with respect to sample programs, each
copy of any portion thereof or any derivative work which is so
distributed must include a copyright notice as follows:
.code begin
(c) Copyright (your company name) (year). All Rights Reserved.
.code end
.*
.if '&lang' eq 'C/C++' .do begin
.*
.section IBM SOMobjects
.*
.np
You may sublicense the IBM SOMobjects Kernel and Workstation DSOM,
Version 2.0 for OS/2 and for Windows but only if
incorporated as part of your application and only if: your
application adds significant function to the function of the IBM
SOM and DSOM components; you retain in any copies made of the IBM
SOM and DSOM components all IBM copyright and proprietary rights
notices contained therein.  Further, you include a copyright
notice in the following format on the media label attached to any
copies of your application which includes the IBM SOM and DSOM
components:
.code begin
(c) Copyright (your company name) and others (date).  All Rights Reserved.
.code end
you indemnify &liccmp and International Business Machines
Corporation, including their subsidiaries, from and against any claim
arising from the distribution of your application or otherwise arising
hereunder excluding any claims that the IBM SOM and DSOM components
infringe a U.S. copyright held by a third party; your application and
the IBM SOM and DSOM components are sublicensed pursuant to a written
license agreement between you and your customer which conforms
substantially to the terms and conditions of the &licagr (printed
elsewhere in this document) together with the additional terms and
conditions set forth herein. Without limiting the generality of the
foregoing, said license agreement shall indicate that you are the
licensor and shall contain provisions limiting &liccmp's and IBM
liability to the same extent as they are limited in the &licagr and
these additional terms and conditions.
.np
You may sublicense to your customers the rights to reproduce and
distribute the IBM SOM and DSOM components provided such
sublicense is in writing and imposes terms and conditions upon
your customers which are no less restrictive as are imposed upon
you as set forth in the &licagr (printed elsewhere in this document)
together with the additional terms and conditions set forth herein.
.do end
.*
.if '&lang' eq 'C/C++' .do begin
.section Microsoft MFC, Win32s
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.section Microsoft Win32s
.do end
.*
.np
You may sublicense
.if '&lang' eq 'C/C++' .do begin
the Microsoft Foundation Class Libraries and Win32s
(collectively called the "MS CODE")
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
Microsoft Win32s
(hereinafter referred to as the "MS CODE")
.do end
but only in object-code form and solely in conjunction with your
application developed using &product and only if:
your application and/or any documentation bears copyright notices
sufficient to protect Microsoft's copyright in the MS CODE;
the MS CODE is not incorporated into any product which would enable
such MS CODE to be used in conjunction with, or to develop application
programs for non-Microsoft operating systems (This restriction does
not apply to MS CODE which runs or can be run on non-Microsoft
operating systems without modification),
you indemnify &liccmp and Microsoft from and against any claim arising
from the distribution of your application or otherwise arising
hereunder excluding any claims that the MS CODE infringes a U.S.
copyright held by a third party;
your application and the MS CODE are sublicensed pursuant to a written
license agreement between you and your customer which conforms
substantially to the terms and conditions of the &licagr (printed
elsewhere in this document) together with the additional terms and
conditions set forth herein.
Without limiting the generality of the foregoing, said license
agreement shall indicate that you are the licensor and shall contain
provisions limiting &liccmp's and Microsoft's liability to the same
extent as they are limited in the &licagr and these additional terms
and conditions.
.if '&lang' eq 'C/C++' .do begin
You may also modify the MFC components of the MS CODE and reproduce
and distribute such modifications in object-code form as part of your
application.
.do end
