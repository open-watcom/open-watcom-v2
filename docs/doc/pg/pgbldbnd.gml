.if '&bldnam' eq 'win386' .do begin
.ix 'WIN386.EXT'
.ix 'WBIND.EXE'
.ix 'supervisor'
.ix 'Windows supervisor'
The "&bldexe" file must now be combined with &company.'s 32-bit Windows
supervisor
.fi WIN386.EXT
using the &company Bind utility.
.fi WBIND.EXE
combines your 32-bit application code and data (".rex" file) with the
32-bit Windows supervisor.
The process involves the following steps:
.autonote
.note
.ix 'WBIND'
.ix 'WIN386.EXT'
.fi WBIND
copies
.fi WIN386.EXT
into the current directory.
.note
.ix 'WBIND.EXE'
.fi WBIND.EXE
optionally runs the resource compiler on the 32-bit Windows supervisor
so that the 32-bit executable can have access to the applications
resources.
.note
.ix 'WBIND.EXE'
.ix 'WIN386.EXT'
.fi WBIND.EXE
concatenates
.fi WIN386.EXT
and the ".rex" file, and creates a ".exe" file with the same name as
the ".rex" file.
.endnote
.np
.ix 'binding 32-bit applications'
.ix 'Windows' 'binding 32-bit applications'
.ix 'WBIND'
The following describes the syntax of the
.fi WBIND
command.
.mbigbox
WBIND file_spec [-d] [-n] [-q] [-s supervisor] [-R rc_options]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote $break $setptnt 15
.ix 'WBIND'
.mnote WBIND
is the name of the &company Bind utility.
.mnote file_spec
is the name of the 32-bit Windows EXE to bind.
.mnote -d
requests that a 32-bit DLL be built.
.mnote -n
indicates that the resource compiler is NOT to be invoked.
.mnote -q
requests that WBIND run in quiet mode
(no informational messages are displayed).
.mnote -s supervisor
specifies the path and name of the Windows supervisor to be bound with
the application.
If not specified, a search of the paths listed in the
.ev PATH
environment variable is performed.
If this search is not successful and the
.ev &pathvarup.
environment variable is defined, the
.fi %&pathvarup.%\BINW
directory is searched.
.mnote -R rc_options
.ix 'resource compiler'
all options after -R are passed to the resource compiler.
.endnote
.np
To bind our example program, the following command may be issued:
.millust begin
&prompt.wbind &demo -n
.millust end
.pc
.ix 'WIN386.EXT'
.ix 'W386DLL.EXT'
.ix 'binding a 32-bit DLL'
If the "s" option is specified, it must identify the location of the
.fi WIN386.EXT
file or the
.fi W386DLL.EXT
file (if you are building a DLL).
.exam begin
&prompt.wbind &demo -n -s c:&pathnam\binw\win386.ext
.exam end
.pc
.ix 'BINW directory'
If the "s" option is not specified, then the
.ev &pathvarup.
environment variable must be defined or the "BINW" directory must be
listed in your
.ev PATH
environment variable.
.exam begin
&prompt.set watcom=c:&pathnam
  or
&prompt.path c:&pathnam\binw;c:\dos;c:\windows
.exam end
.do end
