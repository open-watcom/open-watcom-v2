.section END UNION Statement
.*
.mext begin
      END UNION
.mext end
.np
The
.kw END UNION
statement is used in conjunction with the
.kw UNION
declarative statement.
The
.kw END UNION
statement marks the end of a series of
.kw MAP
structures.
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
