:set symbol="debugopt"  value="d2".
.*
.chap Porting Non-GUI Applications to &bldsys
.*
.np
.ix '&bldsys non-GUI applications'
Generally, an application that is to run in a windowed environment
must be written in such a way as to exploit the Windows Application
Programming Interface (API).
To take an existing character-based (i.e., non-graphical) application
that ran under a system such as DOS and adapt it to run under Windows
can require some considerable effort.
There is a steep learning curve associated with the API function
libraries.
.np
This chapter describes how to create a Windows application quickly and
simply from an application that does not use the Windows API.
The application will make use of &company.'s default windowing support.
.*
.np
Suppose you have a set of &lang applications that previously ran under
a system like DOS and you now wish to run them under &bldos..
To achieve this, you can simply recompile your application with the
appropriate options and link with the appropriate libraries.
We provide a default windowing system that turns your character-mode
application into a simple &bldos Graphical User Interface (GUI)
application.
.np
Normally, a &bldos GUI application makes use of user-interface tools
such as menus, icons, scroll bars, etc.
However, an application that was not designed as a windowed
application (such as a DOS application) can run as a GUI application.
This is achieved by our default windowing system.
The following sections describe the default windowing system.
.*
.im winio
.*
.section The Sample Non-GUI Application
.*
.np
To demonstrate the creation of &bldsys applications, we introduce a
simple sample program.
.*
.if '&lang' eq 'C/C++' .do begin
:set symbol="demo"      value="hello".
:set symbol="demoup"    value="HELLO".
:set symbol="demosuff"  value=".c".
For our example, we are going to use the famous "&demo." program.
.millust begin
#include <stdio.h>

void main()
{
    printf( "Hello world\n" );
}
.millust end
.np
The C++ version of this program follows:
.millust begin
#include <iostream.h>

void main()
{
    cout << "Hello world" << endl;
}
.millust end
.np
The goal of this program is to display the message "Hello world"
on the screen.
.ix 'printf'
The C version uses the C library
.id printf
routine to accomplish this task.
.ix 'iostream'
The C++ version uses the "iostream" library to accomplish this task.
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="demo"      value="sieve".
:set symbol="demoup"    value="SIEVE".
:set symbol="demosuff"  value=".for".
For our example, we are going to use the "sieve" program.
.code begin
* This program computes the prime numbers between 1 and 10,000
* using the Sieve of Eratosthenes algorithm.

      IMPLICIT NONE
      INTEGER UPBOUND
      PARAMETER (UPBOUND=10000)
      INTEGER I, K, PRIMES
      LOGICAL*1 NUMBERS(2:UPBOUND)
      CHARACTER*11 FORM
      PARAMETER (FORM='(A,I5,A,I5)')
      DO I = 2, UPBOUND
          NUMBERS(I) = .TRUE.
      ENDDO
      PRIMES = 0
      DO I = 2, UPBOUND
          IF( NUMBERS(I) )THEN
              PRIMES = PRIMES + 1
              DO K = I + I, UPBOUND, I
                  NUMBERS(K) = .FALSE.
              ENDDO
          ENDIF
      ENDDO
      PRINT FORM, 'The Number of Primes between 1 and ', UPBOUND,
     1            ' are: ', PRIMES
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
.section Building and Running the Non-GUI Application
.*
.np
Very little effort is required to port an existing &lang application
to &bldos..
.np
You must compile and link the file
.fi &demo.&demosuff
specifying the "bw" option.
.millust begin
&prompt.&wclcmd. &sw.l=&bldnam. &bldswt. &demo.&demosuff.
.millust end
.np
The typical messages that appear on the screen are shown in the
following illustration.
.code begin
.im &wclcmd.1
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
can now be run under &bldos as a Windows GUI application.
.*
.section Debugging the Non-GUI Application
.*
.np
.ix 'debugging Non-GUI &bldsys applications'
Let us assume that you wish to debug your application in order to
locate an error in programming.
In the previous section, the "&demo" program was compiled with default
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
For example, to compile and link the "&demo" program with debugging
information, the following command may be issued.
.millust begin
&prompt.&wclcmd. &sw.l=&bldnam. &bldswt. &sw.&debugopt. &demo.&demosuff.
.millust end
.np
The typical messages that appear on the screen are shown in the
following illustration.
.code begin
.im &wclcmd.2
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
Once again, the "&bldexe" file must be combined with &company.'s 32-bit
Windows supervisor
.fi WIN386.EXT
using the &company Bind utility.
This step is described in the previous section.
.do end
.np
To request the &dbgname to assist in debugging the application, select
the &dbgname icon.
It would be too ambitious to describe the debugger in this
introductory chapter so we refer you to the book
entitled
.book &dbgname User's Guide.
.*
.im pgdwlib
