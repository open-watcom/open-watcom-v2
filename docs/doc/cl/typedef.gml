.ix 'typedef'
.ix 'type definition'
.pp
A
.us typedef
declaration introduces a synonym for another type.
It does not introduce a new type.
.pp
The general form of a type definition is:
.cillust begin
.mono typedef
type-information typedef-name:MSEMI.
.cillust end
.pc
The
.us typedef-name
may be a comma-separated list of identifiers, all of which become
synonyms for the type.
The names are in the same
.ix 'name space'
name space as ordinary object names, and can be redefined in
inner blocks. However, this can be confusing and should be avoided.
.pp
The simple declaration,
.millust begin
typedef signed int COUNTER;
.millust end
.pc
declares the identifier
.mono COUNTER
to be equivalent to the type
.kw signed int
..ct ..li .
A subsequent declaration like,
.millust begin
COUNTER ctr;
.millust end
.pc
declares the object
.mono ctr
to be a signed integer.
If, later on, it is necessary to change all counters
to be long signed integers, then only the
.kw typedef
would have to be changed, as follows:
.millust begin
typedef long signed int COUNTER;
.millust end
.pc
All declarations of objects of that type will use the new type.
.pp
The
.kw typedef
can be used to simplify declarations elsewhere in a program.
For example, consider the following structure:
.millust begin
struct complex {
    double    real;
    double    imaginary;
};
.millust end
.pc
To declare an object to be an instance of the structure requires
the following declaration:
.millust begin
struct complex cnum;
.millust end
.keep begin
.pp
Now consider the following structure definition with a type definition:
.millust begin
typedef struct {
    double   real;
    double   imaginary;
} COMPLEX;
.millust end
.keep end
.pc
In this case, the identifier
.mono COMPLEX
refers to the entire structure definition,
including the keyword
.kw struct
..ct ..li .
Therefore, an object can be declared as follows:
.millust begin
COMPLEX cnum;
.millust end
.pc
While this is a simple example, it illustrates a method of making
object declarations more readable.
.pp
Consider the following example, where the object
.mono fnptr
is being declared as a pointer to a function which takes two
parameters, a pointer to a structure
.mono dim3
and an integer.
The function returns a pointer to the structure
.mono dim3.
The declarations could appear as follows:
.millust begin
struct dim3 {
    int  x;
    int  y;
    int  z;
};

struct dim3 * (*fnptr)( struct dim3 *, int );
.millust end
.keep begin
.pc
or as:
.millust begin
typedef struct {
    int  x;
    int  y;
    int  z;
} DIM3;

DIM3 * (*fnptr)( DIM3 *, int );
.millust end
.keep break
.pc
or as:
.millust begin
typedef struct {
    int  x;
    int  y;
    int  z;
} DIM3;
.millust break
..sk 1 c
typedef DIM3 * DIM3FN( DIM3 *, int );

DIM3FN * fnptr;
.millust end
.keep end
.pc
The last example simply declares
.mono fnptr
to be a pointer to a
.mono DIM3FN
..ct ,
while
.mono DIM3FN
is declared to be a function with two parameters,
a pointer to a
.mono DIM3
and an integer.
The function returns a pointer to a
.mono DIM3.
.mono DIM3
is declared to be a structure of three co-ordinates.
.*
.beglevel
.*
.section Compatible Types
.*
.pp
Some operations, such as assignment,
are restricted to operating on two objects of the same
type.
If both operands are already the same type, then no special conversion
is required.
Otherwise, the compiler may alter automatically
one or both operands to make
them the same type. The
.ix 'integral promotion'
integral promotions and
.ix 'arithmetic conversion'
arithmetic conversions are examples.
Other types
may require an explicit
.ix 'cast'
cast.
.pp
The compiler decides whether or not an explicit cast is required based
on the concept of
.ix 'type' 'compatible'
.ix 'compatible types'
.us compatible types.
The following types are compatible:
.begbull
.bull
two types that are declared exactly the same way,
.bull
two types that differ only in the ordering of the type specifiers,
for example,
.kw unsigned long int
and
.kw int long unsigned
..ct ,
.bull
two arrays of members of compatible type, where
both arrays have the same size, or where
one array is declared
without size information,
.bull
two functions that return the same type,
one containing no parameter information,
and the other containing a fixed
number of parameters (no "
..ct .mono ,...
..ct ")
that are not affected by the default argument promotions,
.bull
two structures, defined in separate modules,
that have the same number and names of members, in the same order,
with compatible types,
.bull
two unions, defined in separate modules,
that have the same number and names of members,
with compatible types,
.bull
two enumerated types, defined in separate modules,
that have the same number of enumeration constants, with
the same names and the same values,
.bull
two pointers to compatible types.
.endbull
.endlevel
