.preface Preface
.*
.np
.if '&machsys' eq 'NEC' .do begin
This manual describes the &company C Library support for the Japanese
double-byte character set (DBCS).
.do end
.el .do begin
This manual describes the &company C Library.
It includes the Standard C Library (as defined in the ANSI C Standard)
plus many additional library routines which make application
development for personal computers much easier.
.do end
.if '&machsys' eq 'QNX' .do begin
.np
Copies of this documentation may be ordered from:
.illust begin
        QNX Software Systems Ltd.
        175 Terence Matthews Crescent
        Kanata, Ontario
        CANADA  K2M 1W8
        Phone: 613-591-0931
        Fax:   613-591-3579
.illust end
.do end
.el .if '&machsys' eq 'NEC' .do begin
.np
Copies of this documentation may be ordered from:
.illust begin
        Lifeboat, Inc.
        3-6, Kanda-Nishikicho,
        Chiyoda-ku,
        Tokyo 101, Japan
.illust end
.do end
:cmt. .el .do begin
:cmt. .np
:cmt. Copies of this documentation may be ordered from:
:cmt. .illust begin
:cmt.         WATCOM International Corp.
:cmt.         415 Phillip St.
:cmt.         Waterloo, Ontario
:cmt.         CANADA  N2L 3X2
:cmt.         Phone: 519-886-3700
:cmt.         Fax:   519-747-4971
:cmt. .illust end
:cmt. .do end
.*
.sect Acknowledgements
.*
.im gmlack
.*
:cmt. .np
:cmt. Many members of &company International Corp. have made a significant
:cmt. contribution to the design and implementation of the &company C
:cmt. compiler, the code generator, and the many related tools.
:cmt. The design of this software is based upon ideas evolved and proven
:cmt. over the past decade in other software projects in which these people
:cmt. have been involved.
:cmt. The major project members of the
:cmt. .if '&machsys' eq 'QNX' .do begin
:cmt. QNX
:cmt. .do end
:cmt. .el .do begin
:cmt. DOS, Microsoft Windows, and OS/2
:cmt. .do end
:cmt. implementation of &company C and related tools were Scott Beech, Geno
:cmt. Coschi, Fred Crigger, John Dahms, Craig Eisler, Steven Fry, Dean Gaudet,
:cmt. Michael Glaum, Jim Graham, Bill Hillis, Tom Ivey, Alex Kachura, Jim Randall,
:cmt. Jack Schueler, Anthony Scian, Brian Stecher, Aaron Stein, Anderson Wai,
:cmt. David Wellstood and David Yach.
:cmt. .if '&machsys' ne 'QNX' .do begin
:cmt. .np
:cmt. F. W. Crigger
:cmt. .br
:cmt. J. B. Schueler
:cmt. .br
:cmt. J. W. Welch
.np
&pubdate..
:cmt. .do end
.*
.sect Trademarks Used in this Manual
.*
.np
IBM is a registered trademark of International Business Machines Corp.
.np
Intel is a registered trademark of Intel Corp.
.np
Microsoft, MS, MS-DOS, Windows, Win32, Win32s, Windows NT and
Windows 2000 are either registered trademarks or trademarks of
Microsoft Corporation in the United States and/or other countries.
.np
NetWare, NetWare 386, and Novell are registered trademarks of Novell,
Inc.
.np
.* not using .tmark macros here because xdefs.gml isn't included
UNIX is a registered trademark of The Open Group.
.if '&machsys' eq 'QNX' .do begin
.np
QNX is a registered trademark of QNX Software Systems Ltd.
.do end
.np
.im wtrdmrk
