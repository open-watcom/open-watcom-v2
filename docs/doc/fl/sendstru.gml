.section END STRUCTURE Statement
.*
.mext begin
      END STRUCTURE
.mext end
.np
The
.kw END STRUCTURE
statement is used in conjunction with the
.kw STRUCTURE
declarative statement.
The
.kw END STRUCTURE
statement marks the end of a structure definition.
.exam begin
      STRUCTURE /ADDRESS/
          CHARACTER*20 STREET
          CHARACTER*20 CITY
          CHARACTER*20 STATE
          CHARACTER*20 COUNTRY
          CHARACTER*10 ZIP_CODE
      END STRUCTURE

      STRUCTURE /PEOPLE/
          CHARACTER*20 NAME
          RECORD /ADDRESS/ ADDR
          INTEGER*2 AGE
      END STRUCTURE
.exam end
.np
For more information, see the chapter entitled :HDREF refid='frecord'..
