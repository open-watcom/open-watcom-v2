.*
.*
.im dosfiles
.*
.im wlintro
.im wlsyntax
.np
where
.bi directive
is any of the following:
.begnote $compact
.note ALIAS alias_name=symbol_name{,alias_name=symbol_name}
.if '&exeformat' eq 'win' or '&exeformat' eq 'win nt' .do begin
.note ANONYMOUSEXPORT export{,export} | =lbc_file
.do end
.if '&exeformat' eq 'win nt' .do begin
.note COMMIT mem_type
.do end
.if '&exeformat' ne 'win vxd' .do begin
.note DEBUG dbtype [dblist] | DEBUG [dblist]
.do end
.note DISABLE msg_num{,msg_num}
.note ENDLINK
.note EXPORT export{,export}
.note EXPORT =lbc_file
.note FILE obj_spec{,obj_spec}
.if '&exeformat' eq 'os2' .do begin
.note FORMAT OS2 [exe_type] [dll_form | exe_attrs]
.do end
.if '&exeformat' eq 'win' .do begin
.note FORMAT WINDOWS [dll_form] [MEMORY] [FONT]
.do end
.if '&exeformat' eq 'win vxd' .do begin
.note FORMAT WINDOWS VXD [DYNAMIC]
.do end
.if '&exeformat' eq 'win nt' .do begin
.note FORMAT WINDOWS NT [TNT | RDOS] [dll_form]
.do end
.if '&exeformat' ne 'win vxd' .do begin
.note IMPORT import{,import}
.do end
.note LANGUAGE lang
.note LIBFILE obj_file{,obj_file}
.cw !
.note LIBPATH path_name{&ps.path_name}
.cw ;
.note LIBRARY library_file{,library_file}
.note MODFILE obj_file{,obj_file}
.note MODTRACE obj_module{,obj_module}
.note NAME exe_file
.if '&exeformat' ne 'win nt' and '&exeformat' ne 'win vxd' .do begin
.note NEWSEGMENT
.do end
.cw !
.note PATH path_name{&ps.path_name}
.cw ;
.note OPTION option{,option}
.begnote $compact
.note ALIGNMENT=n
.note ARTIFICIAL
.note [NO]CACHE
.note [NO]CASEEXACT
.if '&exeformat' eq 'win nt' .do begin
.note CHECKSUM
.do end
.if '&exeformat' ne 'win vxd' .do begin
.note CVPACK
.do end
.note DESCRIPTION 'string'
.if '&exeformat' ne 'win vxd' .do begin
.note DOSSEG
.do end
.note ELIMINATE
.note [NO]FARCALLS
.note HEAPSIZE=n
.note IMPFILE[=imp_file]
.note IMPLIB[=imp_lib]
.note INCREMENTAL
.if '&exeformat' eq 'os2' .do begin
.note INTERNALRELOCS
.do end
.if '&exeformat' eq 'win nt' .do begin
.note LINKVERSION=major[.minor]
.do end
.note MANGLEDNAMES
.if '&exeformat' eq 'win' or '&exeformat' eq 'os2' .do begin
.note MANYAUTODATA
.do end
.note MAP[=map_file]
.note MAXERRORS=n
.if '&exeformat' eq 'os2' .do begin
.note MIXED1632
.do end
.note MODNAME=module_name
.note NAMELEN=n
.if '&exeformat' eq 'os2' .do begin
.note NEWFILES
.do end
.if '&exeformat' eq 'win' or '&exeformat' eq 'os2' .do begin
.note NOAUTODATA
.do end
.note NODEFAULTLIBS
.note NOEXTENSION
.if '&exeformat' eq 'win nt' .do begin
.note NORELOCS
.note NOSTDCALL
.do end
.note NOSTUB
.if '&exeformat' eq 'win nt' .do begin
.note OBJALIGN=n
.do end
.if '&exeformat' eq 'os2' or '&exeformat' eq 'win nt' .do begin
.note OFFSET
.do end
.if '&exeformat' ne 'win vxd' .do begin
.note OLDLIBRARY=dll_name
.do end
.if '&exeformat' eq 'win' or '&exeformat' eq 'os2' .do begin
.note ONEAUTODATA
.do end
.note OSNAME='string'
.if '&exeformat' eq 'win nt' .do begin
.note OSVERSION=major[.minor]
.do end
.if '&exeformat' ne 'win nt' and '&exeformat' ne 'win vxd' .do begin
.note PACKCODE=n
.note PACKDATA=n
.do end
.if '&exeformat' eq 'os2' .do begin
.note PROTMODE
.do end
.note QUIET
.note REDEFSOK
.note RESOURCE=resource_file
.if '&exeformat' eq 'win' .do begin
.note RWRELOCCHECK
.do end
.note SHOWDEAD
.note STACK=n
.note START=symbol_name
.note STATICS
.note STUB=stub_name
.note SYMFILE[=symbol_file]
.if '&exeformat' eq 'os2' .do begin
.note TOGGLERELOCS
.do end
.note [NO]UNDEFSOK
.note VERBOSE
.note VERSION=major[.minor]
.note VFREMOVAL
.endnote
.*
.note OPTLIB library_file{,library_file}
.*
.note REFERENCE symbol_name{,symbol_name}
.*
.if '&exeformat' eq 'win nt' .do begin
.note RUNTIME run_option
.do end
.*
.note SEGMENT seg_desc{,seg_desc}
.*
.note SORT [GLOBAL] [ALPHABETICAL]
.*
.note STARTLINK
.*
.note SYMTRACE symbol_name{,symbol_name}
.*
.note SYSTEM BEGIN system_name {directive} END
.*
.note SYSTEM system_name
.*
.note # comment
.*
.note @ directive_file
.*
.endnote
:CMT. .synote
:CMT. .mnote class_name
:CMT. is a class name.
:CMT. .mnote comment
:CMT. is any sequence of characters.
:CMT. .mnote string
:CMT. is a sequence of characters.
:CMT. .mnote directive_file
:CMT. is a file specification for the name of a linker directive file.
:CMT. If no file extension is specified, a file extension of "lnk" is
:CMT. assumed.
:CMT. .mnote dll_name
:CMT. is a file specification for the name of a Dynamic Link Library.
:CMT. If no file extension is specified, a file extension of "&dllextn" is
:CMT. assumed.
:CMT. .mnote entry_name
:CMT. is a function name.
:CMT. .mnote exe_file
:CMT. is a file specification for the name of the executable file.
:CMT. If no file extension is specified, a file extension of "exe" is
:CMT. assumed.
:CMT. .if '&exeformat' eq 'os2' .do begin
:CMT. If a Dynamic Link Library file is being generated, a file
:CMT. extension of "dll" is assumed.
:CMT. .do end
:CMT. .mnote internal_name
:CMT. is a function name.
:CMT. .im libfile
:CMT. .mnote major
:CMT. specifies the major version number.
:CMT. .mnote lbc_file
:CMT. is a file specification for the name of a librarian command file.
:CMT. If no file extension is specified, a file extension of "lbc" is
:CMT. assumed.
:CMT. .mnote map_file
:CMT. is a file specification for the name of the map file.
:CMT. If no file extension is specified, a file extension of "map" is
:CMT. assumed.
:CMT. .mnote minor
:CMT. specifies the minor version number and must be less than 100.
:CMT. .mnote module_name
:CMT. is the name of a Dynamic Link Library.
:CMT. Note that this need not be the same as the file name of the executable
:CMT. file that contains the Dynamic Link Library.
:CMT. .im lnkvalue
:CMT. .im objfile
:CMT. .im objmod
:CMT. .mnote path_name
:CMT. is a path name.
:CMT. .mnote msg_num
:CMT. is a message number.
:CMT. .mnote seg_name
:CMT. is the name of the code or data segment whose attributes are being
:CMT. specified.
:CMT. .mnote stub_name
:CMT. is a file specification for the name of the executable file.
:CMT. If no file extension is specified, a file extension of "exe" is
:CMT. assumed.
:CMT. .mnote symbol_file
:CMT. is a file specification for the name of the symbol file.
:CMT. If no file extension is specified, a file extension of "sym" is
:CMT. assumed.
:CMT. .mnote symbol_name
:CMT. is the name of a symbol.
:CMT. .mnote alias_name
:CMT. is the name of an alias symbol.
:CMT. .mnote system_name
:CMT. is the name of a system.
:CMT. .esynote
.*
.im wlprompt
.*
.if '&exeformat' eq 'win' .do begin
.*
.section Fixed and Moveable Segments
.*
.np
All segments have attributes that tell Windows how to manage the
segment.
One of these attributes specifies whether the segment is fixed or
moveable.
Moveable segments can be moved in memory to satisfy other memory
requests.
When a segment is moved, all near pointers to that segment are still
valid since a near pointer references memory relative to the start of
the segment.
However, far pointers are no longer valid once a segment has been
moved.
Fixed segments, on the other hand, cannot be moved in memory.
A segment must be fixed if there exists far pointers to that segment
that Windows cannot adjust if that segment were moved.
.np
This is a memory-management issue for real-mode Windows only.
However, if a DLL is marked as "fixed", Windows 3.x will place it in
the lower 640K real-mode memory (regardless of the mode in which
Windows 3.x is running).
Since the lower 640K is a limited resource, you normally would want a
DLL to be marked as "moveable".
.np
Most segments, including code and data segments, are moveable.
Some exceptions exist.
If your program contains a far pointer, the segment which it
references must be fixed.
If it were moveable, the segment address portion of the far pointer
would be invalid when Windows moved the segment.
.np
All non-Windows programs are assigned fixed segments when they run
under Windows.
These segments must be fixed since there is no information in the
executable file that describes how segments are referenced.
Whenever possible, your application should consist of moveable
segments since fixed segments can cause memory management problems.
.*
.section Discardable Segments
.*
.np
Moveable segments can also be discardable.
Memory allocated to a discardable segment can be freed and used for
other memory requests.
A "least recently used" (LRU) algorithm is used to determine which
segment to discard when more memory is required.
.np
Discardable segments are usually segments that do not change once they
are loaded into memory.
For example, code segments are discardable since programs do not
usually modify their code segments.
When a segment is discarded, it can be reloaded into memory by
accessing the executable file.
.np
Discardable segments must be moveable since they can be reloaded into
a different area in memory than the area they previously occupied.
Note that moveable segments need not be discardable.
Obviously, data segments that contain read/write data cannot be
discarded.
.*
.do end
.if '&exeformat' ne 'win vxd' .do begin
.*
.section Dynamic Link Libraries
.*
.np
.ix '&opsys program modules'
.ix '&opsys Dynamic Link Libraries'
The &lnkname can generate two forms of executable files;
program modules and Dynamic Link Libraries.
A program module is the executable file that gets loaded by the
operating system when you run your application.
A Dynamic Link Library is really a library of routines that are called
by a program module but not linked into the program module.
The executable code in a Dynamic Link Library is loaded by the
operating system during the execution of a program module when a
routine in the Dynamic Link Library is called.
.np
Program modules are contained in files whose name has a file extension
of "exe".
Dynamic Link Libraries are contained in files whose name has a file
extension of "&dllextn".
The &lnkname "FORMAT" directive can be used to select the type of
executable file to be generated.
.np
Let us consider some of the advantages of using Dynamic Link Libraries
over standard libraries.
.autonote
.note
Functions in Dynamic Link Libraries are not linked into your program.
Only references to the functions in Dynamic Link Libraries are placed
in the program module.
.ix 'import definitions'
These references are called import definitions.
As a result, the linking time is reduced and disk space is saved.
If many applications reference the same Dynamic Link Library, the
saving in disk space can be significant.
.note
Since program modules only reference Dynamic Link Libraries and do not
contain the actual executable code, a Dynamic Link Library can be
updated without re-linking your application.
When your application is executed, it will use the updated version of
the Dynamic Link Library.
.note
Dynamic Link Libraries also allow sharing of code and data between the
applications that use them.
If many applications that use the same Dynamic Link Library are
executing concurrently, the sharing of code and data segments improves
memory utilization.
.endnote
.*
.beglevel
.*
.section Creating a Dynamic Link Library
.*
.np
To create a Dynamic Link Library, you must place the "DLL" keyword
following the system name in the "SYSTEM" directive.
.millust begin
&sys_dr_dll.
.millust end
.pc
In addition, you must specify which functions in the Dynamic Link
Library are to be made available to applications which use it.
This is achieved by using the "EXPORT" directive for each function
that can be called by an application.
.np
Dynamic Link Libraries can reference other Dynamic Link Libraries.
References to other Dynamic Link Libraries are resolved by specifying
"IMPORT" directives or using import libraries.
.*
.section *refid=&unique_lbl.dll Using a Dynamic Link Library
.*
.np
To use a Dynamic Link Library, you must tell the &lnkname which
functions are contained in a Dynamic Link Library and the name of the
Dynamic Link Library.
This is achieved in two ways.
.np
The first method is to use the "IMPORT" directive.
The "IMPORT" directive names the function and the Dynamic Link Library
it belongs to so that the &lnkname can generate an import definition
in the program module.
.np
The second method is to use import libraries.
.ix 'import library'
An import library is a standard library which contains object modules
with special object records that define the functions belonging to a
Dynamic Link Library.
An import library is created from a Dynamic Link Library using the
&libname..
The resulting import library can then be specified in a "LIBRARY"
directive in the same way one would specify a standard library.
See the chapter entitled "The &libname" in the
.book &company C/C++ Tools User's Guide
or the
.book &company FORTRAN 77 Tools User's Guide
for more information on creating import libraries.
.np
Using an import library is the preferred method of providing
references to functions in Dynamic Link Libraries.
When a Dynamic Link Library is modified, typically the import library
corresponding to the modified Dynamic Link Library is updated to
reflect the changes.
Hence, any directive file that specifies the import library in a
"LIBRARY" directive need not be modified.
However, if you are using "IMPORT" directives, you may have to modify
the "IMPORT" directives to reflect the changes in the Dynamic Link
Library.
.endlevel
.do end
.*
.im wlmemlay
.*
.if '&exeformat' ne 'win nt' and '&exeformat' ne 'win vxd' .do begin
.im ms2wlink
.do end
