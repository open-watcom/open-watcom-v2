.ix 'storage class' 'extern'
.ix 'extern storage class'
.*
.pp
If an object is declared with the keyword
.kw extern
inside a function, then the object has
.ix 'linkage' 'external'
.ix 'external linkage'
.us external linkage,
meaning that its value is available to all modules, and to the
function(s) containing the definition in the current module.
No initializer list may be specified in this case, which implies
that the space for the object is allocated in some other module.
.pp
If an object is declared outside of the definition of a function,
and the declaration does not contain either of the keywords
.kw static
or
.kw extern
..ct ,
then the
.ix 'external object' 'creating'
.ix 'creating an external object'
space for the object is created at this point.
The object
has
.ix 'external linkage'
.ix 'linkage' 'external'
.us external linkage,
meaning that it is available to other modules in the program.
.pp
The following examples illustrate the creation of external objects,
provided the declarations occur outside any function:
.millust begin
int   X;
float F;
.millust end
.pp
If the declaration for an object, outside of the definition of a
function, contains the keyword
.kw extern
and has an initializer list, then space for the object is
created at this point, and the object has external linkage.
If, however, the declaration does not include an initializer list,
then the compiler assumes that the object is declared elsewhere.
If, during the remainder of the compilation of the module, no
further declarations of the object are found, or more declarations with
.kw extern
and no initializer list are found,
then the object must have space allocated for it in another
module.
If a subsequent declaration in the same module does have an initializer
list or omits the
.kw extern
keyword, then the space for the object is created at that point.
.pp
The following examples also illustrate the creation of external objects:
.millust begin
extern LIST * ListHead = 0;
       int    StartVal = 77;
.millust end
.pc
However, the next examples illustrate the
.us tentative definition
of external objects. If no further definition of the object of a form
shown above is found, then the object is found outside of the module.
.millust begin
extern LIST * ListEl;
extern int    Z;
.millust end
.pp
Another module may define its own object with
the same name
(provided it has
.us static
storage class),
but it will not be able to access
the external one.
However, this can be confusing and is a questionable
programming practice.
.pp
Any value placed in an
object declared with the
.kw extern
keyword
will remain unchanged until changed by a function
within the same or another module.
.pp
A function that is declared without the keyword
.kw static
has external linkage.
.keep begin
.pp
Suppose a module declares an object (outside of any function definition)
as follows:
.millust begin
struct list_el * ListTop;
.millust end
.keep end
.pc
where the structure
.mono list_el
is defined elsewhere.
This declaration allocates space for and declares the object
.mono ListTop
to be a pointer to a structure
.mono list_el
..ct ,
with external linkage.
Another module with the declaration,
.millust begin
extern struct list_el * ListTop;
.millust end
.pc
refers to the same
object
.mono ListTop
..ct ,
and states that it is found outside of the module.
.pp
Within a program, possibly consisting of more than one
module, each object or function with external linkage must be defined
(have space allocated for it) exactly once.
