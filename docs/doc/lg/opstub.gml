.*
.*
.option STUB
.*
.np
The "STUB" option specifies an executable file containing a "stub"
program that is to be placed at the beginning of the executable file
being generated.
The "stub" program will be executed if the module is executed under
DOS.
The format of the "STUB" option is as follows.
.mbigbox
    OPTION STUB=stub_name
.embigbox
.synote
.mnote stub_name
is a file specification for the name of the stub executable file.
If no file extension is specified, a file extension of "EXE" is
assumed.
.esynote
.np
The &lnkname will search all paths specified in the
.ev PATH
environment variable for the stub executable file.
The stub executable file specified by the "STUB" option must not be
the same as the executable file being generated.
