.section END AT END Statement
.*
.mext begin
      END AT END
.mext end
.np
The
.kw END AT END
statement is used in conjunction with the structured
.kw AT END
statement.
The
.kw END AT END
statement marks the end of a sequence of statements which are part of
an AT END-block.
The
.kw AT END
statement marks the beginning of the AT END-block.
The AT END-block is executed when the preceding
.kw READ
statement terminates because of an end-of-file condition.
.exam begin
      READ( UNIT=1, FMT='(3I5)' ) I, J, K
      AT END DO
          PRINT *, 'END-OF-FILE ENCOUNTERED ON UNIT 1'
          EOFSW = .TRUE.
      END AT END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='fstruct'..
