.beglevel
.ix 'structure'
.pp
A
.us structure
is a type composed of
a sequential group of
.us members
.ix 'member'
.ix 'structure' 'member'
of various types.
Like other types, a structure is a model describing storage requirements
and interpretations, and does not reserve any storage.
Storage is reserved when an
object is declared to be an
.us instance
of the structure.
.pp
Each of the members of a structure must have a name,
with the exception of
.us bit-fields.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
a structure member may be unnamed if the member is a structure or union.
.shade end
..do end
.*
.************************************************************************
.*
.pp
A structure may not contain a member with an incomplete type.
In particular, it may not contain a member with a type of the structure
being defined (otherwise the structure would have indeterminate size),
although it may contain a pointer to it.
.pp
The structure may be given an optional
.ix 'tag'
.us tag
with which the structure may be referenced elsewhere in the program.
If no tag is given, then only those objects listed following
the definition of the structure may have the structure type.
.pp
The
.ix 'name space' 'structures'
.us name space
for structure tags
is different from that of object names, labels and
member names, so a tag may be the same identifier
as one of these other kinds.
A structure tag may not be the same as the tag of a union or enumerated type,
or another structure.
.pp
Each structure has its own
.ix 'name space' 'structure members'
name space, so an identifier may be
used as a member name in more than one structure.
An identifier that is an object name, structure tag,
union tag, union member name, enumeration
tag or label may also be used as a member name without ambiguity.
.pp
Structures help to organize program data by collecting
several related objects into one object.
They are also used for linked lists, trees and for describing
externally-defined regions of data that the application must access.
.keep begin
.pp
The following structure might describe a token identified by
parsing a typed command:
.millust begin
struct tokendef {
    int    length;
    int    type;
    char   text[80];
};
.millust end
.keep end
.pc
This defines a structure containing three members, an integer
containing the token length, another integer containing some
encoding of the token type, and the third an array of 80 characters
containing the text of the token.
The tag of the structure is
.mono tokendef.
.keep begin
.pp
The above definition does not actually create an object containing
the structure.
Creation of an instance of the structure
requires a list of identifiers following the
structure definition, or to use
.mono struct tokendef
in place of a type for declaring an object. For example,
.millust begin
struct tokendef {
    int    length;
    int    type;
    char   text[80];
} token;
.millust end
.keep break
.pc
is equivalent to,
.millust begin
struct tokendef {
    int    length;
    int    type;
    char   text[80];
};

struct tokendef token;
.millust end
.keep end
.pc
Both create the object
.mono token
as an instance of the structure
.mono tokendef.
The
.us type
of
.mono token
is
.mono struct tokendef.
.pp
.ix 'structure' 'member reference'
.ix 'reference to structure member'
References to a member of a structure
are made using the
.ix 'operator' 'dot'
.ix 'operator' '.'
.ix 'dot operator'
.us dot
operator (.).
The first operand of the . operator is the object containing the
structure. The second operand is the name of the member.
For example,
.mono token.length
refers to the
.mono length
member of the
.mono tokendef
structure contained in
.mono token.
.keep begin
.pp
If
.mono tokenptr
is declared as,
.millust begin
struct tokendef * tokenptr;
.millust end
.keep break
.pc
(
..ct .mono tokenptr
is a pointer to a
.mono tokendef
structure), then,
.millust (*tokenptr).length
.keep break
refers to the
.mono length
member of the
.mono tokendef
structure that
.mono tokenptr
points to.
Alternatively,
to refer to a member of a structure,
the
.ix 'arrow operator'
.ix 'operator' 'arrow'
.ix 'operator' '->'
.us arrow
operator (
..ct .mono ->
..ct ) is used:
.keep break
.millust tokenptr->length
is equivalent to,
.millust (*tokenptr).length
.keep end
.shade begin
If a structure contains an unnamed member which is a structure or
union, then the members of the inner structure or union are referenced
as if they were members of the outer structure.
For example,
.millust begin
struct outer {
    struct inner {
        int     a, b;
    };
    int c;
} X;
.millust end
.pp
The members of
.mono X
are referenced as
.mono X.a
..ct ,
.mono X.b
and
.mono X.c
..li.
.* .pp
.* Unnamed structure members may not be portable to other compilers,
.* because they are not required by the ISO standard.
.shade end
.pp
Each
member
.ix 'structure' 'member'
.ix 'member' 'of structure'
of a structure is at a higher address than the previous member.
.ix 'alignment'
Alignment of members may cause (unnamed) gaps between members,
and an unnamed area at the end of the structure.
.*
.********************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers provide a command-line switch
and a
.kwpp #pragma
to control the alignment of members of structures.
See the &userguide. for details.
.pp
In addition, the
.kwix &kwipckd_sp.
.kwfont &kwipacked.
keyword is provided, and if specified before the
.kw struct
keyword, will force the structure to be packed (no alignment, no gaps)
regardless of
the setting of the command-line switch or the
.kwpp #pragma
controlling the alignment of members.
.shade end
..do end
..if '&target' eq '370' or '&target' eq 'PC 370' ..th ..do begin
..do end
.*
.********************************************************************
.*
.pp
A pointer to an object with a structure type, suitably cast,
is also a pointer
to the first member of the structure.
.pp
A structure declaration of the form,
.cillust begin
.mono struct
tag;
.cillust end
.pc
can be used to
:CMT. temporarily "undeclare" the
declare a new
structure within a block,
temporarily hiding the old structure.
When the block ends, the previous structure's hidden
declaration will be restored.
For example,
.millust begin
struct thing { int a,b; };
/* ... */
    {
        struct thing;
        struct s1    { struct thing * thingptr; } tptr;
        struct thing { struct s1 *    s1ptr; }    sptr;
    }
.millust end
.pc
the original definition of
.mono struct thing
is suppressed in order to create
a new definition.
Failure to suppress the original definition would result in
.mono thingptr
being a pointer to the old definition of
.mono thing
rather than the new one.
.pp
Redefining structures
can be confusing and should be avoided.
.*
.section Bit-fields
.*
.ix 'bit-field'
.ix 'structure' 'bit-field'
.pp
A member of a structure can be declared as a
.us bit-field
..ct ,
provided the type of the member is
.kw int
..ct ,
.kw unsigned int
or
.kw signed int
..ct ..li .
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
In addition, the &wcboth. compilers allow the types
.kw char
..ct ,
.kw unsigned char
..ct ,
.kw short int
and
.kw unsigned short int
to be bit-fields.
.shade end
..do end
.* ..if '&target' eq 'PC 370' ..th ..do begin
.* .shade begin
.* In addition, the &wcall. compilers allow the types
.* .kw char
.* ..ct ,
.* .kw unsigned char
.* ..ct ,
.* .kw short int
.* and
.* .kw unsigned short int
.* to be bit-fields.
.* .shade end
.* ..do end
.*
.************************************************************************
.*
.pp
A bit-field declares the member to be a number of bits.
A value may be assigned to the bit-field in the same manner as other
integral types, provided the value can be stored in the number of
bits available. If the value is too big for the bit-field, excess
high bits are discarded when the value is stored.
.pp
The type of the bit-field determines the treatment of the highest bit
of the bit-field.
Signed types cause the high bit to be treated as a sign bit, while
unsigned types do not treat it as a sign bit.
For a bit-field defined with type
.kw int
(and no
.kw signed
or
.kw unsigned
keyword),
whether or not
the high bit is considered a sign bit is implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers treat the high bit
of a bit-field of type
.kw int
as a sign bit.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler does not treat the high bit
of a bit-field of type
.kw int
as a sign bit.
.shade end
..do end
.*
.************************************************************************
.*
.pp
A bit-field is declared by following the member name
by a colon and a constant expression which evaluates to a non-negative
value that does not exceed the number of bits in the type.
.pp
A bit-field may be declared without a name and may be used to align
a structure to an imposed form. Such a bit-field
cannot be referenced.
.pp
If two bit-fields are declared sequentially within the same
structure, and they would both fit within the storage unit
assigned to them by the compiler, then they are both placed
within the same storage unit.
If the second bit-field doesn't fit, then whether it is placed
in the next storage unit, or partially placed in the same unit as
the first and spilled over into the next unit, is
implementation-defined.
.*
.************************************************************************
.*
.shade begin
..if '&target' eq 'PC' ..th ..do begin
The &wcboth. compilers
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
The &wcall. compilers
..do end
place a bit-field in the next storage unit
if it will not fit in the
remaining portion of the previously defined bit-field.
Bit-fields are not allowed to straddle storage unit
boundaries.
.shade end
.*
.************************************************************************
.*
.pp
An unnamed member declared as
.mono :&SYSRB.0
prevents the next bit-field from being placed in the same
storage unit as the previous bit-field.
.pp
The order that bit-fields are placed in the storage unit is
implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers place bit-fields starting at the low-order end
(least significant bit)
of the storage unit. If a 1-bit bit-field is placed alone in an
.kw unsigned int
then a value of 1 in the bit-field corresponds to a value of 1
in the integer.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler places bit-fields starting at the
high-order end
(most significant bit)
of the storage unit.
If a 1-bit bit-field is placed alone in an
.kw unsigned int
then a value of 1 in the bit-field corresponds to a value of
.mono 0x80000000
in the integer.
.shade end
..do end
.*
.************************************************************************
.*
.keep begin
.pp
Consider the following structure definition:
.millust begin
struct list_el {
    struct list_el * link;
    unsigned short   elnum;
    unsigned int     length    : 3;
    signed int       offset    : 4;
    int              flag      : 1;
    char *           text;
};
.millust end
.keep end
.cp 5
.pc
The structure
.mono list_el
contains the following members:
.autonote
.note
.mono link
is a pointer to a
.mono list_el
structure, indicating that instances of this structure
will probably be used in a linked list,
.note
.mono elnum
is an unsigned short integer,
.note
.mono length
is an unsigned bit-field containing 3 bits, allowing values in the range
0 through 7,
.note
.mono offset
is a signed bit-field containing 4 bits, which
will be placed in the
same
integer with
.mono length.
Since the type is
.kw signed int
..ct ,
the range of values for this bit-field is &MINUS.8 through 7,
.note
.mono flag
is a 1-bit field,
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
Since the type is
.kw int
..ct , the &wcboth. compilers will treat the bit
as a sign bit, and the set of values for the
bit-field is &MINUS.1 and 0.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
Although the type is
.kw int
..ct , the &wlooc. compiler will not treat the bit
as a sign bit, and the set of values for the
bit-field is 0 and 1.
.shade end
..do end
.*
.************************************************************************
.*
.note
.mono text
is a pointer to character, possibly a string.
.endnote
.endlevel
