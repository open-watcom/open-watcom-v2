.ix 'predefined objects'
Most programs interact in some manner with the keyboard and screen.
The C programming language provides three values,
:MONO.stdin
:CONT.,
:MONO.stdout
and
:MONO.stderr
:CONT., that are used for communicating with these "standard" devices,
which are opened before the user program starts execution at
:MONO.main()
:PERIOD.
These three values are
:MONO.FILE
pointers and can be used in virtually any
file operation supported by the C library.
:P.
In a similar manner, C++ provides seven objects for communicating
with the same "standard" devices.
C++ provides the three C
:MONO.FILE
pointers
:MONO.stdin
:CONT.,
:MONO.stdout
and
:MONO.stderr
:CONT.,
but they cannot be used with the extractors and inserters provided as part
of the C++ library.
:I1.cin
:I1.cout
:I1.cerr
:I1.clog
C++ provides four new objects, called &cin., &cout., &cerr. and &clog.,
which correspond to
:MONO.stdin
:CONT.,
:MONO.stdout
:CONT.,
:MONO.stderr
and buffered
:MONO.stderr
:PERIOD.
:SECTION.cin
&cin. is an
:MONO.istream
object which is connected to "standard input"
(usually the keyboard) prior to program execution.
Values extracted using the
:MONO.istream
:MONO.operator~b>>
class extractor operators are read from standard input and interpreted
according to the type of the object being extracted.
:P.
Extractions from standard input via &cin. skip whitespace
characters by default because the
:MONO.ios::skipws
bit is on.
The default behavior can be changed with the
:MONO.ios::setf
public member function or with the
:MONO.setiosflags
manipulator.
:SECTION.cout
&cout. is an
:MONO.ostream
object which is connected to "standard output"
(usually the screen) prior to program execution.
Values inserted using the
:MONO.ostream
:MONO.operator~b<<
class inserter operators are converted to characters and written to
standard output according to the type of the object being inserted.
:P.
Insertions to standard output via &cout. are buffered by default because the
:MONO.ios::unitbuf
bit is not on.
The default behavior can be changed with the
:MONO.ios::setf
public member function or with the
:MONO.setiosflags
manipulator.
:SECTION.cerr
&cerr. is an
:MONO.ostream
object which is connected to "standard error"
(the screen) prior to program execution.
Values inserted using the
:MONO.ostream
:MONO.operator~b<<
class inserter operators are
converted to characters and written to standard error according to
the type of the object being inserted.
:P.
Insertions to standard error via &cerr. are not buffered by default
because the
:MONO.ios::unitbuf
bit is on.
The default behavior can be changed with the
:MONO.ios::setf
public member function or with the
:MONO.setiosflags
manipulator.
:SECTION.clog
&clog. is an
:MONO.ostream
object which is connected to "standard error"
(the screen) prior to program execution.
Values inserted using the
:MONO.ostream
:MONO.operator~b<<
class inserter operators are
converted to characters and written to standard error according to
the type of the object being inserted.
:P.
Insertions to standard error via &clog. are buffered by default because
the
:MONO.ios::unitbuf
bit is not on.
The default behavior can be changed with the
:MONO.ios::setf
public member function or with the
:MONO.setiosflags
manipulator.
