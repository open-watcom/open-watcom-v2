.ix 'type' 'enumerated'
.ix 'enumerated type'
.pp
Sometimes it is desirable to have a list of constant
values representing different
things, and the exact values are not relevant.
They may need to be unique or may have duplicates.
For example, a set of actions, colors or keys might be represented
in such a list.
An
.ix 'enumerated type'
.us enumerated type
allows the creation of a list of items.
.pp
An
.ix 'enumerated type'
.us enumerated type
is a set of identifiers that correspond to
constants of type
.kw int
..ct ..li .
These identifiers are called
.ix 'enumeration constant'
.us enumeration constants.
The first identifier in the set has the value
0, and subsequent identifiers
are given the previous value plus one. Wherever a constant
of type
.kw int
is allowed, an enumeration constant may be specified.
.keep begin
.pp
The following type specifier defines the set of actions available in a
simple memo program:
.millust begin
enum actions { DISPLAY, EDIT, PURGE };
.millust end
.keep end
.pc
The enumeration constant
.mono DISPLAY
is equivalent to the integer constant 0, and
.mono EDIT
and
.mono PURGE
are equivalent to 1 and 2 respectively.
.pp
An enumerated type may be given an optional
.ix 'tag'
.us tag
(name) with which it may be identified elsewhere in the program.
In the example above, the tag of the enumerated type is
.mono actions
..ct ,
which becomes a
.ix 'type' 'new'
.ix 'type' 'defining'
.ix 'new type'
.ix 'defining a type'
new type.
If no tag is given, then only those objects listed following
the definition of the type may have the enumerated type.
.pp
The
.ix 'name space' 'enumeration'
.us name space
for enumerated type
tags is different from that of object names, labels and
member names of structures and unions,
so a tag may be the same identifier
as one of these other kinds.
An enumerated type tag may not be the same as the tag of a structure
or union, or another enumerated type.
.keep begin
.pp
Enumeration constants may be given a specific value
by specifying '
..ct .mono =
..ct '
followed by the value. For example,
.millust begin
enum colors { RED = 1, BLUE = 2, GREEN = 4 };
.millust end
.keep break
.pc
creates the constants
.mono RED, BLUE
and
.mono GREEN
with values
1, 2 and 4 respectively.
.keep break
.millust begin
enum fruits { GRAPE, ORANGE = 6, APPLE, PLUM };
.millust end
.pc
creates constants with values 0, 6, 7 and 8.
.keep break
.millust begin
enum fruits { GRAPE, PLUM, RAISIN = GRAPE, PRUNE = PLUM };
.millust end
.pc
makes
.mono GRAPE
and
.mono RAISIN
equal to 0, and
.mono PLUM
and
.mono PRUNE
equal to 1.
.keep end
.keep begin
.pp
The formal specification of an
.ix 'enumerated type'
.ix 'type' 'enumerated'
enumerated
type is as follows:
.cillust begin
.mono enum
identifier
.cor
.mono enum
:HP0.{:eHP0.
enumeration-constant-list
:HP0.}:eHP0.
.cor
.mono enum
identifier
:HP0.{:eHP0.
enumeration-constant-list
:HP0.}:eHP0.
.keep break
.csk
enumeration-constant-list:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.enumeration-constant
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.enumeration-constant, enumeration-constant-list
.keep break
.csk
enumeration-constant:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.identifier
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.identifier = constant-expression
.cillust end
.keep end
.pp
The type of an enumeration is implementation-defined,
although it must be compatible with an integer type.
Many compilers will use
.mono int
..ct ..li .
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
From the following table,
the
&wc286. and
&wc386. compilers will choose the smallest
type that has sufficient range to represent all of the
constants of a particular enumeration:
..sk 1
..im enumtab
.pc
A command-line option may be used to force all enumerations to
.kw int
..ct ..li .
.shade end
..do end
.*
.************************************************************************
.*
.pp
To create an object with enumerated type, one of two forms may be used.
The first form is to create the type as shown above, and then to
declare an object as follows:
.cillust begin
.mono enum
tag object-name:MSEMI.
.cillust end
.keep begin
.pp
For example, the declaration,
.millust begin
enum fruits fruit;
.millust end
.pc
declares the object
.mono fruit
to be the enumerated type
.mono fruits.
.keep end
.pp
The second form is to list the identifiers of the objects following
the closing brace of the enumeration declaration. For example,
.millust begin
enum fruits { GRAPE, ORANGE, APPLE, PLUM } fruit;
.millust end
.pc
Provided no other objects with the same enumeration
are going to be declared, the enumerated
type tag
.mono fruits
is not required. The declaration could be specified as,
.millust begin
enum { GRAPE, ORANGE, APPLE, PLUM } fruit;
.millust end
.pp
An identifier that is an enumeration constant may only appear in one
enumeration type. For example, the constant
.mono ORANGE
may not be included in another enumeration, because the compiler would
then have two values for
.mono ORANGE.
