.pp
When a name
is used in a program, the compiler needs to know what
that name represents.
A
.ix 'declaration' 'of object'
.ix 'declaration' 'of function'
.ix 'object' 'declaration'
.ix 'function' 'declaration'
.us declaration
describes to the compiler what a name is, including:
.begbull
.bull
How much storage it occupies (objects) or how much storage
is required for the value that is returned (functions), and
how the value in that storage is to be interpreted.
This is called the
.us type.
Examples include
.mono int
..ct ,
.mono float
and
.mono struct list
..ct ..li .
.bull
Whether the name is visible only within the module being compiled, or
throughout the program.
This is called the
.us linkage,
and is part of the
.us storage class.
The keywords
.kw extern
and
.kw static
determine the linkage.
.bull
For object names, whether
the object is created every time the function is called and destroyed
every time the function returns.
This is called the
.us storage duration,
and is part of the
.us storage class.
The keywords
.kw extern
..ct ,
.kw static
..ct ,
.kw auto
and
.kw register
determine the storage duration.
.endbull
.pp
The placement of the declaration within the program determines
whether the declaration applies to all functions within the module,
or just to the function within which the declaration appears.
.pp
The
.us definition
.ix 'definition'
of an object is similar to its declaration, except that the storage for
the object is reserved.
Whether the declaration of an object is also a definition depends upon
the placement of the declaration and the attributes of the object.
.keep begin
.pp
.ix 'specifier' 'storage class'
.ix 'type' 'specifier'
The usual form for defining (creating)
an object is as follows:
.cillust begin
storage-class-specifier type-specifier declarator:MSEMI.
.cor
storage-class-specifier type-specifier declarator :MONO.=:eMONO. initializer:MSEMI.
.cillust end
.keep end
.pc
The
.us storage-class-specifier
is optional, and is thoroughly
discussed in the chapter "Storage Classes".
The
.us type-specifier
is also optional, and is thoroughly discussed in the next section and in
the chapter "Advanced Types".
At least one of the
.us storage-class-specifier
and
.us type-specifier
must be specified, and they
may be specified in either order, although it is recommended
that the
.us storage-class-specifier
always be placed first.
.pp
The
.us declarator
is the name of the object being defined along with other information
about its type.
There may be several declarators, separated by commas.
.pp
The
.us initializer
is discussed in the chapter "Initialization of Objects".
.pp
The following are examples of declarations of objects, along
with a brief description of what each one means.
A more complete discussion of the terms used may be found in
the relevant section.
.*
.dm inside begin
.  ..if '&*' eq 'begin' ..th ..do begin
.  .  .keep begin
.  .  ..if &e'&dohelp eq 0 ..do begin
.  .  .  ..sr *value=&INDillust. + 2
.  .  .  ..in +&*value.
.  .  ..do end
.  .  :ZDL termhi=0 tsize='0.25i' break.
.  .  :ZDT.Inside a function
.  .  :ZDD.
.  ..do end
. ..el ..if '&*' eq 'end' ..th ..do begin
.  .  :ZeDL.
.  .  ..if &e'&dohelp eq 0 ..do begin
.  .  .  ..sr *value=&INDillust. + 2
.  .  .  ..in -&*value.
.  .  ..do end
.  .  .keep end
.  ..do end
.dm inside end
.*
.dm outside begin
.  ..if '&*' eq 'begin' ..th ..do begin
.  .  .keep begin
.  .  ..if &e'&dohelp eq 0 ..do begin
.  .  .  ..sr *value=&INDillust. + 2
.  .  .  ..in +&*value.
.  .  ..do end
.  .  :ZDL termhi=0 tsize='0.25i' break.
.  .  :ZDT.Outside a function
.  .  :ZDD.
.  ..do end
. ..el ..if '&*' eq 'end' ..th ..do begin
.  .  :ZeDL.
.  .  ..if &e'&dohelp eq 0 ..do begin
.  .  .  ..sr *value=&INDillust. + 2
.  .  .  ..in -&*value.
.  .  ..do end
.  .  .keep end
.  ..do end
.dm outside end
.*
.keep begin
.millust begin
int x;
.millust end
.inside begin
The object
.mono x
is declared to be an
.us integer,
with
.us automatic storage duration
..ct ..li .
Its
value is available only within the function (or
.ix 'statement' 'compound'
.ix 'compound statement'
compound statement)
in which it is defined.
This is also a definition.
.inside end
.keep end
.outside begin
The object
.mono x
is created and declared to be an
.us integer
with
.us static storage duration
..ct ..li .
Its value is available within the
.us module
in which it is defined, and has
.us external linkage
so that any other module may refer to it by using the declaration,
.millust begin
extern int x;
.millust end
.pc
This is also a definition.
.outside end
.keep begin
.millust begin
register void * memptr;
.millust end
.inside begin
The object
.mono memptr
is declared to be a
.us pointer to
.kw void
(no particular type of object),
and is used frequently in the function.
This is also a definition.
.inside end
.keep end
.outside begin
Not valid because of the
.kw register
storage class.
.outside end
.keep begin
.millust begin
auto long int x, y;
.millust end
.inside begin
The objects
.mono x
and
.mono y
are declared to be
.us signed long integers
with
.us automatic storage duration
..ct ..li .
This is also a definition.
.inside end
.keep end
.outside begin
Not valid because of the
.kw auto
storage class.
.outside end
.keep begin
.millust begin
static int nums[10];
.millust end
.inside begin
The object
.mono nums
is declared to be an
.us array of 10 integers
with
.us static storage duration
..ct ..li .
Its value is only available within the function, and will be preserved
between calls to the function.
This is also a definition.
.inside end
.keep end
.outside begin
The object
.mono nums
is declared to be an
.us array of 10 integers
with
.us static storage duration
..ct ..li .
Its value is only available within the
.us module
..ct ..li .
(The difference is the
.ix 'scope'
.us scope
of the object
.mono nums
..ct ..li .)
This is also a definition.
.outside end
.keep begin
.millust begin
extern int x;
.millust end
.inside begin
The object
.mono x
is declared to be an
.us integer
with
.us static storage duration
..ct ..li .
No other functions within the current module may refer to
.mono x
unless they also declare it.
The object is defined in another module, or elsewhere in this
function or module.
.inside end
.keep end
.outside begin
The object
.mono x
is declared to be an
.us integer
with
.us static storage duration
..ct ..li .
Its value is available to all functions within the module.
The object is defined in another module, or elsewhere in this module.
.outside end
.pp
The appendix "&exdecl." contains many more examples
of declarations of objects and functions.
.*
.dm inside delete
.dm outside delete
.*
.section Name Scope
.*
.ix 'scope'
.ix 'name' 'scope'
.pp
An identifier may be referenced
only within its
.us scope
..ct ..li .
.ix 'scope'
.pp
An identifier declared within a function or within a
.ix 'compound statement'
.ix 'statement' 'compound'
compound statement
within
a function has
.us block
.ix 'block scope'
.ix 'scope' 'block'
scope, and may be referenced only in the block in which it is declared.
The object's scope includes any enclosed blocks and
terminates at the } which terminates the enclosing block.
.pp
An identifier declared within a function prototype (as a parameter to
that function) has
.us function prototype
.ix 'function prototype scope'
.ix 'scope' 'function prototype'
scope, and may not be referenced elsewhere.
Its scope terminates at the ) which terminates the prototype.
.pp
An identifier declared outside of any function or function prototype
has
.us file
.ix 'file scope'
.ix 'scope' 'file'
scope, and may be referenced anywhere within the module in which it
is declared.
If a function contains a declaration for the same identifier,
the identifier with file scope is hidden within the function.
Following the terminating } of the function, the identifier with
file scope becomes visible again.
.pp
A label, which must appear within a function, has
.us function
.ix 'scope' 'function'
.ix 'function' 'scope'
scope.
.*
.section Type Specifiers
.*
.pp
Every object has a
.ul type
.ix 'type'
.ix 'variable' 'type'
.ix 'object' 'type'
associated with it.
Functions may be defined to return a value, and that value
also has a
type.
.ix 'type'
.ix 'function' 'type'
The
type
describes the interpretation of a
value of that type, such as
whether it is signed or unsigned, a pointer, etc.
The type also describes the amount of storage required.
Together, the amount of storage and the interpretation of stored values
describes the range of values that may be stored in that type.
.pp
There are a number of different
types
defined by the C language.
They provide a great deal of power in selecting
methods for storing and moving data, and also contribute to the readability
of the program.
.pp
There are a number of "basic types",
.ix 'basic type'
.ix 'type' 'basic'
those which will appear in virtually
every program. More sophisticated types provide methods to describe data
structures, and are discussed in the chapter "Advanced Types".
.pp
.ix 'specifier' 'type'
.ix 'type' 'specifier'
.ix 'modifier' 'type'
.ix 'type' 'modifier'
.ix 'qualifiers'
.ix 'type' 'qualifiers'
A
.us type specifier
is one or more of:
.illust begin
.kw char
.kw double
.kw float
.kw int
.kw long
.kw short
.kw signed
.kw unsigned
.kw void
.us enumeration
.us structure
.us union
.us typedef name
.illust end
.keep begin
.pc
and may also include the following
.us type qualifiers
.ct :
.illust begin
.kw const
.kw volatile
.illust end
.keep end
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &company. compilers also provide the following extended
.us type qualifiers
..ct :
..sk 1 c
.im tqwatcom
.sk 1
.*
For the extended type qualifiers, see the appendix "&appkw.".
.shade end
..do end
.* ..if '&target' eq 'PC 370' ..th ..do begin
.* .shade begin
.* The &wlooc. compiler does not provide any additional
.* .us type qualifiers
.* ..ct ..li .
.* .shade end
.* ..do end
.*
.************************************************************************
.*
.pp
Various combinations of these keywords may be used when declaring
an object. Refer to the section on the type being defined.
.pp
The main types are
.kw char
..ct ,
.kw int
..ct ,
.kw float
and
.kw double
..ct ..li .
The keywords
.kw short
..ct ,
.kw long
..ct ,
.kw signed
..ct ,
.kw unsigned
..ct ,
.kw const
and
.kw volatile
modify these types.
