.ix 'initialization'
.ix 'object' 'initialization'
.pp
Any definition of an object may include a value or list of values
for initializing it, in which case
the declaration is followed by an equal sign
(:MONO.=:eMONO.)
and the
initial value(s).
.pp
The initial value for an object with
.ix 'static' 'initialization'
.ix 'initialization' 'static'
static storage duration
may be any expression that evaluates to a
constant value, including using the
.us address-of
.ix 'operator' 'address-of'
.ix 'address-of operator'
.ix 'operator' '&'
operator to take the address of a function or object with static
storage duration.
.pp
The initial value for an object with
.ix 'auto' 'initialization'
.ix 'initialization' 'auto'
automatic storage duration
may be any expression that would be valid as an assignment to that
object, including references to other objects.
The evaluations of the initializations occur in the order in which
the definitions of the objects occur.
.*
.section Initialization of Scalar Types
.*
.pp
The initial value for a scalar type
(pointers, integers and floating-point types)
may be enclosed in braces, although braces are not required.
.pp
The following declarations might appear
inside a function:
.millust begin
static   int   MaxRecLen  = 1000;
static   int   MaxMemSize = { 1000 * 8 + 10000 };
         float Pi         = 3.14159;
auto     int   x          = 3;
register int   y          = x * MaxRecLen;
.millust end
.*
.section Initialization of Arrays
.ix 'array' 'initialization'
.ix 'initialization' 'array'
.*
.pp
For arrays of
characters
being initialized with a string literal,
and for arrays of
.ix 'wchar_t'
.kw wchar_t
being initialized with a wide string literal,
the braces around initial values are optional.
For other arrays, the braces are required.
.pp
If an array
of unknown size is initialized, then the size of the array is determined
by the number of initializing values provided. In particular, an
array of characters of unknown size may be initialized using a string
literal, in which case the size of the array is the number of characters
in the string, plus one for the terminating
.ix 'null character'
null character. Each
character of the string is placed in successive elements of the array.
Consider the following array declarations:
.millust begin
char  StartPt[] = "Starting point...";
int   Tabs[]    = { 1, 9, 17, 25, 33, 41 };
float Roots[]   = { 1., 1.414, 1.732, 2., 2.236 };
.millust end
.pc
The object
.mono StartPt
is an array of 18 characters,
.mono Tabs
is an array of 6 integers, and
.mono Roots
is an array of 5 floating-point numbers.
.pp
If an array is declared to have a certain number of elements, then
the maximum number of values in the initialization list is the number
of elements in the array.
An exception is made for arrays of characters,
where the initializer may be a string with the same length as
the number of characters in the array.
Each character from the string is assigned to the corresponding
element of the array.
The null character at the end of the string literal is ignored.
.keep begin
.pp
If there are fewer initialization values than elements
of the array, then any elements not receiving a value
from the list are assigned the value
zero (for arithmetic types),
or the null pointer constant (for pointers).
Consider the following examples:
.millust begin
char  Vowels1[6]  = "aeiouy";
char  Vowels2[6]  = { 'a', 'e', 'i', 'o', 'u', 'y' };
int   Numbers[10] = { 100, 10, 1 };
float Blort[5]    = { 5.6, -2.2 };
.millust end
.keep end
.pc
The objects
.mono Vowels1
and
.mono Vowels2
are both arrays of six characters, and both contain exactly the same
values in each of their corresponding elements.
The object
.mono Numbers
is an array of 10 integers, the first three of which are initialized to
.mono 100
..ct ,
.mono 10
and
.mono 1
..ct ,
and the remaining seven are set to zero.
The object
.mono Blort
is an array of 5 floating-point numbers.
The first two elements are
initialized to
.mono 5.6
and
.mono -2.2
..ct ,
and the remaining three are set to zero.
.keep begin
.pp
If an array of more than one dimension is initialized, then each
subarray may be initialized using a brace-enclosed list of values.
This form will work for an arbitrary number of dimensions. Consider
the following two-dimensional case:
.millust begin
int Box[3][4] = { { 11, 12, 13, 14 },
                  { 21, 22, 23, 24 },
                  { 31, 32, 33, 34 } };
.millust end
.keep end
.pc
The object
.mono Box
is an array of 3 arrays of 4 integers. There are three values in the
initialization list, corresponding to the first dimension (3 rows).
Each
initialization value is itself a list of values corresponding to
the second dimension (4 columns).
In other words, the first list of values
.mono {&SYSRB.11, 12, 13, 14&SYSRB.}
is assigned
to the first row of
.mono Box
..ct ,
the second list of values
.mono {&SYSRB.21, 22, 23, 24&SYSRB.}
is assigned to the
second row of
.mono Box
..ct ,
and the third list of values
.mono {&SYSRB.31, 32, 33, 34&SYSRB.}
is assigned to the
third row of
.mono Box.
.pp
If all values are supplied for initializing an array, or if only
elements from the end of the array are omitted,
then the sub-levels need not be within braces. For example, the
following declaration of
.mono Box
is the same as above:
.millust begin
int Box[3][4] = { 11, 12, 13, 14,
                  21, 22, 23, 24,
                  31, 32, 33, 34 };
.millust end
.pp
The same rules about incomplete initialization lists apply to
multi-dimensional arrays. The following example defines a mathematical
3-by-3 identity matrix:
.millust begin
int Identity[3][3] = { { 1 },
                       { 0, 1 },
                       { 0, 0, 1 } };
.millust end
.pc
The missing values are replaced with zeroes.
The initialization also could have been given as,
.millust begin
int Identity[3][3] = { { 1, 0, 0 },
                       { 0, 1, 0 },
                       { 0, 0, 1 } };
.millust end
.pc
or as,
.millust begin
int Identity[3][3] = { 1, 0, 0,
                       0, 1, 0,
                       0, 0, 1 };
.millust end
.*
.section Initialization of Structures
.ix 'initialization' 'struct'
.ix 'struct' 'initialization'
.*
.pp
Structures
may be initialized in a manner similar to arrays.
The initializer list must be specified within braces.
.keep begin
.pp
For example,
.millust begin
struct printformat {
    int     pagewid;
    char    carr_ctl;
    char *  buffer;
};
.millust break
..sk 1 c
char PrBuffer[256];

struct printformat PrtFmt = { 80, ' ', PrBuffer };
.millust end
.keep end
.pp
Each value from the initializer list is assigned
to each successive member of the structure.
Any unnamed gaps between members or at the end of the structure
(caused by alignment)
are ignored during initialization.
If there are more members of the structure than values specified
by the initializer list, then the remaining members are initialized to
zero (for arithmetic types) or the null pointer constant (for pointers).
.pp
If a structure member is itself an array,
structure or union, then the sub-members
may be initialized using a brace-enclosed initializer list.
If braces are not specified, then
for the purposes of initialization, the sub-members are treated
as if they are members of the outer structure, as
each subsequent initializer value
initializes a sub-member, until no more sub-members are found, in which
case the next member of the outer structure is initialized.
.*
.section Initialization of Unions
.ix 'initialization' 'union'
.ix 'union' 'initialization'
.*
.pp
Initializations of unions is the same as for structures, except that
only the first member of the union may be initialized,
using a brace-enclosed initializer.
.pp
Consider the following example:
.millust begin
struct first3 {
    char first, second, third;
};
.millust break
..sk 1 c
union ustr {
    char           string[20];
    struct first3  firstthree;
};
union ustr Str = { "Hello there" };
.millust end
.pc
The object
.mono Str
is declared to be a union of two types, the first of
which is an array of 20 characters,
and the second of which is a structure that allows direct access
to the first three characters of the string contained in the array.
The array is initialized to the string
.mono "Hello there".
The three characters of
.mono struct first3
will have the characters
.mono 'H', 'e'
and
.mono 'l'.
Had the declaration of
.mono ustr
been,
.millust begin
union ustr {
    struct first3  firstthree;
    char           string[20];
};
.millust end
.pc
then the initialization could only set the first three characters.
:CMT. .pp
:CMT. If a member of the
:CMT. structure or union
:CMT. is itself a structure, union or array type, then the
:CMT. value in the initializer list may be a brace-enclosed list of values.
:CMT. For example,
:CMT. .code begin
:CMT. enum s_type { STRUCT1, STRUCT2 };
:CMT.
:CMT. struct s1 {
:CMT.     enum s_type which;
:CMT.     char *      names[3];
:CMT. };
:CMT. struct s2 {
:CMT.     enum s_type which;
:CMT.     struct s1 * objects[4];
:CMT. };
:CMT.
:CMT. .code break
:CMT. union u1 {
:CMT.     struct s1 st1;
:CMT.     struct s2 st2;
:CMT. };
:CMT. union u2 {
:CMT.     struct s2 st2;
:CMT.     struct s1 st1;
:CMT. };
:CMT.
:CMT. struct s1 S1_1, S1_2, S1_3, S1_4;
:CMT.
:CMT. union u1 Obj1 = { STRUCT1, { "Steve", "Fred", "Jack" } };
:CMT. union u2 Obj2 = { STRUCT2, { &S1_1, &S1_2, &S1_3, &S1_4 } };
:CMT. .code end
:CMT. In this case, the object
:CMT. .mono Obj1
:CMT. is declared to be a
:CMT. .mono union u1
:CMT. type, which has, as its first member, an object of type
:CMT. .mono struct s1.
:CMT. Hence,
:CMT. .mono Obj1
:CMT. is initialized with an integer value
:CMT. .mono (STRUCT1)
:CMT. and an array of pointers to strings.
:CMT. The object
:CMT. .mono Obj2
:CMT. is declared to be a
:CMT. .mono union u2
:CMT. type, which has, as its first member, an object of type
:CMT. .mono struct s2.
:CMT. Hence,
:CMT. .mono Obj2
:CMT. is initialized with an integer value
:CMT. .mono (STRUCT2)
:CMT. and an array of pointers to other objects of type
:CMT. .mono struct s1.
:CMT. .pp
:CMT. In each of these cases, the extra set of braces inside the initializer
:CMT. list could have been omitted, but were included for clarity.
.*
.section Uninitialized Objects
.ix 'uninitialized objects'
.*
.pp
An object with
.us static
storage duration, and no explicit initialization,
will be initialized as if every member that has
arithmetic type was assigned zero and every member that has
a pointer type was assigned a null (zero) pointer.
.pp
An object with
.us automatic
storage duration, and no explicit initialization,
is not initialized. Hence, a reference to such an automatic
object that has not been assigned a value will yield undefined
behavior. On most systems,
the value of the object will be
arbitrary and
unpredictable.
