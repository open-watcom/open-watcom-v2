.section STRUCTURE Statement
.*
.mext begin
      STRUCTURE /typename/
.mext end
.synote 10
.mnote typename
is the name for a new, compound variable, data type.
.endnote
.np
The
.kw STRUCTURE
statement is used in conjunction with the
.kw END STRUCTURE
declarative statement.
The
.kw STRUCTURE
statement marks the start of a structure definition.
.np
The
.kw STRUCTURE
statement defines a new variable type, called a
.us structure.
It does not declare a specific program variable.
The
.kw RECORD
statement is used to declare variables and arrays to be of this
particular structure type.
.np
Structures may be composed of simple FORTRAN types or more complex
structure types.
This is shown in the following example.
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

      RECORD /PEOPLE/ CUSTOMER
.exam end
.np
Element names are local to the structure in which they appear.
The same element name can appear in more than one structure.
Nested structures may have elements with the same name.
A particular element is specified by listing the sequence of
elements required to reach the desired element, separated by
percent symbols (%) or periods (.).
.exam begin
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
