.*
.*
.helppref DOS:
:set symbol="unique_lbl" value="dos".
:set symbol="opsys" value="DOS".
:set symbol="sysprompt" value="".
:set symbol="exeformat" value="dos".
:set symbol="exefmtup" value="DOS".
:set symbol="form_drctv" value="form dos".
:set symbol="syst_drctv" value="system dos".
:set symbol="exeextn" value="exe".
:set symbol="libvarname" value="lib".
:set symbol="tmpvarname" value="tmp".
.*
.im dosfiles
.*
.chap *refid=doschap The DOS Executable File Format
.*
.im wlintro
.im wlsyntax
.np
where
.bi directive
is any of the following:
.begnote $compact
.note ALIAS alias_name=symbol_name{,alias_name=symbol_name}
.if '&overlay' eq 'yes' .do begin
.note AUTOSECTION
.note BEGIN {section_type [INTO ovl_file] {directive}} END
.do end
.note DEBUG dbtype [dblist] | DEBUG [dblist]
.note DISABLE msg_num{,msg_num}
.note ENDLINK
.note FILE obj_spec{,obj_spec}
.if '&overlay' eq 'yes' .do begin
.note FIXEDLIB library_file{,library_file}
.note FORCEVECTOR symbol_name{,symbol_name}
.do end
.note FORMAT DOS [COM]
.note LANGUAGE lang
.note LIBFILE obj_file{,obj_file}
.cw !
.note LIBPATH path_name{&ps.path_name}
.cw ;
.note LIBRARY library_file{,library_file}
:cmt. .note MODFILE obj_file{,obj_file}
.note MODTRACE obj_module{,obj_module}
.note NAME exe_file
.note NEWSEGMENT
.if '&overlay' eq 'yes' .do begin
.note NOVECTOR symbol_name{,symbol_name}
.do end
.note OPTION option{,option}
:cmt. where option is one of
.begnote $compact
.if '&overlay' eq 'yes' .do begin
.note AREA=n
.do end
.note ARTIFICIAL
.note [NO]CACHE
.note [NO]CASEEXACT
.note CVPACK
.if '&overlay' eq 'yes' .do begin
.note DISTRIBUTE
.do end
.note DOSSEG
.if '&overlay' eq 'yes' .do begin
.note DYNAMIC
.do end
.note ELIMINATE
.note [NO]FARCALLS
.note FULLHEADER 
:cmt. crashes linker .note INCREMENTAL
.note MANGLEDNAMES
.note MAP[=map_file]
.note MAXERRORS=n
.note NAMELEN=n
.note NODEFAULTLIBS
.note NOEXTENSION
.if '&overlay' eq 'yes' .do begin
.note NOINDIRECT
.do end
.note OSNAME='string'
.note PACKCODE=n
.note PACKDATA=n
.note QUIET
.note REDEFSOK
.note SHOWDEAD
.if '&overlay' eq 'yes' .do begin
.note SMALL
.do end
.note STACK=n
.if '&overlay' eq 'yes' .do begin
.note STANDARD
.do end
.note START=symbol_name
.note STATICS
.note SYMFILE[=symbol_file]
.note [NO]UNDEFSOK
.note VERBOSE
.note VFREMOVAL
.endnote
.note OPTLIB library_file{,library_file}
.if '&overlay' eq 'yes' .do begin
.note OVERLAY class{,class}
.do end
.cw !
.note PATH path_name{&ps.path_name}
.cw ;
.note REFERENCE symbol_name{,symbol_name}
.if '&overlay' eq 'yes' .do begin
.note SECTION
.do end
.note SORT [GLOBAL] [ALPHABETICAL]
.note STARTLINK
.note SYMTRACE symbol_name{,symbol_name}
.note SYSTEM BEGIN system_name {directive} END
.note SYSTEM system_name
.if '&overlay' eq 'yes' .do begin
.note VECTOR symbol_name{,symbol_name}
.do end
.note # comment
.note @ directive_file
.endnote
:CMT. .synote
:CMT. .im objfile
:CMT. .im libfile
:CMT. .im objmod
:CMT. .mnote exe_file
:CMT. is a file specification for the name of the executable file.
:CMT. If no file extension is specified, a file extension of "exe" is assumed
:CMT. unless a "COM" executable file is being generated in which case a file
:CMT. extension of "com" is assumed.
:CMT. .mnote ovl_file
:CMT. is a file specification for the name of an overlay file.
:CMT. If no file extension is specified, a file extension of "ovl" is assumed.
:CMT. .mnote path_name
:CMT. is a path name.
:CMT. .mnote msg_num
:CMT. is a message number.
:CMT. .mnote directive_file
:CMT. is a file specification for the name of a linker directive file.
:CMT. If no file extension is specified, a file extension of "lnk" is assumed.
:CMT. .mnote map_file
:CMT. is a file specification for the name of the map file.
:CMT. If no file extension is specified, a file extension of "map" is assumed.
:CMT. .mnote symbol_file
:CMT. is a file specification for the name of the symbol file.
:CMT. If no file extension is specified, a file extension of "sym" is assumed.
:CMT. .mnote symbol_name
:CMT. is the name of a symbol.
:CMT. .mnote alias_name
:CMT. is the name of an alias symbol.
:CMT. .mnote system_name
:CMT. is the name of a system.
:CMT. .mnote comment
:CMT. is any sequence of characters.
:CMT. .mnote class
:CMT. is a segment class name.
:CMT. .im lnkvalue
:CMT. .esynote
.*
.im wlprompt
.*
.im wlmemlay
.im wlmem
.*
.section *refid=useover Using Overlays
.*
.np
.if '&overlay' eq 'no' .do begin
The creation of overlaid executables is not supported by this version
of the &lnkname..
.do end
.el .do begin
.ix 'overlays'
Overlays are used primarily for large programs where memory
requirements do not permit all portions of the program to reside in
memory at the same time.
.ix 'root'
.ix 'overlay area'
An overlayed program consists of a
.us root
and a number of
.us overlay areas.
.np
The root always resides in memory.
The root usually contains routines that are frequently used.
For example, a floating-point library might be placed in the root.
Also, any modules extracted from a library file during the linking
process are placed in the root unless the "DISTRIBUTE" option is
specified.
This option tells the &lnkname to distribute modules extracted from
libraries throughout the overlay structure.
See the section entitled :HDREF refid='xdistri'. for information on
how these object modules are distributed.
Libraries can also be placed in the overlay structure by using the
"FIXEDLIB" directive.
See the section entitled :HDREF refid='xfixedl'. for information on how
to use this directive.
.np
An
.us overlay area
is a piece of memory shared by various parts of a program.
Each overlay area has a structure associated with it.
This structure defines where in the overlay area sections of a program
are loaded.
.ix 'overlays'
Sections of a program that are loaded into an overlay area are
called
.us overlays.
.np
The &lnkname supports two overlay managers: the standard overlay manager and
the dynamic overlay manager.
The standard overlay manager requires the user to create an overlay structure
that defines the "call" relationship between the object modules that comprise
an application.
It is the responsibility of the user to define an optimal overlay structure
so as to minimize the number of calls that cause overlays to be loaded.
The "SMALL" and "STANDARD" options select the standard overlay manager.
The "SMALL" option is required if you are linking an application compiled
for a small code memory model.
The "STANDARD" option is required if you are linking an application compiled
for a big code memory model.
By default, the &lnkname assumes your application has been compiled using
a memory model with a big code model.
Option "STANDARD" is the default.
.np
The "DYNAMIC" option,
described in the section entitled :HDREF refid='xdynami'.,
selects the dynamic overlay manager.
The dynamic overlay manager is more sophisticated than the standard overlay
manager.
The user need not be concerned about the "call" relationship between the
object modules that comprise an application.
Basically, each module is placed in its own overlay.
The dynamic overlay manager swaps each module (overlay) into a single overlay
area.
This overlay area is used as a pool of memory from which memory for overlays
is allocated.
The larger the memory pool, the greater the number of modules that can
simultaneously reside in memory.
The size of the overlay area can be controlled by the "AREA" option.
See the section entitled :HDREF refid='xarea'. for information on
using this option.
.np
Note that the dynamic overlay manager can only be used with
applications that have been compiled using the "of" option and a big
code memory model.
.*
.beglevel
.*
.section Defining Overlay Structures
.*
.np
Consider the following directive file.
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define an overlay area.
#
begin
  section file file2
  section file file3, file4
  section file file5
end
.millust end
.autonote
.note
The root consists of
.id file0
and
.id file1.
.note
Three overlays are defined.
The first overlay (overlay #1) contains
.id file2,
the second overlay (overlay #2) contains
.id file3
and
.id file4,
and the third overlay (overlay #3) contains
.id file5.
.endnote
.np
The following diagram depicts the overlay structure.
:cmt. :set symbol='c0' value=&sysin+4
:cmt. :set symbol='c1' value=&c0+12
:cmt. :set symbol='c2' value=&c1+12
:cmt. :set symbol='c3' value=&c2+12
:cmt. .tb set `
:cmt. .tb &c0+1 &c0+4 &c1+1 &c1+4 &c2+1 &c2+4 &c3+2
:cmt. .bx &c0 &c3
:cmt. ```file0```\{ start of root
:cmt. ```file1
:cmt.
:cmt. .bx &c0 &c1 &c2 &c3
:cmt. #1``#2``#3``\{ start of overlay area
:cmt. `file2``file3``file5
:cmt. ```file4
:cmt.
:cmt. .bx end
.millust begin
+-----------------------------------+<- start of root
|                                   |
|               file0               |
|               file1               |
|                                   |
+-----------+-----------+-----------+<- start of overlay
| #1        | #2        | #3        |   area
|           |           |           |
|   file2   |   file3   |   file5   |
|           |   file4   |           |
|           |           |           |
+-----------+-----------+-----------+
.millust end
.autonote Notes:
.note
The 3 overlays are all loaded at the same memory location.
Such overlays are called
.ix 'parallel overlays'
.ix 'overlays parallel'
.us parallel.
.endnote
.np
In the previous example, only one overlay area was defined.
It is possible to define more than one overlay area as demonstrated
by the following example.
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define an overlay area.
#
begin
  section file file2
  section file file3, file4
  section file file5
end
#
# Define an overlay area.
#
begin
  section file file6
  section file file7
  section file file8
end
.millust end
.np
Two overlay areas are defined.
The first is identical to the overlay area defined in the previous
example.
The second overlay area contains three overlays; the first overlay
(overlay #4) contains
.id file6,
the second overlay (overlay #5) contains
.id file7,
and the third overlay (overlay #6) contains
.id file8.
.np
The following diagram depicts the overlay structure.
:cmt. :set symbol='c0' value=&sysin+4
:cmt. :set symbol='c1' value=&c0+12
:cmt. :set symbol='c2' value=&c1+12
:cmt. :set symbol='c3' value=&c2+12
:cmt. .tb set `
:cmt. .tb &c0+1 &c0+4 &c1+1 &c1+4 &c2+1 &c2+4 &c3+2
:cmt. .bx &c0 &c3
:cmt. ```file0```\{ start of root
:cmt. ```file1
:cmt.
:cmt. .bx &c0 &c1 &c2 &c3
:cmt. #1``#2``#3``\{ start of overlay area
:cmt. `file2``file3``file5
:cmt. ```file4
:cmt.
:cmt. .bx
:cmt. #4``#5``#6``\{ start of overlay area
:cmt. `file6``file7``file8
:cmt.
:cmt. .bx end
.millust begin
+-----------------------------------+<- start of root
|                                   |
|               file0               |
|               file1               |
|                                   |
+-----------+-----------+-----------+<- start of overlay
| #1        | #2        | #3        |   area
|           |           |           |
|   file2   |   file3   |   file5   |
|           |   file4   |           |
|           |           |           |
+-----------+-----------+-----------+<- start of overlay
| #4        | #5        | #6        |   area
|           |           |           |
|   file6   |   file7   |   file8   |
|           |           |           |
+-----------+-----------+-----------+
.millust end
.np
In the above example, the "AUTOSECTION" directive could have been used to
define the overlays for the second overlay area.
The following example illustrates the use of the "AUTOSECTION" directive.
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define an overlay area.
#
begin
  section file file2
  section file file3, file4
  section file file5
end
#
# Define an overlay area.
#
begin
  autosection
  file file6
  file file7
  file file8
end
.millust end
.np
In all of the above examples the overlays are placed in the
executable file.
It is possible to place overlays in separate files by specifying
the "INTO" option in the "SECTION" directive that starts the definition
of an overlay.
By specifying the "INTO" option in the "AUTOSECTION" directive, all overlays
created as a result of the "AUTOSECTION" directive are placed in one overlay
file.
.np
Consider the following example.
It is similar to the previous example except for the following.
Overlay #1 is placed in the file "ovl1.ovl", overlay #2 is placed in the
file "ovl2.ovl", overlay #3 is placed in the file "ovl3.ovl" and overlays
#4, #5 and #6 are placed in file "ovl4.ovl".
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define an overlay area.
#
begin
  section into ovl1 file file2
  section into ovl2 file file3, file4
  section into ovl3 file file5
end
#
# Define an overlay area.
#
begin
  autosection into ovl4
  file file6
  file file7
  file file8
end
.millust end
.*
.beglevel
.*
.section The Dynamic Overlay Manager
.*
.np
Let us again consider the above example but this time we will use the
dynamic overlay manager.
The easiest way to take the above overlay structure and use it with
the dynamic overlay manager is to simply specify the "DYNAMIC" option.
.millust begin
option DYNAMIC
.millust end
.pc
Even though we have defined an overlay structure with more than one
overlay area, the &lnkname will allocate one overlay area and overlays
from both overlay areas will be loaded into a single overlay area.
The size of the overlay area created by the &lnkname will be twice the size
of the largest overlay area (unless the "AREA" option is used).
.np
To take full advantage of the dynamic overlay manager, the following sequence
of directives should be used.
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define an overlay area.
#
begin
  autosection into ovl1
  file file2
  autosection into ovl2
  file file3
  file file4
  autosection into ovl3
  file file5
  autosection into ovl4
  file file6
  file file7
  file file8
end
.millust end
.np
In the above example, each module will be in its own overlay.
This will result in a module being loaded into memory only when it is required.
If separate overlay files are not required, a single "AUTOSECTION" directive
could be used as demonstrated by the following example.
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define an overlay area.
#
begin
  autosection
  file file2
  file file3
  file file4
  file file5
  file file6
  file file7
  file file8
end
.millust end
.*
.endlevel
.*
.section Nested Overlay Structures
.*
.np
Nested overlay structures occur when the "BEGIN"-"END" directives
are nested and are only useful if the standard overlay manager is being used.
If you have selected the dynamic overlay manager, the nesting levels will be
ignored and each overlay will be loaded into a single overlay area.
.np
Consider the following directive file.
.millust begin
#
# Define files that belong in the root.
#
file file0, file1
#
# Define a nested overlay structure.
#
begin
  section file file2
  section file file3
  begin
    section file file4, file5
    section file file6
  end
end
.millust end
.autonote Notes:
.note
The root contains
.id file0
and
.id file1.
.note
Four overlays are defined.
The first overlay (overlay #1) contains
.id file2,
the second overlay (overlay #2) contains
.id file3,
the third overlay (overlay #3) contains
.id file4
and
.id file5,
and the fourth overlay (overlay #4) contains
.id file6.
.endnote
.np
The following diagram depicts the overlay structure.
:cmt. :set symbol='c0' value=&sysin+4
:cmt. :set symbol='c1' value=&c0+18
:cmt. :set symbol='c2' value=&c1+9
:cmt. :set symbol='c3' value=&c2+9
:cmt. .tb set `
:cmt. .tb &c0+1 &c0+7 &c1-1 &c1+1 &c1+4 &c2-1 &c2+1 &c2+4 &c3+2
:cmt. .bx &c0 &c3
:cmt. ``file0`````\{ start of root
:cmt. ``file1
:cmt. .bx &c0 &c1 &c3
:cmt. #1```#2`````\{ start of overlay area
:cmt.
:cmt. `file2````file3
:cmt.
:cmt.
:cmt. .bx new &c0 / &c1 &c2 &c3
:cmt. ```#3`file4``#4`file6
:cmt. ````file5
:cmt. .bx can
:cmt. .bx off &c0 &c1 &c2 &c3
.millust begin
+-----------------------------------+<- start of root
|                                   |
|               file0               |
|               file1               |
|                                   |
+-----------+-----------------------+<- start of overlay
| #1        | #2                    |   area
|           |                       |
|   file2   |         file3         |
|           |                       |
|           |                       |
|           +-----------+-----------+<- start of overlay
|           | #3        | #4        |   area
|           |           |           |
|           |   file4   |   file6   |
|           |   file5   |           |
|           |           |           |
+-----------+-----------+-----------+
.millust end
.autonote Notes:
.note
Overlay #1 and overlay #2 are parallel overlays.
Overlay #3 and overlay #4 are also parallel overlays.
.note
Overlay #3 and overlay #4 are loaded in memory following overlay #2.
In this case, overlay #2 is called an
.ix 'overlay' 'ancestor of'
.us ancestor
of overlay #3 and overlay #4.
Conversely, overlay #3 and overlay #4 are
.ix 'overlay' 'descendant of'
.us descendants
of overlay #2.
.note
The root is an ancestor of all overlays.
.endnote
.np
Nested overlays are particularly useful when the routines that make
up one overlay are used only by a few other overlays.
In the above example, the routines in overlay #2 would only be used
by routines in overlay #3 and overlay #4 but not by overlay #1.
.*
.section Rules About Overlays
.*
.np
The &lnkname handles all the details of loading overlays.
No changes to a program have to be made if, for example, it becomes
so large that you have to change to an overlay structure.
Certain rules have to be followed to ensure the proper execution of
your program.
These rules pertain more to the organization of the components of your
program and less to the way it was coded.
.autonote
.note
Care should be taken when passing addresses of functions as arguments.
Consider the following example.
.millust begin
+-----------------------+<- start of root
|                       |
|         main          |
|                       |
+-----------+-----------+<- start of overlay
|  modulea  |  moduleb  |   area
|           |           |
|     f     |     h     |
|     g     |           |
|           |           |
+-----------+-----------+
.millust end
.np
Function
.sy f
passes the address of
.us static
function
.sy g
to function
.sy h.
Function
.sy h
then calls function
.sy g
indirectly.
Function
.sy f
and function
.sy g
are defined in
.sy modulea
and function
.sy h
is defined in
.sy moduleb.
Furthermore, suppose that
.sy modulea
and
.sy moduleb
are parallel overlays.
The linker will not generate an overlay vector for function
.sy g
since it is static so when function
.sy h
calls function
.sy g
indirectly, unpredictable results may occur.
Note that if
.sy g
is a global function, an overlay vector will be generated and the program
will execute correctly.
.note
You should organize the overlay structure to minimize the number of times
overlays have to be loaded into memory.
Consider a loop calling two routines, each routine in a different
overlay.
If the overlay structure is such that the overlays are parallel, that
is they occupy the same memory, each iteration of the loop will cause
2 overlays to be loaded into memory.
This will significantly increase execution time if the loop is
iterated many times.
.note
If a number of overlays have a number of common routines that they
all reference, the common routines will most likely be placed
in an ancestor overlay of the overlays that reference them.
For this reason, whenever an overlay is loaded, all its ancestors
are also loaded.
.note
.ix 'overlay loader'
In an overlayed program, the
.us overlay loader
is included in the executable file.
If we are dealing with relatively small programs, the size of the
overlay loader may be larger than the amount of memory saved by
overlaying the program.
In a larger application, the size of the overlayed version would be
smaller than the size of the non-overlayed version.
Note that overlaying a program results in a larger executable file but
the memory requirements are less.
.note
.ix '__OVLTAB__ linker symbol'
.ix '__OVLSTARTVEC__ linker symbol'
.ix '__OVLENDVEC__ linker symbol'
.ix '__LOVLLDR__ linker symbol'
.ix '__NOVLLDR__ linker symbol'
.ix '__SOVLLDR__ linker symbol'
.ix '__LOVLINIT__ linker symbol'
.ix '__NOVLINIT__ linker symbol'
.ix '__SOVLINIT__ linker symbol'
.ix 'linker symbols' '__OVLTAB__'
.ix 'linker symbols' '__OVLSTARTVEC__'
.ix 'linker symbols' '__OVLENDVEC__'
.ix 'linker symbols' '__LOVLLDR__'
.ix 'linker symbols' '__NOVLLDR__'
.ix 'linker symbols' '__SOVLLDR__'
.ix 'linker symbols' '__LOVLINIT__'
.ix 'linker symbols' '__NOVLINIT__'
.ix 'linker symbols' '__SOVLINIT__'
The symbols "__OVLTAB__", "__OVLSTARTVEC__", "__OVLENDVEC__",
"__LOVLLDR__", "__NOVLLDR__", "__SOVLLDR__",
"__LOVLINIT__", "__NOVLINIT__" and "__SOVLINIT__" are defined
when you use overlays.
Your program should not define these symbols.
.note
When using the dynamic overlay manager, you should not take the
address of static functions.
Static functions are not given overlay vectors, so if the module in
which the address of a static function is taken, is moved by the
dynamic overlay manager, that address will no longer point to the
static function.
.endnote
.*
.section *refid=incover Increasing the Dynamic Overlay Area
.*
.np
.ix 'dynamic overlay manager' 'increasing dynamic overlay area at run-time'
.ix 'overlays' 'increasing dynamic overlay area at run-time'
Unless the "AREA" option has been specified, the default size of the
dynamic overlay area is twice the size of the largest overlay (or
module if each module is its own overlay).
It is possible to add additional overlay areas at run-time so that the
dynamic overlay manager can use the additional memory.
A routine has been provided, called
.id _ovl_addarea.
This function is defined as follows.
.millust begin
void far _ovl_addarea(unsigned segment,unsigned size);
.millust end
.np
The first argument is the segment address of the block memory you wish
to add.
The second argument is the size, in paragraphs, of the memory block.
.np
In assembly language, the function is called
.id _ovl_addarea_
with the first argument being passed in register AX and the second
argument in register DX.
.*
.section How Overlay Files are Opened
.*
.np
The overlay manager normally opens overlay files, including executable
files containing overlays, in compatibility mode.
Compatibility mode is a sharing mode.
A file opened in compatibility mode means that it can be opened any
number of times provided that it is not currently opened under one of
the other sharing modes.
In other words, the file must always be opened in compatibility mode.
.np
The overlay manager keeps most recently used overlay files open for
efficiency.
This means that any application, including the currently executing
application, that may want to open an overlay file, must open it in
compatibility mode.
For example, the executing application may have data at the end of the
executable file that it wishes to access.
.np
If an application wishes to open the file in a sharing mode other than
compatibility mode, the function
.id _ovl_openflags
has been defined which allows the caller to specify the sharing mode
with which the overlay files will be opened by the overlay manager.
This function is defined as follows.
.millust begin
unsigned far _ovl_openflags(unsigned sharing_mode);
.millust end
.np
Legal values for the sharing mode are as follows.
.illust begin
.tb set `
.tb &SYSIN. +15
Sharing Mode`Value
-----------------`-------
compatibility mode`0x00
deny read/write mode`0x01
deny write mode`0x02
deny read mode`0x03
deny none mode`0x04
.tb set
.tb
.illust end
.np
The return value is the previous sharing mode used by the overlay
manager to open overlay files.
.np
Note that DOS opens executable files in compatibility mode when
loading them for execution.
This is important for executable files on networks that may be
accessed simultaneously by many users.
.np
In assembly language, the function is called
.id _ovl_openflags_
with its argument being passed in register AX.
.*
.endlevel
.*
.do end
.*
.im ms2wlink
.*
.helppref
