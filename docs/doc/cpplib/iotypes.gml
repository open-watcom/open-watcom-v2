.ix 'common types'
The set of classes that make up the C++ class library use several
common typedefs and macros.
They are declared in
:MONO.<iostream.h>
and
:MONO.<fstream.h>
:PERIOD.
:TYPL.
:TYP index='streampos'.typedef long streampos;
:TYP index='streamoff'.typedef long streamoff;
:TYP index='filedesc' .typedef int filedesc;
:TYP index='__NOT_EOF'.#define __NOT_EOF 0
:TYP index='EOF'      .#define EOF -1
:eTYPL.
The
:MONO.streampos
type represents an absolute position within the file. For &cmppname, the
file position can be represented by an integral type. For some file systems,
or at a lower level within the file system, the stream position might be
represented by an aggregate (structure) containing information such as
cylinder, track, sector and offset.
:P.
The
:MONO.streamoff
type represents a relative position within the file. The offset can always
be represented as a signed integer quantity since it is a number of
characters before or after an absolute position within the file.
:P.
The
:MONO.filedesc
type represents the type of a C library file handle.  It is used in places
where the I/O stream library takes a C library file handle as an argument.
:P.
The &noteof. macro is defined for cases where a function needs to return
something other than &eof. to indicate success.
:P.
The &eof. macro is defined to be identical to the value provided by the
:MONO.<stdio.h>
header file.
