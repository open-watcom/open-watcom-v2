.ix 'pointer'
.ix 'type' 'pointer'
.pp
A
.us pointer
to an object is equivalent to the address of the
object in the memory of the computer.
.pp
An object may be declared to be a pointer to a type of object, or it may
be declared to be a pointer to no particular type. The form,
.cillust begin
type
.mono *
identifier:MSEMI.
.cillust end
.pc
declares the identifier to be a pointer to the given type. If
.us type
is
.kw void
..ct ,
.ix 'void'
.ix 'pointer' 'to void'
then the identifier is a pointer to no particular type of object
(a generic pointer).
.keep begin
.pp
The following examples illustrate various pointer declarations:
.millust begin
int * intptr;
.millust end
.discuss begin
.mono intptr
is a pointer to an
.kw int
..ct ..li .
.discuss end
.keep break
.millust begin
char * charptr;
.millust end
.discuss begin
.mono charptr
is a pointer to a
.mono char
..ct ..li .
.discuss end
.keep break
.millust begin
struct tokendef * token;
.millust end
.discuss begin
.mono token
is a pointer to the structure
.mono tokendef.
.discuss end
.keep break
.millust begin
char * argv[];
.millust end
.discuss begin
.mono argv
is an array of pointers to
.kw char
or an array of pointers to strings.
.discuss end
.keep break
.millust begin
char ** strptr;
.millust end
.discuss begin
.mono strptr
is a pointer to a pointer to
.kw char
..ct ..li .
.discuss end
.keep break
.millust begin
void * dumpbeg;
.millust end
.discuss begin
.mono dumpbeg
is a pointer, but to no particular type of object.
.discuss end
.keep end
.pp
Any place that a pointer may be used, the constant 0 may also be
used. This value is the
.ix 'null pointer'
.ix 'pointer' 'null'
.us null pointer constant.
The value that is used internally to represent a null pointer is
guaranteed not to be a pointer to an object.
It does not necessarily correspond to the integer value 0. It merely
represents a pointer that
does not currently point at anything.
The macro
.ix 'NULL' 'macro'
.mkw NULL
..ct ,
defined in the header
.hdr <stddef.h>
..ct ,
may also be used in place of 0.
