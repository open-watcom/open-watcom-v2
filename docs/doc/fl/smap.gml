.section MAP Statement
.*
.mext begin
      MAP
.mext end
.np
The
.kw MAP
statement is used in conjunction with the
.kw END MAP
declarative statement.
The
.kw MAP
statement marks the start of a memory mapping structure.
A
.kw MAP
structure must appear within a
.kw UNION
block.
Any number of variables of any type may appear within a memory
map.
At least two
.kw MAP
structures must appear within a
.kw UNION
block.
A
.kw UNION
block permits the mapping of the same storage in several different
ways.
.np
The following example maps out a 4-byte integer on an Intel
80x86-based processor.
.exam begin
      STRUCTURE /MAPINT/
          UNION
              MAP
                  INTEGER*4 LONG
              END MAP
              MAP
                  INTEGER*2 LO_WORD
                  INTEGER*2 HI_WORD
              END MAP
              MAP
                  INTEGER*1 BYTE_0
                  INTEGER*1 BYTE_1
                  INTEGER*1 BYTE_2
                  INTEGER*1 BYTE_3
              END MAP
          END UNION
      END STRUCTURE

      RECORD /MAPINT/ I

      I%LONG = '01020304'x
      PRINT '(2Z4)', I%LO_WORD, I%HI_WORD
      END
.exam end
.np
For more information, see the chapter entitled :HDREF refid='frecord'..
