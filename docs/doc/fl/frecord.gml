.chap *refid=frecord Structures, Unions and Records
.*
.if &e'&dohelp eq 1 .do begin
.np
The following sections describe support for composite data types.
.do end
.*
.section Structures and Records
.*
.np
.ix 'structure'
As an extension to the basic FORTRAN 77 types such as INTEGER, REAL,
LOGICAL, etc., &product supports the creation of hierarchical,
composite data types called
.us structures.
.ix 'record'
A structure is a template describing the form of a
.us record.
It is composed of members or fields of various types, including other
structures.
A structure does not reserve any storage.
.np
For example, you could describe the structure of the COMPLEX data type
using the following construction.
.exam begin
      STRUCTURE /CMPLX/
          REAL REAL_PART
          REAL IMAG_PART
      END STRUCTURE
.exam end
.pc
Since the COMPLEX data type is an intrinsic type of FORTRAN, there is
no need to do so.
The
.kw STRUCTURE
and
.kw END STRUCTURE
statements mark the start and end of a structure definition.
.np
There are, however, many practical examples of collections of data
that may be described using a structure.
Consider, for example, the contents of a data record on disk.
It may contain fields such as last name, first name, and middle
initial which describe the name of a customer.
Each of these fields are fixed in length.
A sample structure declaration might be:
.millust begin
      STRUCTURE /NAME/
          CHARACTER*20 LAST_NAME
          CHARACTER*20 FIRST_NAME
          CHARACTER*1  MIDDLE_INITIAL
      END STRUCTURE
.millust end
.pc
As we stated above, a structure does not allocate storage.
Instead, we have created a new type called
.id NAME
which may be used to describe objects.
Objects of the new type are defined using the
.kw RECORD
statement.
For example, the following statements describe two objects,
.id STUDENT_1
and
.id STUDENT_2
.ct , to be of type
.id NAME.
.millust begin
      RECORD /NAME/ STUDENT_1
      RECORD /NAME/ STUDENT_2
.millust end
.np
There are other attributes of a person besides one's name that could
be recorded in the record.
For example, we can also store a person's date of birth and sex.
First, let us define a
.id DATE
structure.
.millust begin
      STRUCTURE /DATE/
          INTEGER*1 DAY
          INTEGER*1 MONTH
          INTEGER*2 YEAR
      END STRUCTURE
.millust end
.pc
Now we can describe a person in terms of name, date of birth, and sex.
.millust begin
      STRUCTURE /PERSON/
          RECORD /NAME/ NAME
          RECORD /DATE/ BIRTH_DATE
          CHARACTER*1 SEX
      END STRUCTURE

      RECORD /PERSON/ STUDENT
.millust end
.np
Having declared
.id STUDENT
to be of type
.id PERSON
.ct , how do we reference the component parts of
.id STUDENT
.ct ?
The following example illustrates this.
.millust begin
      STUDENT.NAME.LAST_NAME = 'Pugsley'
      STUDENT.NAME.FIRST_NAME = 'Elmar'
      STUDENT.NAME.MIDDLE_INITIAL = 'M'
      STUDENT.BIRTH_DATE.DAY = 21
      STUDENT.BIRTH_DATE.MONTH = 11
      STUDENT.BIRTH_DATE.YEAR = 1959
      STUDENT.SEX = 'M'
.millust end
.pc
The object's name is specified first, followed by a "." (or "%") and
the structure member name.
If the structure member is itself a record then another "." (or "%")
and member name is specified.
This continues until the desired structure member is identified.
The "." or "%" is called a
.us field selection operator.
.np
The previous example contained both a structure called
.id NAME (RECORD /NAME/)
and a structure member called
.id NAME (RECORD /NAME/ NAME).
The structure name is enclosed within slashes ("/").
A structure name must be unique among structure names.
However, the same name can also be used to name either variables or
structure members (fields).
Thus it is possible to have a variable named
.id X,
a structure named
.id X,
and one or more fields named
.id X.
.np
Structure, field, and variable names are all local to the program unit
in which they are defined.
.*
.section Arrays of Records
.*
.np
It is often the case that the individual attributes of objects are
stored in separate arrays.
If, for example, your application deals with 1000 objects with
attributes "size", "weight", and "colour", the traditional approach is
to declare three different arrays.
.millust begin
      PARAMETER (MAX_ELS=1000)

      REAL        SIZE(MAX_ELS)
      INTEGER     WEIGHT(MAX_ELS)
      CHARACTER*2 COLOUR(MAX_ELS)
.millust end
.pc
To read or write the attributes relating to an object, you would
use a statement such as:
.millust begin
      READ(UNIT=3) SIZE(I), WEIGHT(I), COLOUR(I)
.millust end
.np
Using a simple structure, we can express the problem as follows:
.millust begin
      PARAMETER (MAX_ELS=1000)

      STRUCTURE /OBJECT/
          REAL        SIZE
          INTEGER     WEIGHT
          CHARACTER*2 COLOUR
      END STRUCTURE

      RECORD /OBJECT/ ITEM(MAX_ELS)
.millust end
.pc
To read or write the attributes relating to an object, you would
use a statement such as:
.millust begin
      READ(UNIT=3) ITEM(I)
.millust end
.*
.section Unions
.*
.np
Sometimes it is useful to be able to describe parts of structures
in different ways in much the same way that the
.kw EQUIVALENCE
statement is used to describe a specific storage area in different
ways.
The
.kw UNION
-
.kw END UNION
statements are used to mark a section of a structure that will
have alternate storage organizations (MAPs).
The
.kw MAP
-
.kw END MAP
statements are used to define the start and end of an alternate
storage map.
Thus several
.kw MAP
-
.kw END MAP
pairs will appear between a
.kw UNION
-
.kw END UNION
section.
.np
Consider the following example.
The subroutine displays the contents of a field using different
names and formats depending on a
.id TYPE
field.
.exam begin
      SUBROUTINE PRINT_ITEM( ITEM )
          STRUCTURE /DATA_MAP/
              INTEGER TYPE
              UNION
                  MAP
                      LOGICAL           LGL
                  END MAP
                  MAP
                      INTEGER           INT
                  END MAP
                  MAP
                      REAL              FLT
                  END MAP
                  MAP
                      DOUBLE PRECISION  DBL
                  END MAP
              END UNION
          END STRUCTURE

          RECORD /DATA_MAP/ ITEM

          IF( ITEM%TYPE .EQ. 1 ) THEN
              PRINT '(L2)', ITEM%LGL
          ELSEIF( ITEM%TYPE .EQ. 2 ) THEN
              PRINT '(I8)', ITEM%INT
          ELSEIF( ITEM%TYPE .EQ. 3 ) THEN
              PRINT '(E12.5)', ITEM%FLT
          ELSEIF( ITEM%TYPE .EQ. 4 ) THEN
              PRINT '(D12.5)', ITEM%DBL
          ENDIF
      END
.exam end
.np
The organization of the record in memory is as follows:
.millust begin
offset  +0          +4          +8
.* .bx on 7 17 27 37
.sr c0=&INDlvl+11
.sr c1=&INDlvl+21
.sr c2=&INDlvl+31
.sr c3=&INDlvl+41
.bx on &c0 &c1 &c2 &c3
          integer     logical     (slack)
.* .bx 17 27 37
.bx &c1 &c2 &c3
                      integer     (slack)
.bx
                      real        (slack)
.* .bx 17 37
.bx &c1 &c3
                      double precision
.bx off
.millust end
.pc
The first 4 bytes of storage are occupied by
.id TYPE.
The next 4 to 8 bytes of storage are occupied by either
.id LGL,
.id INT,
.id FLT,
or
.id DBL
depending on the interpretation of the contents of the variable
.id TYPE.
The size of the record
.id ITEM
is a total of 12 bytes.
Based on the conventions of the above program example, only 8 bytes of
the record
.id ITEM
are used when
.id TYPE
is 1, 2, or 3.
When
.id TYPE
is 4 then 12 bytes of the record are used.
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
      PRINT '(Z8)', I%LONG
      PRINT '(Z4,1X,Z4)', I%LO_WORD, I%HI_WORD
      PRINT '(Z2,3(1X,Z2))', I%BYTE_0, I%BYTE_1,
     $                       I%BYTE_2, I%BYTE_3
      END
.exam end
.np
The above example produces the following output:
.millust begin
01020304
0304 0102
04 03 02 01
.millust end
