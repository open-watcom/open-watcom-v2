.np
The
.kwm far16
attribute should only be used on systems that permit the calling of
16-bit code from 32-bit code.
Currently, the only supported operating system that allows this is
32-bit OS/2.
If you have any libraries of &functions or APIs that are only
available as 16-bit code and you wish to access these &functions and
APIs from 32-bit code, you must specify the
.kwm far16
attribute.
If the
.kwm far16
attribute is specified,
the compiler will generate special code which allows the 16-bit code
to be called from 32-bit code.
Note that a
.kwm far16
function must be a function whose attributes are those specified by
one of the alias names
.kwm __cdecl
or
.kwm __pascal
.ct .li .
These alias names will be described in a later section.
