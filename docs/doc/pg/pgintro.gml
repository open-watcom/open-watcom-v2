.chap &product Application Development
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if &e'&dohelp eq 1 .do begin
:HBMP 'pguide.bmp' i
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.if &e'&dohelp eq 1 .do begin
:HBMP 'fpguide.bmp' i
.do end
.do end
.np
.ix 'application development'
.if '&target' eq 'DOS' .do begin
This document contains guides to application development for several
environments including 16-bit DOS, 32-bit extended DOS, Windows 3.x,
32-bit extended Windows 3.x, Windows NT/2000/XP, Win9x, OS/2, and Novell
NLMs.
It also describes mixed language (C, FORTRAN) application development.
.do end
.el .if '&target' eq 'AXPNT' .do begin
This document contains a guide to application development for Windows
NT running on the Digital Alpha AXP.
.do end
.el .if '&target' eq 'QNX' .do begin
This document contains a guide to application development for QNX.
.do end
It concludes with a chapter on some general questions and the answers
to them.
.np
This document covers the following topics:
.begbull
.*
.if '&target' eq 'DOS' .do begin
.*
.bull
DOS Programming Guide
.begnote $compact
.note Creating 16-bit DOS Applications
.note Creating 32-bit Phar Lap 386|DOS-Extender Applications
.note Creating 32-bit DOS/4GW Applications
:cmt. .note Creating 32-bit Phar Lap TNT DOS Extender Applications
:cmt. .note Creating 32-bit FlashTek DOS Extender Applications
.note 32-bit Extended DOS Application Development
.endnote
.bull
The DOS/4GW DOS Extender
.begnote $compact
.note The Tenberry Software DOS/4GW DOS Extender
.note Linear Executables
.note Configuring DOS/4GW
.note VMM
.note Interrupt 21H Functions
.note Interrupt 31H DPMI Functions
.note Utilities
.note Error Messages
.note DOS/4GW Commonly Asked Questions
.endnote
.bull
Windows 3.x Programming Guide
.begnote $compact
.note Creating 16-bit Windows 3.x Applications
.note Porting Non-GUI Applications to 16-bit Windows 3.x
.note Creating 32-bit Windows 3.x Applications
.note Porting Non-GUI Applications to 32-bit Windows 3.x
.note The &company 32-bit Windows Extender
.note Windows 3.x 32-bit Programming Overview
.note Windows 32-Bit Dynamic Link Libraries
.note Interfacing Visual Basic and &product DLLs
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.note WIN386 Library Functions and Macros
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note WIN386 Library Subprograms
.do end
.note 32-bit Extended Windows Application Development
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.note Special Variables for Windows Programming
.note Definitions of Windows Terms
.do end
.note Special Windows API Functions
.endnote
.*
.do end
.*
.bull
Windows NT Programming Guide
.begnote $compact
.note Windows NT Programming Overview
.note Creating Windows NT GUI Applications
.note Porting Non-GUI Applications to Windows NT GUI
.note Windows NT Multi-threaded Applications
.note Windows NT Dynamic Link Libraries
.endnote
:cmt. .bull
:cmt. Windows 95 Programming Guide
:cmt. .begnote $compact
:cmt. .note Windows 95 SDK Notes
:cmt. .endnote
.*
.if '&target' eq 'DOS' .do begin
.*
.bull
OS/2 Programming Guide
.begnote $compact
.note Creating 16-bit OS/2 1.x Applications
.note Creating 32-bit OS/2 Applications
.note OS/2 Multi-threaded Applications
.note OS/2 Dynamic Link Libraries
.note Programming for OS/2 Presentation Manager
.endnote
.bull
Novell NLM Programming Guide
.begnote $compact
.note Creating NetWare 386 NLM Applications
.endnote
.bull
Mixed Language Programming
.begnote $compact
.note Inter-Language calls: C and FORTRAN
.endnote
.*
.do end
.*
.bull
Common Problems
.begnote $compact
.note Commonly Asked Questions and Answers
.endnote
.endbull
