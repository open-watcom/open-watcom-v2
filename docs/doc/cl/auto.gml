.ix 'auto'
.ix 'storage class' 'auto'
.pp
The declaration of an object in a function that does not contain
the keywords
.kw static
..ct ,
.kw extern
or
.kw register
declares an object with automatic storage duration.
Such an object may precede its declaration with the keyword
.kw auto
for readability.
.pp
An object declared with no storage class specifier or with
.kw auto
is "addressable", which means that the
.us address-of
.ix 'address-of operator'
.ix 'operator' 'address-of'
.ix 'operator' '&'
operator may be applied to it.
.pp
The programmer should not assume any relationship between the
storage locations of multiple
.kw auto
objects declared in a function.
If relative placement of objects is important, a structure should
be used.
.pp
The following function
illustrates a use for
.kw auto
objects:
.millust begin
extern int FindSize( struct thing * thingptr )
/********************************************/
{
    auto char * start;
    auto char * finish;

    FindEnds( thingptr, &start, &finish );
    return( finish - start + 1 );
}
.millust end
.pp
The addresses of the automatic objects
.mono start
and
.mono finish
are passed to
.mono FindEnds
..ct , which, presumably, modifies them.
