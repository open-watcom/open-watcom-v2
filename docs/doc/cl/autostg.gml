.ix 'automatic storage duration'
.pp
The most commonly used object in a C program is one that has
meaning only within the function in which it is defined. The
object is created when execution of the function is begun
and destroyed when execution of the function is completed.
Such an object is said to have
.us automatic storage duration.
The
.us scope
.ix 'scope'
of the object is said to be the function in which it is defined.
.pp
If such an object has the same name as another object defined
outside the function (using
.kw static
or
.kw extern
..ct ),
then the outside object is hidden from the function.
.pp
Within a function, any object that does not have its declaration
preceded by the keyword
.kw static
or
.kw extern
has automatic storage duration.
.pp
It is possible to declare an object as automatic
within any block of a function. The
.us scope
of such an object is the block in which it is declared, including
any blocks inside it. Any outside block is unable
to access such an object.
.pp
Automatic objects may be
.ix 'initialization' 'auto'
.ix 'auto' 'initialization'
initialized as described in the chapter
"&initobj.".
Initialization of the object only occurs when the block in which
the object is declared is entered normally. In particular, a jump
into a block nested within the function will
.bd not
initialize any objects declared in that block.
This is a questionable programming practice, and should be avoided.
.keep begin
.pp
The following function
checks a string to see if it contains nothing but digits:
.millust begin
extern int IsInt( const char * ptr )
/**********************************/
{
    if( *ptr == '\0' ) return( 0 );
    for( ;; ) {
        char  ch;

        ch = *(ptr++);
        if( ch == '\0' )     return( 1 );
        if( !isdigit( ch ) ) return( 0 );
    }
}
.millust end
.keep end
.pc
The object
.mono ch
has a scope consisting only of the
.kw for
loop.
Any statements before or after the loop cannot
access
.mono ch.
