.ix 'type' 'array'
.ix 'array'
.pp
An
.us array
is a collection of objects which are all of the same type. All
elements (objects) in the array are stored in contiguous (adjacent)
memory.
.pp
References to array elements are usually made through
.us indexing
.ix 'index'
.ix 'array' 'index'
into the array. To facilitate this, the elements of the array
are numbered starting at zero. Hence an array declared with
.mono n
elements is indexed using indices between 0 and
.mono n-1.
.pp
An array may either be given an explicit size (using a constant
expression) or its size
may be determined by the number of values
used to initialize it.
Also, it is possible to declare an array without any size information,
in the following cases:
.begbull
.bull
a parameter to a function is declared as
"array of
.us type
..ct " (in which case the compiler alters the type to be "pointer to
.us type
..ct "),
.bull
an array object has external linkage
(extern)
.ix 'extern'
.ix 'storage class' 'extern'
and the definition
which creates the array is given elsewhere,
.bull
the array is fully declared later in the same module.
.endbull
.pc
An array of undetermined size is an
.ix 'incomplete type'
.us incomplete type
..ct ..li .
.keep begin
.pp
An array declaration is of the following form:
.cillust begin
type identifier
.mono [
constant-expression
.monoon
];
.monooff
.cor
type identifier
..ct .mono [] = {
initializer-list
.monoon
};
.monooff
.cor
type identifier
..ct .mono [
constant-expression
.mono ] = {
initializer-list
.monoon
};
.monooff
.cor
type identifier:MONO.[]:eMONO.:MSEMI.
.cillust end
.keep end
.pc
where
.us type
is the type of each element of the array,
.us identifier
is the name of the array,
.us constant-expression
is an expression that evaluates to a positive integer defining the number
of elements in the array, and
.us initializer-list
is a list of values
(of type
.us type
..ct )
to be assigned to successive elements of the array.
.*
.************************************************************************
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler allows an array to be defined with 0 elements.
.shade end
..do end
.************************************************************************
.*
.pp
For example,
.millust begin
int values[10];
.millust end
.pc
declares
.mono values
to be an array of 10 integers, with indices from 0 to 9.
The expression
.mono values[5]
refers to the sixth integer in the array.
.millust begin
char text[] = { "some stuff" };
.millust end
.pc
declares
.mono text
to be an array of 11 characters, each containing successive letters
from
.mono "some stuff".
The value of
.mono text[10]
is
.monoon
'\0'
.monooff
.ix 'null character'
(the null character),
representing the terminating character in the string (see
Strings).
.millust begin
extern NODES nodelist[];
.millust end
.pc
declares
.mono nodelist
to be an array of
.mono NODES
(defined elsewhere), and the array is
of unknown size. In another
source file or later in the current file,
there must be a corresponding declaration of
.mono nodelist
which defines how big the array actually is.
.pp
It is possible to declare multi-dimensional arrays by including more
than one set of dimensions. For example,
.millust begin
int tbl[2][3];
.millust end
.pc
defines a 2-row by 3-column array of integers.
In fact, it defines an array of 2 arrays of 3 integers. The values
are stored in memory in the following order:
.millust begin
tbl[0][0]
tbl[0][1]
tbl[0][2]
tbl[1][0]
tbl[1][1]
tbl[1][2]
.millust end
.pc
The rows of the table are stored together. This form of storing an
array is called
.us row-major order.
The expression
.mono tbl[1][2]
refers to the element in the last row and last column of the array.
.pp
In an expression, if an array is named without specifying any indices,
the value of the array name is the address of its first element.
In the example,
.millust begin
int   array[10];
int * aptr;

aptr = array;
.millust end
.keep begin
.pc
the assignment to
.mono aptr
is equivalent to,
.millust begin
aptr = &array[0];
.millust end
.keep end
.pc
Since multi-dimensional arrays are just arrays of arrays, it follows that
omission of some, but not all, dimensions is equivalent to taking the
address of the first element of the sub-array.
In the example,
.millust begin
int   array[9][5][2];
int * aptr;

aptr = array[7];
.millust end
.pc
the assignment to
.mono aptr
is equivalent to,
.millust begin
aptr = &array[7][0][0];
.millust end
.pp
Note that no checking of indices is performed at execution time.
An invalid index (less than zero or greater than the highest
index) will refer to memory as if the array was extended to accommodate
the index.
