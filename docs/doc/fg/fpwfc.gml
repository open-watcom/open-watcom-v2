.chap *refid=fpwfc The &product Compiler
.*
.np
.ix 'compiler'
This chapter describes the following topics:
.begbull
.bull
Command line syntax
(see :HDREF refid='wfcclf'.)
.bull
Environment variables used by the compilers
(see :HDREF refid='wfcenv'.)
.bull
Examples of command line syntax
(see :HDREF refid='wfccle'.)
.bull
Interpreting diagnostic messages
(see :HDREF refid='wfccd'.)
.bull
Include file handling
(see :HDREF refid='wfcifp'.)
.endbull
.*
.section *refid=wfcclf &product Command Line Format
.*
.np
.ix 'command line format'
.ix 'compiling' 'command line format'
The formal &product command line syntax is shown below.
.ix 'invoking &product'
.mbigbox
&ccmdup16 [options] [d:][path]filename[.ext] [options]
&ccmdup32 [options] [d:][path]filename[.ext] [options]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.mnote &ccmdup16
is the name of the 16-bit &cmpname compiler.
.mnote &ccmdup32
is the name of the 32-bit &cmpname compiler.
.mnote d:
is an optional drive specification such as "A:", "B:", etc.
If not specified, the default drive is assumed.
.mnote path
is an optional path specification such as
.fi \PROGRAMS\SRC\.
If not specified, the current directory is assumed.
.mnote filename
is the file name of the file to be compiled.
.mnote ext
is the file extension of the file to be compiled.
If omitted, a file extension of "&langsuffup" is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.mnote options
is a list of valid &cmpname options, each preceded by a slash ("/") or
a dash ("&minus.").
Certain options can include a "no" prefix to disable an option.
Options may be specified in any order, with the rightmost option
taking precedence over any conflicting options specified to its left.
.endnote
.*
.section *refid=wfcenv &cvarup16/&cvarup32 Environment Variables
.*
.np
The
.ev &cvarup16
environment variable can be used to specify commonly used &ccmdup16
options.
The
.ev &cvarup32
environment variable can be used to specify commonly used &ccmdup32
options.
These options are processed before options specified on the command
line.
.ix '&setcmdup'
.exam begin 2
&prompt.&setcmd &setdelim.&cvar16=&sw.d1 &sw.ot&setdelim
&prompt.&setcmd &setdelim.&cvar32=&sw.d1 &sw.ot&setdelim
.exam end
.pc
The above example defines the default options to be "d1" (include line
number debugging information in the object file), and "ot" (favour
time optimizations over size optimizations).
.if '&target' ne 'QNX' .do begin
.np
.ix 'environment string' '= substitute'
.ix 'environment string' '#'
Whenever you wish to specify an option that requires the use of an
"=" character, you can use the "#" character in its place.
This is required by the syntax of the "&setcmdup" command.
.do end
.np
Once a particular environment variable has been defined, those options
listed become the default each time the associated compiler is used.
The compiler command line can be used to override any options
specified in the environment string.
.*
.if '&target' ne 'QNX' .do begin
.np
These environment variables are not examined by the &wclname utilities.
Since the &wclname utilities pass the relevant options found in their
associated environment variables to the compiler command line, their
environment variable options take precedence over the options
specified in the environment variables associated with the compilers.
.do end
.keep 18
.hint
.if '&target' eq 'QNX' .do begin
.ix 'user initialization file'
If you use the same compiler options all the time, you may find it
handy to define the environment variable in your user initialization
file.
.do end
.el .do begin
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT'
If you are running DOS and you use the same compiler options all the
time, you may find it handy to define the environment variable in your
DOS system initialization file,
.fi AUTOEXEC.BAT.
.np
.ix 'system initialization' 'Windows NT'
.ix 'Windows NT' 'system initialization'
If you are running Windows NT, use the "System" icon in the
.bd Control Panel
to define environment variables.
.np
.ix 'system initialization file' 'CONFIG.SYS'
.ix 'CONFIG.SYS'
If you are running OS/2 and you use the same compiler options all the
time, you may find it handy to define the environment variable in your
OS/2 system initialization file,
.fi CONFIG.SYS.
.do end
.ehint
.*
.section *refid=wfccle &product Command Line Examples
.*
.np
The following are some examples of using &product to compile &lang
source programs.
.*
.exam begin 5 ~b1
C>&ccmd32 report /d1 /stack
.exam end
.pc
The 32-bit &cmpname compiler processes
.fi REPORT.&langsuffup
producing an object file which contains source line number
information.
Stack overflow checking code is included in the object code.
.*
.exam begin 5 ~b2
C>&ccmd16 kwikdraw /2 /fpi87
.exam end
.pc
The 16-bit &cmpname compiler processes
.fi KWIKDRAW.&langsuffup
producing object code for an Intel 286 system equipped with an Intel
287 numeric data processor (or any upward compatible 386/387, 486
or Intel Pentium system).
While the choice of these options narrows the number of microcomputer
systems where this code will execute, the resulting code will be
highly optimized for this type of system.
.*
.exam begin 5 ~b3
C>&ccmd16 ..\source\modabs /d2
.exam end
.pc
The 16-bit &cmpname compiler processes
:FNAME.~..\SOURCE\MODABS.&langsuffup:eFNAME.
(a file in a directory which is adjacent to the current one).
The object file is placed in the current directory.
Included with the object code and data is information on local
symbols and data types.
The code generated is straight-forward, unoptimized code which can be
readily debugged with &dbgname..
.*
.ix 'memory model'
.exam begin 5 ~b4
C>&ccmd32 /mf calc
.exam end
.pc
The 32-bit &cmpname compiler compiles
.fi CALC.&langsuffup
for the "flat" memory model.
32-bit memory models are described in the
chapter entitled :HDREF refid='mdl386'..
32-bit argument passing conventions are described in the
chapter entitled :HDREF refid='cal386'..
.*
.exam begin 5 ~b5
C>&ccmd32 kwikdraw /fpi87
.exam end
.pc
The 32-bit &cmpname compiler processes
.fi KWIKDRAW.&langsuffup
producing object code for an Intel 386 system equipped with an Intel
80x87 numeric data processor.
.*
.exam begin 5 ~b6
C>set &cvar16=/short /d2 /fo#*.dbj
C>&ccmd16 ..\source\modabs
.exam end
.pc
The options
.id /short,
.id /d2
and
.id /fo=*.dbj
are established as defaults using the
.ev &cvarup16
environment variable.
The 16-bit compiler processes :FNAME.~..\SOURCE\MODABS.&langsuffup:eFNAME.
(a file in a directory which is adjacent to the current one).
The object file is placed in the current directory and it will have a
default file extension of "DBJ".
All INTEGER and LOGICAL variables will have a default type of
INTEGER*2 and LOGICAL*1 unless explicitly typed as INTEGER*4 or
LOGICAL*4.
Source line number and local symbol information are included with the
object file.
.*
.section *refid=wfccd Compiler Diagnostics
.*
.ix 'diagnostics' '&cmpname'
.ix 'error file' '.ERR'
.np
If the &cmpname compiler prints diagnostic messages to the screen, it
will also place a copy of these messages in a file in your current
directory (unless the "noerrorfile" option is specified).
The file will have the same file name as the source file and an
extension of "err".
The compiler issues three types of diagnostic messages, namely
extensions, warnings and errors.
An extension message indicates that you have used a feature which is
supported by &cmpname but that is not part of the FORTRAN 77 language
standard.
A warning message indicates that the compiler has found a questionable
problem in the source code (e.g., an unreachable statement, an
unreferenced variable or statement number, etc.).
A warning message does not prevent the production of an object
file.
An error message indicates that a problem is severe enough that it
must be corrected before the compiler will produce an object file.
The error file is a handy reference when you wish to correct the
errors in the source file.
.np
Just to illustrate the diagnostic features of &cmpname, we will
compile the following program called "DEMO1".
.code begin
* This program demonstrates the following features of
* &company's FORTRAN 77 compiler:
*
*    1. Extensions to the FORTRAN 77 standard are flagged.
*
*    2. Compile time error diagnostics are extensive.  As many
*       errors as possible are diagnosed.
*
*    3. Warning messages are displayed where potential problems
*       can arise.
*
        PROGRAM MAIN
        DIMENSION A(10)
        DO I=1,10
            A(I) = I
            I = I + 1
        ENDLOOP
        GO TO 30
        J = J + 1
30      END
.code end
.np
If we compile this program with the "extensions" option, the following
output appears on the screen.
.code begin
C>&ccmd16 demo1 /exten
WATCOM FORTRAN 77/16 Optimizing Compiler Version &ver..&rev &cdate
.cpyrit 1984
.trdmrk
demo1.for(14): *EXT* DO-05 this DO loop form is not FORTRAN 77 standard
demo1.for(16): *ERR* DO-07 column 13, DO variable cannot be redefined
while DO loop is active
demo1.for(17): *ERR* SP-19 ENDLOOP statement does not match with DO
statement
demo1.for(19): *WRN* ST-08 this statement will never be executed due to
the preceding branch
demo1.for: 9 statements, 0 bytes, 1 extensions, 1 warnings, 2 errors
.code end
.pc
.ix 'diagnostics' 'error'
.ix 'diagnostics' 'warning'
Here we see an example of the three types of messages,
extension (*EXT*), error (*ERR*) and warning (*WRN*).
.np
Diagnostic messages are also included in the listing file if the
"list" option is specified.
If we recompile our program and include the "list" option, a listing
file will be created.
.millust begin
C>&ccmd16 demo1 /exten/list
  or
C>&ccmd32 demo1 /exten/list
.millust end
.np
The contents of the listing file are:
.code begin
WATCOM FORTRAN 77/16 Optimizing Compiler Version &ver..&rev &cdate
.cpyrit 1984
.trdmrk

Options: list,disk,errorfile,extensions,reference,warnings,fpi,oc,of,om,
      os,ot,ox,ml,0,terminal,dependency,fsfloats,gsfloats,libinfo,dt=256,
      align

      1 * This program demonstrates the following features of
      2 * &company's FORTRAN 77 compiler:
      3 *
      4 *    1. Extensions to the FORTRAN 77 standard are flagged.
      5 *
      6 *    2. Compile time error diagnostics are extensive.  As many
      7 *       errors as possible are diagnosed.
      8 *
      9 *    3. Warning messages are displayed where potential problems
     10 *       can arise.
     11 *
     12         PROGRAM MAIN
     13         DIMENSION A(10)
     14         DO I=1,10
*EXT* DO-05 this DO loop form is not FORTRAN 77 standard
     15             A(I) = I
     16             I = I + 1
                    $
*ERR* DO-07 DO variable cannot be redefined while DO loop is active
     17         ENDLOOP
*ERR* SP-19 ENDLOOP statement does not match with DO statement
     18         GO TO 30
     19         J = J + 1
*WRN* ST-08 this statement will never be executed due to the preceding branch
     20 30      END


Code size (in bytes):              0  Number of errors:                  2
Compile time (in seconds):         0  Number of warnings:                1
Number of statements compiled:     9  Number of extensions:              1
.code end
.np
As part of the diagnostic capability of &cmpname, a "$" is often used
to indicate the particular place in the source line where an error has
been detected.
.if &e'&dohelp eq 0 .do begin
.np
The complete list of &cmpname diagnostic messages is presented in
the appendix entitled :HDREF refid='ferrmsg'..
.do end
.*
.section *refid=wfcifp &product INCLUDE File Processing
.*
.ix 'include' 'directive'
.ix 'include' 'header file'
.ix 'include' 'source file'
.ix 'header file' 'including'
.ix 'source file' 'including'
.np
For information on include file processing, see the
section entitled :HDREF refid='fcondi'.
in the chapter entitled :HDREF refid='fcond' page=no..
