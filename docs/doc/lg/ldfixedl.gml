.*
.*
.dirctv FIXEDLIB
.*
.np
The "FIXEDLIB" directive can be used to explicitly place the modules
from a library file in the overlay section in which the "FIXEDLIB"
directive appears.
The format of the "FIXEDLIB" directive (short form "FIX" ) is as
follows.
.mbigbox
    FIXEDLIB library_file{,library_file}
.embigbox
.synote
.mnote library_file
is a file specification for the name of a library file.
If no file extension is specified, a file extension of "lib" is
assumed.
.esynote
.np
Consider the following example.
.millust begin
begin
  section file1, file2
  section file3
  fixedlib mylib
end
.millust end
.pc
Two overlay sections are defined.
The first contains
.id file1
and
.id file2.
The second contains
.id file3
and all modules contained in the library file "mylib.lib".
.np
Note that all modules extracted from library files that appear in a
"LIBRARY" directive are placed in the root unless the "DISTRIBUTE"
option is specified.
For more information on the "DISTRIBUTE" option,
see the section entitled :HDREF refid='xdistri'..
