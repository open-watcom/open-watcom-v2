
FILE        : bld\hdr\readme.txt
LAST REVISED: 2004-08-05
SUBJECT     : Overview of the header project.

This file is intended to provide developers working on the header
project for the first time a quick overview of how the project is
organized and how the headers are generated. Be aware that the
description provided here is not 100% complete. The authors of this
document do not necessarily understand all aspects of the header project
or its history. Thus this document is likely to contain omissions and
errors.

OVERVIEW

The Open Watcom headers are generated from source files in the watcom
directory using a tool named 'wsplice'. The source code for wsplice is
in the file bld\builder\c\wsplice.c. During the build of Open Watcom
this file is compiled and the executable is copied to the appropriate
binary directory under bld\build. The only documentation for wsplice is
contained in the comments at the top of its source file and, of course,
indirectly in the source code itself.

The wsplice program is a kind of preprocessor and provides features that
are similar in some ways to the C preprocessor (although it is much
simpler). In particular, it reads an input file containing wsplice
"directives" and outputs the file that results after processing those
directives. The wsplice program supports the ability to select or ignore
parts of the input file based on the values of "macros" that are defined
on the wsplice command line. In addition, wsplice allows other files to
be included into the output in a manner similar to the way #include
works in a C program.

The makefiles that generate the headers run wsplice on the various .mh
files in the watcom directory. Depending on which headers are being made
and on the features selected, the output of wsplice as applied to the
.mh files will vary. Note that header "fragments" that appear in
multiple headers have been broken out as files with a .sp extension in
the top level directory of the project. These files are included into
various output headers as appropriate by wsplice directives in the .mh
source files. This allows material that appears in multiple headers to
be defined in only one place in the header project, simplifying
maintenence.

Headers for most of the target systems (DOS, Windows, WinNT, OS/2) are
generated into the 'dos' directory. Headers for Linux are generated into
the 'linux' directory. Headers for QNX are disabled by default. When
they are activated the QNX headers are generated into the 'qnx'
directory. As is usual for Open Watcom projects, each of these directories
contains a makefile that will cause wmake to generate the headers for
that target system.

The makefiles in each target directory include 'master.mif' from the top
level directory. This master file contains information about which files
are to be generated and provides implicit rule definitions. The actual
dependency lists (for example which .sp header fragment files are used
in each .mh source file) are defined in deps.mif. In addition there is a
builder lang.ctl file in the top level directory that oversees the build
process and provides commands to copy the generated headers to the rel2
tree when appropriate.

C/C++ LIBRARY HEADERS

The standard C library headers, as defined by ISO C90, are as follows:

<assert.h>         <locale.h>           <stdio.h>
<ctype.h>          <math.h>             <stdlib.h>
<errno.h>          <setjmp.h>           <string.h>
<float.h>          <signal.h>           <time.h>
<iso646.h>         <stdarg.h>           <wchar.h>
<limits.h>         <stddef.h>           <wctype.h>

These headers are significant because Standard C++98 requires that for
each of them a corresponding "cname" style header also be available. For
example, in addition to <stdio.h>, the Standard C++98 requires a header
<cstdio>. Note that ISO C99 defines additional headers that are not
part of Standard C++98.

To satisfy these requirements, the default build process executes
wsplice twice on the source files for the headers above. In addition to
a default execution that generates the "name.h" form of the header,
wsplice is executed with the CNAME macro defined to generate the cname
form of the header. Both name.h and cname forms are mentioned in
master.mif with rules to copy both forms to the rel2 tree in lang.ctl.
Since both name.h and cname forms of a header have the same
dependencies, wmake macros are defined in deps.mif, one for each .mh
file, that are subsequently used to define dependencies for both header
forms. Note that it is awkward to work with extensionless files with
wmake. Currently the rules for building each cname file is explicitly
spelled out in master.mif.

Standard C++98 also specifies many additional headers besides those
describing the C library. In particular:

<algorithm>        <istream>            <set>
<bitset>           <iterator>           <sstream>
<complex>          <limits>             <stack>
<deque>            <list>               <stdexcept>
<exception>        <locale>             <streambuf>
<fstream>          <map>                <string>
<functional>       <memory>             <typeinfo>
<iomanip>          <new>                <utility>
<ios>              <numeric>            <valarray>
<iosfwd>           <ostream>            <vector>
<iostream>         <queue>

For compatibility with older Open Watcom programs the header project
generates by a name.h form and a "cname form" (extensionless) for the
headers that support IOstreams and for certain other headers that
existed in Open Watcom v1.2 or before (new.h, exceptio.h, stdexcep.h,
and typeinfo.h). However, the newer C++ headers are generated only in
their extensionless form. Note, however, that the files complex.mh,
limits.mh, locale.mh, memory.mh, and string.mh are more complicatd. In
some cases (complex.mh, memory.mh) an unrelated .h header is generated
in addition to the C++ standard extensionless header. In other cases
(limits.mh, locale.mh, and string.mh) three files are generated: the C
library .h header, the C++ library "cname" header, and another C++
library header that is extensionless. This adds complications to the
control files.

Notice that several of the official header names in the C++ library are
longer than eight characters. To support the DOS host, Open Watcom
always stores these headers in 8.3 filename format, truncating the name
if necessary. Programmers may still refer to the long name in their
programs---and they are encouraged to do so. The Open Watcom compiler
will look for a truncated name if it can't open the long name.

In addition to the headers required by the C and C++ standards, Open
Watcom also provides several non-standard headers and headers that are
part of Open Watcom's library.

NOTES

1. Many of the headers define too many names if -za is in effect. A
   complete review of them should be done at some point. Probably what
   should happen is some new macros should be defined to distinguish
   between ISO C90 and ISO C99, both with and without extensions. Also
   at some point the _POSIX_C_SOURCE macro should probably be honored as
   well.

   Bart Oldeman wrote in the group openwatcom.contributors:

> My proposal:
> * The new switch -za99 defines __STDC_VERSION__ = 199901L and
>   NO_EXT_KEYS.
> * in stdio.h snprintf() is defined in a section
>   #if !defined(NO_EXT_KEYS) || __STDC_VERSION__ >= 199901L

2. Certain name.h and cname headers are currently identical (<cassert>,
   <cerrno>, <cfloat>, <ciso646>, <climits>). This works because those
   headers only contain macros in the standard. It might make sense,
   however, to create cname headers in the same style as the others just
   for the sake of consistency if nothing else.

3. In setjmp.mh there are pragma auxs being applied to both the _setjmp
   and longjmp functions. Right now I [PeterC] don't know how to make
   that work on functions inside a namespace. I don't think pragma aux
   currently (April 2004) supports namespaces. Thus for the time being I
   left csetjmp identical to setjmp.h even though that is not
   technically correct. I suspect the number of programs using csetjmp
   is minimal. Modern C++ programs are probably using exceptions and
   setjmp.h still works fine in C++ for old style code.

4. The file 'files.dat' in the top level directory was used by the original
   Watcom file packer/installer and contains a list of files to be
   distributed together with flags determining which files are needed when.
   This file is currently not used but might be in the future.

5. In my [PeterC] opinion, Open Watcom's header files are significantly
   more readible than average. This is a good thing; particularly for
   students of C and C++. Every reasonable effort should be made to
   maintain a high degree of readibility in the headers. It is one of
   OW's strengths.

6. If you add headers to the header project that need to be distributed
   as part of the official Open Watcom release, be sure to update the
   manifest file (files.dat) in bld/hdr. Don't forget to update both
   "DOSish" and other platforms if a change affects all targets.
