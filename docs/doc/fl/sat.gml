.section AT END Statement
.*
.mext begin
      AT END DO    [: block-label]

           or

      AT END, stmt
.mext end
.synote 6
.mnote stmt
is an executable statement other than an
.kw AT END
statement.
.endnote
.np
The
.kw AT END
control statement is an extension of the
.kw END=
option of the
.kw READ
statement for sequential files.
It allows a statement or a block of code following the
.kw READ
statement to be executed when an end-of-file condition is
encountered during the read.
The
.kw AT END
statement or block is by-passed if no end-of-file occurs.
It is not valid to use this control statement with direct-access or
internal files.
It is not valid to use this statement when
.kw END=
is also specified in the
.kw READ
statement.
The
.kw AT END
statement or block must immediately follow the
.kw READ
statement to which it applies.
.exam begin
      READ( UNIT=1, FMT='(I5,F10.4)' ) I, X
      AT END DO
          PRINT *, 'END-OF-FILE ENCOUNTERED ON UNIT 1'
          EOFSW = .TRUE.
      END AT END
.exam end
.np
The second form of the
.kw AT END
statement is illustrated below.
.exam begin
      READ( UNIT=1, FMT='(F10.4)' ) X
      AT END, EOFSW = .TRUE.
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
