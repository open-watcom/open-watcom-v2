The following header files are supplied with the Open Watcom C++
library. When a class or function from the library is used in a source
file the related header file should be included in that source file. The
header files can be included multiple times and in any order with no ill
effect.
:P.
The facilities of the C standard library can be used in C++ programs by
including the appropriate "cname" header. In that case all of the C
standard library functions are in namespace
:MONO.std.
For example, to use function
:MONO.std::~printf
one should include the header cstdio. Note that the cname headers
declare in the global namespace any non-standard names they contain as
extensions. It is also possible to include in a C++ program the same
headers used by C programs. In that case, the standard functions are in
both the global namespace as well as in namespace
:MONO.std.
:P.
Some of C++ standard library headers described below come in a form with
a .h extension and in a form without an extension. The extensionless
headers declare their library classes and functions in namespace
:MONO.std.
The headers with a .h extension declare their library classes and
functions in both the global namespace and in namespace
:MONO.std.
Such headers are provided as a convenience and for compatibility with
legacy code. Programs that intend to conform to Standard C++ should use
the extensionless headers to access the facilities of the C++ standard
library.
:P.
Certain headers defined by Standard C++ have names that are
longer than the 8.3 limit imposed by the FAT16 filesystem. Such headers
are provided with names that are truncated to eight characters so they
can be used with the DOS host. However, one can still refer to them in
#include directives using their full names as defined by the standard.
If the Open Watcom C++ compiler is unable to open a header with the long
name, it will truncate the name and try again.
:P.
The Open Watcom C++ library contains some components that were developed
before C++ was standardized. These legacy components continue to be
supported and are described in this documentation.
:P.
The header files are all located in the
:MONO.\WATCOM\H
directory.
:DL termhi=2.
:DT.algorithm (algorith)
:DD.
.ix 'header files' 'algorithm'
This header file defines the standard algorithm templates.
:DT.complex
:DD.
.ix 'header files' 'complex'
This header file defines the
:MONO.std::~complex
class template and related function templates. This template can be
instantiated for the three different floating point types. It can be
used to represent complex numbers and to perform complex arithmetic.
:DT.complex.h
:DD.
.ix 'header files' 'complex'
This header file defines the legacy
:MONO.Complex
class. This class is used to represent complex numbers and to perform
complex arithmetic. The class defined in this header is not the Standard
C++
:MONO.std::~complex
class template.
:DT.exception/exception.h (exceptio/exceptio.h)
:DD.
.ix 'header files' 'exception'
.ix 'exception handling'
This header file defines components to be used with
the exception handling mechanism. It defines the base class of the
standard exception hierarchy.
:DT.functional (function)
:DD.
.ix 'header files' 'functional'
This header file defines the standard functional templates. This
includes the functors and binders described by Standard C++.
:DT.fstream/fstream.h
:DD.
.ix 'header files' 'fstream'
This header file defines the
:MONO.filebuf
:CONT.,
:MONO.fstreambase
:CONT.,
:MONO.ifstream
:CONT.,
:MONO.ofstream
:CONT., and
:MONO.fstream
classes.
These classes are used to perform C++ file input and output operations.
The various class members are declared and inline member functions for
the classes are defined.
:DT.generic.h
:DD.
.ix 'header files' 'generic'
This header file is part of the macro support required to implement
generic containers prior to the introduction of templates in the C++
language. It is retained for backwards compatibility.
:DT.iomanip/iomanip.h
:DD.
.ix 'header files' 'iomanip'
This header file defines the parameterized manipulators.
:DT.ios/ios.h
:DD.
.ix 'header files' 'ios'
This header file defines the class
:MONO.ios
that is used as a base of the other iostream classes.
:DT.iosfwd/iosfwd.h
:DD.
.ix 'header files' 'iosfwd'
This header file provides forward declarations of the iostream classes.
It should be used in cases where the full class definitions are not
needed but where one still wants to declare pointers or references to
iostream related objects. Typically this occurs in a header for another
class that wants to provide overloaded inserter or extractor operators.
By including
:MONO.iosfwd
instead of
:MONO.iostream
(for example), compilation speed can be improved because less material
must be processed by the compiler.
:P.
Note that including
:MONO.iosfwd
is the only appropriate way to forward declare the iostream classes.
Manually writing forward declarations is not recommended.
:DT.iostream/iostream.h
:DD.
.ix 'header files' 'iostream'
This header file (indirectly) defines the
:MONO.ios
:CONT.,
:MONO.istream
:CONT.,
:MONO.ostream
:CONT., and
:MONO.iostream
classes.
These classes form the basis of the C++ formatted input and output support.
The various class members are declared and
inline member functions for the classes are defined.
The &cin., &cout., &cerr., and &clog. predefined objects are declared
along with the non-parameterized manipulators.
:DT.istream/istream.h
:DD.
.ix 'header files' 'istream'
This header file defines class
:MONO.istream
and class
:MONO.iostream.
It also defines their associated parameterless manipulators.
:DT.iterator
:DD.
.ix 'header files' 'iterator'
This header file defines several templates to facilitate the handling of
iterators. In particular, it defines the
:MONO.std::~iterator_traits
template as well as several other supporting iterator related templates.
:DT.limits
:DD.
.ix 'header files' 'limits'
This header file defines the
:MONO.std::~numeric_limits
template and provides specializations of that template for each of the
built-in types.
:P.
Note that this header is not directly related to the header limits.h
from the C standard library (or to the C++ form of that header,
climits).
:DT.list
:DD.
.ix 'header files' 'list'
This header file defines the
:MONO.std::list
class template. It provides a way to make a sequence of objects with
efficient insert and erase operations.
:DT.map
:DD.
.ix 'header files' 'map'
This header file defines the
:MONO.std::~map
and
:MONO.std::multimap
class templates. They provide ways to associate keys to values.
:DT.memory
:DD.
.ix 'header files' 'memory'
This header file defines the default allocator template,
:MONO.std::~allocator,
as well as several function templates for manipulating raw
(uninitialized) memory regions. In addition this header defines the
:MONO.std::~auto_ptr
template.
:P.
Note that the header
:MONO.memory.h
is part of the Open Watcom C library and is unrelated to
:MONO.memory.
:DT.new/new.h
:DD.
.ix 'header files' 'new'
This header file provides declarations to be used with the
intrinsic
:MONO.operator~bnew
and
:MONO.operator~bdelete
memory management functions.
:DT.numeric
:DD.
.ix 'header files' 'numeric'
This header file defines several standard algorithm templates pertaining
to numerical computation.
:DT.ostream/ostream.h
:DD.
.ix 'header files' 'ostream'
This header file defines class
:MONO.ostream.
It also defines its associated parameterless manipulators.
:DT.set
:DD.
.ix 'header files' 'set'
This header file defines the
:MONO.std::~set
and
:MONO.std::multiset
class templates. They provide ways to make ordered collections of
objects with efficient insert, erase, and find operations.
:DT.stdiobuf.h
:DD.
.ix 'header files' 'stdiobuf'
This header file defines the
:MONO.stdiobuf
class which provides the support for the C++ input and output operations to
standard input, standard output, and standard error streams.
:DT.streambuf/streambuf.h (streambu/streambu.h)
:DD.
.ix 'header files' 'streambuf'
This header file defines the
:MONO.streambuf
class which provides the support for buffering of input and output
operations. This header file is automatically included by the
:MONO.iostream.h
header file.
:DT.string
:DD.
.ix 'header files' 'string'
This header file defines the
:MONO.std::basic_string
class template. It also contains the type definitions for
:MONO.std::~string
and
:MONO.std::~wstring.
In addition, this header contains specializations of the
:MONO.std::~char_traits
template for both characters and wide characters.
:DT.string.hpp
:DD.
.ix 'header files' 'string'
This header file defines the legacy
:MONO.String
class. The
:MONO.String
class is used to manipulate character strings. Note that the
:MONO.hpp
extension is used to avoid colliding with the Standard C
:MONO.string.h
header file. The class defined in this header is not the Standard C++
:MONO.std::~string
class.
:DT.strstream.h (strstrea.h)
:DD.
.ix 'header files' 'strstream'
This header files defines the
:MONO.strstreambuf
:CONT.,
:MONO.strstreambase
:CONT.,
:MONO.istrstream
:CONT.,
:MONO.ostrstream
:CONT., and
:MONO.strstream
classes. These classes are used to perform C++ in-memory formatting. The
various class members are declared and inline member functions for the
classes are defined.
:DT.vector
:DD.
.ix 'header files' 'vector'
This header contains the
:MONO.std::~vector
class template.
:DT.wcdefs.h
:DD.
.ix 'header files' 'wcdefs'
This header file contains definitions used by the Open Watcom legacy
container libraries. If a container class needs any of these
definitions, the file is automatically included.
:P.
Note that all headers having names that start with "wc" are related to
the legacy container libraries.
:DT.wclbase.h
:DD.
.ix 'header files' 'wclbase'
This header file defines the base classes which are used
by the list containers.
:DT.wclcom.h
:DD.
.ix 'header files' 'wclcom'
This header file defines the classes which are common
to the list containers.
:DT.wclibase.h
:DD.
.ix 'header files' 'wclibase'
This header file defines the base classes which are used
by the list iterators.
:DT.wclist.h
:DD.
.ix 'header files' 'wclist'
This header file defines the
:MONO.list
.ix 'list containers'
container classes.
The available list container classes are single and double linked versions
of intrusive, value and pointer lists.
:DT.wclistit.h
:DD.
.ix 'header files' 'wclistit'
This header file defines the
:MONO.iterator
.ix 'iterator classes'
classes that correspond to the list containers.
:DT.wcqueue.h
:DD.
.ix 'header files' 'wcqueue'
This header file defines the
:MONO.queue
class.
Entries in a queue class are accessed first in, first out.
:DT.wcstack.h
:DD.
.ix 'header files' 'wcstack'
This header file defines the
:MONO.stack
class.
Entries in a stack class are accessed last in, first out.
:eDL.
