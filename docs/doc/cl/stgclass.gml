.ix 'storage class'
.pp
The
.us storage class
of an object describes:
.begbull
.bull
the duration of the existence of the object. An object may exist
throughout the execution of the program, or only during the span of time
that the function in which it is defined is executing.
In the latter case, each time the function is called, a new instance of
the object is created, and that object is destroyed when the function
returns.
.bull
the
.us scope
.ix 'scope'
of the object. An object may be declared so that it is only
accessible within the function in which it is defined,
within the module
or throughout the entire program.
.endbull
.pp
A
.us storage class specifier
is one of:
.illust begin
.kw auto
.kw register
.kw extern
.kw static
.kw typedef
.illust end
.pc
.kw typedef
is included in the list of storage class specifiers for convenience,
because the syntax of a type definition is the same as for an object
declaration.
A
.kw typedef
declaration does not create an object, only a synonym for a
type, which does not
have a storage class associated with it.
.pp
Only one of these keywords (excluding
.kw typedef
..ct ) may be specified in a declaration of
an object.
.pp
If an object or function is declared with a storage class, but no
type specifier, then the type of the object or function is assumed to be
.kw int
..ct ..li .
.pp
While a storage class specifier may be placed
.ix 'storage class' 'following a type specifier'
following a type
specifier, this tends to be difficult to read.
It is recommended that the storage class (if present)
always be placed first in the declaration.
The ISO C standard states that the ability to place the
storage class specifier other than at the beginning of the declaration
is an obsolescent feature.
