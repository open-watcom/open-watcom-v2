.beglevel
.*
.pp
Near pointers are generally the most efficient type of pointer because
they are small, and the compiler can assume knowledge about what
segment of the computer's memory the pointer (offset) refers to.
Far pointers are the most flexible because they allow the programmer
to access any part of the computer's memory, without limitation to
a particular segment.
However, far pointers are bigger and slower
because of the additional flexibility.
.pp
Based pointers are a compromise between the efficiency of near pointers
and the flexibility of far pointers.
With based pointers, the programmer takes responsibility to tell
the compiler which segment a near pointer (offset) belongs to, but may
still access segments of the computer's memory outside of the normal data
segment (DGROUP).
The result is a pointer type which is as small as and almost as efficient as
a near pointer, but with most of the flexibility of a far pointer.
.pp
An object declared as a based pointer falls into one of the following
categories:
.begbull $compact
.* .bull
.* the based pointer is in the same segment as another named object,
.bull
the based pointer is in the segment described by another object,
.bull
the based pointer, used as a pointer to another object of the same
type (as in a linked list), refers to the same segment,
.bull
the based pointer is an offset to no particular segment, and must
be combined explicitly with a segment value to produce a valid pointer.
.endbull
.pp
To support based pointers, the following keywords are provided:
..sk 1 c
.ix 'keyword'
..sr firsttab=&SYSIN.+3
..tb set $
..tb &firsttab. +9 +9 +9 +9
.kwon
$&kwbased.
.kwix &kwbased_sp.
$&kwsegment.
.kwix &kwsegm_sp.
$&kwsegname.
.kwix &kwsegnm_sp.
$&kwself.
.kwix &kwself_sp.
.kwoff
..tb set
..tb
.keep begin
.pp
The following operator is also provided:
.millust :>
.keep end
.pp
These keywords and operator are described in the following sections.
.pp
Two macros, defined in
.hdr <malloc.h>
are also provided:
.mkwix &mkwNSEG_sp.
.mkwix &mkwNOFF_sp.
.millust begin
&mkwNULLSEG.
&mkwNULLOFF.
.millust end
.pc
They are used in a similar manner to
.mkw NULL
..ct ,
but are used with objects declared as
.kwix &kwsegm_sp.
.kwfont &kwsegment.
and
.kwix &kwbased_sp.
.kwfont &kwbased.
respectively.
.*
.section Segment Constant Based Pointers and Objects
.*
.pp
A segment constant based pointer or object
has its segment value based on a specific, named segment.
A segment constant based object is specified as:
.cillust begin
type :MONO. &kwbased.( &kwsegname.( ":eMONO.segment:MONO." ) ) :eMONO.object-name:MSEMI.
.cillust end
.pc
and a segment constant based pointer is specified as:
.cillust begin
type :MONO. &kwbased.( &kwsegname.( ":eMONO.segment:MONO." ) ) * :eMONO.object-name:MSEMI.
.cillust end
.pc
where
.ul segment
is the name of the segment in which the pointer or object is based.
As shown above,
the segment name is always specified as a string.
There are three special segment names recognized by the compiler:
.millust begin
"_CODE"
"_CONST"
"_DATA"
.millust end
.pc
The
.mono "_CODE"
segment is the default code segment.
The
.mono "_CONST"
segment is the segment containing constant values.
The
.mono "_DATA"
segment is the default data segment.
If the segment name is not one of the three recognized names,
then a segment will be created with that name.
If a segment constant based object is being defined,
then it will be placed in the named segment.
If a segment constant based pointer is being defined,
then it can point at objects in the named segment.
.pp
The following examples illustrate segment constant based pointers and objects:
.millust begin
int &kwbased.( &kwsegname.( "_CODE" ) )   ival = 3;
int &kwbased.( &kwsegname.( "_CODE" ) ) * iptr;
.millust end
.pc
.mono ival
is an object that resides in the default code segment.
.mono iptr
is an object that resides in the data segment
(the usual place for data objects),
but points at an integer
which resides in the default code segment.
.mono iptr
is suitable for pointing at
.mono ival
..ct ..li .
.millust begin
char &kwbased.( &kwsegname.( "GOODTHINGS" ) ) thing;
.millust end
.pc
.mono thing
is an object which resides in the segment
.mono GOODTHINGS
..ct ,
which will be created if it does not already exist.
(The creation of segments is done by the linker, and is a method
of grouping objects and functions.
Nothing is implicitly created during the
execution of the program.)
.*
.section Segment Object Based Pointers
.*
.pp
A segment object based pointer
derives its segment value from another named object.
A segment object based pointer is specified as follows:
.cillust begin
.ul type
.mono &SYSRB.&kwbased.(
.ul segment
.mono ) *
.ul name:MSEMI.
.cillust end
.pc
where
.ul segment
is an object defined
.* or cast
as type
.kwix &kwsegm_sp.
.kwfont &kwsegment.
..ct ..li .
.pp
An object of type
.kwix &kwsegm_sp.
.kwfont &kwsegment.
may contain a segment value.
Such an object is particularly designed for use with segment object
based pointers.
.* .pp
.* The following example illustrates segment object based pointers which use
.* the base (segment value) implied by another object:
.* .millust begin
.* char *                             aptr;
.* char &kwbased.( (&kwsegment.) aptr ) * bptr;
.*
.* char far *                         xptr;
.* char &kwbased.( (&kwsegment.) xptr ) * yptr;
.* .millust end
.* .pc
.* In this case, the object
.* .mono bptr
.* points to characters in the same segment as characters to which
.* .mono aptr
.* points, while
.* the object
.* .mono yptr
.* points to characters in the same (far) segment as characters to which
.* .mono xptr
.* points.
.pp
The following example illustrates a segment object based pointer:
.* which uses
.* a base found in another object:
.millust begin
&kwsegment.             seg;
char &kwbased.( seg ) * cptr;
.millust end
.pc
The object
.mono seg
contains only a segment value.
Whenever the object
.mono cptr
is used to point to a character, the actual pointer value will be
made up of the segment value found in
.mono seg
and the offset value found in
.mono cptr
..ct ..li .
The object
.mono seg
might be assigned values such as the following:
.begbull $compact
.bull
a constant value (eg. the segment containing screen memory),
.bull
the result of the library function
.libfn _bheapseg
..ct ,
.bull
the segment portion of another pointer value, by casting it to the
type
.kwix &kwsegm_sp.
.kwfont &kwsegment.
..ct ..li .
.endbull
.*
.section Void Based Pointers
.*
.pp
A void based pointer must be explicitly combined with a segment value
to produce a reference to a memory location.
A void based pointer does not infer its segment value from another object.
The
.mono :>
.ix 'operator' ':>'
.ix 'base operator'
(base) operator is used to combine a segment value and a void based pointer.
.pp
For example,
on an IBM PC or PS/2 computer, running DOS, with a color monitor,
the screen memory
begins at segment 0xB800, offset 0.
In a video text mode,
to examine the first character currently displayed on the screen,
the following code could be used:
.code begin
extern void main()
  {
    &kwsegment.              screen;
    char &kwbased.( void ) * scrptr;

    screen = 0xB800;
    scrptr = 0;
    printf( "Top left character is '%c'.\n",
            *(screen:>scrptr) );
  }
.code end
.pp
The general form of the
.mono :>
operator is:
.cillust begin
.ul segment
.mono :>
.ul offset
.cillust end
.pc
where
.ul segment
is an expression of type
.kwix &kwsegm_sp.
.kwfont &kwsegment.
..ct ,
and
.ul offset
is an expression of type
.mono &kwbased.(&SYSRB.void&SYSRB.)&SYSRB.*
..ct ..li .
.*
.section Self Based Pointers
.*
.pp
A self based pointer infers its segment value from itself.
It is particularly useful for structures such as linked lists, where
all of the list elements are in the same segment.
A self based pointer pointing to one element may be used to access the
next element, and the compiler will use the same segment as the
original pointer.
.pp
The following example illustrates a function which will print the
values stored in the last two members of a linked list:
.code begin
struct a {
    struct a __based( __self ) * next;
    int                          number;
};
.code end
.sk 1 c
.code begin
extern void PrintLastTwo( struct a far * list )
  {
    __segment                 seg;
    struct a __based( seg ) * aptr;

    seg  = FP_SEG( list );
    aptr = FP_OFF( list );
    for( ; aptr != _NULLOFF; aptr = aptr->next ) {
        if( aptr->next == _NULLOFF ) {
            printf( "Last item is %d\n", aptr->number );
        } else if( aptr->next->next == _NULLOFF ) {
            printf( "Second last item is %d\n", aptr->number );
        }
    }
  }
.code end
.pc
The parameter to the function
.mono PrintLastTwo
is a far pointer, pointing to a linked list structure anywhere in memory.
It is assumed that all members of a particular linked list of this
type reside in the same segment of the computer's memory.
(Another instance of the
linked list might reside entirely in a different segment.)
The object
.mono seg
is given the segment portion of the far pointer.
The object
.mono aptr
is given the offset portion, and is described as being based in the
segment stored in
.mono seg.
.pp
The expression
.mono aptr->next
refers to the
.mono next
member of the
structure stored in memory at
the offset stored in
.mono aptr
and
the segment implied by
.mono aptr
..ct ,
which is the value stored in
.mono seg.
So far, the behavior is no different than if
.mono next
had been declared as,
.millust begin
struct a * next;
.millust end
.pp
The expression
.mono aptr->next->next
illustrates the difference of using a self based pointer.
The first part of the expression (
..ct .mono aptr->next
..ct )
occurs as described above.
However, using the result to point to the next member occurs by
using the offset value found in the
.mono next
member and combining it with the segment value of the
.ul pointer used to get to that member
..ct ,
which is still the segment implied by
.mono aptr
..ct ,
which is the value stored in
.mono seg.
If
.mono next
had not been declared using
.mono &kwbased.(&SYSRB.&kwself.&SYSRB.)
..ct ,
then the second pointing operation would refer to the offset value
found in the
.mono next
member, but with the default data segment (DGROUP), which may or may
not be the same segment as stored in
.mono seg.
.endlevel
