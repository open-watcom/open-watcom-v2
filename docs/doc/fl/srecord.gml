.section RECORD Statement
.*
.mext begin
      RECORD /typename/ name [,name] ...
.mext end
.synote 10
.mnote typename
is the name of a user-defined structure type.
.mnote name
is a variable name, array name, array declarator, function name
or dummy procedure name.
.endnote
.np
The
.kw RECORD
statement is used to assign a structure type to a variable.
.exam begin
      STRUCTURE /ADDRESS/
          CHARACTER*20 STREET
          CHARACTER*15 CITY
          CHARACTER*20 STATE
          CHARACTER*20 COUNTRY
          CHARACTER*10 ZIP_CODE
      END STRUCTURE

      STRUCTURE /PEOPLE/
          CHARACTER*20 NAME
          RECORD /ADDRESS/ ADDR
          INTEGER*2 AGE
      END STRUCTURE

      RECORD /PEOPLE/ CUSTOMER

      CUSTOMER%NAME = 'John Doe'
      CUSTOMER%ADDR%STREET = '22 Main St.'
      CUSTOMER%ADDR%CITY = 'Smallville'
      CUSTOMER%ADDR%STATE = 'Texas'
      CUSTOMER%ADDR%COUNTRY = 'U.S.A.'
      CUSTOMER%ADDR%ZIP_CODE = '78910-1203'
      CUSTOMER%AGE = 23
.exam end
.np
For more information, see the chapter entitled :HDREF refid='frecord'..
