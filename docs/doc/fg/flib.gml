.chap *refid=flib The &cmpname Subprogram Library
.*
.np
&product includes additional FORTRAN subprograms which can be called
from programs compiled by &cmpname..
The following sections describe these subprograms.
.*
.section Subroutine FEXIT
.*
.np
.ix 'subprograms' 'FEXIT subroutine'
.ix 'utility subprograms' 'FEXIT subroutine'
.ix 'FEXIT subroutine'
.ix 'return code'
.ix 'exiting with return code'
The subroutine
.id FEXIT
allows an application to terminate execution with a return code.
It requires one argument of type INTEGER that represents the value to
be returned to the system.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      CALL FEXIT( -1 )
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.section INTEGER Function FGETCMD
.*
.np
.ix 'subprograms' 'FGETCMD function'
.ix 'utility subprograms' 'FGETCMD function'
.ix 'FGETCMD function'
.ix 'command line'
.ix 'arguments'
The INTEGER function
.id FGETCMD
allows an application to obtain the command line from
within an executing program.
.np
The function
.id FGETCMD
requires one argument of type CHARACTER and returns the length of the
command line.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER CMDLEN
      CHARACTER*128 CMDLIN

      CMDLEN = FGETCMD( CMDLIN )
      PRINT *, 'Command length = ', CMDLEN
      PRINT *, 'Command line   = ', CMDLIN
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
If the argument to
.id FGETCMD
is not long enough then only the first part of the command
line is returned.
.endnote
.*
.section INTEGER Function FGETENV
.*
.np
.ix 'subprograms' 'FGETENV function'
.ix 'utility subprograms' 'FGETENV function'
.ix 'FGETENV function'
The INTEGER function
.id FGETENV
allows an application to obtain the value of an
environment string from within an executing program.
.np
The function
.id FGETENV
requires two arguments of type CHARACTER.
The first argument is the character string to look for.
.id FGETENV
places the associated environment string value in the second argument
and returns the length of the environment string.
If no such string is defined, the length returned is zero.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER STRLEN
      CHARACTER*80 STRVAL

      STRLEN = FGETENV( 'PATH', STRVAL )
      PRINT *, 'Environment string length = ', STRLEN
      PRINT *, 'Environment string value  = ', STRVAL
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
If the second argument to
.id FGETENV
is not long enough then only the first part of the value
is returned.
.endnote
.*
.section INTEGER Function FILESIZE
.*
.np
.ix 'subprograms' 'FILESIZE'
.ix 'utility subprograms' 'FILESIZE'
.ix 'FILESIZE'
The INTEGER function
.id FILESIZE
allows an application to determine the size of a file connected to a
specified unit.
.np
The function
.id FILESIZE
requires one argument of type INTEGER, the unit number and returns the
size, in bytes, of the file.
If no file is connected to the specified unit, a value of -1 is
returned.
.exam begin
      INCLUDE 'FSUBLIB.FI'

      OPEN( UNIT=1, FILE=':FNAME.sample.fil:eFNAME.' )
      PRINT *, FILESIZE( 1 )
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.section Subroutine FINTR
.*
.np
.ix 'assembler subprograms' 'subroutine FINTR'
.ix 'utility subprograms' 'subroutine FINTR'
.ix 'FINTR subroutine'
The subroutine
.id FINTR
allows the user to execute any software interrupt from a
FORTRAN 77 program.
.remark
This subroutine is only supported by the DOS and Windows libraries.
.eremark
.np
The subroutine
.id FINTR
requires two arguments.
.autopoint
.point
The first argument is an interrupt number.
The subroutine
.id FINTR
will generate the software interrupt given by the first argument.
The type of this argument must be
.id INTEGER.
.point
The second argument is an
.id INTEGER
array of ten elements.
.endpoint
.np
When
.id FINTR
is called, the array contains the values to be assigned to the
registers prior to issuing the software interrupt.
When control is returned from
.id FINTR,
it contains the values of the registers after the software interrupt
has completed.
The registers are mapped onto the array
.id REGS
as follows.
.millust begin
            31      0
REGS(1)         EAX
REGS(2)         EBX
REGS(3)         ECX
REGS(4)         EDX
REGS(5)         EBP
REGS(6)         ESI
REGS(7)         EDI
REGS(8)       FS | DS
REGS(9)       GS | ES
REGS(10)      eflags
.millust end
.np
For 16-bit systems (e.g., 8088, 8086, 186, 286), only the low-order
16 bits of each register contain meaningful results.
.millust begin
            31      0
REGS(1)           AX
REGS(2)           BX
REGS(3)           CX
REGS(4)           DX
REGS(5)           BP
REGS(6)           SI
REGS(7)           DI
REGS(8)           DS
REGS(9)           ES
REGS(10)       flags
.millust end
.np
The file :FNAME.dos&hxt:eFNAME., located in the
:FNAME.&pathnamup.&pc.src&pc.fortran&pc.dos:eFNAME.
directory, defines a set of equivalences for
ease of use.
The contents of this file are reproduced below.
.code begin
* Define registers: These correspond to the element of an
* array which is to contain the values of the registers.

      integer*4 regd(10), regs(10)
      integer*2 regw(2*10)
      integer*1 regb(4*4)

      integer*4 EAX,EBX,ECX,EDX,EBP,EDI,ESI,EFLAGS
      integer*2 AX,BX,CX,DX,BP,DI,SI,DS,ES,FS,GS,FLAGS
      integer*1 AH,AL,BH,BL,CH,CL,DH,DL
      equivalence (regd,regs),(regd,regw),(regd,regb),
     1(EAX,regd(1)), (EBX,regd(2)), (ECX,regd(3)), (EDX,regd(4)),
     2(EBP,regd(5)), (EDI,regd(6)), (ESI,regd(7)), (EFLAGS,regd(10)),
     3(AX,regw(1)),  (BX,regw(3)),  (CX,regw(5)),  (DX,regw(7)),
     4(BP,regw(9)),  (DI,regw(11)), (SI,regw(13)), (DS,regw(15)),
     5(FS,regw(16)), (ES,regw(17)), (GS,regw(18)), (FLAGS,regw(19)),
     6(AL,regb(1)),  (AH,regb(2)),  (BL,regb(5)),  (BH,regb(6)),
     7(CL,regb(9)),  (CH,regb(10)), (DL,regb(13)), (DH,regb(14))
.code end
.np
The following is extracted from the "CALENDAR" program.
It demonstrates the use of the
.id FINTR
subroutine.
.code begin
      subroutine ClearScreen()
*$noextensions
      implicit none

      include 'dos&hxt'

* Define BIOS functions.

      integer VIDEO_CALL, SCROLL_UP
      parameter (VIDEO_CALL=16, SCROLL_UP=6)

      DS = ES = FS = GS = 0
      AH = SCROLL_UP            ! scroll up
      AL = 0                    ! blank entire window
      CX = 0                    ! set row,column of upper left
      DX = 24*256 + 80          ! set row,column of lower right
      BH = 7                    ! attribute "white on black"
      call fintr( VIDEO_CALL, regs )
      end
.code end
.*
.section INTEGER Function FLUSHUNIT
.*
.np
.ix 'subprograms' 'FLUSHUNIT function'
.ix 'utility subprograms' 'FLUSHUNIT function'
.ix 'FLUSHUNIT function'
The INTEGER function
.id FLUSHUNIT
flushes the internal input/output buffer for a specified unit.
Each file, except special devices such as :FNAME.con:eFNAME., has an
internal buffer.
Buffered input/output is much more efficient since it reduces the
number of system calls which are usually quite expensive.
For example, many
.kw WRITE
operations may be required before filling the internal
file buffer and data is physically transferred to the file.
.np
This function is particularly useful for applications that call
non-FORTRAN subroutines or functions that wish to perform input/output
to a FORTRAN file.
.np
The function
.id FLUSHUNIT
requires one argument, the unit number, of type INTEGER.
It returns an INTEGER value representing the return code of the
input/output operation.
A return value of 0 indicates success; otherwise an error occurred.
.np
The following example will flush the contents of the internal
input/output buffer for unit 7.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER ISTAT

      ISTAT = FLUSHUNIT( 7 )
      IF( ISTAT .NE. 0 )THEN
          PRINT *, 'Error in FLUSHUNIT'
      END IF

      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.section INTEGER Function FNEXTRECL
.*
.np
.ix 'subprograms' 'FNEXTRECL function'
.ix 'utility subprograms' 'FNEXTRECL function'
.ix 'FNEXTRECL function'
The INTEGER function
.id FNEXTRECL
reports the record length of the next unformatted record to be read
sequentially from the specified unit.
.np
The function
.id FNEXTRECL
requires one argument, the unit number, of type INTEGER.
It returns an INTEGER value representing the size of the next record
to be read.
.np
The following example creates an unformatted file and then reads the
records in the file sequentially.
.exam begin
      INCLUDE 'FSUBLIB.FI'

      CHARACTER*80 INPUT

      OPEN(UNIT=2, FILE='UNFORM.TXT', FORM='UNFORMATTED',
     & ACCESS='SEQUENTIAL' )
      WRITE( UNIT=2 ) 'A somewhat longish first record'
      WRITE( UNIT=2 ) 'A short second record'
      WRITE( UNIT=2 ) 'A very, very much longer third record'
      CLOSE( UNIT=2 )

      OPEN(UNIT=2, FILE='UNFORM.TXT', FORM='UNFORMATTED',
     & ACCESS='SEQUENTIAL' )

      I = FNEXTRECL( 2 )
      PRINT *, 'Record length=', I
      READ( UNIT=2 ) INPUT(1:I)
      PRINT *, INPUT(1:I)

      I = FNEXTRECL( 2 )
      PRINT *, 'Record length=', I
      READ( UNIT=2 ) INPUT(1:I)
      PRINT *, INPUT(1:I)

      I = FNEXTRECL( 2 )
      PRINT *, 'Record length=', I
      READ( UNIT=2 ) INPUT(1:I)
      PRINT *, INPUT(1:I)
      CLOSE( UNIT=2 )
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section INTEGER Function FSIGNAL
.*
.np
.ix 'subprograms' 'FSIGNAL function'
.ix 'utility subprograms' 'FSIGNAL function'
.ix 'FSIGNAL function'
The INTEGER function
.id FSIGNAL
allows your application to respond to certain events that occur during
execution.
.begnote $setptnt 12
:DTHD.Event
:DDHD.Meaning
.note SIGBREAK
.ix 'Ctrl/Break'
an interactive attention (Ctrl/Break on keyboard) is signalled
.note SIGFPE
an erroneous floating-point operation occurs (such as division by
zero, overflow and underflow)
.note SIGILL
illegal instruction encountered
.note SIGINT
an interactive attention (Ctrl/C on keyboard) is signalled
.note SIGSEGV
an illegal memory reference is detected
.note SIGTERM
a termination request is sent to the program
.note SIGIDIVZ
integer division by zero
.note SIGIOVFL
integer overflow
.endnote
.np
The function
.id FSIGNAL
requires two arguments.
The first argument is an INTEGER argument and must be one of the events
described above.
The second argument, called the handler, is one of the following.
.autopoint
.point
a subprogram that is called when the event occurs
.point
the value SIG_DFL, causing the default action to be taken when the event
occurs
.point
the value SIG_IGN, causing the event to be ignored
.endpoint
.np
.id FSIGNAL
returns SIG_ERR if the request could not be processed, or the previous
event handler.
.exam begin
      INCLUDE 'FSIGNAL.FI'

      EXTERNAL BREAK_HANDLER
      LOGICAL BREAK_FLAG
      COMMON BREAK_FLAG
      BREAK_FLAG = .FALSE.
      CALL FSIGNAL( SIGBREAK, BREAK_HANDLER )
      WHILE( .NOT. VOLATILE( BREAK_FLAG ) ) CONTINUE
      PRINT *, 'Program Interrupted'
      END

      SUBROUTINE BREAK_HANDLER()
      LOGICAL BREAK_FLAG
      COMMON BREAK_FLAG
      BREAK_FLAG = .TRUE.
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsignal.fi:eFNAME. contains typing and
calling information for
.id FSIGNAL
and should be included when using this function.
This file is located in the :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME.
directory.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
The intrinsic function
.id VOLATILE
is used to indicate that the reference to the variable
.id break_flag
is volatile.
A volatile reference prevents the compiler from caching a variable in
a register.
In this case, we want to retrieve the value of
.id break_flag
from memory each time the loop is iterated.
.endnote
.*
.do end
.*
.section INTEGER Function FSPAWN
.*
.np
.ix 'subprograms' 'FSPAWN function'
.ix 'utility subprograms' 'FSPAWN function'
.ix 'FSPAWN function'
The INTEGER function
.id FSPAWN
allows an application to run another program as a
subprocess.
When the program completes, execution is returned to the
invoking application.
There must be enough available free memory to start the subprocess.
.np
The function
.id FSPAWN
requires two arguments of type CHARACTER.
The first argument is a character string representing the name of
the program to be run.
The string must end in a NULL character (i.e.,
a character with the binary value 0).
.np
The second argument is a character string argument list to be passed
to the program.
The first character of the second argument must
contain, in binary, the length of the remainder of the argument list.
For example, if the argument is the string "HELLO" then
the first character would be CHAR(5) and the remaining characters
would be "HELLO" (see the example below).
.np
.id FSPAWN
returns an INTEGER value representing the status of
subprocess execution.
If the value is negative then the program could not be run.
If the value is positive then the value represents the program's
return code.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER CMDLEN, STATUS
      CHARACTER CMD*128, CMDLIN*128

* COMSPEC will tell us where DOS 'COMMAND.COM' is hiding
      CMDLEN = FGETENV( 'COMSPEC', CMD )
      CMD(CMDLEN+1:CMDLEN+1) = CHAR( 0 )

      CMDLIN = '/c dir *.for'
      CMDLIN(13:13) = CHAR( 0 )

      STATUS = FSPAWN( CMD, CMDLIN )
      PRINT *, 'Program status = ', STATUS
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
The INTEGER function
.id FSYSTEM,
which is described in a later section, implements a more general form
of the example given above.
We recommend its use.
.endnote
.*
.section INTEGER Function FSYSTEM
.*
.np
.ix 'execute a program'
.ix 'subprograms' 'FSYSTEM function'
.ix 'utility subprograms' 'FSYSTEM function'
.ix 'FSYSTEM function'
The INTEGER function
.id FSYSTEM
allows an application to run another program or execute an operating
system command.
.np
The function
.id FSYSTEM
requires one argument of type CHARACTER.
This argument represents a operating system command or a program name
together with any arguments.
.id FSYSTEM
returns an INTEGER value representing the status of subprocess
execution.
If the value is negative, the operating system command interpreter or
shell could not be run (an attempt is made to invoke the system
command interpreter to run the program).
If the value is positive, the value represents the program's
return code.
.np
In the following example, a "COPY" command is executed
and then a hypothetical sorting program is run.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER STATUS

      STATUS = FSYSTEM( 'COPY *.FOR &pc.BACKUP&pc.FOR&pc.SRC' )
      PRINT *, 'Status of COPY command = ', STATUS
      STATUS = FSYSTEM( 'SORTFILE/IN=INP.DAT/OUT=OUT.DAT' )
      PRINT *, 'Status of SORT program = ', STATUS
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section Subroutine FTRACEBACK
.*
.np
.ix 'subprograms' 'FTRACEBACK subroutine'
.ix 'utility subprograms' 'FTRACEBACK subroutine'
.ix 'FTRACEBACK subroutine'
The subroutine
.id FTRACEBACK
allows your application to generate a run-time traceback.
The application must be compiled with the "DEBUG" or "TRACE" option.
It is useful when you wish to disclose a problem in an application and
provide an informative report of where the problem occurred in the
application.
.np
The
.id FTRACEBACK
subroutine requires no arguments.
The
.id FTRACEBACK
subroutine does not terminate program execution.
.exam begin
      SUBROUTINE READREC( UN )

      INCLUDE 'FSUBLIB.FI'

      INTEGER UN
      INTEGER RLEN
      CHARACTER*35 INPUT

      RLEN = FNEXTRECL( UN )
      IF( RLEN .GT. 35 )THEN
        PRINT *, 'Error: Record too long', RLEN
        CALL FTRACEBACK
        STOP
      ELSE
        PRINT *, 'Record length=', RLEN
        READ( UNIT=UN ) INPUT(1:RLEN)
        PRINT *, INPUT(1:RLEN)
      ENDIF
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.do end
.*
.section Subroutine GETDAT
.*
.np
.ix 'subprograms' 'subroutine GETDAT'
.ix 'utility subprograms' 'subroutine GETDAT'
.ix 'GETDAT subroutine'
The subroutine
.id GETDAT
allows an application to obtain the current date.
.np
The subroutine
.id GETDAT
has three arguments of type
.id INTEGER*2.
When control is returned from
.id GETDAT,
they contain the year, month and day
of the current date.
.np
The following program prints the current date in the form
"YY-MM-DD".
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER*2 YEAR, MONTH, DAY
      CALL GETDAT( YEAR, MONTH, DAY )
      PRINT 100, YEAR, MONTH, DAY
100   FORMAT( 1X, I4, '-', I2.2, '-', I2.2 )
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
The arguments to
.id GETDAT
must be of type INTEGER*2 in order to obtain correct results.
.endnote
.*
.section Subroutine GETTIM
.*
.np
.ix 'subprograms' 'subroutine GETTIM'
.ix 'utility subprograms' 'subroutine GETTIM'
.ix 'GETTIM subroutine'
The subroutine
.id GETTIM
allows an application to obtain the current time.
.np
The subroutine
.id GETTIM
has four arguments of type
.id INTEGER*2.
When control is returned from
.id GETTIM,
they contain the hours, minutes, seconds, and hundredths of seconds
of the current time.
.np
The following program prints the current time in the form
"HH:MM:SS.TT".
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER*2 HRS, MINS, SECS, HSECS
      CALL GETTIM( HRS, MINS, SECS, HSECS )
      PRINT 100, HRS, MINS, SECS, HSECS
100   FORMAT( 1X, I2.2, ':', I2.2, ':', I2.2, '.', I2.2 )
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
The arguments to
.id GETTIM
must be of type INTEGER*2 in order to obtain correct results.
.endnote
.*
.section INTEGER Function GROWHANDLES
.*
.np
.ix 'subprograms' 'GROWHANDLES function'
.ix 'utility subprograms' 'GROWHANDLES function'
.ix 'GROWHANDLES function'
The INTEGER function
.id GROWHANDLES
allows an application to increase the maximum number of files that can
be opened.
It requires one argument of type INTEGER representing the maximum
number of files that can be opened and returns an INTEGER value
representing the actual limit.
The actual limit may differ from the specified limit.
For example, memory constraints or system parameters may be such that
the request cannot be satisfied.
.np
The following example attempts to increase the limit on the number of
open files to sixty-four.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER NEW_LIMIT

      NEW_LIMIT = GROWHANDLES( 64 )

      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.section Functions IARGC and IGETARG
.*
.np
.ix 'subprograms' 'function IARGC'
.ix 'utility subprograms' 'function IARGC'
.ix 'IARGC function'
.ix 'subprograms' 'function IGETARG'
.ix 'utility subprograms' 'function IGETARG'
.ix 'IGETARG function'
.ix 'command line'
.ix 'arguments'
The function
.id IARGC
allows an application to determine the number of arguments (including
the program name) used to invoke the program.
The function
.id IGETARG
can be used to retrieve an argument.
.np
Arguments supplied to a program are assigned indices.
Argument zero is the program name, argument one is the first argument,
etc.
The function
.id IGETARG
requires two arguments.
The first argument is the index of the argument to retrieve and is of
type INTEGER.
The second argument is of type CHARACTER and is used to return the
argument.
The size of the argument (number of characters) is returned.
.exam begin
      INCLUDE 'FSUBLIB.FI'
      CHARACTER*128 ARG
      INTEGER ARGC, ARGLEN

      ARGC = IARGC()
      ARGLEN = IGETARG( 0, ARG )
      PRINT *, 'Program name is ', ARG(1:ARGLEN)
      DO I = 1, ARGC - 1
          ARGLEN = IGETARG( I, ARG )
          PRINT '(A, I2, 2A)', 'Argument ', I, ' is ',
     1                          ARG(1:ARGLEN)
      END DO
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.endnote
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section Math Error Functions
.*
.np
.ix 'subprograms' 'math error functions'
.ix 'utility subprograms' 'math error functions'
.ix 'math error functions'
Math error functions are called when an error is detected in a math
library function.
For example, if the second argument to the
.id AMOD
intrinsic function is zero, a math error function will be called.
A number of math error functions are defined in the FORTRAN run-time
libraries and perform default actions when an error is detected.
These actions typically produce an error message to the screen.
.np
It is possible to replace the FORTRAN run-time library version of the
math error functions with your own versions.
The file :FNAME._matherr.for:eFNAME located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory can be used as a
template for defining your own math error functions.
The following functions represent the set of math error functions.
.autopoint
.point
The function
.id __imath2err
is called for math functions of type INTEGER that take two arguments
of type INTEGER.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the first argument passed to the
math function and the third argument is a pointer to the second
argument passed to the math function.
The error function returns a value that is then used as the return
value for the math function.
.point
The function
.id __amath1err
is called for math functions of type REAL that take one argument of
type REAL.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the argument passed to the math
function.
The error function returns a value that is then used as the return
value for the math function.
.point
The function
.id __amath2err
is called for math functions of type REAL that take two arguments of
type REAL.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the first argument passed to the
math function and the third argument is a pointer to the second
argument passed to the math function.
The error function returns a value that is then used as the return
value for the math function.
.point
The function
.id __math1err
is called for math functions of type DOUBLE PRECISION that take one
argument of type DOUBLE PRECISION.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the argument passed to the math
function.
The error function returns a value that is then used as the return
value for the math function.
.point
The function
.id __math2err
is called for math functions of type DOUBLE PRECISION that take two
arguments of type DOUBLE PRECISION.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the first argument passed to the
math function and the third argument is a pointer to the second
argument passed to the math function.
The error function returns a value that is then used as the return
value for the math function.
.point
The function
.id __zmath2err
is called for math functions of type COMPLEX that take two arguments
of type COMPLEX.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the first argument passed to the
math function and the third argument is a pointer to the second
argument passed to the math function.
The error function returns a value that is then used as the return
value for the math function.
.point
The function
.id __qmath2err
is called for math functions of type DOUBLE COMPLEX that take two
arguments of type DOUBLE COMPLEX.
The first argument represents the error information and is an argument
of type INTEGER that is passed by value.
The second argument is a pointer to the first argument passed to the
math function and the third argument is a pointer to the second
argument passed to the math function.
The error function returns a value that is then used as the return
value for the math function.
.endpoint
.np
The include file :FNAME.mathcode.fi:eFNAME. is included by the file
:FNAME._matherr.for:eFNAME. and is located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory.
It defines the information that is contained in the error information
argument that is passed to all math error functions.
.*
.do end
.*
.section INTEGER Function SEEKUNIT
.*
.np
.ix 'subprograms' 'SEEKUNIT function'
.ix 'utility subprograms' 'SEEKUNIT function'
.ix 'SEEKUNIT function'
The INTEGER function
.id SEEKUNIT
permits seeking to a particular byte offset within a file connected
to a FORTRAN unit.
The file must be opened with the following attributes:
.illust begin
FORM='UNFORMATTED'
ACCESS='SEQUENTIAL'
RECORDTYPE='FIXED'
.illust end
.np
The function
.id SEEKUNIT
requires three arguments of type INTEGER, the unit number, the offset
to seek, and the type of positioning to do.
The seek positioning may be absolute (indicated by 0) or relative to
the current position (indicated by 1).
It returns an INTEGER value representing the new offset in the file.
A returned value of -1 indicates that the function call failed.
.np
This function is particularly useful for applications that wish to
change the input/output position for a file connected to a unit.
.np
The following example will set the current input/output position of
the file connected to the specified unit.
.exam begin
      EXTERNAL SEEKUNIT
      INTEGER SEEKUNIT
      INTEGER SEEK_SET, SEEK_CUR
      PARAMETER (SEEK_SET=0, SEEK_CUR=1)

      INTEGER POSITION
      CHARACTER*80 RECORD

      OPEN( UNIT=8, FILE='file', FORM='UNFORMATTED',
     1        ACCESS='SEQUENTIAL', RECORDTYPE='FIXED' )
      POSITION = SEEKUNIT( 8, 10, SEEK_SET )
      IF( POSITION .NE. -1 )THEN
          PRINT *, 'New position is', POSITION
          READ( UNIT=8 ) RECORD
          PRINT *, RECORD
      ENDIF
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
A value of -1 is returned if the requested positioning cannot be done.
.endnote
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section INTEGER Function SETJMP/Subroutine LONGJMP
.*
.np
.ix 'subprograms' 'SETJMP function'
.ix 'utility subprograms' 'SETJMP function'
.ix 'SETJMP function'
.ix 'subprograms' 'LONGJMP subroutine'
.ix 'utility subprograms' 'LONGJMP subroutine'
.ix 'LONGJMP subroutine'
The INTEGER function
.id SETJMP
saves the current executing environment, making it possible to restore
that environment by subsequently calling the
.id LONGJMP
subroutine.
For example, it is possible to implement error handling by using
.id SETJMP
to record the point to which a return will occur following an error.
When an error is detected in a called subprogram, that subprogram uses
.id LONGJMP
to jump back to the recorded position.
The original subprogram which called
.id SETJMP
must still be active (it cannot have returned to the subprogram which
called it).
.np
The
.id SETJMP
function requires one argument.
The argument is a structure of type
.id jmp_buf
and is used to save the current environment.
The return value is an integer and is zero when initially called.
It is non-zero if the return is the result of a call to the
.id LONGJMP
subroutine.
An
.KW IF
statement is often used to handle these two cases.
This is demonstrated in the following example.
.exam begin
      include 'fsignal.fi'
      include 'setjmp.fi'
      record /jmp_buf/ jmp_buf
      common jmp_buf
      external break_handler
      integer rc
      call fsignal( SIGBREAK, break_handler )
      rc = setjmp( jmp_buf )
      if( rc .eq. 0 )then
          call do_it()
      else
          print *, 'abnormal termination:', rc
      endif
      end

      subroutine do_it()
      loop
      end loop
      end

      subroutine break_handler()
      include 'setjmp.fi'
      record /jmp_buf/ jmp_buf
      common jmp_buf
      call longjmp( jmp_buf, -1 )
      end
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.setjmp.fi:eFNAME. contains typing and
calling information for
.id SETJMP
and
.id LONGJMP
and must be included.
Similarly, :FNAME.fsignal.fi:eFNAME. must be included when using the
.id FSIGNAL
function.
These files are located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate these include
files.
.endnote
.*
.do end
.*
.section INTEGER Function SETSYSHANDLE
.*
.np
.ix 'subprograms' 'SETSYSHANDLE function'
.ix 'utility subprograms' 'SETSYSHANDLE function'
.ix 'SETSYSHANDLE function'
The INTEGER function
.id SETSYSHANDLE
allows an application to set the system file handle for a specified
unit.
.np
The function
.id SETSYSHANDLE
requires an argument of type INTEGER, the unit number,
and an argument of type INTEGER*2, the handle,
and returns an INTEGER value representing the success or fail
status of the function call.
A returned value of -1 indicates that the function call failed
and 0 indicates that the function call succeeded.
.np
This function is particularly useful for applications that wish to
set the system file handle for a unit.
The system file handle may have been obtained from a non-FORTRAN
subroutine or function.
.np
The following example will set the system file handle for a paricular
unit.
.tinyexam begin
      INCLUDE 'FSUBLIB.FI'
      INTEGER STDIN, STDOUT
      PARAMETER (STDIN=0, STDOUT=1)

      OPEN( UNIT=8, FORM='FORMATTED' )
      I = SYSHANDLE( 8 )
      PRINT *, 'Old handle was', I
      I = SETSYSHANDLE( 8, STDOUT )
      IF( I .EQ. 0 )THEN
          WRITE( UNIT=8, FMT=* ) 'Output to UNIT 8 which is stdout'
      ENDIF
      END
.tinyexam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
A value of -1 is returned if the unit is not connected to a file.
:cmt. Note that an
:cmt. .kw OPEN
:cmt. statement does not actually allocate a system file handle for the
:cmt. file; the first input/output operation will do the allocation.
.note
Units 5 and 6 are preconnected to the standard input and standard
output devices respectively.
.endnote
.*
.section INTEGER*2 Function SYSHANDLE
.*
.np
.ix 'subprograms' 'SYSHANDLE function'
.ix 'utility subprograms' 'SYSHANDLE function'
.ix 'SYSHANDLE function'
The INTEGER*2 function
.id SYSHANDLE
allows an application to obtain the system file handle
for a specified unit.
.np
The function
.id SYSHANDLE
requires one argument of type INTEGER, the unit number.
and returns an INTEGER*2 value representing the system file handle.
.np
This function is particularly useful for applications that wish to pass
the system file handle to non-FORTRAN subroutines or functions that
wish to perform input/output to a FORTRAN 77 file.
.np
The following example will print the system file handles for the
standard input and standard output devices.
.exam begin
      INCLUDE 'FSUBLIB.FI'

      PRINT *, 'Unit 5 file handle is', SYSHANDLE( 5 )
      PRINT *, 'Unit 6 file handle is', SYSHANDLE( 6 )
      END
.exam end
.autonote Notes:
.note
The FORTRAN include file :FNAME.fsublib.fi:eFNAME., located in the
:FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory, contains typing
and calling information for this subprogram.
The :FNAME.&pathnam.&pc.src&pc.fortran:eFNAME. directory should be
included in the
.ev &incvarup
environment variable so that the compiler can locate the include file.
.note
A value of -1 is returned if the unit is not connected to a file.
:cmt. Note that an
:cmt. .kw OPEN
:cmt. statement does not actually allocate a system file handle for the
:cmt. file; the first input/output operation will do the allocation.
.note
Units 5 and 6 are preconnected to the standard input and standard output
devices respectively.
.endnote
.*
.section REAL Function URAND
.*
.np
.ix 'URAND function'
.ix 'random number generator'
.ix 'utility subprograms' 'function URAND'
The REAL function
.id URAND
returns pseudo-random numbers in the range (0,1).
.np
The function
.id URAND
requires one argument of type INTEGER, the initial seed.
The seed can contain any integer value.
.id URAND
returns a REAL value which is a pseudo-random number in the range
(0.0,1.0).
.np
In the following example, 100 random numbers are printed.
.exam begin
      REAL URAND
      INTEGER SEED

      SEED = 75347
      DO I = 1, 100
         PRINT *, URAND( SEED )
      ENDDO
      END
.exam end
.autonote Notes:
.note
Upon each invocation of
.id URAND,
the seed argument is updated by the random number generator.
Therefore, the argument must not be a constant and, once the seed
value has been set, it must
.us not
be modified by the programmer.
.endnote
.*
.section Default Windowing Functions
.*
.np
The functions described in the following sections provide the
capability to manipulate attributes of various windows created by
&company's default windowing system for Microsoft Windows 3.x,
Windows 95, Windows NT, and IBM OS/2.
A simple default windowing FORTRAN application can be built using the
following command(s):
.begnote $break $setptnt 10
.note 16-bit Windows
.monoon
C>&wclcmd16 [fn1] [fn2] ... /bw /windows /l=windows
.monooff
.note 32-bit Windows
.monoon
C>&wclcmd32 [fn1] [fn2] ... /bw /l=win386
.br
C>wbind -n [fn1]
.monooff
.note 32-bit Windows NT or Windows 95
.monoon
C>&wclcmd32 [fn1] [fn2] ... /bw /l=nt_win
.monooff
.note 32-bit OS/2 Presentation Manager
.monoon
C>&wclcmd32 [fn1] [fn2] ... /bw /l=os2v2_pm
.monooff
.endnote
.begnote
.note Note:
At present, a restriction in Windows NT prevents you from opening the
console device (CON) for both read and write access.
Therefore, it is not possible to open additional windows for both
input and output under Windows NT.
They must be either read-only or write-only windows.
.endnote
.*
.beglevel
.*
.section dwfDeleteOnClose
.*
.millust begin
integer function dwfDeleteOnClose( unit )
integer unit
.millust end
.np
.ix 'default windowing' 'dwfDeleteOnClose'
.ix 'dwfDeleteOnClose function'
The dwfDeleteOnClose function tells the console window that it should
close itself when the corresponding file is closed.
The argument
.id unit
is the unit number associated with the opened console.
.np
This function is one of the support functions that can be called from
an application using &company's default windowing support.
.np
The dwfDeleteOnClose function returns 1 if it was successful and 0 if
not.
.cp 22
.tinyexam begin
      PROGRAM main
      INCLUDE 'FSUBLIB.FI'

      INTEGER rc
      CHARACTER response

      rc = dwfSetAboutDlg( 'Hello World About Dialog',
     1                     'About Hello World'//CHAR(13)//
     2                     'Copyright 1994 by WATCOM'//CHAR(13) )
      rc = dwfSetAppTitle( 'Hello World Application Title' )
      rc = dwfSetConTitle( 5, 'Hello World Console Title' )
      PRINT *, 'Hello World'
      OPEN( unit=3, file='CON' )
      rc = dwfSetConTitle( 3, 'Hello World Second Console Title' )
      rc = dwfDeleteOnClose( 3 )
      WRITE( unit=3, fmt=* ) 'Hello to second console'
      WRITE( unit=3, fmt=* ) 'Press Enter to close this console'
      READ( unit=3, fmt='(A)', end=100, err=100 ) response
100   CLOSE( unit=3 )
      END
.tinyexam end
.*
.section dwfSetAboutDlg
.*
.millust begin
integer function dwfSetAboutDlg( title, text )
character*(*) title
character*(*) text
.millust end
.np
.ix 'default windowing' 'dwfSetAboutDlg'
.ix 'dwfSetAboutDlg function'
The dwfSetAboutDlg function sets the "About" dialog box of the default
windowing system.
The argument
.id title
is a character string that will replace the current title.
If
.id title
is CHAR(0) then the title will not be replaced.
The argument
.id text
is a character string which will be placed in the "About" box.
To get multiple lines, embed a new line character (CHAR(13)) after
each logical line in the string.
If
.id text
is CHAR(0), then the current text in the "About" box will not be
replaced.
.np
This function is one of the support functions that can be called from
an application using &company's default windowing support.
.np
The dwfSetAboutDlg function returns 1 if it was successful and 0 if
not.
.cp 22
.tinyexam begin
      PROGRAM main
      INCLUDE 'FSUBLIB.FI'

      INTEGER rc
      CHARACTER response

      rc = dwfSetAboutDlg( 'Hello World About Dialog',
     1                     'About Hello World'//CHAR(13)//
     2                     'Copyright 1994 by WATCOM'//CHAR(13) )
      rc = dwfSetAppTitle( 'Hello World Application Title' )
      rc = dwfSetConTitle( 5, 'Hello World Console Title' )
      PRINT *, 'Hello World'
      OPEN( unit=3, file='CON' )
      rc = dwfSetConTitle( 3, 'Hello World Second Console Title' )
      rc = dwfDeleteOnClose( 3 )
      WRITE( unit=3, fmt=* ) 'Hello to second console'
      WRITE( unit=3, fmt=* ) 'Press Enter to close this console'
      READ( unit=3, fmt='(A)', end=100, err=100 ) response
100   CLOSE( unit=3 )
      END
.tinyexam end
.*
.section dwfSetAppTitle
.*
.millust begin
integer function dwfSetAppTitle( title )
character*(*) title
.millust end
.np
.ix 'default windowing' 'dwfSetAppTitle'
.ix 'dwfSetAppTitle function'
The dwfSetAppTitle function sets the main window's title.
The argument
.id title
is a character string that will replace the current title.
.np
This function is one of the support functions that can be called from
an application using &company's default windowing support.
.np
The dwfSetAppTitle function returns 1 if it was successful and 0 if
not.
.cp 22
.tinyexam begin
      PROGRAM main
      INCLUDE 'FSUBLIB.FI'

      INTEGER rc
      CHARACTER response

      rc = dwfSetAboutDlg( 'Hello World About Dialog',
     1                     'About Hello World'//CHAR(13)//
     2                     'Copyright 1994 by WATCOM'//CHAR(13) )
      rc = dwfSetAppTitle( 'Hello World Application Title' )
      rc = dwfSetConTitle( 5, 'Hello World Console Title' )
      PRINT *, 'Hello World'
      OPEN( unit=3, file='CON' )
      rc = dwfSetConTitle( 3, 'Hello World Second Console Title' )
      rc = dwfDeleteOnClose( 3 )
      WRITE( unit=3, fmt=* ) 'Hello to second console'
      WRITE( unit=3, fmt=* ) 'Press Enter to close this console'
      READ( unit=3, fmt='(A)', end=100, err=100 ) response
100   CLOSE( unit=3 )
      END
.tinyexam end
.*
.section dwfSetConTitle
.*
.millust begin
integer function dwfSetConTitle( unit, title )
integer unit
character*(*) title
.millust end
.np
.ix 'default windowing' 'dwfSetConTitle'
.ix 'dwfSetConTitle function'
The dwfSetConTitle function sets the console window's title which
corresponds to the unit number passed to it.
The argument
.id unit
is the unit number associated with the opened console.
The argument
.id title
is the character string that will replace the current title.
.np
This function is one of the support functions that can be called from
an application using &company's default windowing support.
.np
The dwfSetConTitle function returns 1 if it was successful and 0 if
not.
.cp 22
.tinyexam begin
      PROGRAM main
      INCLUDE 'FSUBLIB.FI'

      INTEGER rc
      CHARACTER response

      rc = dwfSetAboutDlg( 'Hello World About Dialog',
     1                     'About Hello World'//CHAR(13)//
     2                     'Copyright 1994 by WATCOM'//CHAR(13) )
      rc = dwfSetAppTitle( 'Hello World Application Title' )
      rc = dwfSetConTitle( 5, 'Hello World Console Title' )
      PRINT *, 'Hello World'
      OPEN( unit=3, file='CON' )
      rc = dwfSetConTitle( 3, 'Hello World Second Console Title' )
      rc = dwfDeleteOnClose( 3 )
      WRITE( unit=3, fmt=* ) 'Hello to second console'
      WRITE( unit=3, fmt=* ) 'Press Enter to close this console'
      READ( unit=3, fmt='(A)', end=100, err=100 ) response
100   CLOSE( unit=3 )
      END
.tinyexam end
.*
.section dwfShutDown
.*
.millust begin
integer function dwfShutDown()
.millust end
.np
.ix 'default windowing' 'dwfShutDown'
.ix 'dwfShutDown function'
The dwfShutDown function shuts down the default windowing I/O system.
The application will continue to execute but no windows will be
available for output.
Care should be exercised when using this function since any subsequent
output may cause unpredictable results.
.np
When the application terminates, it will not be necessary to manually
close the main window.
.np
This function is one of the support functions that can be called from
an application using &company's default windowing support.
.np
The dwfShutDown function returns 1 if it was successful and 0 if not.
.cp 22
.tinyexam begin
      PROGRAM main
      INCLUDE 'FSUBLIB.FI'

      INTEGER rc
      CHARACTER response

      rc = dwfSetAboutDlg( 'Hello World About Dialog',
     1                     'About Hello World'//CHAR(13)//
     2                     'Copyright 1994 by WATCOM'//CHAR(13) )
      rc = dwfSetAppTitle( 'Hello World Application Title' )
      rc = dwfSetConTitle( 5, 'Hello World Console Title' )
      PRINT *, 'Hello World'
      OPEN( unit=3, file='CON' )
      rc = dwfSetConTitle( 3, 'Hello World Second Console Title' )
      rc = dwfDeleteOnClose( 3 )
      WRITE( unit=3, fmt=* ) 'Hello to second console'
      WRITE( unit=3, fmt=* ) 'Press Enter to close this console'
      READ( unit=3, fmt='(A)', end=100, err=100 ) response
100   CLOSE( unit=3 )
      rc = dwfShutDown()

*   do more computing that does not involve console input/output
*         .
*         .
*         .

      END
.tinyexam end
.*
.section dwfYield
.*
.millust begin
integer function dwf veld()
.millust end
.np
.ix 'default windowing' 'dwfYield'
.ix 'dwfYield function'
The dwfYield function yields control back to the operating system,
thereby giving other processes a chance to run.
.np
This function is one of the support functions that can be called from
an application using &company's default windowing support.
.np
The dwfYield function returns 1 if it was successful and 0 if not.
.cp 22
.tinyexam begin
      PROGRAM main
      INCLUDE 'FSUBLIB.FI'

      INTEGER rc
      CHARACTER response
      INTEGER i

      rc = dwfSetAboutDlg( 'Hello World About Dialog',
     1                       'About Hello World'//CHAR(13)//
     2                       'Copyright 1994 by WATCOM'//CHAR(13) )
      rc = dwfSetAppTitle( 'Hello World Application Title' )
      rc = dwfSetConTitle( 5, 'Hello World Console Title' )
      PRINT *, 'Hello World'
      OPEN( unit=3, file='CON' )
      rc = dwfSetConTitle( 3, 'Hello World Second Console Title' )
      rc = dwfDeleteOnClose( 3 )
      WRITE( unit=3, fmt=* ) 'Hello to second console'
      WRITE( unit=3, fmt=* ) 'Press Enter to close this console'
      READ( unit=3, fmt='(A)', end=100, err=100 ) response
100   CLOSE( unit=3 )

      DO i = 0, 1000
          rc = dwfYield()
*         do CPU-intensive calculation
*         .
*         .
*         .
      ENDDO
      PRINT *, i

      END
.tinyexam end
.*
.endlevel
