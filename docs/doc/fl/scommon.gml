.section COMMON Statement
.*
.mbox begin
      COMMON [/[cb]/] nlist [[,]/[cb]/ nlist] ...
.mbox end
.synote 7
.mnote cb
is a common block name.
.mnote nlist
is a list of names each separated by a comma.
.endnote
.np
If
.id cb
is omitted, the
.us blank common block
.ix 'common block' 'blank'
.ix 'blank common block'
is assumed.
If specified,
.id cb
is called a
.us named common block.
.ix 'common block' 'named'
.ix 'named common block'
The names appearing in
.id nlist
can be variable names, array names, and array declarators.
Dummy arguments are not allowed in
.id nlist.
.np
The
.kw COMMON
statement allows sharing of blocks of storage between subprograms.
Each name appearing in the
.id nlist
following a common block name
.id cb
is declared to belong to that common block.
A variable or an array name can
belong to only one common block.
A common block name can occur more than once in the same
.kw COMMON
statement as well as in more than one
.kw COMMON
statement.
Lists following successive appearances of the same common block name
in
.kw COMMON
statements are considered a continuation of the list of names belonging
to the common block.
A variable or an array can appear in a
.kw COMMON
statement only once.
.np
Common blocks are defined as follows.
A common block is one consecutive block of storage.
It consists of all the storage sequences of all the entities specified
in all the lists declared to belong to that common block.
The order in which each entity appears in a common block is defined by
the order in which they appear in the lists.
Storage sequences associated to a common block through the
.kw EQUIVALENCE
statement are considered to belong to that common block.
In this way a common block may only be extended beyond the last
storage unit.
The size of a common block is the sum of all the storage sequences of
all the names belonging to that common block plus any storage sequence
which extends the common block through equivalence association.
.np
An
.kw EQUIVALENCE
statement must not cause storage sequences of two different common blocks
to become associated nor should they extend the common block by adding
storage units preceding the first storage unit of the common block.
.exam begin
      DIMENSION A(5)
      COMMON /COMBLK/ A,B(10),C
.exam end
.pc
In this example, the common block
.id COMBLK
contains the array
.id A
followed by the array
.id B
and finally the variable
.id C.
.exam begin
      REAL A,B,C,D
      DIMENSION D(5)
      EQUIVALENCE (B,D)
      COMMON A,B,C
.exam end
.pc
In this example,
.id A,
.id B,
.id C,
and
.id D
belong to the blank common block;
.id A,
.id B,
and
.id C
have been explicitly defined to be in the blank common block
whereas
.id D
has been equivalenced to a variable in common, namely
.id B.
Also note that the
.kw EQUIVALENCE
statement has caused the extension of the common block beyond its
last storage unit.
In this example, array
.id D
has extended the common block by 3 storage units.
.exam begin
* Illegal definition of a common block.
      DIMENSION A(5)
      EQUIVALENCE (A(2),B)
      COMMON /XYZ/ B
.exam end
.pc
This example demonstrates an illegal use of the
.kw COMMON
statement.
.id B
is in the named common block
.id XYZ
since it appeared in a
.kw COMMON
statement.
.id A
is in the common block
.id XYZ
since it was equivalenced to
.id B.
However,
.id A
illegally extends the common block by adding 1 storage unit before the
first storage unit of the common block.
.np
The following outlines the differences between a blank common block
and a named common block.
.begpoint
.point (1)
All named common blocks with the same name in an executable program
must be the same size.
Blank common blocks do not have to be the same size.
.point (2)
Entities in named common blocks can be initialized by using
.kw DATA
statements in block data subprograms; entities in blank common
blocks cannot.
.point (3)
Entities in named common blocks can become undefined after the
execution of a
.kw RETURN
or
.kw END
statement; entities in blank common blocks cannot.
This situation can arise when all subprograms which refer to the named
common block become inactive.
A typical case occurs when program overlays are used.
If the named common block is placed in an overlay, then the entities
in the named common block will become undefined when the overlay is
replaced by another.
Of course, if the named common block is referenced in the main program
then this could never happen.
The main program and any named common blocks referenced in the main
program remain memory-resident until the application terminates.
.np
The
.kw SAVE
statement should be used if entities in named common blocks must
not become undefined.
.endpoint
.np
.xt begin
The FORTRAN 77 standard specifies that a common block cannot contain
both numeric and character data;
&product allows common blocks to contain both numeric and character
data.
.np
The FORTRAN 77 standard specifies that a named common block must be
initialized in a block data subprogram.
&product permits the initialization of named common blocks in other
subprograms.
.xt end
