.section BLOCK DATA Statement
.*
.mbox begin
      BLOCK DATA [sub]
.mbox end
.synote 5
.mnote sub
is an optional symbolic name of the block data subprogram
and must not be the name of an external procedure, main program,
common block, other block data subprogram, or any local name
in the block data subprogram.
.endnote
.np
The
.kw BLOCK DATA
statement is used to define the start of a block data subprogram.
A block data subprogram is used to provide initial values for
variables and array elements in
.us named common blocks.
.ix 'named common block'
.ix 'common block' 'named'
.np
The only statements which are allowed to appear in a block
data subprogram are:
.autopoint
.point
.kw IMPLICIT
.point
.kw PARAMETER
.point
.kw DIMENSION
.point
.kw COMMON
.point
.kw SAVE
.point
.kw EQUIVALENCE
.point
.kw DATA
.point
.xt on
.kw STRUCTURE, END STRUCTURE
.xt off
.point
.xt on
.kw UNION, END UNION
.xt off
.point
.xt on
.kw MAP, END MAP
.xt off
.point
.xt on
.kw RECORD
.xt off
.point
.kw END
.point
type statements
.endpoint
.exam begin
      BLOCK DATA INITCB
      DIMENSION A(10), B(10)
      COMMON /CB/ A, B
      DATA A/10*1.0/, B/10*2.0/
      END
.exam end
.pc
In the above example, the arrays
.id A
and
.id B
in the named common block
.id CB
are initialized.
.autonote Notes:
.note
More than one named common block may appear in a block data subprogram.
.note
All entities of the named common block(s) must be specified.
.note
Not all entities need be given initial values.
.note
Only entities that appear in (or are associated, through the
.kw EQUIVALENCE
statement, with entries in) a named common block may be given initial
values.
.note
Only one unnamed block data subprogram may occur in an executable
program.
.note
A named block data subprogram may occur only once in an executable
program.
.endnote
