.* .mono .THINGME gives Script a hard time so use back quote instead
:set symbol='sysper'     value='~.'
:set symbol="prod16"    value="16-bit &product".
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol='libdir'     value='&libdir32.'
:set symbol='compcmd'    value='wcc386'
:set symbol='ocompcmd'   value='wcc'
:set symbol='compopt'    value='/mf /d1'
:set symbol='compprdopt' value='/mf'
:set symbol='warnopt'    value='/w3'
:set symbol='optnam'     value='compile_options'
:set symbol='optcont'    value='compile_options_$(version)'
:set symbol='optprod'    value='compile_options_production'
:set symbol='optdeb'     value='compile_options_debugging'
:set symbol='wlinkdebug' value='debug all'
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol='libdir'     value='&libdir32.'
:set symbol='compcmd'    value='wfc386'
:set symbol='ocompcmd'   value='wfc'
:set symbol='compopt'    value='/mf /d1'
:set symbol='compprdopt' value='/mf'
:set symbol='warnopt'    value='/warn'
:set symbol='optnam'     value='compile_options'
:set symbol='optcont'    value='compile_options_$(version)'
:set symbol='optprod'    value='compile_options_production'
:set symbol='optdeb'     value='compile_options_debugging'
:set symbol='wlinkdebug' value='debug all'
.do end
.*
.chap The &makname Utility
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix '&makname' '&makcmdup'
.ix 'make' '&makcmdup'
.ix 'program maintenance'
.ix 'maintenance'
The &makname utility is useful in the development of programs
and text processing but is general enough to be used in many different
applications.
.ix '&makcmdup' 'time-stamp'
.ix 'time-stamp'
Make uses the fact that each file has a time-stamp associated with it
that indicates the last time the file was updated.
Make uses this time-stamp to decide which files are out of date with
respect to each other.
For instance, if we have an input data file and an output report file
we would like the output report file to accurately reflect the
contents of the input data file.
In terms of time-stamps, we would like the output report to have a
more recent time-stamp than the input data file (we will say that the
output report file should be "younger" than the input data file).
If the input file had been modified then we would know from the
younger time-stamp (in comparison to the report file) that the report
file was out of date and should be updated.
Make may be used in this and many other situations to ensure that
files are kept up to date.
.np
Some readers will be quite familiar with the concepts of the Make file
maintenance tool.
&makname is patterned after the Make utility found on UNIX systems.
The next major section is simply intended to summarize, for reference
purposes only, the syntax and options of Make's command line and
special macros.
Subsequent sections go into the philosophy and capabilities of
&makname..
If you are not familiar with the capabilities of the Make utility, we
recommend that you skip to the next major section entitled "Dependency
Declarations" and read on.
.*
.section &makname Reference
.*
.np
.ix 'make' 'reference'
.ix '&makcmdup' 'reference'
The following sub-sections serve as a reference guide to the &makname
utility.
.*
.beglevel
.*
.section &makname Command Line Format
.*
.np
.ix '&makcmdup command line' 'format'
The formal &makname command line syntax is shown below.
.ix '&makcmdup command line' 'invoking &makcmdup'
.ix 'command line format' '&makcmdup'
.mbigbox
&makcmdup [options] [macro_defs] [targets]
.embigbox
.pc
As indicated by the square brackets [ ], all items are optional.
.begnote
.note options
is a list of valid &makname options, each preceded by a slash ("/")
or a dash ("&minus.").
Options may be specified in any order.
.note macro_defs
is a list of valid &makname macro definitions.
.ix '&makcmdup command line' 'defining macros'
Macro definitions are of the form:
.millust begin
A=B
.millust end
.pc
and are readily identified by the presence of the "=" (the "#"
character may be used instead of the "=" character if necessary).
Surround the definition with quotes (") if it contains blanks (e.g.,
"debug_opt=&wlinkdebug").
The macro definitions specified on the command line supersede any
macro definitions defined in makefiles.
Macro names are case-insensitive unless the "ms" option is used to
select Microsoft NMAKE mode.
.ix '&makcmdup command line' 'targets'
.note targets
is one or more targets described in the makefile.
.endnote
.*
.section &makname Options Summary
.*
.np
.ix '&makcmdup command line' 'options'
In this section, we present a terse summary of the &makname
options.
.ix '&makcmdup command line' 'help'
.ix '&makcmdup command line' 'summary'
This summary is displayed on the screen by simply entering "&makcmdup
?" on the command line.
.exam begin
C>&makcmd ?
.exam end
.begnote $compact
.note &sw.a
make all targets by ignoring time-stamps
.note &sw.b
block/ignore all implicit rules
.note &sw.c
do not verify the existence of files made
.note &sw.d
debug mode - echo all work as it progresses
.note &sw.e
always erase target after error/interrupt (disables prompting)
.note &sw.f
the next parameter is a name of dependency description file
.note &sw.h
do not print out &maksname identification lines (no header)
.note &sw.i
ignore return status of all commands executed
.note &sw.k
on error/interrupt: continue on next target
.note &sw.l
the next parameter is the name of a output log file
.note &sw.m
do not search for MAKEINIT file
.note &sw.ms
.ix 'NMAKE'
Microsoft NMAKE mode
.note &sw.n
no execute mode - print commands without executing
.note &sw.o
use circular implicit rule path
.note &sw.p
print the dependency tree as understood from the file
.note &sw.q
query mode - check targets without updating them
.note &sw.r
do not use default definitions
.note &sw.s
silent mode - do not print commands before execution
.note &sw.sn
noisy mode - always print commands before execution
.note &sw.t
touch files instead of executing commands
.note &sw.u
UNIX compatibility mode
.note &sw.v
verbose listing of inline files
.note &sw.y
show why a target will be updated
.note &sw.z
do not erase target after error/interrupt (disables prompting)
.endnote
.*
.section *refid=clo Command Line Options
.*
.np
Command line options, available with &makname, allow you to control
the processing of the makefile.
:OPTLIST.
:OPT name='a'
.ix '&makcmdup options' 'a'
.ix 'global recompile'
.ix 'recompile'
make all targets by ignoring time-stamps
.np
The "a" option is a safe way to update every target.
.ix '&makcmdup' 'touch'
.ix 'touch'
.ix '&tchcmdup'
For program maintenance, it is the preferred method over deleting
object files or touching source files.
:OPT name='b'
.ix '&makcmdup options' 'b'
block/ignore all implicit rules
.np
The "b" option will indicate to &maksname that you do not want any
implicit rule checking done.
The "b" option is useful in makefiles containing double colon "::"
explicit rules because an implicit rule search is conducted after a
double colon "::" target is updated.
.ix '&makcmdup directives' '.BLOCK'
.ix 'BLOCK' '&makcmdup directive'
Including the directive
.id &sysper.BLOCK
in a makefile also will disable implicit rule checking.
:OPT name='c'
.ix '&makcmdup options' 'c'
do not verify the existence of files made
.np
&maksname will check to ensure that a target exists after the
associated command list is executed.
The target existence checking may be disabled with the "c" option.
The "c" option is useful in processing makefiles that were developed
with other Make utilities.
.ix '&makcmdup directives' '.NOCHECK'
.ix 'NOCHECK' '&makcmdup directive'
The
.id &sysper.NOCHECK
directive is used to disable target existence checks in a makefile.
:OPT name='d'
.ix '&makcmdup' 'debugging makefiles'
.ix 'debugging makefiles'
.ix '&makcmdup options' 'd'
debug mode - echo all work as it progresses
.np
The "d" option will print out information about the time-stamp of
files and indicate how the makefile processing is proceeding.
:OPT name='e'
.ix '&makcmdup options' 'e'
.ix '&makcmdup' 'target deletion prompt'
.ix 'target deletion prompt'
always erase target after error/interrupt (disables prompting)
.np
The "e" option will indicate to &maksname that, if an error or
interrupt occurs during makefile processing, the current target being
made may be deleted without prompting.
.ix '&makcmdup directives' '.ERASE'
.ix 'ERASE' '&makcmdup directive'
The
.id &sysper.ERASE
directive may be used as an equivalent option in a makefile.
:OPT name='f'
.ix '&makcmdup options' 'f'
the next parameter is a name of dependency description file
.np
The "f" option specifies that the next parameter on the command line
is the name of a makefile which must be processed.
If the "f" option is specified then the search for the default
makefile named "MAKEFILE" is not done.
.ix '&makcmdup' 'MAKEFILE'
.ix 'MAKEFILE'
Any number of makefiles may be processed with the "f" option.
.exam begin
&makcmd /f myfile
&makcmd /f myfile1 /f myfile2
.exam end
:OPT name='h'
.ix '&makcmdup options' 'h'
do not print out &maksname identification lines (no header)
.np
The "h" option is useful for less verbose output.
Combined with the "q" option, this allows a batch file to silently
query if an application is up to date.
Combined with the "n" option, a batch file could be produced
containing the commands necessary to update the application.
:OPT name='i'
.ix '&makcmdup options' 'i'
ignore return status of all commands executed
.np
.ix '&makcmdup directives' '.IGNORE'
.ix 'IGNORE' '&makcmdup directive'
The "i" option is equivalent to the
.id &sysper.IGNORE
directive.
:OPT name='k'
.ix '&makcmdup options' 'k'
on error/interrupt: continue on next target
.np
&maksname will stop updating targets when a non-zero status is returned
by a command.
The "k" option will continue processing targets that do not depend on
the target that caused the error.
.ix '&makcmdup directives' '.CONTINUE'
.ix 'CONTINUE' '&makcmdup directive'
The
.id &sysper.CONTINUE
directive in a makefile will enable this error handling capability.
:OPT name='l'
.ix '&makcmdup options' 'l'
the next parameter is the name of a output log file
.np
&maksname will output an error message when a non-zero status is
returned by a command.
The "l" option specifies a file that will record all error messages
output by &maksname during the processing of the makefile.
:OPT name='m'
.ix '&makcmdup options' 'm'
do not search for the MAKEINIT file
.np
The default action for &maksname is to search for an initialization
file called "MAKEINIT" or "TOOLS.INI" if the "ms" option is set.
The "m" option will indicate to &maksname that processing of the
MAKEINIT file is not desired.
:OPT name='ms'
.ix 'NMAKE'
.ix '&makcmdup options' 'ms'
Microsoft NMAKE mode
.np
The default action for &maksname is to process makefiles using
&company syntax rules.
.ix 'Microsoft compatibility' 'NMAKE'
The "ms" option will indicate to &maksname that it should process
makefiles using Microsoft syntax rules.
For example, the line continuation in NMAKE is a backslash ("\") at
the end of the line.
:OPT name='n'
.ix '&makcmdup options' 'n'
no execute mode - print commands without executing
.np
The "n" option will print out what commands should be executed to
update the application without actually executing them.
Combined with the "h" option, a batch file could be produced which
would contain the commands necessary to update the application.
.exam begin
&makcmd /h /n >update.bat
update
.exam end
.pc
This is useful for applications which require all available resources
(memory and devices) for executing the updating commands.
:OPT name='o'
.ix '&makcmdup options' 'o'
use circular implicit rule path
.np
.ix '&makcmdup directives' '.OPTIMIZE'
.ix 'OPTIMIZE' '&makcmdup directive'
When this option is specified, &maksname will use a circular path
specification search which may save on disk activity for large
makefiles.
The "o" option is equivalent to the
.id &sysper.OPTIMIZE
directive.
:OPT name='p'
.ix '&makcmdup options' 'p'
print out makefile information
.np
The "p" option will cause &maksname to print out information about all
the explicit rules, implicit rules, and macro definitions.
:OPT name='q'
.ix '&makcmdup options' 'q'
query mode - check targets without updating them
.np
The "q" option will cause &maksname to return a status of 1 if the
application requires updating; it will return a status of 0 otherwise.
Here is a example batch file using the "q" option:
.exam begin
&makcmd /q
if errorstatus 0 goto noupdate
&makcmd /q /h /n >\tmp\update.bat
call \tmp\update.bat
:noupdate
.exam end
:OPT name='r'
.ix '&makcmdup options' 'r'
do not use default definitions
.np
The default definitions are:
.millust begin
__MAKEOPTS__ = <options passed to &makcmdup>
__MAKEFILES__ = <list of makefiles>
__VERSION__ = <version number>
__LOADDLL__= defined if DLL loading supported
__MSDOS__ =  defined if MS/DOS version
__NT__ = defined if Windows NT version
__NT386__ = defined if x86 Windows NT version
.*__NTAXP__ = defined if Alpha AXP Windows NT version
__OS2__ = defined if OS/2 version
__QNX__ = defined if QNX version
__LINUX__ = defined if Linux version
__LINUX386__ = defined if x86 Linux version
.*__LINUXPPC__ = defined if PowerPC Linux version
.*__LINUXMIPS__ = defined if MIPS Linux version
__UNIX__ = defined if QNX or Linux version
MAKE = <name of file containing &makcmdup>
#endif
# clear &sysper.EXTENSIONS list
&sysper.EXTENSIONS:

# In general,
# set &sysper.EXTENSIONS list as follows
&sysper.EXTENSIONS: .exe .nlm .dsk .lan .exp &
            .lib .obj &
            .i &
            .asm .c .cpp .cxx .cc .for .pas .cob &
            .h .hpp .hxx .hh .fi .mif .inc
.millust end
.np
For Microsoft NMAKE compatibility (when you use the "ms" option), the
following default definitions are established.
.millust begin
# For Microsoft NMAKE compatibility switch,
# set &sysper.EXTENSIONS list as follows
&sysper.EXTENSIONS: .exe .obj .asm .c .cpp .cxx &
            .bas .cbl .for .f .f90 .pas .res .rc

%MAKEFLAGS=$(%MAKEFLAGS) $(__MAKEOPTS__)
MAKE=<name of file containing &makcmdup>
AS=ml
BC=bc
CC=cl
COBOL=cobol
CPP=cl
CXX=cl
FOR=fl
PASCAL=pl
RC=rc
&sysper.asm.exe:
    $(AS) $(AFLAGS) $*&sysper.asm
&sysper.asm.obj:
    $(AS) $(AFLAGS) /c $*&sysper.asm
&sysper.c.exe:
    $(CC) $(CFLAGS) $*&sysper.c
&sysper.c.obj:
    $(CC) $(CFLAGS) /c $*&sysper.c
&sysper.cpp.exe:
    $(CPP) $(CPPFLAGS) $*&sysper.cpp
&sysper.cpp.obj:
    $(CPP) $(CPPFLAGS) /c $*&sysper.cpp
&sysper.cxx.exe:
    $(CXX) $(CXXFLAGS) $*&sysper.cxx
&sysper.cxx.obj:
    $(CXX) $(CXXFLAGS) $*&sysper.cxx
&sysper.bas.obj:
    $(BC) $(BFLAGS) $*&sysper.bas
&sysper.cbl.exe:
    $(COBOL) $(COBFLAGS) $*&sysper.cbl, $*.exe;
&sysper.cbl.obj:
    $(COBOL) $(COBFLAGS) $*&sysper.cbl;
&sysper.f.exe:
    $(FOR) $(FFLAGS) $*&sysper.f
&sysper.f.obj:
    $(FOR) /c $(FFLAGS) $*&sysper.f
&sysper.f90.exe:
    $(FOR) $(FFLAGS) $*&sysper.f90
&sysper.f90.obj:
    $(FOR) /c $(FFLAGS) $*&sysper.f90
&sysper.for.exe:
    $(FOR) $(FFLAGS) $*&sysper.for
&sysper.for.obj:
    $(FOR) /c $(FFLAGS) $*&sysper.for
&sysper.pas.exe:
    $(PASCAL) $(PFLAGS) $*&sysper.pas
&sysper.pas.obj:
    $(PASCAL) /c $(PFLAGS) $*&sysper.pas
&sysper.rc.res:
    $(RC) $(RFLAGS) /r $*
.millust end
.pc
For OS/2, the
.id __MSDOS__
macro will be replaced by
.id __OS2__
and for Windows NT, the
.id __MSDOS__
macro will be replaced by
.id __NT__.
.np
For UNIX make compatibility (when you use the "u" option), the
following default definition is established.
.millust begin
&sysper.EXTENSIONS: .exe .obj .c .y .l .f

%MAKEFLAGS=$(%MAKEFLAGS) $(__MAKEOPTS__)
MAKE=<name of file containing &makcmdup>
YACC=yacc
YFLAGS=
LEX=lex
LFLAGS=
LDFLAGS=
CC=cl
FC=fl
&sysper.asm.exe:
    $(AS) $(AFLAGS) $*&sysper.asm
&sysper.c.exe:
    $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
&sysper.f.exe:
    $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $<
&sysper.c.obj:
    $(CC) $(CFLAGS) -c $<
&sysper.f.obj:
    $(FC) $(FFLAGS) -c $<
&sysper.y.obj:
    $(YACC) $(YFLAGS) $<
    $(CC) $(CFLAGS) -c y.tab.c
    del y.tab.c
    move y.tab.obj $@
&sysper.l.obj:
    $(LEX) $(LFLAGS) $<
    $(CC) $(CFLAGS) -c lex.yy.c
    del lex.yy.c
    move lex.yy.obj $@
&sysper.y.c:
    $(YACC) $(YFLAGS) $<
    move y.tab.c $@
&sysper.l.c:
    $(LEX) $(LFLAGS) $<
    move lex.yy.c $@

.millust end
The "r" option will disable these definitions before processing any
makefiles.
:OPT name='s'
.ix '&makcmdup options' 's'
silent mode - do not print commands before execution
.np
The "s" option is equivalent to the
.id &sysper.SILENT
directive.
:OPT name='sn'
.ix '&makcmdup options' 'sn'
noisy mode - always print commands before execution
.np
The "sn" option overrules all silencing controls.
It can be used to assist in debugging a makefile.
:OPT name='t'
.ix '&makcmdup options' 't'
touch files instead of executing commands
.np
.ix '&makcmdup' 'touch'
.ix 'touch'
.ix '&tchcmdup'
Sometimes there are changes which are purely cosmetic (adding a
comment to a source file) that will cause targets to be updated
needlessly thus wasting computer resources.
The "t" option will make files appear younger without altering their
contents.
The "t" option is useful but should be used with caution.
:OPT name='u'
.ix '&makcmdup options' 'u'
.ix '&makcmdup' 'UNIX compatibility mode'
.ix 'UNIX compatibility mode in &maksname'
UNIX compatibility mode
.np
The "u" option will indicate to &maksname that the line continuation
character should be a backslash "\" rather than an ampersand "&".
:OPT name='v'
.ix '&makcmdup options' 'v'
The "v" option enables a verbose listing of inline temporary files.
:OPT name='y'
.ix '&makcmdup options' 'y'
The "y" option enables the display of a progress line denoting which
dependent file has caused a target to be updated. This is a useful
option for helping to debug makefiles.
:OPT name='z'
.ix '&makcmdup options' 'z'
.ix '&makcmdup' 'target deletion prompt'
.ix 'target deletion prompt'
.ix '&makcmdup directives' '.HOLD'
.ix 'HOLD' '&makcmdup directive'
do not erase target after error/interrupt (disables prompting)
.np
The "z" option will indicate to &maksname that if an error or interrupt
occurs during makefile processing then the current target being made
should not be deleted.
The
.id &sysper.HOLD
directive in a makefile has the same effect as the "z" option.
:eOPTLIST.
.*
.section Special Macros
.*
.np
.ix '&makcmdup' 'special macros'
&makname has many different special macros.
Here are some of the simpler ones.
.begpoint $compact $break
:DTHD.Macro
:DDHD.Expansion
.point $$
.ix '&makcmdup special macros' '$$'
represents the character "$"
.point $#
.ix '&makcmdup special macros' '$#'
represents the character "#"
.point $@
.ix '&makcmdup special macros' '$@'
full file name of the target
.point $*
.ix '&makcmdup special macros' '$*'
target with the extension removed
.point $<
.ix '&makcmdup special macros' '$<'
list of all dependents
.point $?
.ix '&makcmdup special macros' '$?'
list of dependents that are younger than the target
.endpoint
.pc
The following macros are for more sophisticated makefiles.
.begpoint $break $setptnt 14
:DTHD.Macro
:DDHD.Expansion
.point __MSDOS__
This macro is defined in the MS/DOS environment.
.point __NT__
This macro is defined in the Windows NT environment.
.point __OS2__
This macro is defined in the OS/2 environment.
.point __LINUX__
This macro is defined in the Linux environment.
.point __QNX__
This macro is defined in the QNX environment.
.point __UNIX__
This macro is defined in the Linux or QNX environment.
.point __MAKEOPTS__
contains all of the command line options that &makcmdup was invoked
with except for any use of the "f" or "n" options.
.point __MAKEFILES__
contains the names of all of the makefiles processed at the time of
expansion (includes the file currently being processed)
.point MAKE
contains the full name of the file that contains &makcmdup.
.point __VERSION__
contains the wmake version.
.endpoint
.pc
The next three tables contain macros that are valid during execution
of command lists for explicit rules, implicit rules, and the
.id &sysper.ERROR
directive.
The expansion is presented for the following example:
.exam begin
a:\dir\target.ext : b:\dir1\dep1.ex1 c:\dir2\dep2.ex2
.exam end
.begpoint $compact $break
:DTHD.Macro
:DDHD.Expansion
.point $^@
.ix '&makcmdup special macros' '$^ form'
.ix '&makcmdup special macros' '$^@'
a:\dir\target.ext
.point $^*
.ix '&makcmdup special macros' '$^*'
a:\dir\target
.point $^&
.ix '&makcmdup special macros' '$^&'
target
.point $^.
.ix '&makcmdup special macros' '$^.'
target.ext
.point $^:
.ix '&makcmdup special macros' '$^:'
a:\dir\
.endpoint
.begpoint $compact $break
:DTHD.Macro
:DDHD.Expansion
.point $[@
.ix '&makcmdup special macros' '$[ form'
.ix '&makcmdup special macros' '$[@'
b:\dir1\dep1.ex1
.point $[*
.ix '&makcmdup special macros' '$[*'
b:\dir1\dep1
.point $[&
.ix '&makcmdup special macros' '$[&'
dep1
.point $[.
.ix '&makcmdup special macros' '$[.'
dep1.ex1
.point $[:
.ix '&makcmdup special macros' '$[:'
b:\dir1\
.endpoint
.begpoint $compact $break
:DTHD.Macro
:DDHD.Expansion
.point $]@
.ix '&makcmdup special macros' '$] form'
.ix '&makcmdup special macros' '$]@'
c:\dir2\dep2.ex2
.point $]*
.ix '&makcmdup special macros' '$]*'
c:\dir2\dep2
.point $]&
.ix '&makcmdup special macros' '$]&'
dep2
.point $].
.ix '&makcmdup special macros' '$].'
dep2.ex2
.point $]:
.ix '&makcmdup special macros' '$]:'
c:\dir2\
.endpoint
.*
.endlevel
.*
.section Dependency Declarations
.*
.np
.ix '&makcmdup' 'dependency'
.ix 'dependency'
In order for &makname to be effective, a list of file dependencies
must be declared.
.ix '&makcmdup' 'declarations'
.ix 'declarations'
.ix '&makcmdup' 'MAKEFILE'
.ix 'MAKEFILE'
The declarations may be entered into a text file of any name but
&maksname will read a file called "MAKEFILE" by default if it is
invoked as follows:
.ix 'invoking &makname'
.ix '&makcmdup command line' 'invoking &makcmdup'
.exam begin
C>&makcmd
.exam end
.pc
.ix '&makcmdup options' 'f'
If you want to use a file that is not called "MAKEFILE" then the
command line option "f" will cause &maksname to read the specified file
instead of the default "MAKEFILE".
.ix 'invoking &makname'
.ix '&makcmdup command line' 'invoking &makcmdup'
.exam begin
C>&makcmd /f myfile
.exam end
.np
We will now go through an example to illustrate how &maksname may be
used for a simple application.
Suppose we have an input file, a report file, and a report generator
program then we may declare a dependency as follows:
.ix '&makcmdup' 'MAKEFILE comments'
.ix 'MAKEFILE comments'
.millust begin
#
# (a comment in a makefile starts with a "#")
# simple dependency declaration
#
balance.lst : ledger.dat
        doreport
.millust end
.pc
Note that the dependency declaration starts at the beginning of a line
while commands always have at least one blank or tab before them.
.ix '&makcmdup' 'single colon explicit rule'
.ix 'single colon explicit rule'
.ix '&makcmdup' '":" explicit rule'
.ix 'colon (:)' 'explicit rule in &makcmdup'
.ix '&makcmdup' 'explicit rule'
.ix 'explicit rule'
This form of a dependency declaration is called an
.us explicit rule.
.ix '&makcmdup' 'target'
.ix 'target'
The file "BALANCE.LST" is called the
.us target
of the rule.
.ix '&makcmdup' 'rule command list'
.ix 'rule command list'
.ix '&makcmdup' 'dependent'
.ix 'dependent'
The
.us dependent
of the rule is the file "LEDGER.DAT" while "DOREPORT" forms one line
of the
.us rule command list.
The dependent is separated from the target by a colon.
.hint
A good habit to develop is to always put spaces around the colon so
that it will not be confused with drive specifications (e.g., a:).
.ehint
.np
The explicit rule declaration indicates to &maksname that the program
"DOREPORT" should be executed if "LEDGER.DAT" is younger than
"BALANCE.LST" or if "BALANCE.LST" does not yet exist.
In general, if the dependent file has a more recent modification date
and time than the target file then &makname will execute the
specified command.
.ix '&makcmdup' 'dependent'
.ix 'dependent'
.ix '&makcmdup' 'Feldman, S.I.'
.ix 'Feldman, S.I.'
.ix '&makcmdup' 'Bell Laboratories'
.ix 'Bell Laboratories'
.ix '&makcmdup' 'UNIX'
.ix 'UNIX'
.remark
The terminology employed here is used by S.I.Feldman of Bell
Laboratories in
.us Make - A Program for Maintaining Computer Programs.
http://www.softlab.ntua.gr/facilities/documentation/unix/docs/make.txt
has a copy of this seminal article.
Confusion often arises from the use of the word "dependent".
In this context, it means "a subordinate part".
In the example, "LEDGER.DAT" is a subordinate part of the report
"BALANCE.LST".
.eremark
.*
.section Multiple Dependents
.*
.np
Suppose that our report "BALANCE.LST" becomes out-of-date if any of
the files "LEDGER.DAT", "SALES.DAT" or "PURCHASE.DAT" are modified.
We may modify the dependency rule as follows:
.millust begin
#
# multiple dependents rule
#
balance.lst : ledger.dat sales.dat purchase.dat
        doreport
.millust end
.pc
.ix '&makcmdup' 'multiple dependents'
.ix 'multiple dependents'
This is an example of a rule with multiple dependents.
In this situation, the program "DOREPORT" should be executed if any of
"LEDGER.DAT", "SALES.DAT" or "PURCHASE.DAT" are younger than
"BALANCE.LST" or if "BALANCE.LST" does not yet exist.
In cases where there are multiple dependents,
if any of the dependent files has a more recent modification date and
time than the target file then &makname will execute the specified
command.
.*
.section Multiple Targets
.*
.np
Suppose that the "DOREPORT" program produces two reports.
If both of these reports require updating as a result of modification
to the dependent files, we could change the rule as follows:
.millust begin
#
# multiple targets and multiple dependents rule
#
balance.lst summary.lst : ledger.dat sales.dat purchase.dat
        doreport
.millust end
.pc
.ix '&makcmdup' 'multiple targets'
.ix 'multiple targets'
Suppose that you entered the command:
.millust begin
&makcmd
.millust end
.pc
which causes &maksname to start processing the rules described in
"MAKEFILE".
In the case where multiple targets are listed in the makefile,
&maksname will, by default, process only the first target it
encounters.
In the example, Make will check the date and time of "BALANCE.LST"
against its dependents since this is the first target listed.
.np
To indicate that some other target should be processed, the target is
specified as an argument to the Make command.
.exam begin
&makcmd summary.lst
.exam end
.np
There are a number of interesting points to consider:
.autonote
.note
By default, Make will only check that the target file exists after the
command ("DOREPORT" in this example) is executed.
It does not check that the target's time-stamp shows it to be younger.
If the target file does not exist after the command has been executed,
an error is reported.
.note
There is no guarantee that the command you have specified does update
the target file.
In other words, simply because you have stated a dependency does not
mean that one exists.
.note
Furthermore, it is not implied that other targets in our list will not
be updated.
In the case of our example, you can assume that we have designed the
"doreport" command to update both targets.
.endnote
.*
.section Multiple Rules
.*
.np
A makefile may consist of any number of rules.
Note that the following:
.millust begin
target1 target2 : dependent1 dependent2 dependent3
        command list
.millust end
.pc
is equivalent to:
.millust begin
target1 : dependent1 dependent2 dependent3
        command list

target2 : dependent1 dependent2 dependent3
        command list
.millust end
.pc
Also, the rules may depend on the targets of other rules.
.millust begin
#
# rule 1: this rule uses rule 2
#
balance.lst summary.lst : ledger.dat sales.dat purchase.dat
        doreport

#
# rule 2: used by rules 1 and 3
#
sales.dat : canada.dat england.dat usa.dat
        dosales

#
# rule 3: this rule uses rule 2
#
year.lst : ledger.dat sales.dat purchase.dat
        doyearly
.millust end
.pc
.ix '&makcmdup' '":" behaviour'
.ix 'colon (:)' 'behaviour in &makcmdup'
The dependents are checked to see if they are the targets of any other
rules in the makefile in which case they are updated.
This process of updating dependents that are targets in other rules
continues until a rule is reached that has only simple dependents that
are not targets of rules.
At this point, if the target does not exist or if any of the
dependents is younger than the target then the command list associated
with the rule is executed.
.hint
The term "updating", in this context, refers to the process of
checking the time-stamps of dependents and running the specified
command list whenever they are out-of-date.
Whenever a dependent is the target of some other rule, the dependent
must be brought up-to-date first.
Stated another way, if "A" depends on "B" and "B" depends on "C" and
"C" is younger than "B" then we must update "B" before we update "A".
.ehint
.pc
&maksname will check to ensure that the target exists after its
associated command list is executed.
The target existence checking may be disabled in two ways:
.autopoint
.point
use the command line option "c"
.point
.ix '&makcmdup directives' '.NOCHECK'
.ix 'NOCHECK' '&makcmdup directive'
use the
.id &sysper.NOCHECK
directive.
.endpoint
.np
The rule checking returns to the previous rule that had the target as
a dependent.
Upon returning to the rule, the command list is executed if the target
does not exist or if any of the updated dependents are now younger
than the target.
If you were to type:
.millust begin
&makcmd
.millust end
.pc
here are the steps that would occur with the previous makefile:
.millust begin
update(balance.lst) (rule 1)

  update(ledger.dat)            (not a target)
  update(sales.dat)             (found rule 2)

    update(canada.dat)          (not a target)
    update(england.dat)         (not a target)
    update(usa.dat)             (not a target)
    IF sales.dat does not exist                      OR
       any of (canada.dat,england.dat,usa.dat)
         is younger than sales.dat
    THEN execute "dosales"

  update(purchase.dat)          (not a target)
  IF balance.lst does not exist                      OR
     any of (ledger.dat,sales.dat,purchase.dat)
       is younger than (balance.lst)
  THEN execute "doreport"
.millust end
.pc
The third rule in the makefile will not be included in this update
sequence of steps.
Recall that the default target that is "updated" is the first target
in the first rule encountered in the makefile.
This is the default action taken by &maksname when no target is
specified on the command line.
If you were to type:
.millust begin
&makcmd year.lst
.millust end
.pc
then the file "YEAR.LST" would be updated.
As &maksname reads the rules in "MAKEFILE", it discovers that updating
"YEAR.LST" involves updating "SALES.DAT".
The update sequence is similar to the previous example.
.*
.section Command Lists
.*
A command list is a sequence of one or more commands.
Each command is preceded by one or more spaces or tabs.
Command lists may also be used to construct inline files "on the fly".
Macros substitute in command lists and in inline files.
An inline file is introduced by "<<" in a command in a command list.
Data to insert into that file is placed (left-justified) in the command list.
The data is terminated by "<<" in the first column.
It is not possible to place a line which starts "<<" in an inline file.
More than one inline file may be created in a command.
Data for each is placed in order of reference in the command.
.pc
In building the Open Watcom system, it is sometimes necessary to do some text
substitution with a program called vi. This needs a file of instructions.
The following simplifies an example used to build Open Watcom
so that inline files may be shown.
Without inline files, this is done as:
.millust begin
$(dllname).imp : $(dllname).lbc ../../trimlbc.vi
    cp $(dllname).lbc $(dllname).imp
    $(vi) -s ../../trimlbc.vi $(dllname).imp

where trimlbc.vi consists of
set magic
set magicstring = ()
atomic
%s/\.dll'/'/
%s/^(\+\+')(.*)('\.'.*')\.[0-9]+$/\1\2\3..'\2'/
x
.millust end
A doubled "$" to produce a single dollar is notable when an inline file is used:
.millust begin
$(dllname).imp : $(dllname).lbc
    cp $(dllname).lbc $(dllname).imp
    $(vi) -s << $(dllname).imp
set magic
set magicstring = ()
atomic
%s/\.dll'/'/
%s/^(\+\+')(.*)('\.'.*')\.[0-9]+$$/\1\2\3..'\2'/
x
<<
.millust end
A filename may follow a "<<" on a command line to cause a file with that
name to be created. (Otherwise, '&makcmdup' chooses a name.)
"keep" or "nokeep" may follow a terminating "<<" to show what to do
with the file after usage. The default is "nokeep" which zaps it.
.*
.section Final Commands (.AFTER)
.*
.ix '&makcmdup directives' '.AFTER'
.ix 'AFTER' '&makcmdup directive'
The
.id &sysper.AFTER
directive specifies commands for &maksname to run after it has done all other commands.
See the section entitled
:HDREF refid='cld'.
for a full description of its use.
.*
.section Ignoring Dependent Timestamps (.ALWAYS)
.*
.ix '&makcmdup directives' '.ALWAYS'
.ix 'ALWAYS' '&makcmdup directive'
The
.id &sysper.ALWAYS
directive indicates to &maksname that the target should always be updated
regardless of the timestamps of its dependents.
.millust begin
#
# .always directive
#

foo : bar .always
    wtouch $@
.millust end
.pc
foo is updated each time &maksname is run.
.*
.section Automatic Dependency Detection (.AUTODEPEND)
.*
.np
.ix '&makcmdup directives' '.AUTODEPEND'
.ix 'AUTODEPEND' '&makcmdup directive'
Explicit listing of dependencies in a makefile can often be tedious in
the development and maintenance phases of a project.
The &cmpname compiler will insert dependency information into the
object file as it processes source files so that a complete snapshot
of the files necessary to build the object file are recorded.
Since all files do not have dependency information contained within
them in a standard form, it is necessary to indicate to &maksname when
dependencies are present.
.np
To illustrate the use of the
.id &sysper.AUTODEPEND
directive, we will show its use in an implicit rule and in an explicit
rule.
.millust begin
#
# .AUTODEPEND example
#
&sysper.&langsuff..obj: .AUTODEPEND
        &compcmd $[* $(compile_options)

test&exe : a.obj b.obj c.obj test.res
        &lnkcmd FILE a.obj, b.obj, c.obj
        &wrccmd /q /bt=windows test.res test&exe

test.res : test.rc test.ico .AUTODEPEND
        &wrccmd /ad /q /bt=windows /r $[@ $^@
.millust end
.np
In the above example, &maksname will use the contents of the object
file to determine whether the object file has to be built during
processing.  The &wrcname can also insert dependency
information into a resource file that can be used by &maksname..
.*
.section Initial Commands (.BEFORE)
.*
.ix '&makcmdup directives' '.BEFORE'
.ix 'BEFORE' '&makcmdup directive'
The
.id &sysper.BEFORE
directive specifies commands for &maksname to run before it does any other command.
See the section entitled
:HDREF refid='cld'.
for a full description of its use.
.*
.section Disable Implicit Rules (.BLOCK)
.*
.ix '&makcmdup directives' '.BLOCK'
.ix 'BLOCK' '&makcmdup directive'
The
.id &sysper.BLOCK
directive and the "b" command line option are alternative controls to
cause implicit rules to be ignored.
See the section entitled
:HDREF refid='clo'.
for a full description of its use.
.*
.section Ignoring Errors (.CONTINUE)
.*
.ix '&makcmdup directives' '.CONTINUE'
.ix 'CONTINUE' '&makcmdup directive'
The
.id &sysper.CONTINUE
directive and the "b" command line option are alternative controls to
cause failing commands to be ignored.
See the section entitled
:HDREF refid='clo'.
for a full description of its use.
.millust begin
#
# .continue example
#

&sysper.continue

all: bad good
    @%null

bad:
    false

good:
    touch $@
.millust end
.pc
Although the command list for bad fails, that for good is done.
Without the directive, good is not built.
.*
.section Default Command List (.DEFAULT)
.*
.ix '&makcmdup directives' '.DEFAULT'
.ix 'DEFAULT' '&makcmdup directive'
The
.id &sysper.DEFAULT
directive provides a default command list for those targets which lack one.
See the section entitled
:HDREF refid='cld'.
for a full description of its use.
.millust begin
#
# .default example
#

&sysper.default
    @echo Using default rule to update target "$@"
    @echo because of dependent(s) "$<"
    wtouch $@

all: foo

foo:
    wtouch foo
.millust end
.pc
"all" has no command list. The one supplied to the default directive
is executed instead.
.*
.section Erasing Targets After Error (.ERASE)
.*
.np
.ix '&makcmdup directives' '.ERASE'
.ix 'ERASE' '&makcmdup directive'
.ix '&makcmdup' 'return codes'
.ix 'return codes'
Most operating system utilities and programs have special return codes
that indicate error conditions.
&makname will check the return code for every command executed.
If the return code is non-zero, &maksname will stop processing the
current rule and optionally delete the current target being updated.
By default, &maksname will prompt for deletion of the current target.
The
.id &sysper.ERASE
directive indicates to &maksname that the target should be deleted if
an error occurs during the execution of the associated command list.
No prompt is issued in this case.
Here is an example of the
.id &sysper.ERASE
directive:
.millust begin
#
# .ERASE example
#
&sysper.ERASE
balance.lst : ledger.dat sales.dat purchase.dat
        doreport
.millust end
.pc
If the program "DOREPORT" executes and its return code is non-zero
then &maksname will attempt to delete "BALANCE.LST".
.*
.section Error Action (.ERROR)
.*
.ix '&makcmdup directives' '.ERROR'
.ix 'ERROR' '&makcmdup directive'
The
.id &sysper.ERROR
directive supplies a command list for error conditions.
See the section entitled
:HDREF refid='cld'.
for a full description of its use.
.millust begin
#
# .error example
#

&sysper.error:
        @echo it is good that "$@" is known

all : .symbolic
        false
.millust end
.*
.section Ignoring Target Timestamp (.EXISTSONLY)
.*
.ix '&makcmdup directives' '.EXISTSONLY'
.ix 'EXISTSONLY' '&makcmdup directive'
The
.id &sysper.EXISTSONLY
directive indicates to &maksname that the target should not be updated if it
already exists, regardless of its timestamp.
.millust begin
#
# .existsonly directive
#

foo: .existsonly
        wtouch $@
.millust end
.pc
If absent, this file creates foo; if present, this file does nothing.
.*
.section Specifying Explicitly Updated Targets (.EXPLICIT)
.*
.ix '&makcmdup directives' '.EXPLICIT'
.ix 'EXPLICIT' '&makcmdup directive'
The
.id &sysper.EXPLICIT
directive may me used to specify a target that needs to be explicitly
updated. Normally, the first target in a makefule will be implicitly updated
if no target is specified on &maksname command line. The
.id &sysper.EXPLICIT
directive prevents this, and is useful for instance when creating files
designed to be included for other make files.
.millust begin
#
# .EXPLICIT example
#
target : .symbolic .explicit
        @echo updating first target

next : .symbolic
        @echo updating next target
.millust end
.pc
In the above example, &maksname will not automatically update "target",
despite the fact that it is the first one listed.
.*
.section *refid=extensions Defining Recognized File Extensions (.EXTENSIONS)
.*
.ix '&makcmdup directives' '.EXTENSIONS'
.ix 'EXTENSIONS' '&makcmdup directive'
The
.id &sysper.EXTENSIONS
directive and its synonym, the
.id &sysper.SUFFIXES
directive declare which extensions are allowed to be used in implicit
rules and how these extensions are ordered.
.id &sysper.EXTENSIONS
is the traditional Watcom name;
.id &sysper.SUFFIXES
is the corresponding POSIX name.

The default
.id &sysper.EXTENSIONS
declaration is:
.code begin
&sysper.EXTENSIONS:
&sysper.EXTENSIONS: .exe .nlm .dsk .lan .exp .lib .obj &
             .i .asm .c .cpp .cxx .cc .for .pas .cob &
             .h .hpp .hxx .hh .fi .mif .inc
.code end
.pc
A
.id &sysper.EXTENSIONS
directive with an empty list will clear the
.id &sysper.EXTENSIONS
list and any previously defined implicit rules.
Any subsequent
.id &sysper.EXTENSIONS
directives will add extensions to the end of the list.
.hint
The default
.id &sysper.EXTENSIONS
declaration could have been coded as:
.np
:cmt. .millust
&sysper.EXTENSIONS:
.br
&sysper.EXTENSIONS: .exe
.br
&sysper.EXTENSIONS: .nlm .dsk .lan .exp
.br
&sysper.EXTENSIONS: .lib
.br
&sysper.EXTENSIONS: .obj
.br
&sysper.EXTENSIONS: .i .asm .c .cpp .cxx .cc
.br
&sysper.EXTENSIONS: .for .pas .cob
.br
&sysper.EXTENSIONS: .h .hpp .hxx .hh .fi .mif .inc
.br
&sysper.EXTENSIONS: .inc
:cmt. .emillust
.np
with identical results.
.ehint
.pc
&maksname will not allow any implicit rule declarations that use
extensions that are not in the current
.id &sysper.EXTENSIONS
list.
.millust begin
#
# .extensions and .suffixes directives
#

&sysper.suffixes : # Clear list
&sysper.extensions : .foo .bar

&sysper.bar.foo:
    copy $< $@

fubar.foo:

fubar.bar: .existsonly
    wtouch $@
.millust end
.pc
The first time this example runs, &maksname creates fubar.foo.
This example always ensures that fubar.foo is a copy of fubar.bar.
Note the implicit connection beween the two files.
.*
.section Approximate Timestamp Matching (.FUZZY)
.*
.ix '&makcmdup directives' '.FUZZY'
.ix 'FUZZY' '&makcmdup directive'
The
.id &sysper.FUZZY
directive allows
.ix '&makcmdup directives' '.AUTODEPEND'
.ix 'AUTODEPEND' '&makcmdup directive'
.id &sysper.AUTODEPEND
times to be out by a minute without considering a target out of date.
It is only useful in conjunction with the
.ix '&makcmdup directives' '.JUST_ENOUGH'
.ix 'JUST_ENOUGH' '&makcmdup directive'
.id &sysper.JUST_ENOUGH
directive when &maksname is calculating the timestamp to set the target to.
.*
.section Preserving Targets After Error (.HOLD)
.*
.np
.ix '&makcmdup directives' '.HOLD'
.ix 'HOLD' '&makcmdup directive'
.ix '&makcmdup' 'return codes'
.ix 'return codes'
Most operating system utilities and programs have special return codes
that indicate error conditions.
&makname will check the return code for every command executed.
If the return code is non-zero, &maksname will stop processing the
current rule and optionally delete the current target being updated.
By default, &maksname will prompt for deletion of the current target.
The
.id &sysper.HOLD
directive indicates to &maksname that the target should not be deleted
if an error occurs during the execution of the associated command
list.
No prompt is issued in this case.
.ix '&makcmdup directives' '.PRECIOUS'
.ix 'PRECIOUS' '&makcmdup directive'
The
.id &sysper.HOLD
directive is similar to
.id &sysper.PRECIOUS
but applies to all targets listed in the makefile.
Here is an example of the
.id &sysper.HOLD
directive:
.millust begin
#
# .HOLD example
#
&sysper.HOLD
balance.lst : ledger.dat sales.dat purchase.dat
        doreport
.millust end
.pc
If the program "DOREPORT" executes and its return code is non-zero
then &maksname will not delete "BALANCE.LST".
.*
.section Ignoring Return Codes (.IGNORE)
.*
.np
.ix '&makcmdup directives' '.IGNORE'
.ix '&makcmdup command prefix' '-'
.ix 'IGNORE' '&makcmdup directive'
.ix '&makcmdup' 'ignoring return codes'
.ix 'ignoring return codes'
Some programs do not have meaningful return codes so for these
programs we want to ignore the return code completely.
There are different ways to ignore return codes namely,
.autopoint
.point
.ix '&makcmdup options' 'i'
use the command line option "i"
.point
put a "&minus" in front of specific commands, or
.point
use the
.id &sysper.IGNORE
directive.
.endpoint
.np
In the following example, the rule:
.millust begin
#
# ignore return code example
#
balance.lst : ledger.dat sales.dat purchase.dat
        -doreport
.millust end
.pc
will ignore the return status from the program "DOREPORT".
Using the dash in front of the command is the preferred method for
ignoring return codes because it allows &maksname to check all the
other return codes.
.np
The
.id &sysper.IGNORE
directive is used as follows:
.millust begin
#
# .IGNORE example
#
&sysper.IGNORE
balance.lst : ledger.dat sales.dat purchase.dat
        doreport
.millust end
.pc
Using the
.id &sysper.IGNORE
directive will cause &maksname to ignore the return code for every
command.
The "i" command line option and the
.id &sysper.IGNORE
directive prohibit &maksname from performing any error checking on the
commands executed and, as such, should be used with caution.
.np
Another way to handle non-zero return codes is to continue processing
targets which do not depend on the target that had a non-zero return
code during execution of its associated command list.
There are two ways of indicating to &maksname that processing should
continue after a non-zero return code:
.autopoint
.point
use the command line option "k"
.point
use the
.id &sysper.CONTINUE
directive.
.endpoint
.*
.section Minimising Target Timestamp (.JUST_ENOUGH)
.*
.ix '&makcmdup directives' '.JUST_ENOUGH'
.ix 'JUST_ENOUGH' '&makcmdup directive'
The
.id &sysper.JUST_ENOUGH
directive is equivalent to the "j" command line option.
The timestamps of created targets are set to be the same as those of their
youngest dependendents.
.millust begin
#
# .JUST_ENOUGH example
#

&sysper.just_enough

&sysper.c.exe:
        wcl386 -zq $<

hello.exe:
.millust end
.pc
hello.exe is given the same timestamp as hello.c, and not the usual timestamp
corresponding to when hello.exe was built.
.*
.section Updating Targets Multiple Times (.MULTIPLE)
.*
.ix '&makcmdup directives' '.MULTIPLE'
.ix 'MULTIPLE' '&makcmdup directive'
The
.id &sysper.MULTIPLE
directive is used to update a target multiple times. Normally, &maksname
will only update each target once while processing a makefile. The
.id &sysper.MULTIPLE
directive is useful if a target needs to be updated more than once, for
instance in case the target is destroyed during processing of other targets.
Consider the following example:
.millust begin
#
# example not using .multiple
#

all: targ1 targ2

target:
        wtouch target

targ1: target
        rm target
        wtouch targ1

targ2: target
        rm target
        wtouch targ2
.millust end
.pc
This makefile will fail because "target" is destroyed when updating "targ1",
and later is implicitly expected to exist when updating "targ2". Using the
.id &sysper.MULTIPLE
directive will work around this problem:
.millust begin
#
# .MULTIPLE example
#

all : targ1 targ2

target : .multiple
        wtouch target

targ1 : target
        rm target
        wtouch targ1

targ2 : target
        rm target
        wtouch targ2
.millust end
.pc
Now &maksname will attempt to update "target" again when updating "targ2",
discover that "target" doesn't exist, and recreate it.
.*
.section Ignoring Target Timestamp (.NOCHECK)
.*
.ix '&makcmdup directives' '.NOCHECK'
.ix 'NOCHECK' '&makcmdup directive'
The
.id &sysper.NOCHECK
directive is used to disable target existence checks in a makefile.
See the section entitled
:HDREF refid='clo'.
for a full description of its use.
.*
.section Cache Search Path (.OPTIMIZE)
.*
.ix '&makcmdup directives' '.OPTIMIZE'
.ix 'OPTIMIZE' '&makcmdup directive'
The
.id &sysper.OPTIMIZE
directive and the equivalent "o" command line option cause &maksname
to use a circular path search.
If a file is found in a particular directory, that directory will be
the first searched for the next file.

See the section entitled
:HDREF refid='clo'.
for a full description of its use.
.*
.section Preserving Targets (.PRECIOUS)
.*
.np
.ix '&makcmdup directives' '.PRECIOUS'
.ix 'PRECIOUS' '&makcmdup directive'
.ix '&makcmdup' 'return codes'
.ix 'return codes'
Most operating system utilities and programs have special return codes
that indicate error conditions.
&makname will check the return code for every command executed.
If the return code is non-zero, &maksname will stop processing the
current rule and optionally delete the current target being updated.
If a file is precious enough that this treatment of return codes is
not wanted then the
.id &sysper.PRECIOUS
directive may be used.
The
.id &sysper.PRECIOUS
directive indicates to &maksname that the target should not be deleted
if an error occurs during the execution of the associated command
list.
Here is an example of the
.id &sysper.PRECIOUS
directive:
.code begin
#
# .PRECIOUS example
#
balance summary : sales.dat purchase.dat .PRECIOUS
        doreport
.code end
.pc
If the program "DOREPORT" executes and its return code is non-zero
then &maksname will not attempt to delete "BALANCE" or "SUMMARY".
If only one of the files is precious then the makefile could be coded
as follows:
.millust begin
#
# .PRECIOUS example
#
balance : .PRECIOUS
balance summary : sales.dat purchase.dat
        doreport
.millust end
.pc
The file "BALANCE.LST" will not be deleted if an error occurs while
the program "DOREPORT" is executing.
.*
.section Name Command Sequence (.PROCEDURE)
.*
.ix '&makcmdup directives' '.PROCEDURE'
.ix 'PROCEDURE' '&makcmdup directive'
The
.id &sysper.PROCEDURE
directive may be used to construct "procedures" in a makefile.
.millust begin
#
# .procedure example
#

all: .symbolic
    @%make proc

proc: .procedure
    @echo Executing procedure "proc"
.millust end
.*
.section Re-Checking Target Timestamp (.RECHECK)
.*
.ix '&makcmdup directives' '.RECHECK'
.ix 'RECHECK' '&makcmdup directive'
Make will re-check the target's timestamp, rather than assuming it was updated
by its command list. This is useful if the target is built by another make-
style tool, as in the following example:
.millust begin
#
# .RECHECK example
#
foo.gz : foo
        gzip foo

foo : .ALWAYS .RECHECK
        nant -buildfile:foo.build
.millust end
.pc
foo's command list will always be run, but foo will only be compressed if the
timestamp is actually changed.
.*
.section Suppressing Terminal Output (.SILENT)
.*
.np
.ix '&makcmdup directives' '.SILENT'
.ix '&makcmdup command prefix' '@'
.ix 'SILENT' '&makcmdup directive'
As commands are executed, &makname will print out the current command
before it is executed.
.ix '&makcmdup' 'suppressing output'
.ix 'suppressing output'
It is possible to execute the makefile without having the commands
printed.
There are three ways to inhibit the printing of the commands before
they are executed, namely:
.autopoint
.point
.ix '&makcmdup options' 's'
use the command line option "s"
.point
put an "@" in front of specific commands, or
.point
use the
.id &sysper.SILENT
directive.
.endpoint
.np
In the following example, the rule:
.millust begin
#
# silent command example
#
balance summary : ledger.dat sales.dat purchase.dat
        @doreport
.millust end
.pc
will prevent the string "doreport" from being printed on the
screen before the command is executed.
.np
The
.id &sysper.SILENT
directive is used as follows:
.millust begin
#
# .SILENT example
#
&sysper.SILENT
balance summary : ledger.dat sales.dat purchase.dat
        doreport
.millust end
.pc
Using the
.id &sysper.SILENT
directive or the "s" command line option will inhibit the printing of
all commands before they are executed.
The "sn" command line option can be used to veto any silencing control.
.np
At this point, most of the capability of &maksname may be realized.
Methods for making makefiles more succinct will be discussed.
.*
.section Defining Recognized File Extensions (.SUFFIXES)
.*
.ix '&makcmdup directives' '.SUFFIXES'
.ix 'SUFFIXES' '&makcmdup directive'
The
.id &sysper.SUFFIXES
directive declares which extensions are allowed to be used in implicit
rules and how these extensions are ordered. It is a synonym for the
.id &sysper.EXTENSIONS
directive.  See the section entitled
:HDREF refid='extensions'.
for a full description of both directives.
.*
.section Targets Without Any Dependents (.SYMBOLIC)
.*
.np
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
There must always be at least one target in a rule but it is not
necessary to have any dependents.
If a target does not have any dependents, the command list associated
with the rule will always be executed if the target is updated.
.np
You might ask, "What may a rule with no dependents be used for?".
A rule with no dependents may be used to describe actions that are
useful for the group of files being maintained.
Possible uses include backing up files, cleaning up files, or printing
files.
.np
To illustrate the use of the
.id &sysper.SYMBOLIC
directive, we will add two new rules to the previous example.
First, we will omit the
.id &sysper.SYMBOLIC
directive and observe what will happen when it is not present.
.millust begin
#
# rule 4: backup the data files
#
backup :
        echo "insert backup disk"
        pause
        copy *.dat a:
        echo "backup complete"
.millust break
#
# rule 5: cleanup temporary files
#
cleanup :
        del *.tmp
        del \tmp\*.*
.millust end
.pc
and then execute the command:
.millust begin
&makcmd backup
.millust end
.pc
&maksname will execute the command list associated with the "backup"
target and issue an error message indicating that the file "BACKUP"
does not exist after the command list was executed.
The same thing would happen if we typed:
.millust begin
&makcmd cleanup
.millust end
.pc
In this makefile we are using "backup" and "cleanup" to represent
actions we want performed.
The names are not real files but rather they are symbolic names.
This special type of target may be declared with the
.id &sysper.SYMBOLIC
directive.
This time, we show rules 4 and 5 with the appropriate addition of
.id &sysper.SYMBOLIC
directives.
.millust begin
#
# rule 4: backup the data files
#
backup : .SYMBOLIC
        echo "insert backup disk"
        pause
        copy *.dat a:
        echo "backup complete"
.millust break
#
# rule 5: cleanup temporary files
#
cleanup : .SYMBOLIC
        del *.tmp
        del \tmp\*.*
.millust end
.pc
The use of the
.id &sysper.SYMBOLIC
directive indicates to &maksname that the target should always be
updated internally after the command list associated with the rule has
been executed.
A short form for the common idiom of singular
.id &sysper.SYMBOLIC
targets like:
.millust begin
target : .SYMBOLIC
        commands
.millust end
.pc
is:
.millust begin
target
        commands
.millust end
.pc
This kind of target definition is useful for many types of management
tasks that can be described in a makefile.
.*
.section Macros
.*
.np
.ix '&makcmdup' 'macros'
.ix 'macros'
&makname has a simple macro facility that may be used to improve
makefiles by making them easier to read and maintain.
.ix '&makcmdup' 'macro identifier'
A macro identifier may be composed from a string of alphabetic
characters and numeric characters.
The underscore character is also allowed in a macro identifier.
If the macro identifier starts with a "%" character, the macro
identifier represents an environment variable.
For instance, the macro identifier "%path" represents the environment
variable "path".
.begpoint $compact $setptnt 16
:DTHD.Macro identifiers
:DDHD.Valid?
.point 2morrow
yes
.point stitch_in_9
yes
.point invalid~~id
no
.point 2b_or_not_2b
yes
.point %path
yes
.point reports
yes
.point !@#*%
no
.endpoint
.np
We will use a programming example to show how macros are used.
The programming example involves four &lang source files and two
&header files.
Here is the initial makefile (before macros):
.millust begin
#
# programming example
# (before macros)
#
plot&exe : main.obj input.obj calc.obj output.obj
        &lnkcmd @plot

main.obj : main.&langsuff defs.&hdrsuff globals.&hdrsuff
        &compcmd main &compopt &warnopt

calc.obj : calc.&langsuff defs.&hdrsuff globals.&hdrsuff
        &compcmd calc &compopt &warnopt

.millust break
input.obj : input.&langsuff defs.&hdrsuff globals.&hdrsuff
        &compcmd input &compopt &warnopt

output.obj : output.&langsuff defs.&hdrsuff globals.&hdrsuff
        &compcmd output &compopt &warnopt
.millust end
.np
Macros become useful when changes must be made to makefiles.
If the programmer wanted to change the compiler options for the
different compiles, the programmer would have to make a global
change to the makefile.
With this simple example, it is quite easy to make the change but try
to imagine a more complex example with different programs having
similar options.
The global change made by the editor could cause problems by changing
the options for other programs.
A good habit to develop is to define macros for any programs that have
command line options.
In our example, we would change the makefile to be:
.millust begin
#
# programming example
# (after macros)
#
link_options =
compiler = &compcmd
compile_options = &compopt &warnopt

plot&exe : main.obj input.obj calc.obj output.obj
        &lnkcmd $(link_options) @plot

main.obj : main.&langsuff defs.&hdrsuff globals.&hdrsuff
        $(compiler) main $(compile_options)

.millust break
calc.obj : calc.&langsuff defs.&hdrsuff globals.&hdrsuff
        $(compiler) calc $(compile_options)

input.obj : input.&langsuff defs.&hdrsuff globals.&hdrsuff
        $(compiler) input $(compile_options)

output.obj : output.&langsuff defs.&hdrsuff globals.&hdrsuff
        $(compiler) output $(compile_options)
.millust end
.pc
A macro definition consists of a macro identifier starting on the
beginning of the line followed by an "=" which in turn is followed by
the text to be replaced.
A macro may be redefined, with the latest declaration being used for
subsequent expansions (no warning is given upon redefinition of a
macro).
The replacement text may contain macro references.
.np
A macro reference may occur in two forms.
The previous example illustrates one way to reference macros whereby
the macro identifier is delimited by "$(" and ")".
The parentheses are optional so the macros "compiler" and
"compile_options" could be referenced by:
.millust begin
main.obj : main.&langsuff defs.&hdrsuff globals.&hdrsuff
        $compiler main $compile_options
.millust end
.pc
Certain ambiguities may arise with this form of macro reference.
For instance, examine this makefile fragment:
.exam begin
temporary_dir = \tmp\
temporary_file = $temporary_dirtmp000.tmp
.exam end
.pc
The intention of the declarations is to have a macro that will expand
into a file specification for a temporary file.
&maksname will collect the largest identifier possible before macro
expansion occurs.
The macro reference is followed by text that looks like part of the
macro identifier ("tmp000") so the macro identifier that will be
referenced will be "temporary_dirtmp000".
The incorrect macro identifier will not be defined so an error message
will be issued.
.np
If the makefile fragment was:
.millust begin
temporary_dir = \tmp\
temporary_file = $(temporary_dir)tmp000.tmp
.millust end
.pc
there would be no ambiguity.
The preferred way to reference macros is to enclose the macro
identifier by "$(" and ")".
.np
Macro references are expanded immediately on dependency lines (and
thus may not contain references to macros that have not been defined)
but other macro references have their expansion deferred until they
are used in a command.
In the previous example, the macros "link_options", "compiler", and
"compile_options" will not be expanded until the commands that
reference them are executed.
.np
Another use for macros is to replace large amounts of text with a much
smaller macro reference.
In our example, we only have two &header files but suppose we had very
many &header files.
Each explicit rule would be very large and difficult to read and
maintain.
We will use the previous example makefile to illustrate this use of
macros.
.millust begin
#
# programming example
# (with more macros)
#
link_options =
compiler = &compcmd
compile_options = &compopt &warnopt

&header._files = defs.&hdrsuff globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

.millust break
plot&exe : $(object_files)
        &lnkcmd $(link_options) @plot

main.obj : main.&langsuff $(&header._files)
        $(compiler) main $(compile_options)

.millust break
calc.obj : calc.&langsuff $(&header._files)
        $(compiler) calc $(compile_options)

input.obj : input.&langsuff $(&header._files)
        $(compiler) input $(compile_options)

output.obj : output.&langsuff $(&header._files)
        $(compiler) output $(compile_options)
.millust end
.pc
Notice the ampersand ("&") at the end of the macro definition for
"object_files".
The ampersand indicates that the macro definition continues on the
next line.
.ix '&makcmdup' 'line continuation'
.ix 'line continuation'
In general, if you want to continue a line in a makefile, use an
ampersand ("&") at the end of the line.
.np
There are special macros provided by &maksname to access
environment variable names.
.ix '&makcmdup' 'environment variables'
.ix 'environment variables'
.ix '&makcmdup special macros' '$(%path)'
.ix '&makcmdup special macros' '$(%<environment_var>)'
To access the
.ev PATH
environment variable in a makefile, we use the macro identifier
"%path".
For example, if we have the following line in a command list:
.exam begin
      echo $(%path)
.exam end
.pc
it will print out the current value of the
.ev PATH
environment variable when it is executed.
.np
There are two other special environment macros that are predefined by
&maksname..
.ix '&makcmdup special macros' '$(%cdrive)'
The macro identifier "%cdrive" will expand into one letter
representing the current drive. Note that it is operating system
dependent whether the cd command changes the current drive.
.ix '&makcmdup special macros' '$(%cwd)'
The macro identifier "%cwd" will expand into the current working
directory.
These macro identifiers are not very useful unless we can specify that
they be expanded immediately.
.ix '&makcmdup special macros' '$+'
.ix '&makcmdup special macros' '$-'
The complementary macros "$+" and "$&minus." respectively turn on and
turn off immediate expansion of macros.
The scope of the "$+" macro is the current line after which the
default macro expansion behaviour is resumed.
A possible use of these macros is illustrated by the following example
makefile.
.ix '&makcmdup special macros' '$(%cdrive)'
.ix '&makcmdup special macros' '$(%cwd)'
.ix '&makcmdup special macros' '$+'
.ix '&makcmdup special macros' '$-'
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
.millust begin
#
# $(%cdrive), $(%cwd), $+, and $- example
#
dir1 = $(%cdrive):$(%cwd)
dir2 = $+ $(dir1) $-
example : .SYMBOLIC
        cd ..
        echo $(dir1)
        echo $(dir2)
.millust end
.pc
Which would produce the following output if the current working
directory is
C:&pathnamup.\SOURCE\EXAMPLE:
.exam begin
(command output only)
C:&pathnamup.\SOURCE
C:&pathnamup.\SOURCE\EXAMPLE
.exam end
.pc
The macro definition for "dir2" forces immediate expansion of the
"%cdrive" and "%cwd" macros thus defining "dir2" to be the current
directory that &maksname was invoked in.
The macro "dir1" is not expanded until execution time when the current
directory has changed from the initial directory.
.np
Combining the $+ and $&minus. special macros with the special macro
identifiers "%cdrive" and "%cwd" is a useful makefile technique.
The $+ and $&minus. special macros are general enough to be used in
many different ways.
.np
.ix '&makcmdup special macros' '$+'
.ix '&makcmdup special macros' '$-'
Constructing other macros is another use for the $+ and $&minus.
special macros.
&maksname allows macros to be redefined and combining this with the $+
and $&minus. special macros, similar looking macros may be
constructed.
.ix '&makcmdup special macros' '$+'
.ix '&makcmdup special macros' '$-'
.ix '&makcmdup' 'macro construction'
.ix 'macro construction'
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
.code begin
#
# macro construction with $+ and $-
#
template = file1.$(ext) file2.$(ext) file3.$(ext) file4.$(ext)
ext = dat
data_files = $+ $(template) $-
ext = lst
listing_files = $+ $(template) $-

example : .SYMBOLIC
        echo $(data_files)
        echo $(listing_files)
.code end
.pc
This makefile would produce the following output:
.exam begin
file1.dat file2.dat file3.dat file4.dat
file1.lst file2.lst file3.lst file4.lst
.exam end
.pc
Adding more text to a macro can also be done with the $+ and $&minus.
special macros.
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
.millust begin
#
# macro addition with $+ and $-
#
objs = file1.obj file2.obj file3.obj
objs = $+$(objs)$- file4.obj
objs = $+$(objs)$- file5.obj

example : .SYMBOLIC
        echo $(objs)
.millust end
.pc
This makefile would produce the following output:
.exam begin
file1.obj file2.obj file3.obj file4.obj file5.obj
.exam end
.pc
&maksname provides a shorthand notation for this type of macro
operation.
Text can be added to a macro by using the "+=" macro assignment.
The previous makefile can be written as:
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
.millust begin
#
# macro addition with +=
#
objs  = file1.obj file2.obj file3.obj
objs += file4.obj
objs += file5.obj

example : .SYMBOLIC
        echo $(objs)
.millust end
.pc
and still produce the same results.
The shorthand notation "+=" supported by &maksname provides a quick way
to add more text to macros.
.pc
&maksname provides the "!inject" preprocessor directive to append
a "word" (one or more graphic characters) to one or more macros.
The previous makefile is adapted to show the usage:
.ix '&makcmdup preprocessing' '!inject'
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
.millust begin
#
# macro construction with !inject
#
!inject file1.obj objs objs12 objs13 objs14 objs15
!inject file2.obj objs objs12 objs13 objs14 objs15
!inject file3.obj objs        objs13 objs14 objs15
!inject file4.obj objs               objs14 objs15
!inject file5.obj objs                      objs15

example : .SYMBOLIC
        echo $(objs)
        echo $(objs12)
        echo $(objs13)
        echo $(objs14)
        echo $(objs15)
.millust end
This makefile would produce the following output:
.exam begin
file1.obj file2.obj file3.obj file4.obj file5.obj
file1.obj file2.obj
file1.obj file2.obj file3.obj
file1.obj file2.obj file3.obj file4.obj
file1.obj file2.obj file3.obj file4.obj file5.obj
.exam end
.pc
The "!inject" preprocessor directive supported by &maksname
provides a way to append a word to several macros.
.np
There are instances when it is useful to have macro identifiers that
have macro references contained in them.
If you wanted to print out an informative message before linking the
executable that was different between the debugging and production
version, we would express it as follows:
.ix '&lnkcmdup debug options'
.millust begin
#
# programming example
# (macro selection)
#
version = debugging          # debugging version

msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.millust break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

.millust break
&header._files = defs.&hdrsuff globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @plot

main.obj : main.&langsuff $(&header._files)
        $(compiler) main $(compile_options)

.millust break
calc.obj : calc.&langsuff $(&header._files)
        $(compiler) calc $(compile_options)

input.obj : input.&langsuff $(&header._files)
        $(compiler) input $(compile_options)

output.obj : output.&langsuff $(&header._files)
        $(compiler) output $(compile_options)
.millust end
.pc
Take notice of the macro references that are of the form:
.millust begin
$(<partial_macro_identifier>$(version))
.millust end
.pc
The expansion of a macro reference begins by expanding any macros seen
until a matching right parenthesis is found.
The macro identifier that is present after the matching parenthesis is
found will be expanded.
The other form of macro reference namely:
.millust begin
$<macro_identifier>
.millust end
.pc
may be used in a similar fashion.
The previous example would be of the form:
.millust begin
$<partial_macro_identifier>$version
.millust end
.pc
Macro expansion occurs until a character that cannot be in a macro
identifier is found (on the same line as the "$") after which the
resultant macro identifier is expanded.
If you want two macros to be concatenated then the line would have to
be coded:
.millust begin
$(macro1)$(macro2)
.millust end
.np
The use of parentheses is the preferred method for macro references
because it completely specifies the order of expansion.
.np
In the previous example, we can see that the four command lines that
invoke the compiler are very similar in form.
We may make use of these similarities by denoting the command by a
macro reference.
We need to be able to define a macro that will expand into the correct
command when processed.
Fortunately, &maksname can reference the first member of the dependent
list, the last member of the dependent list, and the current target
being updated with the use of some special macros.
These special macros have the form:
.ix '&makcmdup special macros' '$^@'
.ix '&makcmdup special macros' '$^*'
.ix '&makcmdup special macros' '$^&'
.ix '&makcmdup special macros' '$^.'
.ix '&makcmdup special macros' '$^:'
.ix '&makcmdup special macros' '$[@'
.ix '&makcmdup special macros' '$[*'
.ix '&makcmdup special macros' '$[&'
.ix '&makcmdup special macros' '$[.'
.ix '&makcmdup special macros' '$[:'
.ix '&makcmdup special macros' '$]@'
.ix '&makcmdup special macros' '$]*'
.ix '&makcmdup special macros' '$]&'
.ix '&makcmdup special macros' '$].'
.ix '&makcmdup special macros' '$]:'
.ix '&makcmdup special macros' '$^ form'
.ix '&makcmdup special macros' '$[ form'
.ix '&makcmdup special macros' '$] form'
.millust begin
$<file_specifier><form_qualifier>
.millust end
.pc
where <file_specifier> is one of:
.begpoint
.point "^"
represents the current target being updated
.point "["
represents the first member of the dependent list
.point "]"
represents the last member of the dependent list
.endpoint
.np
and <form_qualifier> is one of:
.begpoint
.point "@"
full file name
.point "*"
file name with extension removed
.point "&"
file name with path and extension removed
.point "."
file name with path removed
.point ":"
path of file name
.endpoint
.np
If the file "D:\DIR1\DIR2\NAME.EXT" is the current target being
updated then the following example will show how the form qualifiers
are used.
.begpoint
:DTHD.Macro
:DDHD.Expansion for D:\DIR1\DIR2\NAME.EXT
.point $^@
.fi D:\DIR1\DIR2\NAME.EXT
.point $^*
.fi D:\DIR1\DIR2\NAME
.point $^&
.fi NAME
.point $^.
.fi NAME.EXT
.point $^:
.fi D:\DIR1\DIR2\
.endpoint
.np
These special macros provide the capability to reference targets and
dependents in a variety of ways.
.millust begin
#
# programming example
# (more macros)
#
version = debugging          # debugging version

msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.millust break
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

compiler_command = &compcmd $[* $(compile_options)

&header._files = defs.&hdrsuff globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

.millust break
plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

main.obj : main.&langsuff $(&header._files)
        $(compiler_command)

calc.obj : calc.&langsuff $(&header._files)
        $(compiler_command)

input.obj : input.&langsuff $(&header._files)
        $(compiler_command)

output.obj : output.&langsuff $(&header._files)
        $(compiler_command)
.millust end
.np
This example illustrates the use of the special dependency macros.
Notice the use of "$^*" in the linker command.
The macro expands into the string "plot" since "plot&exe" is the
target when the command is processed.
The use of the special dependency macros is recommended because they
make use of information that is already contained in the dependency
rule.
.np
At this point, we know that macro references begin with a "$" and that
comments begin with a "#".
What happens if we want to use these characters without their special
meaning?
&maksname has two special macros that provide these characters to you.
.ix '&makcmdup special macros' '$$'
.ix '&makcmdup special macros' '$#'
The special macro "$$" will result in a "$" when expanded and "$#"
will expand into a "#".
These special macros are provided so that you are not forced to work
around the special meanings of the "$" and "#" characters.
.np
There is also a simple macro text substitution facility.
We have previously seen that a macro call can be made with $(macroname).
The construct $(macroname:string1=string2) substitutes macroname with
each occurrence of string1 replaced by string2. We have already seen that
it can be useful for a macro to be a set of object file names separated
by spaces. The file directive in &lnkcmd can accept a set of names separated
by commas.
.millust begin
#
# programming example
# (macro substitution)
#

&sysper.c.obj:
        &compcmd -zq $*.c

object_files = main.obj input.obj calc.obj output.obj

plot&exe : $(object_files)
        &lnkcmd name $@ file $(object_files: =,)

.millust end
.np
Note that macro substitution cannot be used with special macros.
.np
It is also worth noting that although the above example shows a valid
approach, the same problem, that is, providing a list of object files
to &lnkcmd, can be solved without macro subsitutions. The solution is
using the {} syntax of &lnkcmd, as shown in the following example.
Refer to the &lnkname Guide for details.
.millust begin
#
# programming example
# (not using macro substitution)
#

&sysper.c.obj:
        &compcmd -zq $*.c

object_files = main.obj input.obj calc.obj output.obj

plot&exe : $(object_files)
        &lnkcmd name $@ file { $(object_files) }

.millust end
.*
.section Implicit Rules
.*
.np
&makname is capable of accepting declarations of commonly used
dependencies.
.ix '&makcmdup' 'implicit rule'
.ix 'implicit rule'
These declarations are called "implicit rules" as opposed to "explicit
rules" which were discussed previously.
Implicit rules may be applied only in instances where you are able to
describe a dependency in terms of file extensions.
.hint
Recall that a file extension is the portion of the file name which
follows the period.
In the file specification:
.np
.fi c:\dos\ansi.sys
.np
the file extension is "SYS".
.ehint
.np
An implicit rule provides a command list for a dependency between
files with certain extensions.
The form of an implicit rule is as follows:
.millust begin
&sysper.<dependent_extension>.<target_extension>:
        <command_list>
.millust end
.pc
Implicit rules are used if a file has not been declared as a target in
any explicit rule or the file has been declared as a target in an
explicit rule with no command list.
.ix '&makcmdup' 'dependent extension'
.ix 'dependent extension'
For a given target file, a search is conducted to see if there are any
implicit rules defined for the target file's extension in which case
&maksname will then check if the file with the dependent extension in
the implicit rule exists.
If the file with the dependent extension exists then the command list
associated with the implicit rule is executed and processing of the
makefile continues.
.np
Other implicit rules for the target extension are searched in a
similar fashion.
The order in which the dependent extensions are checked becomes
important if there is more than one implicit rule declaration for a
target extension.
If we have the following makefile fragment:
.exam begin
&sysper.pas.obj:
        (command list)
&sysper.&langsuff..obj:
        (command list)
.exam end
.pc
an ambiguity arises.
If we have a target file "TEST.OBJ" then which do we check for first,
"TEST.PAS" or "TEST.&langsuffup"?
.ix '&makcmdup directives' '.EXTENSIONS'
.ix 'EXTENSIONS' '&makcmdup directive'
&maksname handles this with the previously described
.id &sysper.EXTENSIONS
directive.
Returning to our makefile fragment:
.millust begin
&sysper.pas.obj:
        (command list)
&sysper.&langsuff..obj:
        (command list)
.millust end
.pc
and our target file "TEST.OBJ", we know that the
.id &sysper.EXTENSIONS
list determines in what order the dependents "TEST.PAS" and
"TEST.&langsuffup" will be tried.
If the .EXTENSIONS declaration is:
.exam begin
&sysper.EXTENSIONS:
.if '&lang' eq 'C' .do begin
&sysper.EXTENSIONS: .exe .obj .asm .pas .c .for .cob
.do end
.if '&lang' eq 'C/C++' .do begin
&sysper.EXTENSIONS: .exe .obj .asm .pas .c .cpp .for .cob
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&sysper.EXTENSIONS: .exe .obj .asm .pas .for .c .cob
.do end
.exam end
.pc
we can see that the dependent file "TEST.PAS" will be tried first as a
possible dependent with "TEST.&langsuffup" being tried next.
.np
One apparent problem with implicit rules and their associated command
lists is that they are used for many different targets and dependents
during the processing of a makefile.
The same problem occurs with commands constructed from macros.
.ix '&makcmdup special macros' '$^ form'
.ix '&makcmdup special macros' '$[ form'
.ix '&makcmdup special macros' '$] form'
Recall that there is a set of special macros that start with "$^",
"$[", or "$]" that reference the target, first dependent, or last
dependent of an explicit dependency rule.
In an implicit rule there may be only one dependent or many dependents
depending on whether the rule is being executed for a target with a
single colon ":" or double colon "::" dependency.
If the target has a single colon or double colon dependency, the "$^",
"$[", and "$]" special macros will reflect the values in the rule that
caused the implicit rule to be invoked.
.ix 'implicit rules' '$^ form'
.ix 'implicit rules' '$[ form'
.ix 'implicit rules' '$] form'
Otherwise, if the target does not have a dependency rule then the "$["
and "$]" special macros will be set to the same value, namely, the
file found in the implicit rule search.
.np
We will use the last programming example to illustrate a possible use
of implicit rules.
.millust begin
#
# programming example
# (implicit rules)
#
version = debugging          # debugging version

msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.millust break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

&header._files = defs.&hdrsuff globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

&sysper.&langsuff..obj:
        $(compiler) $[* $(compile_options)

.millust break
main.obj : main.&langsuff $(&header._files)

calc.obj : calc.&langsuff $(&header._files)

input.obj : input.&langsuff $(&header._files)

output.obj : output.&langsuff $(&header._files)
.millust end
.np
As this makefile is processed, any time an object file is found to be
older than its associated source file or &header files then &maksname
will attempt to execute the command list associated with the explicit
rule.
Since there are no command lists associated with the four object file
targets, an implicit rule search is conducted.
Suppose "CALC.OBJ" was older than "CALC.&langsuffup".
The lack of a command list in the explicit rule with "CALC.OBJ" as a
target causes the ".&langsuff..obj" implicit rule to be invoked for
"CALC.OBJ".
The file "CALC.&langsuffup" is found to exist so the commands
.millust begin
        &compcmd calc &compopt &warnopt
        echo linking debug version ...
        &lnkcmd &wlinkdebug @plot
.millust end
.pc
are executed.
The last two commands are a result of the compilation of
"CALC.&langsuffup" producing a "CALC.OBJ" file that is younger than
the "PLOT&exeup" file that in turn must be generated again.
.np
The use of implicit rules is straightforward when all the files that
the makefile deals with are in the current directory.
Larger applications may have files that are in many different
directories.
Suppose we moved the programming example files to three
sub-directories.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol='src'       value='c'
:set symbol='srcup'     value='C'
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol='src'       value='src'
:set symbol='srcup'     value='SRC'
.do end
.begpoint
:DTHD.Files
:DDHD.Sub-directory
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.point *.&hdrsuffup
.fi \EXAMPLE\&hdrsuffup
.point *.&langsuffup
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.point include files
.fi \EXAMPLE\INC
.point source files
.do end
.fi \EXAMPLE\&srcup
.point rest
.fi \EXAMPLE\O
.endpoint
.np
Now the previous makefile (located in the \EXAMPLE\O sub-directory)
would look like this:
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol='hdir'    value='h_dir'
:set symbol='cdir'    value='c_dir'
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol='hdir'    value='i_dir'
:set symbol='cdir'    value='s_dir'
.do end
.code begin
#
# programming example
# (implicit rules)
#
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&hdir   = \example\&hdrsuff\ #sub-directory containing &header files
&cdir   = \example\&src\ #sub-directory containing &lang files
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&hdir   = \example\inc\ #sub-directory containing include files
&cdir   = \example\&src\ #sub-directory containing source files
.do end
version = debugging   # debugging version

.code break
msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.code break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

.code break
&header._files = $(&hdir)defs.&hdrsuff $(&hdir)globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

.code break
plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

&sysper.&langsuff..obj:
        $(compiler) $[* $(compile_options)

.code break
main.obj : $(&cdir)main.&langsuff $(&header._files)

calc.obj : $(&cdir)calc.&langsuff $(&header._files)

input.obj : $(&cdir)input.&langsuff $(&header._files)

output.obj : $(&cdir)output.&langsuff $(&header._files)
.code end
.pc
Suppose "\EXAMPLE\O\CALC.OBJ" was older than
"\EXAMPLE\&srcup\CALC.&langsuffup".
The lack of a command list in the explicit rule with "CALC.OBJ" as a
target causes the ".&langsuff..obj" implicit rule to be invoked for
"CALC.OBJ".
At this time, the file "\EXAMPLE\O\CALC.&langsuffup" is not found so
an error is reported indicating that "CALC.OBJ" could not be updated.
How may implicit rules be useful in larger applications if they will
only search the current directory for the dependent file?
We must specify more information about the dependent extension (in
this case ".&langsuffup").
.ix '&makcmdup' 'path'
.ix 'path'
We do this by associating a path with the dependent extension as
follows:
.ix '&makcmdup' 'finding targets'
.ix 'finding targets'
.millust begin
&sysper.<dependent_extension> : <path_specification>
.millust end
.pc
This allows the implicit rule search to find the files with the
dependent extension.
.hint
A valid path specification is made up of directory specifications
separated by semicolons (";").
Here are some path specifications:
.sk 1
.cw !
.mono D:;C:\DOS;C:\UTILS;C:\WC
.br
.mono C:\SYS
.br
.mono A:\BIN;D:
.cw ;
.np
Notice that these path specifications are identical to the form
required by the operating system shell's "PATH" command.
.ehint
.np
Our makefile will be correct now if we add the new declaration as
follows:
.code begin
#
# programming example
# (implicit rules)
#
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&hdir   = \example\&hdrsuff\ #sub-directory containing &header files
&cdir   = \example\&src\ #sub-directory containing &lang files
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&hdir   = \example\inc\ #sub-directory containing include files
&cdir   = \example\&src\ #sub-directory containing source files
.do end
version = debugging    # debugging version

.code break
msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.code break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

&header._files = $(&hdir)defs.&hdrsuff $(&hdir)globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

.code break
plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

.code break
&sysper.&langsuff:     $(&cdir)
&sysper.&langsuff..obj:
        $(compiler) $[* $(compile_options)

.code break
main.obj : $(&cdir)main.&langsuff $(&header._files)

calc.obj : $(&cdir)calc.&langsuff $(&header._files)

input.obj : $(&cdir)input.&langsuff $(&header._files)

output.obj : $(&cdir)output.&langsuff $(&header._files)
.code end
.pc
Suppose "\EXAMPLE\O\CALC.OBJ" is older than
"\EXAMPLE\&srcup\CALC.&langsuffup".
The lack of a command list in the explicit rule with "CALC.OBJ" as a
target will cause the ".&langsuff..obj" implicit rule to be invoked
for "CALC.OBJ".
The dependent extension ".&langsuffup" has a path associated with it
so the file "\EXAMPLE\&srcup\CALC.&langsuffup" is found to exist.
The commands
.millust begin
        &compcmd \EXAMPLE\&srcup\CALC &compopt &warnopt
        echo linking debug version ...
        &lnkcmd &wlinkdebug @plot
.millust end
.pc
are executed to update the necessary files.
.np
.ix '&makcmdup' 'larger applications'
.ix 'larger applications'
.ix '&makcmdup' 'multiple source directories'
.ix 'multiple source directories'
If the application requires many source files in different directories
&maksname will search for the files using their associated path
specifications.
For instance, if the current example files were setup as follows:
.begpoint $break
:DTHD.Sub-directory
:DDHD.Contents
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.point \EXAMPLE\&hdrsuffup
.fi DEFS.&hdrsuffup,
.fi GLOBALS.&hdrsuffup
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.point \EXAMPLE\INC
.fi DEFS.&hdrsuffup,
.fi GLOBALS.&hdrsuffup
.do end
.point \EXAMPLE\&srcup\PROGRAM
.fi MAIN.&langsuffup,
.fi CALC.&langsuffup
.point \EXAMPLE\&srcup\SCREEN
.fi INPUT.&langsuffup,
.fi OUTPUT.&langsuffup
.point \EXAMPLE\O
.fi PLOT&exeup,
.fi MAKEFILE,
.fi MAIN.OBJ,
.fi CALC.OBJ,
.fi INPUT.OBJ,
.fi OUTPUT.OBJ
.endpoint
.pc
the makefile would be changed to:
.code begin
#
# programming example
# (implicit rules)
#
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&hdir        = ..\h\    # sub-directory with &header files
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&hdir        = ..\inc\  # sub-directory with &header files
.do end
                        # sub-directories with &lang source files
program_dir  = ..\&langsuff\program\ # - MAIN.&langsuffup, CALC.&langsuffup
screen_dir   = ..\&langsuff\screen\  # - INPUT.&langsuffup, OUTPUT.&langsuffup
version      = debugging     # debugging version

.code break
msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.code break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

.code break
&header._files = $(&hdir)defs.&hdrsuff $(&hdir)globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

.code break
plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

&sysper.&langsuff:     $(program_dir);$(screen_dir)
&sysper.&langsuff..obj:
        $(compiler) $[* $(compile_options)

.code break
main.obj : $(program_dir)main.&langsuff $(&header._files)

calc.obj : $(program_dir)calc.&langsuff $(&header._files)

input.obj : $(screen_dir)input.&langsuff $(&header._files)

output.obj : $(screen_dir)output.&langsuff $(&header._files)
.code end
.pc
Suppose that there is a change in the "DEFS.&hdrsuffup" file which
causes all the source files to be recompiled.
The implicit rule ".&langsuff..obj" is invoked for every object file
so the corresponding ".&langsuffup" file must be found for each ".OBJ"
file.
We will show where &maksname searches for the &lang source files.
.code begin
update    main.obj
  test    ..\&langsuff\program\main.&langsuff            (it does exist)
  execute &compcmd ..\&langsuff\program\main &compopt &warnopt

update    calc.obj
  test    ..\&langsuff\program\calc.&langsuff            (it does exist)
  execute &compcmd ..\&langsuff\program\calc &compopt &warnopt

.code break
update    input.obj
  test    ..\&langsuff\program\input.&langsuff       (it does not exist)
  test    ..\&langsuff\screen\input.&langsuff            (it does exist)
  execute &compcmd ..\&langsuff\screen\input &compopt &warnopt

update    output.obj
  test    ..\&langsuff\program\output.&langsuff      (it does not exist)
  test    ..\&langsuff\screen\output.&langsuff           (it does exist)
  execute &compcmd ..\&langsuff\screen\output &compopt &warnopt

etc.
.code end
.pc
Notice that &maksname checked the sub-directory "..\&srcup\PROGRAM" for
the files "INPUT.&langsuffup" and "OUTPUT.&langsuffup".
&maksname optionally may use a circular path specification search which
may save on disk activity for large makefiles.
The circular path searching may be used in two different ways:
.autopoint
.point
use the command line option "o"
.point
.ix '&makcmdup directives' '.OPTIMIZE'
.ix 'OPTIMIZE' '&makcmdup directive'
use the
.id &sysper.OPTIMIZE
directive.
.endpoint
.np
&maksname will retain (for each suffix) what sub-directory yielded the
last successful search for a file.
The search for a file is resumed at this directory in the hope that
wasted disk activity will be minimized.
If the file cannot be found in the sub-directory then &maksname will
search the next sub-directory in the path specification (cycling to
the first sub-directory in the path specification after an
unsuccessful search in the last sub-directory).
.np
Changing the previous example to include this feature, results in the
following:
.code begin
#
# programming example
# (optimized path searching)
#
&sysper.OPTIMIZE

.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&hdir        = ..\h\    # sub-directory with &header files
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&hdir        = ..\inc\  # sub-directory with &header files
.do end
                        # sub-directories with &lang source files
program_dir  = ..\&langsuff\program\ # - MAIN.&langsuffup, CALC.&langsuffup
screen_dir   = ..\&langsuff\screen\  # - INPUT.&langsuffup, OUTPUT.&langsuffup
version      = debugging     # debugging version

msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.code break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

.code break
&header._files = $(&hdir)defs.&hdrsuff $(&hdir)globals.&hdrsuff
object_files = main.obj input.obj calc.obj &
               output.obj

.code break
plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

.code break
&sysper.&langsuff:     $(program_dir);$(screen_dir)
&sysper.&langsuff..obj:
        $(compiler) $[* $(compile_options)

.code break
main.obj : $(program_dir)main.&langsuff $(&header._files)

calc.obj : $(program_dir)calc.&langsuff $(&header._files)

input.obj : $(screen_dir)input.&langsuff $(&header._files)

output.obj : $(screen_dir)output.&langsuff $(&header._files)
.code end
.pc
Suppose again that there is a change in the "DEFS.&hdrsuffup" file
which causes all the source files to be recompiled.
We will show where &maksname searches for the &lang source files using
the optimized path specification searching.
.code begin
update    main.obj
  test    ..\&langsuff\program\main.&langsuff            (it does exist)
  execute &compcmd ..\&langsuff\program\main &compopt &warnopt

update    calc.obj
  test    ..\&langsuff\program\calc.&langsuff            (it does exist)
  execute &compcmd ..\&langsuff\program\calc &compopt &warnopt

.code break
update    input.obj
  test    ..\&langsuff\program\input.&langsuff       (it does not exist)
  test    ..\&langsuff\screen\input.&langsuff            (it does exist)
  execute &compcmd ..\&langsuff\screen\input &compopt &warnopt

update    output.obj
  test    ..\&langsuff\screen\output.&langsuff           (it does exist)
  execute &compcmd ..\&langsuff\screen\output &compopt &warnopt

etc.
.code end
.pc
&maksname did not check the sub-directory "..\&srcup\PROGRAM" for the
file "OUTPUT.&langsuffup" because the last successful attempt to find
a ".&langsuffup" file occurred in the "..\&srcup\SCREEN"
sub-directory.
In this small example, the amount of disk activity saved by &maksname
is not substantial but the savings become much more pronounced in
larger makefiles.
.hint
The simple heuristic method that &maksname uses for optimizing path
specification searches namely, keeping track of the last successful
sub-directory, is very effective in reducing the amount of disk
activity during the processing of a makefile.
A pitfall to avoid is having two files with the same name in the path.
The version of the file that is used to update the target depends on
the previous searches.
Care should be taken when using files that have the same name with
path specifications.
.ehint
.np
Large makefiles for projects written in &lang may become difficult to
maintain with all the &header file dependencies.
Ignoring &header file dependencies and using implicit rules may reduce
the size of the makefile while keeping most of the functionality
intact.
The previous example may be made smaller by using this idea.
.code begin
#
# programming example
# (no &header dependencies)
#
&sysper.OPTIMIZE

.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&hdir        = ..\h\    # sub-directory with &header files
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&hdir        = ..\inc\  # sub-directory with &header files
.do end
                        # sub-directories with &lang source files
program_dir  = ..\&langsuff\program\ # - MAIN.&langsuffup, CALC.&langsuffup
screen_dir   = ..\&langsuff\screen\  # - INPUT.&langsuffup, OUTPUT.&langsuffup
version      = debugging     # debugging version

msg_production = linking production version ...
msg_debugging = linking debug version ...

link_options_production =
link_options_debugging = &wlinkdebug
link_options = $(link_options_$(version))

.code break
compiler = &compcmd
compile_options_production = &compprdopt &warnopt
compile_options_debugging = &compopt &warnopt
compile_options = $(compile_options_$(version))

.code break
object_files = main.obj input.obj calc.obj &
               output.obj

.code break
plot&exe : $(object_files)
        echo $(msg_$(version))
        &lnkcmd $(link_options) @$^*

.code break
&sysper.&langsuff:     $(program_dir);$(screen_dir)
&sysper.&langsuff..obj:
        $(compiler) $[* $(compile_options)
.code end
.pc
Implicit rules are very useful in this regard providing you are aware
that you have to make up for the information that is missing from the
makefile.
In the case of &lang programs, you must ensure that you force &maksname
to compile any programs affected by changes in &header files.
.ix '&makcmdup options' 'a'
.ix '&makcmdup' 'recompile'
.ix 'recompile'
Forcing &maksname to compile programs may be done by touching source
files (not recommended), deleting object files, or using the "a"
option and targets on the command line.
Here is how the files "INPUT.OBJ" and "MAIN.OBJ" may be recompiled if
a change in some &header file affects both files.
.exam begin
del input.obj
del main.obj
&makcmd
.exam end
.pc
or using the "a" option
.exam begin
&makcmd /a input.obj main.obj
.exam end
.pc
The possibility of introducing bugs into programs is present when
using this makefile technique because it does not protect the
programmer completely from object modules becoming out-of-date.
The use of implicit rules without header file dependencies is a viable
makefile technique but it is not without its pitfalls.
.*
.section Double Colon Explicit Rules
.*
.np
.ix 'explicit rule'
.ix '&makcmdup' 'explicit rule'
.ix '&makcmdup' 'double colon explicit rule'
.ix 'double colon explicit rule'
.ix '&makcmdup' '"::" explicit rule'
.ix 'double-colon (::)' 'explicit rule in &makcmdup'
Single colon ":" explicit rules are useful in many makefile
applications.
However, the single colon rule has certain restrictions that make it
difficult to express more complex dependency relationships.
The restrictions imposed on single colon ":" explicit rules are:
.autopoint
.point
only one command list is allowed for each target
.point
after the command list is executed, the target is considered up to
date
.endpoint
.np
The first restriction becomes evident when you want to update a target
in different ways (i.e., when the target is out of date with respect
to different dependents).
The double colon explicit rule removes this restriction.
.millust begin
#
# multiple command lists
#
target1 :: dependent1 dependent2
        command1

target1 :: dependent3 dependent4
        command2
.millust end
.pc
Notice that if "target1" is out of date with respect to either
"dependent1" or "dependent2" then "command1" will be executed.
The double colon "::" explicit rule does not consider the target (in
this case "target1") up to date after the command list is executed.
&maksname will continue to attempt to update "target1".
Afterwards "command2" will be executed if "target1" is out of date
with respect to either "dependent3" or "dependent4".
It is possible that both "command1" and "command2" will be executed.
As a result of the target not being considered up to date, an implicit
rule search will be conducted on "target1" also.
&maksname will process the double colon "::" explicit rules in the
order that they are encountered in the makefile.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.ix 'prototypes'
.ix 'generating prototypes'
A useful application of the double colon "::" explicit rule involves
maintaining and using prototype information generated by a compiler.
.ix '&makcmdup special macros' '$+'
.ix '&makcmdup special macros' '$-'
.ix '&makcmdup' 'macro construction'
.ix 'macro construction'
.ix '&makcmdup' '.DEF files'
.ix 'DEF files'
.millust begin
#
# double colon "::" example
#
compiler = &compcmd
options = &warnopt

# generate macros for the .OBJ and .DEF files
template = module1.$(ext) module2.$(ext) module3.$(ext)
ext = obj
objs = $+ $(template) $-
ext = def
defs = $+ $(template) $-

# add .DEF to the extensions list
&sysper.EXTENSIONS:
&sysper.EXTENSIONS: .exe .obj .def .&langsuff

.millust break
# implicit rules for the .OBJ and .DEF files
&sysper.&langsuff..obj:
        $(compiler) $[* $(options)

# generate the prototype file (only do a syntax check)
&sysper.&langsuff..def:
        $(compiler) $[* $(options) /v/zs

program&exe :: $(defs)
        erase *.err

program&exe :: $(objs)
        &lnkcmd @$^*

.millust end
.pc
The ".OBJ" files are updated to complete the update of the file
"PROGRAM&exeup".
.ix '&makcmdup' '"::" behaviour'
.ix 'double-colon (::)' 'behaviour in &makcmdup'
It is important to keep in mind that &maksname does not consider the
file "PROGRAM&exeup" up to date until it has conducted a final
implicit rule search.
The double colon "::" explicit rule is useful when describing complex
update actions.
.do end
.*
.section Preprocessing Directives
.*
.np
.ix '&makcmdup' 'preprocessing directives'
.ix 'preprocessing directives' '&makcmdup'
One of the primary objectives in using a make utility is to improve
the development and maintenance of projects.
A programming project consisting of many makefiles in different
sub-directories may become unwieldy to maintain.
.ix '&makcmdup' 'large projects'
.ix 'large projects'
.ix '&makcmdup' 'duplicated information'
.ix 'duplicated information'
The maintenance problem stems from the amount of duplicated
information scattered throughout the project makefiles.
&maksname provides a method to reduce the amount of duplicated
information present in makefiles.
.ix '&makcmdup' 'common information'
.ix 'common information'
Preprocessing directives provide the capability for different
makefiles to make use of common information.
.*
.beglevel
.*
.section File Inclusion
.*
.np
.ix '&makcmdup preprocessing' '!include'
A common solution to the "duplicated information" problem involves
referencing text contained in one file from many different files.
&maksname supports file inclusion with the
.id !include
preprocessing directive.
.ix '&makcmdup' 'libraries'
.ix 'libraries'
.ix '&makcmdup' 'maintaining libraries'
.ix 'maintaining libraries'
.ix '&makcmdup' 'different memory model libraries'
.ix 'different memory model libraries'
.ix '&makcmdup' 'memory model'
.ix 'memory model'
The development of object libraries, using &prod16, for the different
80x86 16-bit memory models provides an ideal example to illustrate the
use of the
.id !include
preprocessing directive.
.begpoint $break
:DTHD.Sub-directory
:DDHD.Contents
.point \WINDOW
.fi WINDOW.CMD,
.fi WINDOW.MIF
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.point \WINDOW\H
.fi PROTO.&hdrsuffup,
.fi GLOBALS.&hdrsuffup,
.fi BIOS_DEF.&hdrsuffup
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.point \WINDOW\INC
.fi PROTO.&hdrsuffup,
.fi GLOBALS.&hdrsuffup,
.fi BIOS_DEF.&hdrsuffup
.do end
.point \WINDOW\&srcup
.fi WINDOW.&langsuffup,
.fi KEYBOARD.&langsuffup,
.fi MOUSE.&langsuffup,
.fi BIOS.&langsuffup
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.point \WINDOW\SCSD
small model object files,
.fi MAKEFILE,
.fi WINDOW_S.LIB
.point \WINDOW\SCBD
compact model object files,
.fi MAKEFILE,
.fi WINDOW_C.LIB
.do end
.point \WINDOW\BCSD
medium model object files,
.fi MAKEFILE,
.fi WINDOW_M.LIB
.point \WINDOW\BCBD
large model object files,
.fi MAKEFILE,
.fi WINDOW_L.LIB
.point \WINDOW\BCHD
huge model object files,
.fi MAKEFILE,
.fi WINDOW_L.LIB
.endpoint
.np
The &libcmdup command file "WINDOW.CMD" contains the list of library
operations required to build the libraries.
The contents of "WINDOW.CMD" are:
.millust begin
-+window
-+bios
-+keyboard
-+mouse
.millust end
.pc
.ix 'replace'
The "&minus.+" library manager command indicates to &libcmdup that the
object file should be replaced in the library.
.np
The file "WINDOW.MIF" contains the makefile declarations that are
common to every memory model.
.ix '&makcmdup' 'include file'
.ix 'make' 'include file'
The ".MIF" extension will be used for all the Make Include Files
discussed in this manual.
This extension is also in the default extension list so it is a
recommended extension for &maksname include files.
The contents of the "WINDOW.MIF" file is as follows:
.millust begin
#
# example of a Make Include File
#
common = /d1 &warnopt          # common options
objs = window.obj bios.obj keyboard.obj mouse.obj

&sysper.&langsuff: ..\&src
&sysper.&langsuff..obj:
        &ocompcmd $[* $(common) $(local) /m$(model)

window_$(model).lib : $(objs)
        &libcmd window_$(model) @..\window
.millust end
.pc
The macros "model" and "local" are defined by the file "MAKEFILE" in
each object directory.
An example of the file "MAKEFILE" in the medium memory model object
directory is:
.millust begin
#
# !include example
#
model = m         # memory model required
local =           # memory model specific options
!include ..\window.mif
.millust end
.pc
Notice that changes that affect all the memory models may be made in
one file, namely "WINDOW.MIF".
Any changes that are specific to a memory model may be made to the
"MAKEFILE" in the object directory.
To update the medium memory model library, the following commands may
be executed:
.ix 'invoking &makname'
.ix '&makcmdup command line' 'invoking &makcmdup'
.exam begin
C>cd \window\bcsd
C>&makcmd
.exam end
.np
.ix '&makcmdup' 'batch files'
.ix 'batch files'
A DOS ".BAT" or OS/2 ".CMD" file may be used to update all the
different memory models.
If the following DOS "MAKEALL.BAT" (OS/2 "MAKEALL.CMD") file is
located somewhere in the "PATH", we may update all the
libraries.
.millust begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
cd \window\scsd
wmake %1 %2 %3 %4 %5 %6 %7 %8 %9
cd \window\scbd
wmake %1 %2 %3 %4 %5 %6 %7 %8 %9
.do end
cd \window\bcsd
wmake %1 %2 %3 %4 %5 %6 %7 %8 %9
cd \window\bcbd
wmake %1 %2 %3 %4 %5 %6 %7 %8 %9
cd \window\bchd
wmake %1 %2 %3 %4 %5 %6 %7 %8 %9
.millust end
.pc
The batch file parameters are useful if you want to specify options to
&maksname..
For instance, a global recompile may be done by executing:
.ix '&makcmdup options' 'a'
.ix 'global recompile'
.ix 'recompile'
.exam begin
C>makeall /a
.exam end
.pc
The
.id !include
preprocessing directive is a good way to partition common information
so that it may be maintained easily.
.np
Another use of the
.id !include
involves program generated makefile information.
For instance, if we have a program called "WMKMK" that will search
through source files and generate a file called "WMKMK.MIF" that
contains:
.code begin
#
# program generated makefile information
#
&langsuffup_to_OBJ = $(compiler) $[* $(compile_options)

OBJECTS = WINDOW.OBJ BIOS.OBJ KEYBOARD.OBJ MOUSE.OBJ

.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
WINDOW.OBJ : ..\&srcup\WINDOW.&langsuffup ..\H\PROTO.&hdrsuffup ..\H\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
BIOS.OBJ : ..\&srcup\BIOS.&langsuffup ..\H\BIOS_DEF.&hdrsuffup ..\H\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
KEYBOARD.OBJ : ..\&srcup\KEYBOARD.&langsuffup ..\H\PROTO.&hdrsuffup ..\H\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
MOUSE.OBJ : ..\&srcup\MOUSE.&langsuffup ..\H\PROTO.&hdrsuffup ..\H\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
WINDOW.OBJ : ..\&srcup\WINDOW.&langsuffup ..\INC\PROTO.&hdrsuffup ..\INC\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
BIOS.OBJ : ..\&srcup\BIOS.&langsuffup ..\INC\BIOS_DEF.&hdrsuffup ..\INC\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
KEYBOARD.OBJ : ..\&srcup\KEYBOARD.&langsuffup ..\INC\PROTO.&hdrsuffup ..\INC\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
MOUSE.OBJ : ..\&srcup\MOUSE.&langsuffup ..\INC\PROTO.&hdrsuffup ..\INC\GLOBALS.&hdrsuffup
    $(&langsuffup_to_OBJ)
.do end
.code end
.pc
In order to use this program generated makefile information, we use a
"MAKEFILE" containing:
.code begin
#
# makefile that makes use of generated makefile information
#
compile_options = &compopt &warnopt

first_target : window.lib .SYMBOLIC
        echo done

!include wmkmk.mif

window.lib : $(OBJECTS)
        &libcmd window $(OBJECTS)

make : .SYMBOLIC
.if '&lang' eq 'C' .do begin
        wmkmk /r ..\&src\*.c+..\h
.do end
.if '&lang' eq 'C/C++' .do begin
        wmkmk /r ..\&src\*.c+..\&src\*.cpp+..\h
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
        wmkmk /r ..\&src\*.for+..\inc
.do end
.code end
.pc
Notice that there is a symbolic target "first_target" that is used as
a "place holder".
The default behaviour for &maksname is to "make" the first target
encountered in the makefile.
The symbolic target "first_target" ensures that we have control over
what file will be updated first (in this case "WINDOW.LIB").
The use of the
.id !include
preprocessing directive simplifies the use of program generated
makefile information because any changes are localized to the file
"MAKEFILE".
As program development continues, the file "WMKMK.MIF" may be
regenerated so that subsequent invocations of &makcmdup benefit from
the new makefile information.
The file "MAKEFILE" even contains the command to regenerate the file
"WMKMK.MIF".
The symbolic target "make" has an associated command list that will
regenerate the file "WMKMK.MIF".
The command list can be executed by typing the following command:
.ix '&makcmdup command line' 'targets'
.ix 'invoking &makname'
.ix '&makcmdup command line' 'invoking &makcmdup'
.exam begin
C>&makcmd make
.exam end
.pc
.ix '&makcmdup' 'reducing maintenance'
.ix 'reducing maintenance'
The use of the
.id !include
preprocessing directive is a simple way to reduce maintenance of
related makefiles.
.keep 20
.hint
Macros are expanded on
.id !include
preprocessor control lines.
This allows many benefits like:
.np
.ul
!include $(%env_var)
.pc
so that the files that &maksname will process can be controlled through
many different avenues like internal macros, command line macros, and
environment variables.
.np
Another way to access files is through the suffix path feature of
&maksname..
A definition like
.np
.ul
&sysper.mif : c:\mymifs;d:\some\more\mifs
.pc
will cause &maksname to search different paths for any make include
files.
.ehint
.*
.section Conditional Processing
.*
.np
&makname has conditional preprocessing directives available that allow
different declarations to be processed.
The conditional preprocessing directives allow the makefile to
.autopoint
.point
check whether a macro is defined, and
.point
check whether a macro has a certain value.
.endpoint
.np
.ix '&makcmdup' 'macro identifier'
.ix 'macro identifier'
.ix '&makcmdup' 'environment variables'
.ix 'environment variables'
.ix '&makcmdup special macros' '$(%<environment_var>)'
The macros that can be checked include
.autopoint
.point
normal macros "$(<macro_identifier>)"
.point
environment macros "$(%<environment_var>)"
.endpoint
.np
The conditional preprocessing directives allow a makefile to adapt to
different external conditions based on the values of macros or
environment variables.
.ix '&makcmdup command line' 'defining macros'
We can define macros on the &makcmdup command line as shown in the
following example.
.exam begin
C>&makcmd "macro=some text with spaces in it"
.exam end
.np
.ix '&makcmdup options' 'f'
Alternatively, we can include a makefile that defines the macros if
all the macros cannot fit on the command line.
This is shown in the following example:
.exam begin
C>&makcmd /f macdef.mif /f makefile
.exam end
.np
Also, environment variables can be set before &makcmdup is invoked.
This is shown in the following example:
.exam begin
C>set macro=some text with spaces in it
C>&makcmd
.exam end
.np
Now that we know how to convey information to &maksname through either
macros or environment variables, we will look at how this information
can be used to influence makefile processing.
.np
&maksname has conditional preprocessing directives that are similar to
the C preprocessor directives.
&maksname supports these preprocessor directives:
.ix '&makcmdup preprocessing' '!ifeq'
.ix '&makcmdup preprocessing' '!ifneq'
.ix '&makcmdup preprocessing' '!ifeqi'
.ix '&makcmdup preprocessing' '!ifneqi'
.ix '&makcmdup preprocessing' '!ifdef'
.ix '&makcmdup preprocessing' '!ifndef'
.ix '&makcmdup preprocessing' '!else'
.ix '&makcmdup preprocessing' '!endif'
.millust begin
!ifeq
!ifneq
!ifeqi
!ifneqi
!ifdef
!ifndef
.millust end
.pc
along with
.millust begin
!else
!endif
.millust end
.pc
Together these preprocessor directives allow selection of makefile
declarations to be based on either the value or the existence of a
macro.
.np
Environment variables can be checked by using an environment variable
name prefixed with a "%".
A common use of a conditional preprocessing directive involves setting
environment variables.
.ix 'environment variables' '&libvarup'
.ix '&makcmdup' 'environment variables'
.ix 'environment variables'
.ix '&makcmdup' 'setting environment variables'
.ix 'setting environment variables'
.millust begin
#
# setting an environment variable
#
!ifndef %&libvar

&sysper.BEFORE
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
        set &libvar=c:&pathnam.&libdir.;c:&pathnam.&libdir.\dos
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
        set &libvar=c:&pathnam.&libdir.\dos
.do end

!endif
.millust end
.pc
If you are writing portable applications, you might want
to have:
.millust begin
#
# checking a macro
#
!include version.mif

!ifdef OS2
machine = /2          # compile for 286
!else
machine = /0          # default: 8086
!endif
.millust end
.pc
The
.id !ifdef
("if defined")
and
.id !ifndef
("if not defined")
conditional preprocessing directives are useful for checking boolean
conditions.
In other words,
the
.id !ifdef
and
.id !ifndef
are useful for "yes-no" conditions.
There are instances where it would be useful to check a macro against
a value.
In order to use the value checking preprocessor directives, we must
know the exact value of a macro.
A macro definition is of the form:
.millust begin
<macro_identifier> = <text> <comment>
.millust end
.pc
.ix '&makcmdup' 'macros'
.ix 'macros'
.ix '&makcmdup' 'macro definition'
.ix 'macro definition'
.ix '&makcmdup' 'macro text'
.ix 'macro text'
&maksname will first strip any comment off the line.
The macro definition will then be the text following the equal "="
sign with leading and trailing blanks removed.
Initially this might not seem like a sensible way to define a macro
but it does lend itself well to defining macros that are common in
makefiles.
For instance, it allows definitions like:
.code begin
#
# sample macro definitions
#
link_options    = debug line   # line number debugging
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
compile_options = /d1 /s       # line numbers, no stack checking
.do end
.el .if '&lang' eq 'FORTRAN 77' .do begin
compile_options = /d1 /nostack # line numbers, no stack checking
.do end
.code end
.pc
These definitions are both readable and useful.
.ix '&makcmdup' 'checking macro values'
.ix 'checking macro values'
A makefile can handle differences between compilers with the
.id !ifeq,
.id !ifneq,
.id !ifeqi
and
.id !ifneqi
conditional preprocessing directives.
The first two perform case sensitive comparisons while the last two
perform case insensitive comparisons.
One way of setting up adaptive makefiles is:
.millust begin
#
# options made simple
#
compiler        = &compcmd

stack_overflow  = No   # yes -> check for stack overflow
line_info       = Yes  # yes -> generate line numbers

.millust break
!ifeq compiler &compcmd
!ifneqi stack_overflow   yes
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
stack_option    =       /s
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
stack_option    =       /nostack
.do end
!endif
!ifeqi line_info         yes
line_option     =       /d1
!endif
!endif

.millust break
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
!ifeq compiler tcc
!ifeqi stack_overflow    yes
stack_option    =       -N
!endif
!ifeqi line_info         yes
line_option     =       -y
!endif
!endif
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
!ifeq compiler fl32
!ifeqi stack_overflow    yes
stack_option    =       -Ge
!endif
!ifeqi line_info         yes
line_option     =       -Zd
!endif
!endif
.do end

.millust break
#
# make sure the macros are defined
#
!ifndef stack_option
stack_option    =
!endif
!ifndef line_option
line_option    =
!endif

example : &sysper.SYMBOLIC
        echo $(compiler) $(stack_option) $(line_option)
.millust end
.pc
The conditional preprocessing directives can be very useful to hide
differences, exploit similarities, and organize declarations for
applications that use many different programs.
.np
.ix '&makcmdup preprocessing' '!define'
Another directive is the
.id !define
directive.
This directive is equivalent to the normal type of macro definition
(i.e., macro = text) but will make C programmers feel more at home.
One important distinction is that the
.id !define
preprocessor directive may be used to reflect the logical structure of
macro definitions in conditional processing.
For instance, the previous makefile could have been written in this style:
.millust begin
!ifndef stack_option
!  define stack_option
!endif
!ifndef line_option
!  define line_option
!endif
.millust end
.pc
The "!" character must be in the first column but the directive
keyword can be indented.
This freedom applies to all of the preprocessing directives.
The
.id !else
preprocessing directive benefits from this type of style because
.id !else
can also check conditions like:
.millust begin
!else ifeq
!else ifneq
!else ifeqi
!else ifneqi
!else ifdef
!else ifndef
.millust end
.pc
so that logical structures like:
.millust begin
!ifdef %version
!  ifeq %version debugging
!    define option debug all
!  else ifeq %version beta
!    define option debug line
!  else ifeq %version production
!    define option debug
!  else
!    error invalid value in VERSION
!  endif
!endif
.millust end
.pc
can be used.
The above example checks the environment variable "VERSION" for three
possible values and acts accordingly.
.np
.ix '&makcmdup preprocessing' '!error'
Another derivative from the C language preprocessor is the
.id !error
directive which has the form of
.millust begin
!error <text>
.millust end
.pc
in &maksname..
This directive will print out the text and terminate processing of the
makefile.
It is very useful in preventing errors from macros that are not
defined properly.
Here is an example of the
.id !error
preprocessing directive.
.millust begin
!ifndef stack_option
!  error stack_option is not defined
!endif
!ifndef line_option
!  error line_option is not defined
!endif
.millust end
.np
.ix '&makcmdup preprocessing' '!undef'
There is one more directive that can be used in a makefile.
The
.id !undef
preprocessing directive will clear a macro definition.
The
.id !undef
preprocessing directive has the form:
.millust begin
!undef <macro_identifier>
.millust end
.pc
The macro identifier can represent a normal macro or an environment
variable.
A macro can be cleared after it is no longer needed.
Clearing a macro will reduce the memory requirements for a makefile.
If the macro identifier represents an environment variable (i.e., the
identifier has a "%" prefix) then the environment variable will be
deleted from the current environment.
The
.id !undef
preprocessing directive is useful for deleting environment variables
and reducing the amount of internal memory required during makefile
processing.
.*
.section Loading Dynamic Link Libraries
.*
.np
.ix '&makcmdup preprocessing' '!loaddll'
.ix 'DLL support'
.ix '&makcmdup preprocessing' 'DLL support'
&makname supports loading of Dynamic Link Library (DLL) versions of
&company software through the use of the
.id !loaddll
preprocessing directive.
This support is available on Win32 and 32-bit OS/2 platforms.
Performance is greatly improved by avoiding a reload of the software
for each file to be processed.
The syntax of the
.id !loaddll
preprocessing directive is:
.millust begin
!loaddll $(exename) $(dllname)
.millust end
.pc
where
.id $(exename)
is the command name used in the makefile and
.id $(dllname)
is the name of the DLL to be loaded and executed in its place.
For example, consider the following makefile which contains a list of
commands and their corresponding DLL versions.
.millust begin
# Default compilation macros for sample programs
#
# Compile switches that are enabled

CFLAGS  = -d1
CC      = wpp386 $(CFLAGS)

LFLAGS  = DEBUG ALL
LINK    = wlink  $(LFLAGS)

!ifdef __LOADDLL__
!  loaddll wcc      wccd
!  loaddll wccaxp   wccdaxp
!  loaddll wcc386   wccd386
!  loaddll wpp      wppdi86
!  loaddll wppaxp   wppdaxp
!  loaddll wpp386   wppd386
!  loaddll wlink    wlinkd
!  loaddll wlib     wlibd
!endif

&sysper.c.obj:
        $(CC) $*.c
.millust end
.np
.ix '__LOADDLL__'
.ix '&makcmdup preprocessing' '__LOADDLL__'
The
.id __LOADDLL__
symbol is defined for versions of &makname that support the
.id !loaddll
preprocessing directive.
The
.id !ifdef __LOADDLL__
construct ensures that the makefile can be processed by an older
version of &makname..
.np
&maksname will look up the
.id wpp386
command in its DLL load table and find a match.
It will then attempt to load the corresponding DLL (i.e.,
.id wppd386.dll
.ct )
and pass it the command line for processing.
The lookup is case insensitive but must match in all other respects.
For example, if a path is included with the command name then the same
path must be specified in the
.id !loaddll
preprocessing directive.
This problem can be avoided through the use of macros as illustrated
below.
.millust begin
# Default compilation macros for sample programs
#
# Compile switches  that are enabled
#
cc286   = wpp
cc286d  = wppdi86
cc386   = wpp386
cc386d  = wppd386
linker  = wlink
linkerd = wlinkd

CFLAGS  = -d1
CC      = $(cc386) $(CFLAGS)

LFLAGS  = DEBUG ALL
LINK    = wlink  $(LFLAGS)

!ifdef __LOADDLL__
!loaddll $(cc286)   $(cc286d)
!loaddll $(cc386)   $(cc386d)
!loaddll $(linker)  $(linkerd)
!endif

&sysper.c.obj:
        $(CC) $*.c
.millust end
.np
A path and/or extension may be specified with the DLL name if desired.
.*
.endlevel
.*
.section *refid=cld Command List Directives
.*
.np
&makname supports special directives that provide command lists for
different purposes.
.ix '&makcmdup directives' '.DEFAULT'
.ix 'DEFAULT' '&makcmdup directive'
If a command list cannot be found while updating a target then the
directive
.id &sysper.DEFAULT
may be used to provide one.
A simple
.id &sysper.DEFAULT
command list which makes the target appear to be updated is:
.ix '&makcmdup' 'touch'
.ix 'touch'
.ix '&makcmdup' '&tchcmdup'
.ix '&tchcmdup'
.millust begin
&sysper.DEFAULT
        &tchcmd $^@
.millust end
.pc
The &tchname utility sets the time-stamp on the file to the
current time.
The effect of the above rule will be to "update" the file without
altering its contents.
.ix '&makcmdup directives' '.BEFORE'
.ix 'BEFORE' '&makcmdup directive'
.ix '&makcmdup directives' '.AFTER'
.ix 'AFTER' '&makcmdup directive'
.np
In some applications it is necessary to execute some commands before
any other commands are executed and likewise it is useful to be able
to execute some commands after all other commands are executed.
&maksname supports this capability by checking to see if the
.id &sysper.BEFORE
and
.id &sysper.AFTER
directives have been used.
If the
.id &sysper.BEFORE
directive has been used, the
.id &sysper.BEFORE
command list is executed before any commands are executed.
Similarly the
.id &sysper.AFTER
command list is executed after processing is finished.
It is important to note that if all the files are up to date and no
commands must be executed,
the
.id &sysper.BEFORE
and
.id &sysper.AFTER
command lists are never executed.
If some commands are executed to update targets and errors are
detected (non-zero return status, macro expansion errors), the
.id &sysper.AFTER
command list is not executed
(the
.id &sysper.ERROR
directive supplies a command list for error conditions and is
discussed in this section).
These two directives may be used for maintenance as illustrated in the
following example:
.ix 'echo' '&makcmdup'
.ix 'pause' '&makcmdup'
.millust begin
#
# .BEFORE and .AFTER example
#
&sysper.BEFORE
        echo .BEFORE command list executed
&sysper.AFTER
        echo .AFTER command list executed
#
# rest of makefile follows
#
        .
        .
        .
.millust end
.pc
If all the targets in the makefile are up to date then neither the
.id &sysper.BEFORE
nor the
.id &sysper.AFTER
command lists will be executed.
If any of the targets are not up to date then before any commands to
update the target are executed, the
.id &sysper.BEFORE
command list will be executed.
The
.id &sysper.AFTER
command list will be executed only if there were no errors detected
during the updating of the targets.
The
.id &sysper.BEFORE,
.id &sysper.DEFAULT,
and
.id &sysper.AFTER
command list directives provide the capability to execute commands
before, during, and after the makefile processing.
.ix '&makcmdup directives' '.ERROR'
.ix 'ERROR' '&makcmdup directive'
.np
&maksname also supports the
.id &sysper.ERROR
directive.
The
.id &sysper.ERROR
directive supplies a command list to be executed if an error occurs
during the updating of a target.
.millust begin
#
# .ERROR example
#
&sysper.ERROR
        beep
#
# rest of makefile follows
#
        .
        .
        .
.millust end
.pc
The above makefile will audibly signal you that an error has occurred
during the makefile processing.
If any errors occur during the
.id &sysper.ERROR
command list execution, makefile processing is terminated.
.*
.section MAKEINIT File
.*
.np
As you become proficient at using &makname, you will probably want to
isolate common makefile declarations so that there is less duplication
among different makefiles.
.ix '&makcmdup' 'MAKEINIT'
.ix 'MAKEINIT'
.ix '&makcmdup' 'initialization file'
.ix 'initialization file'
&maksname will search for a file called "MAKEINIT" (or "TOOLS.INI" when
the "ms" option is set) and process it
before any other makefiles.
The search for the "MAKEINIT" file will occur along the current
"PATH".
If the file "MAKEINIT" is not found, processing continues without any
errors.
By default, &maksname defines a set of data described at the "r" option.
The use of a "MAKEINIT" file will allow you to reuse common
declarations and will result in simpler, more maintainable makefiles.
.*
.section Command List Execution
.*
.np
.ix '&makcmdup' 'command execution'
.ix 'command execution'
&makname is a program which must execute other programs and operating
system shell commands.
There are three basic types of executable files in DOS.
.autopoint
.point
.fi &sysper.COM
files
.point
.fi &sysper.EXE
files
.point
.fi &sysper.BAT
files
.endpoint
.np
There are two basic types of executable files in Windows NT.
.autopoint
.point
.fi &sysper.EXE
files
.point
.fi &sysper.BAT
files
.endpoint
.np
There are two basic types of executable files in OS/2.
.autopoint
.point
.fi &sysper.EXE
files
.point
.fi &sysper.CMD
files
.endpoint
.np
The
.fi &sysper.COM
and
.fi &sysper.EXE
files may be loaded into memory and executed.
.ix 'shell' 'COMMAND.COM'
.ix 'COMMAND.COM shell'
The
.fi &sysper.BAT
files must be executed by the DOS command processor or shell,
"COMMAND.COM".
.ix 'shell' 'CMD.EXE'
.ix 'CMD.EXE shell'
The
.fi &sysper.CMD
files must be executed by the OS/2 command processor or shell,
"CMD.EXE"
&maksname will search along the "PATH" for the command and
depending on the file extension the file will be executed in the
proper manner.
.np
.ix '&makcmdup' '< redirection'
.ix '&makcmdup' '> redirection'
.ix '&makcmdup' '| redirection'
If &maksname detects any input or output redirection characters (these
are ">", "<", and "|") in the command, it will be executed by the
shell.
.np
.ix '&makcmdup' '* command execution'
Under DOS, an asterisk prefix (*) will cause &maksname to examine the
length of the command argument.
If it is too long (> 126 characters), it will take the command
argument and stuff it into a temporary environment variable and
then execute the command with "@env_var" as its argument.
Suppose the following sample makefile fragment contained a very long
command line argument.
.millust begin
#
# Asterisk example
#
    *foo myfile /a /b /c ... /x /y /z
.millust end
.pc
&maksname will perform something logically similar to the following
steps.
.millust begin
    set TEMPVAR001=myfile /a /b /c ... /x /y /z
    foo @TEMPVAR001
.millust end
.pc
The command must, of course, support the "@env_var" syntax.
Typically, DOS commands do not support this syntax but many of the
&company tools do.
.np
.ix '&makcmdup' '! command execution'
The exclamation mark prefix (!) will force a command to be executed by
the shell.
Also, the command will be executed by the shell if the command is an
internal shell command from the following list:
.begpoint $compact
.point break
(check for Ctrl+Break)
.point call
(nest batch files)
.point chdir
(change current directory)
.point cd
(change current directory)
.point cls
(clear the screen)
.point cmd
(start NT or OS/2 command processor)
.point command
(start DOS command processor)
.point copy
(copy or combine files)
.point ctty
(DOS redirect input/output to COM port)
.point d:
(change drive where "d" represents a drive specifier)
.point date
(set system date)
.point del
(erase files)
.point dir
(display contents in a directory)
.point echo
(display commands as they are processed)
.point erase
(erase files)
.point for
(repetitively process commands, intercepted by &makcmdup)
.point if
(allow conditional processing of commands)
.point md
(make directory)
.point mkdir
(make directory)
.point path
(set search path)
.point pause
(suspend batch operations)
.point prompt
(change command prompt)
.point ren
(rename files)
.point rename
(rename files)
.point rmdir
(remove directory)
.point rd
(remove directory)
.point rm
(erase files, intercepted by &makcmdup)
.point set
(set environment variables, intercepted by &makcmdup)
.point time
(set system time)
.point type
(display contents of a file)
.point ver
(display the operating system version number)
.point verify
(set data verification)
.point vol
(display disk volume label)
.endpoint
.np
.ix 'SET' 'using &makname'
The operating system shell "SET" command is intercepted by &maksname..
The "SET" command may be used to set environment variables to values
required during makefile processing.
The environment variable changes are only valid during makefile
processing and do not affect the values that were in effect before
&maksname was invoked.
The "SET" command may be used to initialize environment variables
necessary for the makefile commands to execute properly.
.ix 'system initialization file'
The setting of environment variables in makefiles reduces the number
of "SET" commands required in the system initialization file.
Here is an example with the &cmpname compiler.
.ix 'environment variables' '&incvarup'
.ix 'environment variables' '&libvarup'
.ix '&makcmdup' 'environment variables'
.ix '&makcmdup' 'setting environment variables'
.ix 'environment variables'
.ix 'setting environment variables'
.ix '&makcmdup special macros' '$(%path)'
.ix 'SET' '&incvarup environment variable'
.ix 'SET' '&libvarup environment variable'
.millust begin
#
# SET example
#
&sysper.BEFORE
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
        set &incvar=c:\special\h;$(%&incvar)
        set &libvar=c:&pathnam.&libdir.;c:&pathnam.&libdir.\dos
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
        set &incvar=c:\special\inc;$(%&incvar)
        set &libvar=c:&pathnam.&libdir.\dos
.do end
#
# rest of makefile follows
#
        .
        .
        .
.millust end
.pc
The first "SET" command will set up the
.ev &incvarup
environment variable so that the &cmpname compiler may find header
files.
Notice that the old value of the
.ev &incvarup
environment variable is used in setting the new value.
.np
The second "SET" command indicates to the &lnkname that libraries may
be found in the indicated directories.
.np
Environment variables may be used also as dynamic variables that may
communicate information between different parts of the makefile.
An example of communication within a makefile is illustrated in the
following example.
.ix '&makcmdup' 'dynamic variables'
.ix 'dynamic variables'
.ix 'message passing'
.ix 'communication'
.millust begin
#
# internal makefile communication
#
&sysper.BEFORE
        set message=message text 1
        echo *$(%message)*
        set message=
        echo *$(%message)*

&sysper.example : another_target .SYMBOLIC
        echo *$(%message)*

another_target : .SYMBOLIC
        set message=message text 2
.millust end
.pc
The output of the previous makefile would be:
.millust begin
(command output only)
*message text 1*
**
*message text 2*
.millust end
.pc
.ix 'SET' 'using &makname'
&maksname handles the "SET" command so that it appears to work in an
intuitive manner similar to the operating system shell's "SET"
command.
The "SET" command also may be used to allow commands to relay
information to commands that are executed afterwards.
.np
.ix 'FOR' 'using &makname'
The DOS "FOR" command is intercepted by &maksname..
The reason for this is that DOS has a fixed limit for the size of
a command thus making it unusable for large makefile applications.
One such application that can be done easily with &maksname is the
construction of a &lnkcmdup command file from a makefile.
The idea behind the next example is to have one file that contains
the list of object files.
Anytime this file is changed, say, after a new module has been added,
a new linker command file will be generated which in turn, will
cause the linker to relink the executable.
First we need the makefile to define the list of object files, this
file is "OBJDEF.MIF" and it declares a macro "objs" which has as its
value the list of object files in the application.
The content of the "OBJDEF.MIF" file is:
.millust begin
#
# list of object files
#
objs = &
     window.obj &
     bios.obj &
     keyboard.obj &
     mouse.obj
.millust end
.np
The main makefile ("MAKEFILE") is:
.millust begin
#
# FOR command example
#
!include objdef.mif

plot&exe : $(objs) plot.lnk
        &lnkcmd @plot

plot.lnk : objdef.mif
        echo NAME $^& >$^@
        echo DEBUG all >>$^@
        for %i in ($(objs)) do echo FILE %i >>$^@
.millust end
.pc
This makefile would produce a file "PLOT.LNK" automatically whenever
the list of object files is changed (anytime "OBJDEF.MIF" is changed).
For the above example, the file "PLOT.LNK" would contain:
.millust begin
NAME plot
DEBUG all
FILE window.obj
FILE bios.obj
FILE keyboard.obj
FILE mouse.obj
.millust end
.np
&maksname supports nine internal commands:
.autopoint
.point
.ix '&makcmdup internal commands' '%abort'
.id %abort
.point
.ix '&makcmdup internal commands' '%append'
.id %append
.point
.ix '&makcmdup internal commands' '%create'
.id %create
.point
.ix '&makcmdup internal commands' '%erase'
.id %erase
.point
.ix '&makcmdup internal commands' '%make'
.id %make
.point
.ix '&makcmdup internal commands' '%null'
.id %null
.point
.ix '&makcmdup internal commands' '%quit'
.id %quit
.point
.ix '&makcmdup internal commands' '%stop'
.id %stop
.point
.ix '&makcmdup internal commands' '%write'
.id %write
.endpoint
.np
.ix '&makcmdup internal commands' '%abort'
.ix '&makcmdup internal commands' '%quit'
The
.id %abort
and
.id %quit
internal commands terminate execution of &maksname and return to
the operating system shell:
.id %abort
sets a non-zero exit code;
.id %quit
sets a zero exit code.
.millust begin
#
# %abort and %quit example
#
done_enough :
        %quit

suicide :
        %abort
.millust end
.np
The
.id %append,
.id %create,
.id %erase,
and
.id %write
internal commands allow &makcmdup to generate files under makefile
control.
This is useful for files that have contents that depend on makefile
contents.
Through the use of macros and the "for" command, &maksname becomes a
very powerful tool in maintaining lists of files for other programs.
.np
The
.id %append
internal command appends a text line to the end of a file (which
is created if absent) while the
.id %write
internal command creates or truncates a file and writes one line of
text into it.
Both commands have the same form, namely:
.millust begin
%append <file> <text>
%write <file> <text>
.millust end
.pc
where
.id <file>
is a file specification and
.id <text>
is arbitrary text.
.np
The
.id %create
internal command will create or truncate a file so that the file does
not contain any text while the
.id %erase
internal command will delete a file.
Both commands have the same form, namely:
.millust begin
%create <file>
%erase <file>
.millust end
.pc
where
.id <file>
is a file specification.
.np
Full macro processing is performed on these internal commands so the
full power of &makcmdup can be used.
The following example illustrates a common use of these internal
commands.
.ix '&makcmdup internal commands' '%append'
.ix '&makcmdup internal commands' '%create'
.ix '&makcmdup internal commands' '%erase'
.ix '&makcmdup internal commands' '%write'
.millust begin
#
# %append, %create, %erase, and %write example
#
!include objdef.mif

plot&exe : $(objs) plot.lnk
        &lnkcmd @plot

plot.lnk : objdef.mif
        %create $^@
        %append $^@ NAME $^&
        # Next line equivalent to previous two lines.
        %create $^@ NAME $^&
        %append $^@ DEBUG all
        for %i in ($(objs)) do %append $^@ FILE %i

clean : .SYMBOLIC
        %erase plot.lnk
.millust end
.pc
The above code demonstrates a valuable technique that can generate
directive files for &lnkcmdup, &libcmdup, and other utilities.
.np
The
.id %make
internal command permits the updating of a specific target and
has the form:
.millust begin
%make <target>
.millust end
.pc
where
.id <target>
is a target in the makefile.
.ix '&makcmdup internal commands' '%make'
.millust begin
#
# %make example
#
!include objdef.mif

plot&exe : $(objs)
        %make plot.lnk
        &lnkcmd @plot

plot.lnk : objdef.mif
        %create $^@
        %append $^@ NAME $^&
        %append $^@ DEBUG all
        for %i in ($(objs)) do %append $^@ FILE %i
.millust end
There seem to be other ways of doing the same thing.
Among them is putting plot.lnk into the list of dependencies:
.millust begin
#
# %make counter-example
#
!include objdef.mif

plot&exe : $(objs) plot.lnk
        &lnkcmd @plot

plot.lnk : objdef.mif
        %create $^@
        %append $^@ NAME $^&
        %append $^@ DEBUG all
        for %i in ($(objs)) do %append $^@ FILE %i
.millust end
and using a make variable:
.millust begin
#
# %make counter-example
#
!include objdef.mif

plot&exe : $(objs)
        &lnkcmd NAME $^& DEBUG all FILE { $(objs) }
.millust end
.np
.ix '&makcmdup internal commands' '%null'
The
.id %null
internal command does absolutely nothing.
It is useful because &maksname demands that a command list be present
whenever a target is updated.
.ix '&makcmdup internal commands' '%null'
.ix '&makcmdup directives' '.SYMBOLIC'
.ix 'SYMBOLIC' '&makcmdup directive'
.millust begin
#
# %null example
#
all : application1 application2 .SYMBOLIC
        %null

application1 : appl1&exe .SYMBOLIC
        %null

application2 : appl2&exe .SYMBOLIC
        %null

appl1&exe : (dependents ...)
        (commands)

appl2&exe : (dependents ...)
        (commands)
.millust end
.pc
Through the use of the
.id %null
internal command, multiple application makefiles may be produced that
are quite readable and maintainable.
.np
.ix '&makcmdup internal commands' '%stop'
The
.id %stop
internal command will temporarily suspend makefile processing and print
out a message asking whether Makefile processing should continue.
&maksname will wait for either the "y" key (indicating that the Makefile
processing should continue) or the "n" key.
If the "n" key is pressed, makefile processing will stop.
.ix '&makcmdup' 'debugging makefiles'
.ix 'debugging makefiles'
.ix '&makcmdup internal commands' '%stop'
The
.id %stop
internal command is very useful for debugging makefiles but it may be
used also to develop interactive makefiles.
.millust begin
#
# %stop example
#
all : appl1&exe .SYMBOLIC
        %null

appl1&exe : (dependents ...)
        @echo Are you feeling lucky? Punk!
        @%stop
        (commands)
.millust end
.np
.*
.section Compatibility Between &makname and UNIX Make
.*
.np
.ix '&makcmdup' 'UNIX'
.ix 'UNIX'
&makname was originally based on the UNIX Make utility.
The PC's operating environment presents a base of users
which may or may not be familiar with the UNIX operating system.
&maksname is designed to be a PC product with some UNIX
compatibility.
The line continuation in UNIX Make is a backslash ("\") at the end of
the line.
The backslash ("\") is used by the operating system for directory
specifications and as such will be confused with line continuation.
For example, you could type:
.millust begin
cd \
.millust end
.pc
along with other commands ... and get unexpected results.
However, if your makefile does not contain path separator
characters ("\") and you wish to use "\" as a line continuation indicator
then you can use the &maksname "u" (UNIX compatibility mode) option.
.* .hint
.* The &edname may be used to change UNIX compatible line
.* continuations to the required ampersand "&".
.* The editor command to effect this change is:
.* .np
.* :cmt. .millust
.* *c/\%$/&/
.* :cmt. .emillust
.* .np
.* The command will change every backslash ("\") at the end of a line to
.* an ampersand ("&").
.* .ehint
.np
Also, in the UNIX operating system there is no concept of file
extensions, only the concept of a file suffix.
.ix '&makcmdup directives' '.SUFFIXES'
.ix 'SUFFIXES' '&makcmdup directive'
&maksname will accept the UNIX Make directive
.id &sysper.SUFFIXES
for compatibility with UNIX makefiles.
The UNIX compatible special macros supported are:
.begpoint $compact
:DTHD.Macro
:DDHD.Expansion
.ix '&makcmdup special macros' '$@'
.point $@
full name of the target
.ix '&makcmdup special macros' '$*'
.point $*
target with the extension removed
.ix '&makcmdup special macros' '$<'
.point $<
list of all dependents
.ix '&makcmdup special macros' '$?'
.point $?
list of dependents that are younger than the target
.endpoint
.np
The extra checking of makefiles done by &maksname will require
modifications to UNIX makefiles.
.ix '&makcmdup directives' '.NOCHECK'
.ix 'NOCHECK' '&makcmdup directive'
The UNIX Make utility does not check for the existence of targets
after the associated command list is executed so the "c" or the
.id &sysper.NOCHECK
directive should be used to disable this checking.
.ix '&makcmdup internal commands' '%null'
The lack of a command list to update a target is ignored by the UNIX
Make utility but &makname requires the special internal command
.id %null
to specify a null command list.
In summary, &maksname supports many of the features of the UNIX Make
utility but is not 100% compatible.
.*
.section &makname Diagnostic Messages
.*
.im wmakemsg
