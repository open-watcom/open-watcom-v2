.ix 'void'
.ix 'type' 'void'
.pp
The
.kw void
type has several purposes:
.autonote
.note
To declare an object as being a pointer to no particular type.
For example,
.millust begin
void * membegin;
.millust end
.pc
defines
.mono membegin
as being a pointer. It does not point to anything without a
.us cast
.ix 'cast operator'
.ix 'operator' 'cast'
operator.
The statement,
.millust begin
*(char *) membegin = '\0';
.millust end
.pc
will place a zero in the character at which
.mono membegin
points.
.note
To declare a function as not returning a value.
For example,
.millust begin
void rewind( FILE * stream );
.millust end
.pc
declares the standard library function
.libfn rewind
which takes one parameter and returns nothing.
.note
To evaluate an expression for its side-effects, discarding the
result of the expression.
For example,
.millust begin
(void) getchar();
.millust end
.pc
calls the library function
.libfn getchar
..ct , which normally returns a character.
In this case, the character is discarded, effectively advancing
one character in the file without caring what character is read.
This use of
.kw void
is primarily for readability, because casting the expression to the
void type will be done automatically.
The above example could also be written as,
.millust begin
getchar();
.millust end
.endnote
.pc
The keyword
.kw void
is also used in one other instance. If a function
takes no parameters,
.kw void
may be used in the declaration.
For example,
.millust begin
int getchar( void );
.millust end
.pc
declares the standard library function
.libfn getchar
..ct ,
which takes no parameters and returns an integer.
.pp
No object (other than a function) may be declared with type
.kw void
..ct ..li .
