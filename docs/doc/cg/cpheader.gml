.chap *refid=cphdr Precompiled Headers
.*
.if &e'&dohelp eq 0 .do begin
.section Using Precompiled Headers
.do end
.*
.np
.ix 'precompiled headers'
&product supports the use of precompiled headers to decrease the time
required to compile several source files that include the same header
file.
.*
.section When to Precompile Header Files
.*
.np
.ix 'precompiled headers' 'uses'
Using precompiled headers reduces compilation time when:
.begbull
.bull
You always use a large body of code that changes infrequently.
.bull
Your program comprises multiple modules, all of which use the same
first include file and the same compilation options.
In this case, the first include file along with all the files that it
includes can be precompiled into one precompiled header.
.endbull
.np
Because the compiler only uses the first include file to create
a precompiled header, you may want to create a master or global
header file that includes all the other header files that you wish
to have precompiled.
Then all source files should include this master header file
as the first
.id #include
in the source file.
Even if you don't use a master header file, you can benefit from
using precompiled headers for Windows programs by using
.mono #include <windows.h>
as the first include file, or by using
.mono #include <afxwin.h>
as the first include file for MFC applications.
.np
The first compilation &mdash. the one that creates the precompiled
header file &mdash. takes a bit longer than subsequent compilations.
Subsequent compilations can proceed more quickly by including the
precompiled header.
.np
You can precompile C and C++ programs.
In C++ programming, it is common practice to separate class
interface information into header files which can later be
included in programs that use the class.
By precompiling these headers, you can reduce the time a program
takes to compile.
.remark
Although you can use only one precompiled header (:FNAME..PCH:eFNAME.)
file per source file, you can use multiple
.fi .PCH
files in a project.
.eremark
.*
.section Creating and Using Precompiled Headers
.*
.np
.ix 'precompiled headers' 'using'
Precompiled code is stored in a file called a precompiled header when
you use the precompiled header option
(
.ct;.bd &sw.fh
or
.ct;.bd &sw.fhq
.ct )
on the command line.
The
.bd &sw.fh
option causes the compiler to either create a precompiled header or
use the precompiled header if it already exists.
The
.bd &sw.fhq
option is similar but prevents the compiler from issuing informational
or warning messages about precompiled header files.
The default name of the precompiled header file is one of
.fi WCC.PCH,
.fi WCC386.PCH,
.fi WPP.PCH,
or
.fi WPP386.PCH
(depending on the compiler used).
You can also control the name of the precompiled header that is
created or used with the
.bd &sw.fh=filename
or
.bd &sw.fhq=filename
("specify precompiled header filename") options.
.exam begin
&sw.fh=projectx.pch
&sw.fhq=projectx.pch
.exam end
.*
.section The "&sw.fh[q]" (Precompiled Header) Option
.*
.np
.ix 'precompiled headers' 'compiler options'
The
.bd &sw.fh
option instructs the compiler to use a precompiled header file with a
default name of
.fi WCC.PCH,
.fi WCC386.PCH,
.fi WPP.PCH,
or
.fi WPP386.PCH
(depending on the compiler used) if it exists or to create one if it
does not.
The file is created in the current directory.
You can use the
.bd &sw.fh=filename
option to change the default name (and placement) of the precompiled
header.
Add the letter "q" (for "quiet") to the option name to prevent the
compiler from displaying precompiled header activity information.
.np
The following command line uses the
.bd &sw.fh
option to create a precompiled header.
.exam begin
&pcmd16. &sw.fh myprog.cpp
&pcmd32. &sw.fh myprog.cpp
.exam end
.pc
The following command line creates a precompiled header named
.fi myprog.pch
and places it in the
.fi &pc.projpch
directory.
.exam begin
&pcmd16. &sw.fh=&pc.projpch&pc.myprog.pch myprog.cpp
&pcmd32. &sw.fh=&pc.projpch&pc.myprog.pch myprog.cpp
.exam end
.np
The precompiled header is created and/or used when the compiler
encounters the first
.id #include
directive that occurs in the source file.
In a subsequent compilation, the compiler performs a consistency check
to see if it can use an existing precompiled header.
If the consistency check fails then the compiler discards the existing
precompiled header and builds a new one.
.np
The
.bd &sw.fhq
form of the precompiled header option prevents the compiler from
issuing warning or informational messages about precompiled header
files.
For example, if you change a header file, the compiler will tell you
that it changed and that it must regenerate the precompiled header
file.
If you specify
.bd &sw.fhq
then the compiler just generates the new precompiled header file
without displaying a message.
.*
.section *refid=cphdrc Consistency Rules for Precompiled Headers
.*
.np
.ix 'precompiled headers' 'rules'
If a precompiled header file exists (either the default file or
one specified by
.bd &sw.fh=filename
.ct ),
it is compared to the current compilation for consistency.
A new precompiled header file is created and the new file overwrites
the old unless the following requirements are met:
.begbull
.bull
The current compiler options must match those specified when the
precompiled header was created.
.bull
The current working directory must match that specified when the
precompiled header was created.
.bull
The name of the first
.id #include
directive must match the one that was specified when the precompiled
header was created.
.bull
All macros defined prior to the first
.id #include
directive must have the same values as the macros defined when the
precompiled header was created.
A sequence of
.id #define
directives need not occur in exactly the same order because there are
no semantic order dependencies for
.id #define
directives.
:cmt. .bull
:cmt. If new macros are defined on a subsequent compile that were not
:cmt. defined when the precompiled header was created, you may need to
:cmt. manually erase the precompiled header in order to successfully compile
:cmt. the file.
.bull
The value and order of include paths specified on the command line
with
.bd &sw.i
options must match those specified when the precompiled header was
created.
.bull
The time stamps of all the header files (all files specified with
.id #include
directives) used to build the precompiled header must match those that
existed when the precompiled header was created.
.endbull
