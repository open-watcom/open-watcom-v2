.ix 'union'
.pp
A
.us union
is similar to a structure, except that each member of a union
is placed starting at the same storage location, rather than
in sequentially
higher storage locations.
(The Pascal term for a union is "variant record".)
.pp
The
.ix 'name space' 'unions'
.us name space
for union tags is different from that of object names, labels and
member names, so a tag may be the same identifier
as one of these other kinds.
The tag may not be the same identifier as the tag of a structure,
enumeration
or another union.
.pp
Each union has its own
.ix 'name space' 'union members'
name space, so an identifier may be
used as a member name in several different unions.
An identifier that is an object name, structure tag, structure member name,
union tag, enumeration
tag or label may also be used as a member name without ambiguity.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
unions, like structures, may contain unnamed members that are structures
or unions.
References to the members of an unnamed structure or union are made as
if the members of the inner structure or union were at the outer level.
.* .pp
.* Unnamed union members may not be portable to other compilers,
.* because they are not required by the ISO standard.
..do end
.shade end
.*
.************************************************************************
.*
.pp
The size of a union is the size of the largest of the members
it contains.
.pp
A pointer to an object that is a union points to each of the members
of the union. If one or more of the members of the union is a
.ix 'bit-field'
.us bit-field
..ct ,
then a pointer to the object also points to the storage unit in which
the bit-field resides.
.pp
Storing a value in one member of a union, and then referring to it via
another member is only meaningful when the different members have
the same type. Members of a union may themselves be structures, and
if some or all of the members start with the same members in each
structure, then references to those structure members may be
made via any of the union members.
For example, consider the following structure and union definitions:
.millust begin
struct rec1 {
    int       rectype;
    int       v1,v2,v3;
    char *    text;
};

.millust break
struct rec2 {
    int       rectype;
    short int flags : 8;
    enum      {red, blue, green} hue;
};

.millust break
union alt_rec {
    struct rec1   val1;
    struct rec2   val2;
};
.millust end
.pc
.mono alt_rec
is a union defining two members
.mono val1
and
.mono val2
..ct ,
which are two different forms of a record, namely the structures
.mono rec1
and
.mono rec2
respectively.
Each of the different record forms starts with the member
.mono rectype.
The following program fragment would be valid:
.millust begin
union alt_rec  record;
/* ... */
record.rec1.rectype = 33;
DoSomething( record.rec2.rectype );
.millust end
.pc
However, the following fragment would exhibit implementation-defined
behavior:
.millust begin
record.rec1.v1 = 27;
DoSomethingElse( record.rec2.hue );
.millust end
.pp
In other words, unless several members of a union are themselves
structures where the first
few
members are of the same type,
a program should not store into a union member and retrieve a
value using another union member.
Generally, a flag or other indicator is kept to describe which member
of the union is currently the "active" member.
