.pp
.if '&machsys' eq 'QNX' .do begin
.autonote
.note
The "l" form of the &*1. functions (&*1.l...) contain an argument list
terminated by a
.mono NULL
pointer.
The argument
.arg arg0
should point to a filename that is associated with the program being
loaded.
.note
The "v" form of the &*1. functions (&*1.v...) contain a pointer to
an argument vector.
The value in
.arg argv[0]
should point to a filename that is associated with the program being
loaded.
The last member of
.arg argv
must be a
.mono NULL
pointer.
The value of
.arg argv
cannot be
.mono NULL
.ct , but argv[0] can be a
.mono NULL
pointer if no argument strings are passed.
.note
The "p" form of the &*1. functions (&*1.lp..., &*1.vp...) use paths
listed in the "PATH" environment variable to locate the program to be
loaded provided that the following conditions are met.
The argument
.arg file
identifies the name of program to be loaded.
If no path character (&pc) is included in the name, an attempt is made
to load the program from one of the paths in the "PATH" environment
variable.
If "PATH" is not defined, the current working directory is used.
If a path character (&pc) is included in the name, the program is
loaded as in the following point.
.note
If a "p" form of the &*1. functions is not used,
.arg path
must identify the program to be loaded, including a path if required.
Unlike the "p" form of the &*1. functions, only one attempt is made to
locate and load the program.
.note
The "e" form of the &*1. functions (&*1....e) pass a pointer to a new
environment for the program being loaded.
The argument
.arg envp
is an array of character pointers to null-terminated strings.
The array of pointers is terminated by a
.mono NULL
pointer.
The value of
.arg envp
cannot be
.mono NULL
.ct , but envp[0] can be a
.mono NULL
pointer if no environment strings are passed.
.endnote
.do end
.el .do begin
The program is located by using the following logic in sequence:
.autonote
.note
An attempt is made to locate the program in the current working
directory if no directory specification precedes the program name;
otherwise, an attempt is made in the specified directory.
.note
If no file extension is given,
an attempt is made to find the program name,
in the directory indicated in the first point, with
.mono .COM
concatenated to the end of the program name.
.note
If no file extension is given,
an attempt is made to find the program name,
in the directory indicated in the first point, with
.mono .EXE
concatenated to the end of the program name.
.note
When no directory specification is given as part of the program name,
the
.kw &*1.lp
.ct,
.kw &*1.lpe
.ct,
.kw &*1.vp
.ct,
and
.kw &*1.vpe
functions will repeat
the preceding three steps for each of the directories
specified by the
.kw PATH
environment variable.
The command
.millust begin
path c:&pc.myapps;d:&pc.lib&pc.applns
.millust end
.pc
indicates that the two directories
.millust begin
c:&pc.myapps
d:&pc.lib&pc.applns
.millust end
.pc
are to be searched.
The
.doscmd PATH
(without any directory specification) will cause the current path
definition to be displayed.
.endnote
.do end
.pp
An error is detected when the program cannot be found.
