.section VOLATILE Statement
.*
.mbox begin
      VOLATILE [a [,a] ...]
.mbox end
.synote
.mnote a
is a variable name or an array name.
.endnote
.np
The
.kw VOLATILE
statement is used to indicate that a variable or an element of an
array may be updated concurrently by other code.
A volatile variable or array element will not be cached (in a
register) by the code generator.
Each time a volatile variable or array element is referenced, it is
loaded from memory.
Each time a volatile variable or array element is updated, it is
stored back into memory.
.autonote Notes:
.setptnt 0 5
.note
A name cannot appear in a
.kw VOLATILE
statement more than once in the same program unit.
.note
Dummy arguments, procedure names, and common block names are not
permitted in a
.kw VOLATILE
statement.
.endnote
.np
In the following example, the subroutine
.id A_THREAD
waits on the
.id HoldThreads
semaphore.
It uses the
.kw VOLATILE
statement to ensure that the variable is re-loaded from memory each
time through the loop.
.exam begin
      SUBROUTINE A_THREAD()

      STRUCTURE /RTL_CRITICAL_SECTION/
          INTEGER*4 DebugInfo
          INTEGER*4 LockCount
          INTEGER*4 RecursionCount
          INTEGER*4 OwningThread
          INTEGER*4 LockSemaphore
          INTEGER*4 Reserved
      END STRUCTURE
.exam break
      INTEGER NumThreads
      LOGICAL HoldThreads
      VOLATILE HoldThreads
      RECORD /RTL_CRITICAL_SECTION/ CriticalSection
      COMMON NumThreads, HoldThreads, CriticalSection
      INTEGER threadid

.exam break
      WHILE( HoldThreads )DO
          CALL Sleep( 1 )
      END WHILE
      PRINT '(''Hi from thread '', i4)', threadid()
      CALL EnterCriticalSection( CriticalSection )
      NumThreads = NumThreads - 1
      CALL LeaveCriticalSection( CriticalSection )
      CALL endthread()
      END
.exam end
