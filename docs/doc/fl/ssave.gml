.section SAVE Statement
.*
.mbox begin
      SAVE [a [,a] ...]
.mbox end
.synote
.mnote a
is a named common block preceded and followed by a slash (/), a
variable name or an array name.
.endnote
.np
The
.kw SAVE
statement is used to retain the value of an entity
after the execution
of a
.kw RETURN
or
.kw END
statement in a subprogram.
Upon re-entry to the subprogram, the entity will have the same
value it had when exit was made from the subprogram.
However, an entity belonging to a common block that has appeared
in a
.kw SAVE
statement may become redefined in another program unit.
.autonote Notes:
.setptnt 0 5
.note
A name cannot appear in a
.kw SAVE
statement more than once in the same program unit.
.note
Dummy arguments, procedure names and names belonging to a common block
are not permitted in a
.kw SAVE
statement.
.note
A
.kw SAVE
statement with no list is identical to a
.kw SAVE
statement containing all allowable names in a program unit.
.note
A common block name appearing in a
.kw SAVE
statement has the same effect of specifying all names belonging to that
common block in the
.kw SAVE
statement.
.note
If a named common block is specified in a
.kw SAVE
statement in a subprogram, it must be specified in a
.kw SAVE
statement in every subprogram in which that common block appears.
Furthermore, upon executing a
.kw RETURN
or
.kw END
statement, the current values of the entities in that common block are
made available to the next program unit executed in which that common
block appears.
.note
If a named common block is specified in a
.kw SAVE
statement in the main program unit,
the current values of the entities in that common block are
made available to every subprogram that specifies that common
block.
In this case, a
.kw SAVE
statement has no effect in the subprogram.
.endnote
.np
In the following example, the subroutine
.id BLKINIT
initializes the entities of the common block
.id BLK
and uses a
.kw SAVE
statement to ensure that their values are made available to
subroutine
.id BLKPRT.
.exam begin
      PROGRAM MAIN
          .
          .
          .
      CALL BLKINIT
      CALL BLKPRT
          .
          .
          .
      END

      SUBROUTINE BLKINIT
      COMMON /BLK/ A,B,C
      SAVE /BLK/
      A = 1.0
      B = 2.0
      C = 3.0
      END

      SUBROUTINE BLKPRT
      COMMON /BLK/ A,B,C
      SAVE /BLK/
      PRINT *, A, B, C
      END
.exam end
