.begnote $break
.*
.note address
.ix 'address'
An address is a location in a computer's memory.
Each storage location (byte) has
.* a unique
an address by which it is referenced. A
.us pointer
is an address.
.*
.note aggregate
.ix 'aggregate'
An aggregate type is either an
.us array
or a
.us structure.
The term
.us aggregate
refers to the fact that arrays and structures are made up of other
types.
.*
.note alignment
.ix 'alignment'
On some computers, objects such as integers, pointers and
floating-point numbers may be stored only at certain addresses (for
example, only at even addresses). An attempt to reference an object
that is not properly aligned may cause the program to fail. Other
computers may not require alignment, but may suggest it in order to
increase the speed of execution of programs.
.pp
C compilers align all objects that require it, including putting
padding characters within structures and arrays, if necessary.
However, it is still possible for a program to attempt to reference an
improperly-aligned object.
.*
.************************************************************************
.*
.shade begin
..if '&target' eq 'PC' ..th ..do begin
The &wcboth. compilers align structure members by default.
A command line switch, or the
.mono pack
pragma, may be used to control this behavior. Other objects may also
be aligned by default.
.pp
See the &userguide. for default values and other details.
..do end
..if '&target' eq 'PC 370' ..th ..do begin
By default, the &wcall. compilers do not align objects.
.pp
With &wcboth., a command line switch, or the
.mono pack
pragma, may be used to force alignment of objects.
.pp
With &wlooc., a command line switch may be used to force alignment.
.pp
See the &pragma. for details.
..do end
.shade end
.*
.************************************************************************
.*
.note argument
.ix 'argument'
.ix 'parameter'
An argument to a function call is an expression whose value is
assigned to the parameter for the function. The function may modify
the parameter, but the original argument is unaffected. This method of
passing values to a function is often called
.ix 'call by value'
.us call by value.
.pp
The argument may be a pointer to an object, in which case the function
may modify the object to which the pointer points, while the argument
value (the pointer) is unaffected.
.*
.note array
.ix 'array'
An array is a set of objects of the same type, grouped into adjacent
memory locations. References to elements of the array are made by
.ix 'subscript'
.us subscripts
or
.ix 'index'
.us indices.
.*
.note assignment
.ix 'assignment'
Assignment is the storing of a value into an object, which is usually
done with the
.ix 'operator' '='
.ix 'assignment operator'
.mono =
operator.
.*
.note automatic storage duration
.ix 'automatic storage duration'
An object with automatic storage duration is created when the
.us function
in which it is defined is invoked, and is destroyed when the function
returns to the caller.
.*
.note bit
.ix 'bit'
A bit is the smallest possible unit of information, representing one
of two values, 0 or 1.
If the bit is 0, it is said to be
.us off.
If the bit is 1, it is said to be
.us on.
.pp
A bit is not representable by an
.us address,
but is part of a
.us byte,
which does have an address.
.shade begin
Most processors,
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
including the Intel 80x86 family of processors,
..do end
..if '&target' eq 'PC 370' ..th ..do begin
including the Intel 80x86 family of processors and the IBM 370 family
of processors,
..do end
.*
.************************************************************************
.*
have 8 bits
in a byte.
.shade end
.*
.note bit-field
.ix 'bit-field'
A bit-field is a type that contains a specified number of bits.
.*
.note block
.ix 'block'
A block is a part of a function that begins with
.mono {
and ends with
.mono }
and contains declarations of objects and statements that perform some
action. A block is also called a
.ix 'compound statement'
.us compound statement.
.*
.note byte
.ix 'byte'
A byte is the smallest unit of storage representable by a unique
.us address,
usually capable of holding one character of information.
.shade begin
Most processors,
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
including the Intel 80x86 family of processors,
..do end
..if '&target' eq 'PC 370' ..th ..do begin
including the Intel 80x86 family of processors and the IBM 370 family
of processors,
..do end
.*
.************************************************************************
.*
have 8
.us bits
in a byte.
.shade end
.*
.note cast
.ix 'cast'
To cast an object is to explicitly convert it to another
.us type.
.*
.note character constant
.ix 'character constant'
.ix 'constant' 'character'
A character constant is usually one character (possibly a
.ix 'trigraphs'
.us trigraph
or
.ix 'escape sequences'
.us escape sequence)
contained within single-quotes (for example,
.mono 'a', '??('
and
.monoon
'\n'
.monooff
..ct ).
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
.shade begin
The &wcboth. compilers allow character constants with one, two, three
or four characters.
.shade end
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcall. compilers allow character constants with one, two, three
or four characters.
.shade end
..do end
.*
.************************************************************************
.*
.note comment
.ix 'comment'
A comment is a sequence of characters, outside of a
.ix 'string literal'
.ix 'string'
.us string literal
or
.ix 'character constant'
.ix 'constant' 'character'
.us character constant,
starting with
.mono /*
and ending with
.mono */.
The comment is only examined to find the
.mono */
that terminates it.
Hence, a comment may not contain another comment.
.*
.note compiler
.ix 'compiler'
A compiler is a program which reads a file containing programming
language statements and translates it into instructions that the
computer can understand.
.pp
For example, a C compiler translates statements described in this book.
.*
.note compound statement
.ix 'compound statement'
A compound statement is a part of a function that begins with
.mono {
and ends with
.mono }
and contains declarations of objects and statements that perform some
action. A compound statement is also called a
.ix 'block'
.us block.
.*
.note declaration
.ix 'declaration'
A declaration describes the attributes of an object or function, such
as the storage duration, linkage, and type.
The space for an object is reserved when its
.us definition
is found.
The declaration of a function describes the function arguments and
type and is also called a function prototype. The declaration of a
function does not include the statements to be executed when the
function is called.
.*
.note decrement
.ix 'decrement'
To decrement a number is to subtract (one) from it. To decrement a
pointer is to decrease its value by the size of the object to which
the pointer points.
.*
.note definition
.ix 'definition'
A definition of an object is the same as a
.us declaration,
except that the storage for the object is reserved when its definition
is found. A function definition includes the statements to be executed
when the function is called.
.*
.note exception
.ix 'exception'
An exception occurs when an operand to an operator has an invalid
value. Division by zero is a common exception.
.*
.note floating-point
.ix 'floating-point'
A floating-point number is a member of a subset of the mathematical
set of real numbers, containing (possibly) a fraction and an exponent.
The floating-point
.us type
is represented by one of the keywords
.kw float
..ct ,
.kw double
or
.kw long double
..ct ..li .
.*
.note function
.ix 'function'
A function is a collection of declarations and statements, preceded by
a declaration of the name of the function and the
.us parameters
to it, as well as a possible
.us return value.
The statements describe a series of steps to be taken after the
function is called, and before it finishes.
.*
.note header
.ix 'header'
A header contains C source, usually function prototypes, structure and
union definitions, linkages to externally-defined objects and macro
definitions. A header is included using the
.kwpp #include
preprocessor directive.
.* A header may or may not be a separate source
.* file.
.*
.keep begin
.note identifier
.ix 'identifier'
An identifier is a sequence of characters, starting with a letter or
underscore, and consisting of letters, digits and underscores. An
identifier is used as the name of an object, a tag, function, typedef,
label, macro or member of a structure or union.
.keep end
.*
.note implementation-defined behavior
.ix 'implementation-defined behavior'
Behavior that is implementation-defined depends on how a particular C
compiler handles a certain case. All C compilers must document their
behavior in these cases.
.*
.note incomplete type
.ix 'incomplete type'
An incomplete type is one which has been declared, but its size or
structure has not yet been stated. An example is an array of items
that was declared without specifying how many items. The
.kw void
type is also an incomplete type, but it can never be completed.
.*
.note increment
.ix 'increment'
To increment a number is to add (one) to it. To increment a pointer is
to increase its value by the size of the object to which the pointer
points.
.*
.note index
.ix 'index'
An index (or
.us subscript
..ct )
is a number used to reference an element of an
.us array.
It is an integral value. The first element of an array has the index
zero.
.*
.note indirection
.ix 'indirection'
Indirection occurs when an object that is a pointer to an object is
actually used to point to it. The unary form of the
.ix 'operator' 'indirection'
.ix 'operator' 'unary *'
.ix 'operator' '*'
.ix 'indirection operator'
.ix 'unary operator' '*'
.mono *
operator, or the
.ix 'operator' 'arrow'
.ix 'operator' '->'
.ix 'arrow operator'
.mono ->
operator are used for indirection.
.*
.note initialization
.ix 'initialization'
The initialization of an object is the act of giving it its first
(initial) value. This may be done by giving an initialization value
when the object is declared, or by explicitly assigning it a value.
.*
.note integer
.ix 'integer'
An integer is a
.us type
that is a subset of the mathematical set of integers. It is
represented by the keyword
.kw int
..ct ,
and has a number of variations including
.kw signed char
..ct ,
.kw unsigned char
..ct ,
.kw short signed int
..ct ,
.kw short unsigned int
..ct ,
.kw signed int
..ct ,
.kw unsigned int
..ct ,
.kw long signed int
..ct ,
.kw long unsigned int
..ct ,
.kw long long signed int
and
.kw long long unsigned int
..ct ..li .
.*
.note integral promotion
.ix 'integral promotion'
An object or constant that is a
.kw char
..ct ,
.kw short int
..ct ,
.kw int
bit-field, or of
.kw enum
type, that is used in an expression, is promoted to an
.kw int
(if
.kw int
is large enough to contain all possible values of the smaller
type) or
.kw unsigned int
..ct ..li .
.*
.note keyword
.ix 'keyword'
A keyword is an
.us identifier
that is reserved for use by the compiler. No object name or other use
of an identifier may use a keyword.
.*
.note label
.ix 'label'
A label is an
.us identifier
that corresponds to a particular
.us statement
in a
.us function.
It may be used by the
.ix 'statement' 'goto'
.ix 'goto statement'
.kw goto
statement.
.kw default
is a special label which is used with the
.kw switch
statement.
.*
.note library function
.ix 'library function'
A library function is a function provided with the C compiler that
performs some commonly needed action. The C language standard
describes a set of functions that all C compilers must provide.
Whether or not the function actually generates a function call is
implementation-defined.
.*
.note line
.ix 'line'
A line is conceptually similar to a line as seen in a text editor. The
line in a text editor may be called a physical line. Several physical
lines may be joined together into one logical line (or just "line") by
ending all but the last line with a
.monoon
\
.monooff
symbol. C does not normally require statements to fit onto one
line, so using the
.monoon
\
.monooff
symbol is usually only necessary when defining
.ix 'macro'
.us macros.
.*
.note linkage
.ix 'linkage'
An object with
.us external
linkage may be referenced by any
.us module
in the program. An object with
.us internal
linkage may be referenced only within the module in which it is
defined. An object with :ITAL.no:eITAL. linkage may only be referenced
within the
.us block
in which it is defined.
.*
.note lint
.ix 'lint'
lint is a utility program, often provided with the compiler, which
detects problems that the compiler will accept as syntactically valid,
but likely are not what the programmer intended.
.*
.note lvalue
.ix 'lvalue'
An lvalue is an expression that designates an object. The term
originally comes from the assignment expression,
.millust L = R
in which the left operand
.mono L
to the assignment operator must be a modifiable value. The most common
form of lvalue is the identifier of an object.
.pp
If an expression
.mono E
evaluates to a pointer to an object, then
.mono *E
is an lvalue that designates the object to which
.mono E
points.
In particular, if
.mono E
is declared as a "pointer to
.kw int
..ct ",
then both
.mono E
and
.mono *E
are lvalues having the respective types "pointer to
.kw int
..ct "
and
.kw int
..ct ..li .
.*
.note macro
.ix 'macro'
There are two kinds of macros. An object-like macro is an
.us identifier
that is replaced by a sequence of
.us tokens.
A function-like macro is an apparent function call which is replaced
by a sequence of tokens.
.*
.keep begin
.note module
.ix 'module'
Referred to in the C language standard as a
.us translation unit,
a module is usually a file containing C source code. A module may
include headers or other source files, and have conditional
compilation (preprocessing directives), object declarations, and/or
functions. A module is thus considered to be a C source file after the
included files and conditional compilation have been processed.
.* or the resulting
.* declarations of objects and functions.
.keep end
.*
.note name space
.ix 'name space'
A name space is a category of identifiers. The same identifier may
appear in different name spaces. For example, the identifier
.mono thing
may be a label, object name, tag and member of a structure or union,
all at the same time, since each of these has its own name space. The
syntax of the use of the identifier resolves which category the
identifier falls into.
.*
.note nesting
.ix 'nesting'
Nesting is placing something inside something else. For example, a
.kw for
statement may, as part of its body, contain another
.kw for
statement. The second
.kw for
is said to be nested inside the first. Another form of nesting occurs
when source files include other files.
.*
.note null pointer constant
.ix 'null pointer'
.ix 'pointer' 'null'
The value zero, when used in a place where a pointer type is expected,
is considered to be a null pointer constant, which is a value that
indicates that the pointer does not currently point to anything. The
compiler interprets the zero as a special value, and does not
guarantee that the actual value of the pointer will be zero.
.pp
The macro
.mkw NULL
is often used to represent the null pointer constant.
.*
.note null character
.ix 'null character'
The character with all
.us bits
set to zero is used to terminate
.us strings,
and is called the null character. It is represented by the
.us escape sequence
.mono \0
in a string, or as the character constant
.mono '\0'
..ct ..li .
.*
.note object
.ix 'object'
An object is a collection of
.us bytes
in the storage of the computer, used to represent values. The size and
meaning of the object is determined by its
.us type.
A
.us scalar
object
is often referred to as a
.us variable.
.*
.note parameter
.ix 'parameter'
.ix 'argument'
A parameter to a function is a "local copy" of the argument values
determined in the call to the function. Any modification of a
parameter value does not affect the argument to the function call.
However, an argument (and hence a parameter) may be a pointer to an
object, in which case the function may modify the object to which its
parameter points.
.*
.note pointer
.ix 'pointer'
An object that contains the
.us address
of another object is said to be a pointer to that object.
.*
.note portable
.ix 'portable'
Portable software is written in such a way that it is relatively easy
to make the software run on different hardware or operating systems.
.*
.note precedence
.ix 'precedence'
Precedence is the set of implicit rules for determining the order of
execution of an expression in the absence of parentheses.
.*
.keep begin
.note preprocessor
.ix 'preprocessor'
The preprocessor:
.begbull $compact
.bull
examines
.us tokens
for
.us macros
and does appropriate substitutions if necessary,
.bull
includes headers or other source files, and,
.bull
includes or excludes input lines based on
.kwpp #if
directives
.endbull
.pc
before the compiler translates the source.
.keep end
.*
.note recursion
.ix 'recursion'
Recursion occurs when a
.us function
calls itself either directly, or by calling another function which
calls it. See recursion. (!)
.*
.note register
.ix 'register'
A register is a special part of the computer, usually not part of the
addressable storage. Registers may contain values and are generally
faster to use than storage.
.pp
The
.us keyword
.kw register
may be used when declaring an object with
.us automatic storage duration,
indicating to the compiler that this object will be heavily used, and
the compiler should attempt to optimize the use of this object,
possibly by placing it in a machine register.
.*
.note return value
.ix 'return value'
A return value is the value returned by a
.us function
via the
.kw return
statement.
.*
.note rounding
.ix 'rounding'
A value is rounded when the representation used to store a value is
not exact. The value may be increased or decreased to the nearest
value that may be accurately represented.
.*
.note scalar
.ix 'scalar'
A scalar is an object that is not a structure, union or array.
Basically, it is a single item, with type such as character, any of
the various integer types, or floating-point.
.*
.note scope
.ix 'scope'
The scope of an
.us identifier
identifies the part of the
.us module
that may refererence it. An object with
.us block
scope may only be referenced within the block in which it is defined.
An object with
.us file
scope may be referred to anywhere within the file in which it is
defined.
.*
.note sequence point
.ix 'sequence point'
A sequence point is a point at which all
.us side-effects
from previously executed statements will have been resolved, and no
side-effects from statements not yet executed will have occurred.
Normally, the programmer will not need to worry about sequence points,
as it is the compiler's job to ensure that side-effects are resolved
at the proper time.
.*
.note side-effect
.ix 'side-effect'
A side-effect modifies a value of an object, causing a change in the
state of the program. The most common side-effect is
.us assignment,
whereby the value of the left operand is changed.
.*
.note signed
.ix 'signed'
A signed value can represent both negative and positive values.
.pp
The
.us keyword
.kw signed
may be used with the types
.kw char
..ct ,
.kw short int
..ct ,
.kw int
..ct ,
.kw long int
and
.kw long long int
..ct ..li .
.*
.note statement
.ix 'statement'
A statement describes the actions that are to be taken by the program.
(Statements are distinct from the declarations of objects.)
.*
.note static storage duration
.ix 'static storage duration'
An object with static storage duration is created when the program is
invoked, and destroyed when the program exits. Any value stored in the
object will remain until explicitly modified.
.*
.note string
.ix 'string'
A string is a sequence of characters terminated by a
.us null character.
A reference to a string is made with the
.us address
of the first character.
.*
.note string literal
.ix 'string literal'
A string literal is a sequence of zero or more characters enclosed
within double-quotes and is a constant. Adjacent string literals are
concatenated into one string literal. The value of a string literal is
the sequence of characters within the quotes, plus a
.us null character
(
..ct .mono \0
..ct )
placed at the end.
.*
.note structure
.ix 'structure'
.ix 'type' 'structure'
A structure is a
.us type
which is a set of named members of (possibly different) types, which
reside in memory starting at adjacent and sequentially increasing
storage locations.
.*
.note subscript
.ix 'subscript'
A subscript (or
.us index
..ct )
is a number used to reference an element of an
.us array.
It is a non-negative integral value. The first element of an array has
the subscript zero.
.*
.keep begin
.note tag
.ix 'tag'
A tag is an identifier which names a structure, union or enumeration.
In the declaration,
.millust begin
enum nums { ZERO, ONE, TWO } value;
.millust end
.pc
.mono nums
is the tag of the enumeration, while
.mono value
is an object declared with the enumeration type.
.keep end
.*
.note token
.ix 'token'
A token is the unit used by the
.us preprocessor
for scanning for
.us macros,
and by the
.us compiler
for scanning the input source lines. Each identifier, constant and
comment is one token, while other characters are each, individually,
one token.
.*
.note type
.ix 'type'
The type of an
.us object
describes the size of the object, and what interpretation is to be
used when using the value of the object. It may include information
such as whether the value is
.kw signed
or
.kw unsigned
..ct ,
and what range of values it may contain.
.*
.note undefined behavior
.ix 'undefined behavior'
Undefined behavior occurs when an erroneous program construct or bad
data is used, and the standard does not impose a behavior. Possible
actions of undefined behavior include ignoring the problem, behaving
in a documented manner, terminating the compilation with an error, and
terminating the execution with an error.
.*
.note union
.ix 'union'
.ix 'type' 'union'
A union is a
.us type
which is a set of named members of (possibly different) types, which
reside in memory starting at the same memory location.
.*
.note unsigned
.ix 'unsigned'
An unsigned value is one that can represent only non-negative values.
..pp
The
.us keyword
.kw unsigned
may be used with the types
.kw char
..ct ,
.kw short int
..ct ,
.kw int
..ct ,
.kw long int
and
.kw long long int
..ct ..li .
.*
.note variable
.ix 'variable'
A variable is generally the same thing as an
.us object.
.ix 'object'
It is most often used to refer to
.us scalar
objects.
.*
.note void
.ix 'void'
.ix 'type' 'void'
The
.kw void
.us type
is a special type that really indicates "no particular type". An
object that is a "pointer to
.kw void
..ct " may not be used to point at
anything without it first being
.us cast
to the appropriate type.
.pp
The
.us keyword
.kw void
is also used as the type of a
.us function
that has no
.us return value,
and as the
.us parameter
list of a function that requires no parameters.
.*
.keep end
.endnote
