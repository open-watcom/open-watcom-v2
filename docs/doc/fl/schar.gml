.section Standard CHARACTER Statement
.*
.mbox begin
      CHARACTER[*len [,]] name [,name] ...
.mbox end
.synote 6
.mnote name
is one of the following forms:
.np
.mono v[*len]
.np
.mono a(d)[*len]
.np
.xt on
.mono a[*len](d)
.xt off
.mnote v
is a variable name, symbolic name of a constant, function name or
dummy procedure name.
.mnote a
is an array name.
.mnote (d)
is that part of the array declarator defining the dimensions of the
array.
.mnote len
is called the
.us length specification
.ix 'length specification'
and is the length
(number of characters) of a character variable, character array
element, symbolic character constant or character function.
It has one of the following forms:
.begnote
.note (1)
An unsigned positive integer constant.
.note (2)
A positive integer constant expression enclosed in parentheses.
.note (3)
An asterisk in parentheses
.mono (*).
.endnote
.endnote
.np
The length specification immediately following the word
.mono CHARACTER
is the length specification for each entity in the statement not having
its own length specification.
If omitted, the default is 1.
An entity with its own length specification overrides
the default length specification or the length specification immediately
following the word
.mono CHARACTER.
Note that for an array the length specification applies to
.us each
element of the array.
.exam begin
      DIMENSION C(-5:5)
      CHARACTER A, B*10(10), C*20
      CHARACTER*7 X, Y, Z*4
.exam end
.np
The
.mono (*)
length specification is only allowed for external functions,
dummy arguments or symbolic character constants.
If a dummy argument has a length specification of
.mono (*),
it assumes the length of the corresponding actual argument.
If the actual argument is an array name, the length assumed by the
dummy argument is the length of an array element of the actual
array.
.exam begin
      SUBROUTINE OUTCHR( STR )
      CHARACTER STR*(*)
      PRINT *, STR
      END
.exam end
.pc
In this example,
.id STR
is a character variable whose length is the length
of the actual argument.
Thus
.id OUTCHR
can be called with a character entity of any length.
.np
If an external function has a length specification of
.mono (*)
declared in
a function subprogram, the function name must appear as the name of a
function in a
.kw FUNCTION
or
.kw ENTRY
statement in the same subprogram.
When the function is called, the function assumes the length
specified in the program unit that called it.
In the following example, when
.id F
is called its length is assumed to be 10.
.cp 14
.exam begin
      CHARACTER*(10) F
          .
          .
          .
      PRINT *, F()
          .
          .
          .
      END

      CHARACTER*(*) FUNCTION F
      F = 'HELLO'
      END
.exam end
.np
The following example is illegal since
.id F
does not appear in a
.kw FUNCTION
or
.kw ENTRY
statement.
.exam begin
* Illegal definition of function F.
      CHARACTER*(*) F
          .
          .
          .
      PRINT *, F()
          .
          .
          .
      END
.exam end
.pc
The length specified for a character function in the program unit that
referenced it must agree with the length specified in the subprogram
that defines the character function.
Note that there is always agreement if the function is defined to
have a length specification of
.mono (*).
.np
If a symbolic name is of type character and has a length specification
of
.mono (*),
it assumes the length of the corresponding character constant
expression in the
.kw PARAMETER
statement.
.np
The length specification of a character statement function or
statement function dummy argument must not be
.mono (*).
