.chap Creating &bldsys Applications
.*
.np
.ix '&bldsys applications'
This chapter describes how to compile and link &bldsys applications
simply and quickly.
In this chapter, we look at applications written to exploit the &bldos
Application Programming Interface (API).
.np
We will illustrate the steps to creating &bldsys applications by
taking a small sample application and showing you how to compile,
link, run and debug it.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&bldos.' eq 'Windows 3.x' .do begin
.np
Note - It is supposed you are working on the host with &bldos. installed.
If you are on the host with any other operating system you should setup
INCLUDE environment variable correctly to compile for &bldsys target.
.np
You can do that by command (DOS, OS/2, NT)
.np
set INCLUDE=%WATCOM%\h;%WATCOM%\h\win
.np
or by command (LINUX)
.np
export INCLUDE=$WATCOM/h:$WATCOM/h/win
.do end
.do end
.*
.if '&bldnam' ne 'nt' .section The Sample GUI Application
.el .section The Sample Character-mode Application
.*
.np
To demonstrate the creation of &bldsys applications, we introduce a
simple sample program.
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="demo"      value="hello".
:set symbol="demoup"    value="HELLO".
:set symbol="demosuff"  value=".c".
.ix '&bldsys application'
The following example is the "&demo." program adapted for Windows.
.millust begin
.if '&bldnam' ne 'nt' .do begin
#include <windows.h>

int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInst,
                    LPSTR lpCmdLine, int nCmdShow )
{
    MessageBox( NULL, "Hello world",
                "&product. for Windows",
                MB_OK | MB_TASKMODAL );
    return( 0 );
}
.do end
.el .do begin
#include <stdio.h>

void main()
{
    printf( "Hello world\n" );
}
.do end
.millust end
.np
The goal of this program is to display the message "Hello world"
on the screen.
.if '&bldnam' ne 'nt' .do begin
.ix 'MessageBox'
The
.id MessageBox
Windows API function is used to accomplish this task.
.do end
.el .do begin
.ix 'printf'
The C library
.id printf
routine is used to accomplish this task.
.do end
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="demo"      value="sieve".
:set symbol="demoup"    value="SIEVE".
:set symbol="demosuff"  value=".for".
For our example, we are going to use the "sieve" program.
.code begin
.if '&bldnam' ne 'nt' .do begin
*$include winapi.fi
*$noreference
      INTEGER*2 FUNCTION FWINMAIN( hInstance, hPrevInstance,
     &                             lpszCmdLine, nCmdShow )
*$reference
      IMPLICIT NONE
      INTEGER*2 hInstance, hPrevInstance, nCmdShow
      INTEGER*4 lpszCmdLine
      include 'windows.fi'

.code break
* This program computes the prime numbers between 1 and 10,000
* using the Sieve of Eratosthenes algorithm.

.do end
.el .do begin
.code break
* This program computes the prime numbers between 1 and 10,000
* using the Sieve of Eratosthenes algorithm.

      IMPLICIT NONE
.do end
      INTEGER UPBOUND
      PARAMETER (UPBOUND=10000)
      INTEGER I, K, PRIMES
      LOGICAL*1 NUMBERS(2:UPBOUND)
      CHARACTER*11 FORM
      CHARACTER*60 BUFFER
      PARAMETER (FORM='(A,I5,A,I5)')
.code break
      DO I = 2, UPBOUND
          NUMBERS(I) = .TRUE.
      ENDDO
.code break
      PRIMES = 0
      DO I = 2, UPBOUND
          IF( NUMBERS(I) )THEN
              PRIMES = PRIMES + 1
              DO K = I + I, UPBOUND, I
                  NUMBERS(K) = .FALSE.
              ENDDO
          ENDIF
      ENDDO
.code break
.if '&bldnam' ne 'nt' .do begin
      WRITE(BUFFER, FORM) 'The Number of Primes between 1 and ',
     &      UPBOUND, ' are: ', PRIMES
      CALL MessageBox( 0, BUFFER,
     &           'Sieve of Eratosthenes'c,
     &           MB_OK .OR. MB_TASKMODAL )
.do end
.el .do begin
      PRINT FORM, 'The Number of Primes between 1 and ', UPBOUND,
     1            ' are: ', PRIMES
.do end
      END
.code end
.ix 'sieve'
.np
The goal of this program is to count the prime numbers between 1 and
10,000.
It uses the famous
.us Sieve of Eratosthenes
algorithm to accomplish this task.
.do end
.*
We will take you through the steps necessary to produce this result.
.*
.if '&bldnam' ne 'nt' .section Building and Running the GUI Application
.el .section Building and Running the Character-mode Application
.*
.np
.ix 'building &bldos applications'
To compile and link our example program which is stored in the file
.fi &demo..&langsuff
.ct , enter the following command:
.millust begin
&prompt.&wclcmd. &sw.l=&bldnam. &bldswt. &demo.&demosuff.
.millust end
.np
The typical messages that appear on the screen are shown in the
following illustration.
.code begin
.im &wclcmd.w1
.code end
:CMT. .np
:CMT. .ix 'compilation speed'
:CMT. .ix 'dots'
:CMT. Depending on the size of the file, the complexity of the code
:CMT. optimizations that have been requested, and the speed of your
:CMT. processor, you might see some dots appear on the screen.
:CMT. These dots are displayed by the compiler during the code
:CMT. optimization/generation phase.
.np
If you examine the current directory, you will find that two files
have been created.
.ix 'object file'
.ix 'executable file'
These are
.fi &demo.&obj
(the result of compiling
.fi &demo..&langsuff
.ct ) and
.fi &demo.&bldexe
(the result of linking
.fi &demo.&obj
with the appropriate &product libraries).
.*
.im pgbldbnd
.np
The resultant &bldsys application
.fi &demoup..EXE
can now be run under &bldos..
.*
.if '&bldnam' ne 'nt' .section Debugging the GUI Application
.el .section Debugging the Character-mode Application
.*
.np
.ix 'debugging &bldos applications'
Let us assume that you wish to debug your application in order to
locate an error in programming.
In the previous section, the "&demo." program was compiled with default
compile and link options.
When debugging an application, it is useful to refer to the symbolic
names of routines and variables.
It is also convenient to debug at the source line level rather than
the machine language level.
To do this, we must direct both the compiler and linker to include
additional debugging information in the object and executable files.
Using the
.kw &wclcmdup
command, this is fairly straightforward.
.kw &wclcmdup
recognizes the &cmpname compiler "debug" options and will create the
appropriate debug directives for the &lnkname..
.np
For example, to compile and link the "&demo." program with debugging
information, the following command may be issued.
.millust begin
&prompt.&wclcmd. &sw.l=&bldnam. &bldswt. &sw.&debugopt. &demo.&demosuff.
.millust end
.np
The typical messages that appear on the screen are shown in the
following illustration.
.code begin
.im &wclcmd.w2
.code end
.np
The "&debugopt." option requests the maximum amount of debugging
information that can be provided by the &cmpname compiler.
.kw &wclcmdup
will make sure that this debugging information is included in the
executable file that is produced by the linker.
.np
The
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
"Code size"
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
"bytes"
.do end
value is larger than in the previous example since selection of the
"&debugopt." option results in fewer code optimizations by default.
You can request more optimization by specifying the appropriate
options.
However, you do so at the risk of making it more difficult for
yourself to determine the relationship between the object code and the
original source language code.
.if '&bldnam' eq 'win386' .do begin
.np
Once again, the "&bldexe." file must be combined with &company.'s 32-bit
Windows supervisor
.fi WIN386.EXT
using the &company Bind utility.
This step is described in the previous section.
.do end
.np
To request the &dbgname to assist in debugging the application, select
the &dbgname icon.
It would be too ambitious to describe the debugger in this
introductory chapter so we refer you to the book entitled
.book &dbgname User's Guide.
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.np
There are more extensive examples of Windows applications written in
&lang. in the
.fi &pathnamup.\SAMPLES\FORTRAN\WIN
directory.
The example programs are
.fi ELLIPSE.FOR
and
.fi FWCOPY.FOR.
.do end
