.ix 'static'
.ix 'storage class' 'static'
.pp
An object with
.us static storage duration
is created and initialized only once, prior to the execution
of the program.
Any value stored in such an object is retained throughout the
program unless it is explicitly altered by the program
(or it is declared with the
.kw volatile
keyword).
.pp
Any object that is declared outside the scope of a function
has
static
storage duration.
.pp
There are three types of static objects:
.autonote
.note
objects whose values are only available within the function
in which they are defined
.ix 'no linkage'
.ix 'linkage' 'no'
(no linkage).
For example,
.millust begin
extern void Fn( int x )
{
    static int ObjCount;
/* ... */
}
.millust end
.note
objects whose values are
only available within the module in which they are defined
.ix 'internal linkage'
.ix 'linkage' 'internal'
(internal linkage).
For example,
.millust begin
static int ObjCount;

extern void Fn( int x )
{
/* ... */
}
.millust end
.note
objects whose values are available to all components of
the program
.ix 'external linkage'
.ix 'linkage' 'external'
(external linkage).
For example,
.millust begin
extern int ObjCount = { 0 };

extern void Fn( int x )
{
/* ... */
}
.millust end
.endnote
.pc
The first two types are defined with the keyword
.kw static
..ct ,
while the third is defined with the (optional) keyword
.kw extern
..ct ..li .
