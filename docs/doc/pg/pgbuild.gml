.if '&target' eq 'QNX' .do begin
:set symbol="demoname"  value="-o &demo ".
:set symbol="libopt"    value="-l ".
:set symbol="debugopt"  value="g".
:set symbol="nameopt"   value="o".
:set symbol="namesuff"  value=" ".
.do end
.el .do begin
:set symbol="demoname"  value="".
:set symbol="libopt"    value="".
:set symbol="debugopt"  value="d2".
:set symbol="nameopt"   value="fe".
:set symbol="namesuff"  value="=".
.do end
.*
.if '&bldhost' eq 'OS/2' .do begin
:set symbol="oprompt"    value=&prompt.
:set symbol="prompt"     value="[C:\]".
.do end
.*
.chap Creating &bldsys Applications
.*
.ix '&bldsys applications'
.if '&bldhost' eq 'OS/2' .do begin
.np
.ix 'OS/2' 'fullscreen application'
.ix 'OS/2' 'PM-compatible application'
.ix 'OS/2' 'Presentation Manager application'
An OS/2 application can be one of the following;
a fullscreen application, a PM-compatible application, or a
Presentation Manager application.
A fullscreen application runs in its own screen group.
A PM-compatible application will run in an OS/2 fullscreen environment
or in a window in the Presentation Manager screen group but does not
take direct advantage of menus, mouse or other features available in
the Presentation Manager.
A Presentation Manager application has full access to the complete set
of user-interface tools such as menus, icons, scroll bars, etc.
.np
This chapter deals with the creation of OS/2 fullscreen applications.
For information on creating Presentation Manager applications, refer to
the section entitled :HDREF refid='os2pm'..
.do end
.el .do begin
.np
This chapter describes how to compile and link &bldsys applications
simply and quickly.
.do end
.np
We will illustrate the steps to creating &bldsys applications by
taking a small sample application and showing you how to compile,
link, run and debug it.
.*
.section The Sample Application
.*
.np
To demonstrate the creation of &bldsys applications using command-line
oriented tools, we introduce a simple sample program.
.*
.if '&lang' eq 'C/C++' .do begin
:set symbol="demo"      value="hello".
:set symbol="demoup"    value="HELLO".
:set symbol="demosuff"  value=".c".
.ix '&demo program'
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
.if '&target' eq 'QNX' .do begin
:set symbol="demosuff"  value=".f".
.do end
.ix '&demo program'
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
.section Building and Running the Sample &bldos Application
.*
.np
.ix 'building &bldos applications'
To compile and link our example program which is stored in the file
.fi &demo..&langsuff
.ct , enter the following command:
.millust begin
.if '&bldhost' eq 'QNX' .do begin
&prompt.&wclcmd &demoname.&demo.&demosuff
.do end
.el .do begin
&prompt.&wclcmd &sw.l=&bldnam &demoname.&demo.&demosuff
.do end
.millust end
.if '&bldhost' eq 'QNX' .do begin
.np
By default, the
.kw &wclcmdup
command will create 32-bit applications.
You can create 16-bit applications by specifying the "2" option as
demonstrated in the following example.
.millust begin
&prompt.&wclcmd &sw.2 &demoname.&demo.&demosuff
.millust end
.do end
.np
The typical messages that appear on the screen are shown in the
following illustration.
.code begin
.im &wclcmd.1
.code end
.np
Provided that no errors were encountered during the compile or link
phases, the "&demo" program may now be run.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
&prompt.&bldrun.&demo
Hello world
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
&prompt.&bldrun.&demo
The Number of Primes between 1 and 10000 are:  1229
.millust end
.do end
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
It is
.fi &demo.&bldexe
that is run by &bldhost when you enter the "&bldrun.&demo" command.
.*
.section Debugging the Sample &bldos Application
.*
.np
.ix 'debugging &bldos applications'
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
recognizes
.if '&target' ne 'QNX' .do begin
the &cmpname compiler
.do end
"debug" options and will create the appropriate debug directives for
the &lnkname..
.np
For example, to compile and link the "&demo" program with debugging
information, the following command may be issued.
.if '&target' eq 'QNX' .do begin
.millust begin
&prompt.&wclcmd &sw.&debugopt. &demoname.&demo.&demosuff
.millust end
.do end
.el .do begin
.millust begin
&prompt.&wclcmd &sw.l=&bldnam. &sw.&debugopt. &demoname.&demo.&demosuff
.millust end
.do end
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
.if '&target' eq 'QNX' .do begin
The size of the generated code
.do end
.el .do begin
The
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
"Code size"
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
"bytes"
.do end
value
.do end
is larger than in the previous example since selection of the
"&debugopt." option results in fewer code optimizations by default.
You can request more optimization by specifying the appropriate
options.
However, you do so at the risk of making it more difficult for
yourself to determine the relationship between the object code and the
original source language code.
.if '&bldnam' eq 'os2' .do begin
.np
.ix 'dynamic link libraries'
.ix 'BINP directory'
.ix 'DLL directory'
.ix 'system configuration file'
.ix 'CONFIG.SYS'
For OS/2, you should also include the
.fi BINP&pc.DLL
directory in the "LIBPATH" directive of the system configuration file
.fi CONFIG.SYS.
It contains the &dbgname Dynamic Link Libraries (DLLs).
.exam begin
libpath=&dr3&pathnam&pc.binp&pc.dll
.exam end
.do end
.np
To request the &dbgname to assist in debugging the application, the
following command may be issued.
.millust begin
&prompt.&blddbg &demo
.millust end
.np
It would be too ambitious to describe the debugger in this
introductory chapter so we refer you to the book entitled
.book &dbgname User's Guide.
.*
.if '&bldhost' eq 'OS/2' .do begin
:set symbol="prompt"     value=&oprompt.
.do end
