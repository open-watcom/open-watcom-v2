.section DEALLOCATE Statement
.*
.mext begin
      DEALLOCATE (arraylist [, STAT = ierr])
.mext end
.synote 8
.mnote arraylist
is a list of allocatable array names separated by commas.
.mnote ierr
is an integer variable that returns the status of the attempted
deallocation.
.endnote
.np
Allocatable arrays may be dynamically allocated and deallocated at
execution time.
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
The
.kw DEALLOCATE
statement frees up any memory allocated for the specified array(s).
It then disassociates the specified array(s) from the memory to which
it was associated.
The deallocation does not necessarily succeed.
For example, an attempt to deallocate an array that was not previously
allocated will cause an error.
.cp 10
.exam begin
      DIMENSION A(:), B(:,:)
        .
        .
        .
      ALLOCATE( A(N), B(0:4,5) )
        .
        .
        .
      DEALLOCATE( A )
.exam end
.np
More than one allocatable array may appear in an
.kw DEALLOCATE
statement, separated by commas.
.cp 10
.exam begin
      DIMENSION A(:), B(:,:)
        .
        .
        .
      ALLOCATE( A(N), B(0:4,5) )
        .
        .
        .
      DEALLOCATE( A, B )
.exam end
.np
If the deallocation fails and the
.kw STAT=
specifier was not used, an execution-time error occurs.
If the
.kw STAT=
specifier is used, the specified variable returns a zero value if the
deallocation succeeded, and a non-zero value if the deallocation failed.
.cp 12
.exam begin
      DIMENSION A(:), B(:,:)
        .
        .
        .
      ALLOCATE( A(N), B(0:4,5), STAT=IALLOC )
      IF( IALLOC .NE. 0 ) PRINT *, 'Allocation failure'
        .
        .
        .
      DEALLOCATE( A, B, STAT=IFREE )
      IF( IFREE .NE. 0 ) PRINT *, 'Deallocation failure'
.exam end
.np
An attempt to deallocate an unallocated array results in an
execution-time error.
The array must be allocated first (see the
.kw ALLOCATE
statement).
.np
An array that was allocated using the
.kw LOCATION=
specifier need not be deallocated.
.np
For more information on arrays, see the chapter
entitled :HDREF refid='farrays'..
