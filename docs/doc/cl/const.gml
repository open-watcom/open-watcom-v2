.ix 'const'
.ix 'type' 'const'
.pp
An object may be declared with the keyword
.kw const
..ct ..li .
Such an object may not be modified directly by the program.
For objects with static storage duration,
this type qualifier describes to the compiler
which objects may be placed in read-only memory, if the computer
supports such a concept.
It also provides the opportunity for the
compiler to detect attempts to modify the object.
The compiler may also generate better code when it knows that
an object will not be modified.
.pp
Even though an object is declared to be constant, it is possible to
modify its value indirectly by storing its address
(using a cast)
in another object
declared to be a pointer to the same type (without the
.mono const
..ct ),
and then using the second
object to modify the value to which it points.
However, this should be done with caution,
and may fail on computers with protected
memory.
.pp
If the declaration of an object does not include
.mono *
..ct ,
that is to say it is not a pointer of any kind, then the keyword
.kw const
appearing anywhere in the type specifier (including any
.kw typedef
..ct 's)
indicates that the object is constant and may not be changed.
If the object is a pointer and
.kw const
appears to the left of the
.mono *
..ct ,
the object is a pointer to a constant value, meaning that
the value to which the pointer points may not be modified,
although the pointer value may be changed.
If
.kw const
appears to the right of the
.mono *
..ct ,
the object is a constant pointer to a value, meaning that the pointer
to the value may not be changed,
although what the pointer points to may be changed.
If
.kw const
appears on both sides of the
.mono *
..ct ,
the object is a constant pointer to a constant value, meaning that the
pointer and the object to which it points
may not be changed.
.pp
If the declaration of a structure, union or array includes
.kw const
..ct ,
then each member of the type, when referred to, is treated as if
.kw const
had been specified.
.*.pp
.*If the type of an object is declared only with the keyword
.*.kw const
.*..ct , then
.*.kw int
.*is implied.
.keep begin
.pp
The declarations,
.millust begin
const int   baseyear = 1900;
const int * byptr;
.millust end
.keep end
.pc
declare the object
.mono baseyear
to be an integer whose value is constant and set to 1900, and
the object
.mono byptr
to be a pointer to a constant object of integer type. If
.mono byptr
was made to point to another integer that was not, in fact,
declared to be constant, then
.mono byptr
could not be used to modify that value.
.mono byptr
may be used to get a value from an integer object, and never
to change it.
Another way of stating it is that what
.mono byptr
points to is constant, but
.mono byptr
itself is not constant.
.keep begin
.pp
The declarations,
.millust begin
int         baseyear;
int * const byptr = &baseyear;
.millust end
.keep end
.pc
declare the object
.mono byptr
as a constant pointer to an integer, in this case
the object
.mono baseyear.
The value of
.mono baseyear
may be modified
via
.mono byptr
..ct ,
but the value of
.mono byptr
itself may not be changed.
In this case,
.mono byptr
itself is constant, but what
.mono byptr
points to is not constant.
