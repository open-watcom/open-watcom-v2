.section ALLOCATE Statement
.*
.mext begin
ALLOCATE (array([l:]u[,[l:]u,...])[,...][,LOCATION=loc])
    or
ALLOCATE (array([l:]u[,[l:]u,...])[,...][,STAT=ierr])
    or
ALLOCATE (char*len)
.mext end
.synote 8
.mnote array
is the name of an allocatable array.
.mnote l
is an integer expression that sets the lower bound of the array
dimension.
.mnote u
is an integer expression that sets the upper bound of the array
dimension.
.mnote char
is the name of an allocatable character variable.
.mnote len
is an integer expression that sets the length of the character
variable.
.mnote LOCATION = loc
.br
.id loc
is an integer expression that specifies the location of the allocated
memory.
.mnote STAT = ierr
.br
.id ierr
is an allocation status specifier.
The integer variable or integer array element
.id ierr
is defined with 0 if the allocation succeeded, 1 if the allocation
failed, and 2 if the array is already allocated.
The
.kw STAT=
specifier may not be used with the
.kw LOCATION=
specifier.
.endnote
.np
Allocatable arrays and character variables may be dynamically
allocated and deallocated at execution time.
An array must have been declared allocatable by specifying its
dimensions using colons only.
No array bounds are specified.
.cp 2
.exam begin
      DIMENSION A(:), B(:,:)
.exam end
.pc
In the above example,
.id A
is declared to be a one-dimensional allocatable array
and
.id B
is declared to be a two-dimensional allocatable array.
.np
A character variable must have been declared allocatable by
specifying its size as (*).
.cp 2
.exam begin
      CHARACTER C*(*)
.exam end
.np
For an allocatable array, the
.kw ALLOCATE
statement establishes the lower and upper bounds of each array
dimension and calculates the amount of memory required for the
array.
.np
For an allocatable character variable, the
.kw ALLOCATE
statement establishes the number of characters in the character
variable and thus the size of the character variable.
.np
If there is no
.kw LOCATION=
specifier, it then attempts to dynamically allocate memory for the
array or character variable.
The success of the allocation can be checked by using the
.kw STAT=
specifier.
.np
If there is a
.kw LOCATION=
specifier, the expression in the specification is evaluated and that
value is used as the address of the array or character variable.
This permits the programmer to specify a substitute memory allocator
or to map the array or character variable onto a fixed memory
location.
.cp 7
.exam begin
      DIMENSION A(:), B(:,:)
          .
          .
          .
      ALLOCATE( A(N) )
      ALLOCATE( B(0:4,5) )
.exam end
.np
More than one allocatable array or character variable may appear in an
.kw ALLOCATE
statement, separated by commas.
.cp 6
.exam begin
      DIMENSION A(:), B(:,:)
          .
          .
          .
      ALLOCATE( A(N), B(0:4,5) )
.exam end
.np
If the allocation fails and the
.kw STAT=
specifier was not used, an execution-time error occurs.
If the
.kw STAT=
specifier is used, the specified variable returns a zero value if the
allocation succeeded, and a non-zero value if the allocation failed.
.cp 7
.exam begin
      DIMENSION A(:), B(:,:)
          .
          .
          .
      ALLOCATE( A(N), B(0:4,5), STAT=IALLOC )
      IF( IALLOC .NE. 0 ) PRINT *, 'Allocation failure'
.exam end
.np
An attempt to allocate a previously allocated array or character
variable results in an execution-time error.
If the
.kw LOCATION=
specifier was not used, the array or character variable must be
deallocated first before it can be allocated a second time (see the
.kw DEALLOCATE
statement).
.np
An absolute memory location may be specified using the
.kw LOCATION=
specifier.
.cp 9
.exam begin
      CHARACTER*1 SCREEN(:,:)
      N = 80*25
*$IFDEF __386__
      ALLOCATE( SCREEN(0:1,0:N-1), LOCATION='B8000'x )
*$ELSE
      ALLOCATE( SCREEN(0:1,0:N-1), LOCATION='B8000000'x )
*$ENDIF
      DO I = 0, N-1
          SCREEN(0,I) = '*'
      ENDDO
      END
.exam end
.pc
The above example maps the array
.id SCREEN
onto the IBM PC colour monitor screen memory and then fills the screen
with asterisks (16-bit real-mode only).
The character is stored in
.id SCREEN(0,I)
and the character attribute (unchanged in this example) is stored in
.id SCREEN(1,I).
The column major ordering of arrays must be taken into consideration
when mapping an array onto a fixed area of memory.
.np
The following example is similar but uses an allocatable character
variable.
.cp 9
.exam begin
      CHARACTER*(*) SCREEN
      INTEGER SCRSIZE, I
      PARAMETER ( SCRSIZE = 80*25*2 )
*$IFDEF __386__
      ALLOCATE( SCREEN*SCRSIZE, LOCATION='B8000'X )
*$ELSE
      ALLOCATE( SCREEN*SCRSIZE, LOCATION='B8000000'X )
*$ENDIF
      DO I = 1, SCRSIZE, 2
          SCREEN(I:I) = '*'
      ENDDO
      END
.exam end
.np
A user-defined memory allocator may be specified using the
.kw LOCATION=
specifier.
.cp 9
.exam begin
      CHARACTER*1 BUFFER(:)
      N = 128
      ALLOCATE( BUFFER(0:N-1), LOCATION=MYALLOC(N) )
          .
          .
          .
      END
.exam end
.pc
Perhaps a better way to check for a successful allocation, in this
case, would be the following.
.cp 9
.exam begin
      CHARACTER*1 BUFFER(:)
      N = 128
      LOC = MYALLOC( N )
      IF( LOC .EQ. 0 ) STOP
      ALLOCATE( BUFFER(0:N-1), LOCATION=LOC )
          .
          .
          .
      END
.exam end
.np
For more information on arrays, see the chapter
entitled :HDREF refid='farrays'..
