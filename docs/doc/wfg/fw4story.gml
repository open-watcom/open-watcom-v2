.npsect The WATFOR Story
.*
.np
The &product compiler's design is rooted in, what to many of us
are, the early days of computing.  In the summer of 1965, four
undergraduate students of the University of Waterloo, Gus German,
Jim Mitchell, Richard Shirley and Robert Zarnke, led by Peter
Shantz, developed a FORTRAN compiler for the IBM&rt. 7040 computer
and called it WATFOR&rt..  Its objectives were fast compilation
speed and effective error diagnostics at both compile and
execution time.  It eliminated the need for a separate linking
procedure and, as a result, FORTRAN programs which contained no
syntax errors were placed into immediate execution.  Similar
experiments were carried on at the University of Wisconsin (FORGO
on the IBM 1620) and at Purdue University (PUFFT on the IBM
7090).
.np
This simple, one-step approach to the processing of programs was
revolutionary to many people.  Non-experienced programmers could
be taught programming at minimal cost in time and computing
resources.  Experienced programmers quickly saw the benefits of
the compiler's good diagnostic capabilities and fast turn-around.
Gone were the days of pouring through computer memory "dumps"
trying to discover the error in a particular program.  This early
version of WATFOR quickly gained popularity and over 75
institutions installed it on their IBM 7040 systems.  The
distribution of the compiler was handled by Sandra Bruce (nee
Hope).
.np
In 1966, the University decided to replace the 7040 with an IBM
360 computer.  This meant that a replacement for the 7040 version
of WATFOR had to be created for this new computer.  A team of
full-time employees and undergraduate students was formed to
write an IBM 360 version.  The project members, Betty Schmidt,
Paul Dirksen, Paul Cress, Lothar K.  "Ned" Kesselhut, Bill Kindree
and Dereck Meek, who were later joined by Mike Doyle, Rod Milne,
Ron Hurdal and Lynn Williams, completed 360 WATFOR in the early
part of 1967.  The compiler was a great success and many other
institutions (universities, colleges, businesses and governmental
agencies) started using the WATFOR compiler to meet needs similar
to those experienced at the University of Waterloo.  The
distribution of the software and customer support was carried on
by Sandra Ward.
.np
As a result of ideas put forth by the SHARE FORTRAN Committee and
others, a new version of WATFOR, called WATFIV&rt., was produced in
1968.  WATFIV introduced new features such as CHARACTER variables
and direct-access input-output.  For their contribution to the
advancement of computing through their efforts in the
WATFOR/WATFIV projects, Paul Cress and Paul Dirksen were
presented with the Grace Murray Hopper Award by the Association
for Computing Machinery (ACM) in 1972.  The WATFIV compiler was
included in the DATAPRO Honour Roll for 1975 and 1976, and has
received honourable mention in other years.
.np
Along the way a number of people were involved with the
maintenance and enhancement of WATFOR and WATFIV.  They include
Bernie Murphy, Martin Wiseman and Yvonne Johnson.
.np
In 1974, a compiler with characteristics similar to the IBM
implementation was created for the Digital Equipment Corporation
PDP11&tm. computer and called WATFOR-11.  The team members, Jack
Schueler, Jim Welch and Terry Wilkinson, were later joined by Ian
McPhee who had been busy adding new control statements to the
WATFIV compiler to facilitate Structured Programming (SP).  These
new statements included the block IF (later included in the ANSI
X3.9-1978 language standard), WHILE, UNTIL, etc.  A new version of
WATFIV, called WATFIV-S, was announced in 1974 and a few months
later, WATFOR-11S (the "S" indicating the new SP features) was
also announced.  The original SP features were later enhanced with
additional statements by Bruce Hay in WATFIV-S (1980) and by Jack
Schueler in WATFOR-11S (1981).
.np
.* As of this date, there are over 350 installations using WATFIV
.* and over 140 installations using WATFOR-11.
WATFIV and WATFOR-11 compilers are widely used and have earned a
worldwide reputation.  Universities and corporations on every
continent use these compilers and a number of other Waterloo and
WATCOM software products which have been developed in the WATFOR
tradition.  Currently, more than 3,000 mini and mainframe computer
licenses and over 100,000 microcomputer licenses are held
worldwide for this family of software products.
.np
A very important contributor to this success story has been
Professor J. Wesley Graham.
Over the years, he has provided leadership and inspiration to all
of those involved in the various projects.
Although many of the people we have mentioned above have since
departed, he has managed to entice a number of dedicated people
to stay in Waterloo.
Continuity of personnel has resulted in continuity of product
support and maintenance.
This ongoing service to our users over the years has been
instrumental to the continued popularity of the software.
.np
During the 1970's, the ANSI X3J3 subcommittee (the FORTRAN
language standard group) developed a new language standard which
was officially approved in April, 1978.  This standard, designated
FORTRAN 77, introduced many new statements into the language.  In
fact, the previous language standard FORTRAN 66 was a very small
document and described, what was in effect, a subset of most
implementations of FORTRAN.  For example, the WATFIV and WATFOR-11
implementations were based upon the IBM definition of FORTRAN-IV.
.np
It became obvious, as programmers began to use the features of
FORTRAN 77, that a new compiler was required which combined the
desirable attributes of the WATFIV compiler and supported the new
language standard.  In January of 1983, a project to develop a
FORTRAN 77 compiler was started at WATCOM Systems Inc.  Under the
leadership of Jack Schueler, a team of full-time WATCOM employees
and undergraduate students from the University of Waterloo's
Co-operative Computer Science program was involved in the
creation of the compiler.  The major work was done by Geno Coschi,
Fred Crigger, John Dahms, Jim Graham, Jack Schueler, Anthony
Scian and Paul Van Oorschot.  They were assisted by Rod Cremasco,
John McCormick, David McKee and Brian Stecher.  Many of the team
members from former WATFOR/WATFIV compiler projects, who are now
working with WATCOM, provided valuable input.  These include Bruce
Hay, Ian McPhee, Sandra Ward, Jim Welch and Terry Wilkinson.
.np
Unlike the previous WATFOR compilers, a significant portion of
WATFOR-77
.fn begin
WATFOR and WATFIV are registered trademarks of the University of
Waterloo; the name "WATFOR-77" is used with the permission of the
University of Waterloo.
.fn end
was to be written in a portable systems language.
We hoped this approach would make the implementation of the compiler
on various computer systems somewhat easier.
This just wasn't feasible for earlier WATFOR compilers which were
written entirely in machine language.
.np
It was decided that two components of the compiler would not be
portable.  The code generator would translate FORTRAN statements
into native computer instructions and store them in memory.  The
first version of WATFOR-77 would generate instructions for the
IBM 370 computer architecture.  As well, most of the
execution-time support (undefined variable checking, subscript
evaluation, intrinsic routines like SIN, ATAN2, etc.) would be
written in assembly language.  We felt this approach would ensure
optimal performance of the executable code.
.np
In September of 1984, the first version of WATFOR-77 was
installed at the University of Waterloo for use by members of the
Department of Computing Services.  It was an implementation for
the IBM 370 series of computers running the VM/SP CMS&rt. operating
system.
.np
A few months earlier, in May of 1984, a project was started to
implement the WATFOR-77 compiler on the IBM Personal Computer.
The members of this project included Geno Coschi, Fred Crigger,
Tim Galvin, Athos Kasapi, Jack Schueler, Terry Skomorowski and
Brian Stecher.
.np
In April of 1985, this second version of WATFOR-77 was installed
at the University of Waterloo for use by students of the Faculty
of Engineering.  The compiler could run on a 256K IBM Personal
Computer using IBM PC DOS&tm. 2.0 and did not require special
floating-point hardware.  The personal computers were connected to
the JANET network.  Subsequently, the compiler was installed on
the WatStar personal computer network.
.np
In the fall of 1985, a Japanese version of WATFOR-77 was
delivered to IBM Japan for the IBM JX Personal Computer.
This version produces Japanese language error messages and
supports the Kanji, Hiragana and Katakana character sets for
variable names and character strings.
To support the use of the JX version of WATFOR-77, the WATFOR-77
Language Reference manual and User's Guide were translated into
Japanese.
.np
During the summer of 1986, the IBM PC version of WATFOR-77 was adapted
to run on the Unisys ICON&rt., an iAPX-186 based microcomputer system
that runs the QNX&rt. operating system.
Since the UNIX&rt.-like QNX was quite different from IBM PC DOS, parts
of the run-time system were rewritten.
This implementation of WATFOR-77 was made available in September of
1986.
.np
During the summer of 1985, a project was started to adapt WATFOR-77 to
the Digital VAX&rt. computer series running the VMS&rt. operating
system.
The members of this project included Geno Coschi, Marc Ouellette,
Jack Schueler and Terry Skomorowski.
This implementation was made available in March of 1987.
.np
In the spring of 1988, a version of WATFOR-77 was adapted to the
Japanese IBM PS/55 family of personal computers.
As on the IBM JX Personal Computer, this version produces Japanese
language error messages and supports the Kanji, Hiragana and Katakana
character sets for variable names and character strings.
.np
Also, in the spring of 1988, a new project was begun to develop an
optimizing FORTRAN 77 compiler.
This compiler was based on the work done in the WATFOR-77 and WATCOM C
compiler projects.
The code generator used was the same one that was employed by the
WATCOM C compiler.
The code produced by the WATCOM C compiler was (and still is) superior
to other C compilers.
We realized that we could repeat this achievement in a FORTRAN
optimizing compiler.
The FORTRAN 77 optimizing compiler was first shipped in mid 1990.
.np
In October of 1990, the 25th anniversary of WATFOR was celebrated.
All those who were involved in the development of the various WATFOR
compilers were invited to the University of Waterloo for a reunion.
.np
In the spring of 1992, a version of WATFOR-77 was adapted to the NEC
PC-9801 family of personal computers.
This version was similar to the IBM PS/55 version but was modified to
accommodate architectural differences between the IBM PS/55 and the
NEC PC-9801.
.np
In January of 1992, the development of a 32-bit version of WATFOR-77
for 386- and 486-based personal computers was begun.
The first version of the 32-bit WATFOR-77 was shipped in the fall of
1992.
.sk 2 c
.*
.dashes
.*
.sk 2 c
IBM and IBM 370 VM/SP CMS are registered trademarks of
International Business Machines Corporation;
PC DOS is a trademark of IBM;
PDP, VAX and VMS are trademarks of Digital Equipment
Corporation;
ICON is a registered trademark of Unisys;
.tmarkqnx
.tmarkunix
