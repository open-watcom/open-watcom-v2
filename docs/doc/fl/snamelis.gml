.section NAMELIST Statement
.*
.mext begin
      NAMELIST /name/ vlist [[,]/name/ vlist] ...
.mext end
.synote 7
.mnote name
is the name, enclosed in slashes, of a group of variables.
It may not be the same as a variable or array name.
.mnote vlist
is a list of variable names and array names separated by commas.
.endnote
.np
The
.kw NAMELIST
statement is used to declare a group name for a set of variables so that
they may be read or written with a single namelist-directed
.kw READ
.ct ,
.kw WRITE
.ct , or
.kw PRINT
statement.
.np
The list of variable or array names belonging to a
.kw NAMELIST
name ends with a new
.kw NAMELIST
name enclosed in slashes or with the end of the
.kw NAMELIST
statement.
The same variable name may appear in more than one namelist.
.np
A dummy variable, dummy array name, or allocatable array may not appear in a
.kw NAMELIST
list.
Also, a variable whose type is a user-defined structure may not appear in a
.kw NAMELIST
list.
.np
The
.kw NAMELIST
statement must precede any statement function definitions and all
executable statements.
A
.kw NAMELIST
name must be declared in a
.kw NAMELIST
statement and may be declared only once.
The name may appear only in input/output statements.
The
.kw READ
.ct ,
.kw WRITE
.ct , and
.kw PRINT
statements may be used to transmit data between a file and the
variables specified in a namelist.
.cp 20
.exam begin
      CHARACTER*20 NAME
      CHARACTER*20 STREET
      CHARACTER*15 CITY
      CHARACTER*20 STATE
      CHARACTER*20 COUNTRY
      CHARACTER*10 ZIP_CODE
      INTEGER AGE
      INTEGER MARKS(10)
      NAMELIST /PERSON/ NAME, STREET, CITY, STATE,
     +                  COUNTRY, ZIP_CODE, AGE, MARKS

      OPEN( UNIT=1, FILE='PEOPLE' )
      LOOP
          READ( UNIT=1, FMT=PERSON, END=99 )
          WRITE( UNIT=6, FMT=PERSON )
      ENDLOOP
99    CLOSE( UNIT=1 )
      END
.exam end
.np
The following example shows another form of a namelist-directed
.kw READ
statement.
.cp 15
.exam begin
      CHARACTER*20 NAME
      CHARACTER*20 STREET
      CHARACTER*15 CITY
      CHARACTER*20 STATE
      CHARACTER*20 COUNTRY
      CHARACTER*10 ZIP_CODE
      INTEGER AGE
      INTEGER MARKS(10)
      NAMELIST /PERSON/ NAME, STREET, CITY, STATE,
     +                  COUNTRY, ZIP_CODE, AGE, MARKS

      READ PERSON
      PRINT PERSON
      END
.exam end
.np
The input data must be in a special format.
The first character in each record must be blank.
The second character in the first record of a group of data records
must be an ampersand (&amp) or dollar sign ($) immediately followed
by the
.kw NAMELIST
name.
The
.kw NAMELIST
name must be followed by a blank and must not contain any imbedded
blanks.
This name is followed by data items separated by commas.
The end of a data group is signaled by the character "&amp" or "$",
optionally followed by the string "END".
If the "&amp" character was used to start the group, then it must be
used to end the group.
If the "$" character was used to start the group, then it must be
used to end the group.
.np
The form of the data items in an input record is:
.begnote $break
.note Name = Constant
The name may be a variable name or an array element name.
The constant may be integer, real, complex, logical or character.
Logical constants may be in the form "T" or ".TRUE" and "F" or
".FALSE".
Character constants must be contained within apostrophes.
Subscripts must be of integer type.
.note ArrayName = Set of Constants
The set of constants consists of constants of the type integer, real,
complex, logical or character.
The constants are separated by commas.
The number of constants must be less than or equal to the number of
elements in the array.
Successive occurrences of the same constant may be represented in the
form
.id r*constant,
where
.id r
is a non-zero integer constant specifying the number of times the
constant is to occur.
.endnote
.np
The variable and array names specified in the input file must appear
in the
.kw NAMELIST
list, but the order is not important.
A name that has been made equivalent to a name in the input data cannot
be substituted for that name in the
.kw NAMELIST
list.
The list can contain names of items in
.kw COMMON
but must not contain dummy argument names.
.np
Each data record must begin with a blank followed by a complete
variable or array name or constant.
Embedded blanks are not permitted in name or constants.
Trailing blanks after integers and exponents are treated as zeros.
.exam begin
 &amp.PERSON
    NAME = 'John Doe'
    STREET = '22 Main St.' CITY = 'Smallville'
    STATE = 'Texas'        COUNTRY = 'U.S.A.'
    ZIP_CODE = '78910-1203'
    MARKS = 73, 82, 3*78, 89, 2*93, 91, 88
    AGE = 23
 &amp.END
.exam end
.np
The form of the data items in an output record is suitable for input
using a namelist-directed
.kw READ
statement.
.autonote
.note
Output records are written using the ampersand character (&amp), not
the dollar sign ($), although the dollar sign is accepted as an
alternative during input.
That is, the output data is preceded by "&amp.name" and is followed by
"&amp.END".
.note
All variable and array names specified in the
.kw NAMELIST
list and their values are written out, each according to
its type.
.note
Character data is enclosed in apostrophes.
.note
The fields for the data are made large enough to contain all
the significant digits.
.note
The values of a complete array are written out in columns.
.endnote
.np
For more information, see the chapters
entitled :HDREF refid='fformat'. and :HDREF refid='fio'..
