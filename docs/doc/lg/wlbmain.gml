.*
.*
.section The &libname Command Line
.*
.np
The following describes the &libname command line.
.ix '&libcmdup ' 'command line format'
.ix 'command line format ' '&libcmdup'
.mbigbox
.if '&target' eq 'QNX' .do begin
&libcmdup [options_1] lib_file [cmd_list]
.do end
.el .do begin
&libcmdup [options_1] lib_file [options_2] [cmd_list]
.do end
.embigbox
.pc
The square brackets "[]" denote items which are optional.
.begnote
.note lib_file
is the file specification for the library file to be processed.
If no file extension is specified, a file extension of "lib" is assumed.
.note options_1
is a list of valid options.
.if '&cmpcmd' eq 'wcexp' .do begin
Options may be specified in any order and are preceded by a "/" or
"~-" character.
.do end
.el .do begin
Options may be specified in any order.
.if '&target' eq 'QNX' .do begin
Options are preceded by a "~-" character.
.do end
.el .do begin
If you are using a DOS, OS/2 or Windows-hosted version of the
&libname, options are preceded by a "/" or "~-" character.
If you are using a UNIX-hosted version of the &libname,
options are preceded by a "~-" character.
.do end
.do end
.if '&target' ne 'QNX' .do begin
.note options_2
is a list of valid options.
These options
.if '&cmpcmd' ne 'wcexp' .do begin
are only permitted if you are running a DOS, OS/2 or Windows-hosted
version of the &libname and
.do end
must be preceded by a "/" character.
The "~-" character cannot be used as an option delimiter for options
following the library file name since it will be interpreted as a
delete command.
.do end
.note cmd_list
is a list of commands to the &libname specifying what operations are
to be performed.
Each command in
.sy cmd_list
is separated by a space.
.endnote
.np
The following is a summary of valid options.
Items enclosed in square brackets "[]" are optional.
Items separated by an or-bar "|" and enclosed in parentheses "()"
indicate that one of the items must be specified.
Items enclosed in angle brackets "<>" are to be replaced with a
user-supplied name or value (the "<>" are not included in what you
specify).
.begnote $break $compact
.note ?
display the usage message
.note b
suppress creation of backup file
.note c
perform case sensitive comparison
.note d=<output_directory>
directory in which extracted object modules will be placed
.note fa
output AR format library (host default ar format)
.note fab
output AR format library (BSD ar format)
.note fac
output AR format library (COFF ar format)
.note fag
output AR format library (GNU ar format)
.note fm
output MLIB format library
.note fo
output OMF format library
.note h
display the usage message
.note ia
generate AXP import records
.note ii
generate X86 import records
.note ip
generate PPC import records
.note ie
generate ELF import records
.note ic
generate COFF import records
.note io
generate OMF import records
.note i(r|n)(n|o)
imports for the resident/non-resident names table are to be imported by
name/ordinal.
.note l[=<list_file>]
create a listing file
.note m
display C++ mangled names
.note n
always create a new library
.note o=<output_file>
set output file name for library
.note p=<record_size>
set library page size
(supported for "OMF" library format only)
.note pa
set optimal library page size automaticaly
(supported for "OMF" library format only)
.note q
suppress identification banner
.note s
strip line number records from object files
(supported for "OMF" library format only)
.note t
remove path information from module name specified in THEADR records
(supported for "OMF" library format only)
.note v
do not suppress identification banner
.note x
extract all object modules from library
.note zld
strip file dependency info from object files
(supported for "OMF" library format only)
.endnote
.np
The following sections describe the operations that can be performed on
a library file.
Note that before making a change to a library file, the &libname
makes a backup copy of the original library file unless the "o" option
is used to specify an output library file whose name is
different than the original library file, or the "b" option is used to
suppress the creation of the backup file.
The backup copy has the same file name as the original library file
but has a file extension of "bak".
Hence,
.bd lib_file
should not have a file extension of "bak".
.ix '&libcmdup ' 'operations'
.*
.section &libname Module Commands
.*
.np
The following is a summary of basic &libname module manipulation commands:
.begnote $break $compact
.note +
add module to a library
.note -
remove module from a library
.note * or :
extract module from a library (
.bd :
is used with a UNIX-hosted version of the &libname, otherwise
.bd *
is used)
.note ++
add import library entry
.endnote
.*
.section Adding Modules to a Library File
.*
.np
.ix 'library file ' 'adding to a'
An object file can be added to a library file by specifying a
.bd +obj_file
command where
.bd obj_file
is the file specification for an object file.
.if '&cmpcmd' eq 'wcexp' .do begin
A file extension of "obj" is assumed if none is specified.
.do end
.el .do begin
.if '&target' eq 'QNX' .do begin
A file extension of "o" is assumed if none is specified.
.do end
.el .do begin
If you are using a DOS, OS/2 or Windows-hosted version of the
&libname, a file extension of "obj" is assumed if none is specified.
If you are using a UNIX-hosted version of the &libname,
a file extension of "o" is assumed if none is specified.
.do end
.do end
If the library file does not exist, a warning message will be issued
and the library file will be created.
.exam begin
&libcmd mylib +myobj
.exam end
.pc
In the above example, the object file "myobj" is added to the
library file "mylib.lib".
.np
When a module is added to a library, the &libname will issue a warning
if a symbol redefinition occurs.
This will occur
if a symbol in the module being added is already defined
in another module that already exists in the library file.
Note that the module will be added to the library in any case.
.np
It is also possible to combine two library files together.
The following example adds all modules in the library "newlib.lib"
to the library "mylib.lib".
.exam begin
&libcmd mylib +newlib.lib
.exam end
.pc
Note that you must specify the "lib" file extension.
Otherwise, the &libname will assume you are adding an object file.
.*
.section Deleting Modules from a Library File
.*
.np
.ix 'library file ' 'deleting from a'
A module can be deleted from a library file by specifying a
.bd -mod_name
command where
.bd mod_name
is the file name of the object file when it was added to the library
with the directory and file extension removed.
.exam begin
&libcmd mylib -myobj
.exam end
.pc
In the above example, the &libname is instructed to delete the module
"myobj" from the library file "mylib.lib".
.np
It is also possible to specify a library file instead of a module name.
.exam begin
&libcmd mylib -oldlib.lib
.exam end
.pc
In the above example, all modules in the library file "oldlib.lib" are
removed from the library file "mylib.lib".
Note that you must specify the "lib" file extension.
Otherwise, the &libname will assume you are removing an object module.
.*
.section Replacing Modules in a Library File
.*
.np
.ix 'library file ' 'replacing a module in a'
A module can be replaced by specifying a
.bd -+mod_name
or
.bd +-mod_name
command.
The module
.bd mod_name
is deleted from the library.
The object file "mod_name" is then added to the library.
.exam begin
&libcmd mylib -+myobj
.exam end
.pc
In the above example, the module "myobj" is replaced by the object
file "myobj".
.np
It is also possible to merge two library files.
.exam begin
&libcmd mylib -+updlib.lib
.exam end
.pc
In the above example, all modules in the library file "updlib.lib" replace
the corresponding modules in the library file "mylib.lib".
Any module in the library "updlib.lib" not in library "mylib.lib" is
added to the library "mylib.lib".
Note that you must specify the "lib" file extension.
Otherwise, the &libname will assume you are replacing an object module.
.*
.section Extracting a Module from a Library File
.*
.np
.ix 'library file ' 'extracting from a'
A module can be extracted from a library file by specifying a
.if '&target' eq 'QNX' .do begin
.bd :mod_name
[=file_name] command.
.do end
.el .do begin
.bd *mod_name
[=file_name] command for a DOS, OS/2 or Windows-hosted version of the &libname
or a
.bd :mod_name
[=file_name] command for a UNIX-hosted version of the &libname..
.do end
The module
.bd mod_name
is not deleted but is copied to a disk file.
If
.bd mod_name
is preceded by a path specification, the output file will be placed in the
directory identified by the path specification.
If
.bd mod_name
is followed by a file extension, the output file will contain the
specified file extension.
.*
.exam begin
.if '&target' eq 'QNX' .do begin
&libcmd mylib :myobj
.do end
.el .do begin
&libcmd mylib *myobj        DOS, OS/2 or Windows-hosted
    or
&libcmd mylib :myobj        UNIX-hosted
.do end
.exam end
.pc
In the above example, the module "myobj" is copied to a disk file.
The disk file will be an object file with file name "myobj".
.if '&target' eq 'QNX' .do begin
A file extension of "o" will be used.
.do end
.el .do begin
If you are running a DOS, OS/2 or Windows-hosted version of the
&libname, a file extension of "obj" will be used.
If you are running a UNIX-hosted version of the &libname, a file
extension of "o" will be used.
.do end
.*
.exam begin
.if '&target' eq 'QNX' .do begin
&libcmd mylib :myobj.out
.do end
.el .do begin
&libcmd mylib *myobj.out    DOS, OS/2 or Windows-hosted
    or
&libcmd mylib :myobj.out    UNIX-hosted
.do end
.exam end
.pc
In the above example, the module "myobj" will be extracted from the
library file "mylib.lib" and placed in the file "myobj.out"
.np
The following form of the extract command can be used if the module name
is not the same as the output file name.
.np
.exam begin
.if '&target' eq 'QNX' .do begin
&libcmd mylib :myobj=newmyobj.out
.do end
.el .do begin
&libcmd mylib *myobj=newmyobj.out    DOS, OS/2 or Windows-hosted
    or
&libcmd mylib :myobj=newmyobj.out    UNIX-hosted
.do end
.exam end
.np
You can extract a module from a file and have that module deleted from
the library file by specifying a
.if '&target' eq 'QNX' .do begin
.bd :-mod_name
command.
.do end
.el .do begin
.bd *-mod_name
command for a DOS, OS/2 or Windows-hosted version of the &libname
or a
.bd :-mod_name
command for a UNIX-hosted version of the &libname..
.do end
The following example performs the same operations as in the previous
example but, in addition, the module is deleted from the library file.
.*
.exam begin
.if '&target' eq 'QNX' .do begin
&libcmd mylib :-myobj.out
.do end
.el .do begin
&libcmd mylib *-myobj.out   DOS, OS/2 or Windows-hosted
    or
&libcmd mylib :-myobj.out   UNIX-hosted
.do end
.exam end
.pc
Note that the same result is achieved if the delete operator precedes the
extract operator.
.if '&cmpcmd' ne 'wcexp' .do begin
.*
.section Creating Import Libraries
.*
.np
.ix 'Dynamic Link Library' 'imports'
.ix 'import library'
The &libname can also be used to create import libraries from Dynamic
Link Libraries.
Import libraries are used when linking
.if '&prodname' = 'WATFOR-77' .do begin
OS/2 applications.
.do end
.el .do begin
OS/2, Win16 or Win32 applications.
.do end
.exam begin
&libcmd implib +dynamic.dll
.exam end
.np
In the above example, the following actions are performed.
For each external symbol in the specified Dynamic Link Library, a
special object module is created that identifies the external symbol
and the actual name of the Dynamic Link Library it is defined in.
This object module is then added to the specified library.
The resulting library is called an import library.
.np
Note that you must specify the "dll" file extension.
Otherwise, the &libname will assume you are adding an object file.
.*
.section Creating Import Library Entries
.*
.np
.ix 'Dynamic Link Library' 'imports'
.ix 'import library'
An import library entry can be created and added to a library by
specifying a command of the following form.
.im wlbimp
.np
.exam begin
&libcmd math ++__sin.trig.sin.1
.exam end
.np
In the above example, an import library entry will be created for symbol
.id sin
and added to the library "math.lib".
The symbol
.id sin
is defined
in the Dynamic Link Library called "trig.dll" as
.id __sin.
When an application is linked with the library "math.lib",
the resulting executable file will contain an import by ordinal value 1.
If the ordinal value was omitted, the resulting executable file would
contain an import by name
.id sin.
.do end
.*
.section Commands from a File or Environment Variable
.*
.np
.ix '&libcmdup' 'command file'
.ix 'LBC command file'
The &libname can be instructed to process all commands in a disk file
or environment variable by specifying the
.bd @name
command where
.bd name
is a file specification for the command file or the name of an
environment variable.
A file extension of "lbc" is assumed for files if none is specified.
The commands must be one of those previously described.
.exam begin
&libcmd mylib @mycmd
.exam end
.pc
In the above example, all commands in the environment variable "mycmd"
or file "mycmd.lbc" are processed by the &libname..
.*
.section &libname Options
.*
.np
.ix '&libcmdup options'
The following sections describe the list of options allowed when
invoking the &libname..
.*
.beglevel
.*
.section Suppress Creation of Backup File - "b" Option
.*
.np
.ix '&libcmdup options ' 'b'
The "b" option tells the &libname to not create a backup library file.
In the following example, the object file identified by "new" will be added
to the library file "mylib.lib".
.exam begin
&libcmd -b mylib +new
.exam end
.pc
If the library file "mylib.lib" already exits, no backup library file
("mylib.bak") will be created.
.*
.section Case Sensitive Symbol Names - "c" Option
.*
.np
.ix '&libcmdup options ' 'c'
The "c" option tells the &libname to use a case sensitive compare
when comparing a symbol to be added to the library to a symbol
already in the library file.
This will cause the names "myrtn" and "MYRTN" to be treated as different
symbols.
By default, comparisons are case insensitive.
That is the symbol "myrtn" is the same as the symbol "MYRTN".
.*
.section Specify Output Directory - "d" Option
.*
.np
.ix '&libcmdup options ' 'd'
The "d" option tells the &libname the directory in which all extracted
modules are to be placed.
The default is to place all extracted modules in the current directory.
.np
In the following example, the module "mymod" is extracted from the library
"mylib.lib".
.if '&target' eq 'QNX' .do begin
The module will be placed in the file "/o/mymod.o".
.do end
.el .do begin
If you are running a DOS, OS/2 or Windows-hosted version of the
&libname, the module will be placed in the file "\obj\mymod.obj".
If you are running a UNIX-hosted version of the &libname, the module
will be placed in the file "/o/mymod.o".
.do end
.exam begin
.if '&target' eq 'QNX' .do begin
&libcmd -d=/o mymod
.do end
.el .do begin
&libcmd -d=\obj mymod       DOS, OS/2 or Windows-hosted
    or
&libcmd -d=/o mymod         UNIX-hosted
.do end
.exam end
.*
.section Specify Output Format - "f" Option
.*
.np
.ix '&libcmdup options ' 'f'
The "f" option tells the &libname the format of the output library.
The default output format is determined by the type of object files
that are added to the library when it is created.
The possible output format options are:
.begnote
.note fa
output AR format library, host OS default ar format
.note fab
output AR format library, BSD ar format
.note fac
output AR format library, COFF ar format
.note fag
output AR format library, GNU ar format
.note fm
output MLIB format library
.note fo
output OMF format library
.endnote
.*
.section Generating Imports - "i" Option
.*
.np
.ix '&libcmdup options ' 'i'
.ix 'import library'
.ix 'library' 'import'
The "i" option can be used to describe type of import library to
create.
.begnote
.note ia
generate AXP import records
.note ii
generate X86 import records
.note ip
generate PPC import records
.note ie
generate ELF import records
.note ic
generate COFF import records
.note io
generate OMF import records
.endnote
.np
.ix 'Dynamic Link Library' 'imports'
When creating import libraries from Dynamic Link Libraries, import
entries for the names in the resident and non-resident names tables
are created.
The "i" option can be used to describe the method used to import these
names.
.begnote
.note iro
Specifying "iro" causes imports for names in the resident names table
to be imported by ordinal.
.note irn
Specifying "irn" causes imports for names in the resident names table
to be imported by name.
This is the default.
.note ino
Specifying "ino" causes imports for names in the non-resident names
table to be imported by ordinal.
This is the default.
.note inn
Specifying "inn" causes imports for names in the non-resident names
table to be imported by name.
.endnote
.exam begin
&libcmd -iro -inn implib +dynamic.dll
.exam end
.np
Note that you must specify the "dll" file extension for the Dynamic Link
Library.
Otherwise an object file will be assumed.
.*
.section Creating a Listing File - "l" Option
.*
.np
.ix '&libcmdup options ' 'l (lower case L)'
The "l" (lower case "L") option instructs the &libname to produce a
list of the names of all symbols that can be found in the library file
to a listing file.
The file name of the listing file is the same as the file name of the
library file.
The file extension of the listing file is "lst".
.exam begin
&libcmd -l mylib
.exam end
.pc
In the above example, the &libname is instructed to list the contents
of the library file "mylib.lib" and produce the output to a listing
file called "mylib.lst".
.np
An alternate form of this option is
.mono -l=list_file.
With this form, you can specify the name of the listing file.
When specifying a listing file name, a file extension of "lst" is
assumed if none is specified.
.exam begin
&libcmd -l=mylib.out mylib
.exam end
.pc
In the above example, the &libname is instructed to list the contents
of the library file "mylib.lib" and produce the output to a listing file
called "mylib.out".
.np
You can get a listing of the contents of a library file to the terminal
by specifying
only the library name on the command line as demonstrated by the
following example.
.exam begin
&libcmd mylib
.exam end
.*
.section Display C++ Mangled Names - "m" Option
.*
.np
.ix '&libcmdup options ' 'm'
The "m" option instructs the &libname to display C++ mangled names
rather than displaying their demangled form.
The default is to interpret mangled C++ names and display them in
a somewhat more intelligible form.
.*
.section Always Create a New Library - "n" Option
.*
.np
.ix '&libcmdup options ' 'n'
The "n" option tells the &libname to always create a new library file.
If the library file already exists, a backup copy is made
(unless the "b" option was specified).
The original contents of the library are discarded and a new library is
created.
If the "n" option was not specified, the existing library would be updated.
.exam begin
&libcmd -n mylib +myobj
.exam end
.np
In the above example, a library file called "mylib.lib" is created.
It will contain a single object module, namely "myobj", regardless of
the contents of "mylib.lib" prior to issuing the above command.
If "mylib.lib" already exists, it will be renamed to "mylib.bak".
.*
.section Specifying an Output File Name - "o" Option
.*
.np
.ix '&libcmdup options ' 'o'
The "o" option can be used to specify the output library file name if
you want the original library to remain unchanged and a new library
created.
.exam begin
&libcmd -o=newlib lib1 +lib2.lib
.exam end
.pc
In the above example, the modules from "lib1.lib" and
"lib2.lib" are added to the library "newlib.lib".
Note that since the original library remains unchanged, no backup copy
is created.
Also, if the "l" option is used to specify a listing file, the listing
file will assume the file name of the output library.
.*
.section Specifying a Library Record Size - "p" and "pa" Options
.*
.np
.ix '&libcmdup options ' 'p'
The "p" option specifies the record size in bytes for each record
in the library file.
The record size must be a power of 2 and in the range 16 to 32768.
If the record size is less than 16, it will be rounded up to 16.
If the record size is greater than 16 and not a power of 2, it will be
rounded up to the nearest power of 2.
The default record size is 256 bytes.
.np
Each entry in the dictionary of a library file contains an offset from the
start of the file which points to a module.
The offset is 16 bits and is a multiple of the record size.
Since the default record size is 256, the maximum size of a library file
for a record size of 256 is 256*64K.
If the size of the library file increases beyond this size, you must
increase the record size.
.exam begin
&libcmd -p=512 lib1 +lib2.lib
.exam end
.pc
In the above example, the &libname is instructed to create/update
the library file "lib1.lib" by adding the modules from the library
file "lib2.lib".
The record size of the resulting library file is 512 bytes.
.np
.ix '&libcmdup options ' 'pa'
The "pa" option specifies the record size is determined automaticaly to be minimal in size.
.np
.exam begin
&libcmd -pa lib1 +lib2.lib
.exam end
.pc
In the above example, the &libname is instructed to create/update
the library file "lib1.lib" by adding the modules from the library
file "lib2.lib".
The record size of the resulting library file is optimal (minimal) regardles of what each library page size is.
.*
.section Operate Quietly - "q" Option
.*
.np
.ix '&libcmdup options ' 'q'
The "q" option suppressing the banner and copyright notice that is normally
displayed when the &libname is invoked.
.exam begin
&libcmd -q -l mylib
.exam end
.*
.section Strip Line Number Records - "s" Option
.*
.np
.ix '&libcmdup options ' 's'
The "s" option tells the &libname to remove line number records from
object files that are being added to a library.
.if '&prodname' = 'WATFOR-77' .do begin
Line number records are not generated by &prodname.
This option is useful if you are using object files generated by another
compiler that does generate line number records.
.do end
.el .do begin
Line number records are generated in the object file if the "d1" option
is specified when compiling the source code.
.do end
.exam begin
&libcmd -s mylib +myobj
.exam end
.*
.section Trim Module Name - "t" Option
.*
.np
.ix '&libcmdup options ' 't'
The "t" option tells the &libname to remove path information from the module
name specified in THEADR records in object files that are being added to a
library.
The module name is created from the file name by the compiler and placed
in the THEADR record of the object file.
The module name will contain path information if the file name given to the
compiler contains path information.
.exam begin
&libcmd -t mylib +myobj
.exam end
.*
.section Operate Verbosely - "v" Option
.*
.np
.ix '&libcmdup options ' 'v'
The "v" option enables the display of the banner and copyright notice
when the &libname is invoked.
.exam begin
&libcmd -v -l mylib
.exam end
.*
.section Explode Library File - "x" Option
.*
.np
.ix '&libcmdup options ' 'x'
The "x" option tells the &libname to extract all modules from the library.
Note that the modules are not deleted from the library.
Object modules will be placed in the current directory unless the
"d" option is used to specify an alternate directory.
.np
In the following example all modules will be extracted from the library
"mylib.lib" and placed in the current directory.
.exam begin
&libcmd -x mylib
.exam end
.np
In the following example, all modules will be extracted from the library
"mylib.lib".
.if '&target' eq 'QNX' .do begin
The module will be placed in the file "/o" directory.
.do end
.el .do begin
If you are running a DOS, OS/2 or Windows-hosted version of the
&libname, the module will be placed in the "\obj" directory.
If you are running a UNIX-hosted version of the &libname, the module
will be placed in the file "/o" directory.
.do end
.exam begin
.if '&target' eq 'QNX' .do begin
&libcmd -x -d=/o mylib
.do end
.el .do begin
&libcmd -x -d=\obj mylib    DOS, OS/2 or Windows-hosted
    or
&libcmd -x -d=/o mylib      UNIX-hosted
.do end
.exam end
.endlevel
