.*
.*
.helppref Phar Lap:
:set symbol="unique_lbl" value="phr".
:set symbol="opsys" value="Phar Lap 386|DOS-Extender".
:set symbol="sysprompt" value="".
:set symbol="exeformat" value="phar".
:set symbol="exefmtup" value="PHAR".
:set symbol="form_drctv" value="form phar".
:set symbol="syst_drctv" value="system pharlap".
:set symbol="exeextn" value="exp".
:set symbol="libvarname" value="lib".
:set symbol="tmpvarname" value="tmp".
.*
.im dosfiles
.*
.chap *refid=phrchap The Phar Lap Executable File Format
.*
.im wlintro
.im wlsyntax
.np
where
.bi directive
is any of the following:
.begnote $compact
.note ALIAS alias_name=symbol_name{,alias_name=symbol_name}
.note DEBUG dbtype [dblist] | DEBUG [dblist]
.note DISABLE msg_num{,msg_num}
.note ENDLINK
.note FILE obj_spec{,obj_spec}
.note FORMAT PHARLAP [EXTENDED | REX | SEGMENTED]
.note LANGUAGE lang
.note LIBFILE obj_file{,obj_file}
.cw !
.note LIBPATH path_name{&ps.path_name}
.cw ;
.note LIBRARY library_file{,library_file}
.note MODFILE obj_file{,obj_file}
.note MODTRACE obj_module{,obj_module}
.note NAME exe_file
.note OPTION option{,option}
:cmt. where option is one of
.begnote $compact
.note ARTIFICIAL
.note [NO]CACHE
.note [NO]CASEEXACT
.note CVPACK
.note DOSSEG
.note ELIMINATE
.note [NO]FARCALLS
.note INCREMENTAL
.note MANGLEDNAMES
.note MAP[=map_file]
.note MAXDATA=n
.note MAXERRORS=n
.note MINDATA=n
.note NAMELEN=n
.note NODEFAULTLIBS
.note NOEXTENSION
.note OFFSET=n
.note OSNAME='string'
.note QUIET
.note REDEFSOK
.note SHOWDEAD
.note STACK=n
.note START=symbol_name
.note STATICS
.note SYMFILE[=symbol_file]
.note [NO]UNDEFSOK
.note VERBOSE
.note VFREMOVAL
.endnote
.note OPTLIB library_file{,library_file}
.cw !
.note PATH path_name{&ps.path_name}
.cw ;
.note REFERENCE symbol_name{,symbol_name}
.note RUNTIME run_option{,run_option}
.note SORT [GLOBAL] [ALPHABETICAL]
.note STARTLINK
.note SYMTRACE symbol_name{,symbol_name}
.note SYSTEM BEGIN system_name {directive} END
.note SYSTEM system_name
.note # comment
.note @ directive_file
.endnote
:CMT. .synote
:CMT. .im objfile
:CMT. .im libfile
:CMT. .im objmod
:CMT. .mnote exe_file
:CMT. is a file specification for the name of the executable file.
:CMT. If no file extension is specified, a file extension of "exp" is assumed
:CMT. unless a relocatable executable file is being generated in which case
:CMT. a file extension of "rex" is assumed.
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
:CMT. is a symbol name.
:CMT. .mnote alias_name
:CMT. is the name of an alias symbol.
:CMT. .mnote system_name
:CMT. is the name of a system.
:CMT. .mnote comment
:CMT. is any sequence of characters.
:CMT. .im lnkvalue
:CMT. .esynote
.*
.im wlprompt
.*
.section 32-bit Protected-Mode Applications
.*
.np
.ix '386|DOS-Extender'
The &lnkname generates executable files that run under Phar Lap's
386|DOS-Extender.
386|DOS-Extender provides a 32-bit protected-mode environment for
programs running under PC DOS.
Running in 32-bit protected mode allows your program to access all of
the memory in your machine.
.np
.ix 'running in 32-bit protected mode'
Essentially, what 386|DOS-Extender does is provide an interface between
your application and DOS running in real mode.
Whenever your program issues a software interrupt (DOS and BIOS system
calls), 386|DOS-Extender intercepts the requests, transfers data
between the protected-mode and real-mode address space, and calls the
corresponding DOS system function running in real mode.
.*
.section Memory Usage
.*
.np
When running a program under 386|DOS-Extender, memory for the program is
allocated from conventional memory (memory below one megabyte) and
extended memory.
Conventional memory is allocated from a block of memory that is obtained
from DOS by 386|DOS-Extender at initialization time.
By default, all available memory is allocated at initialization time;
no conventional memory remains free.
The "MINREAL" and "MAXREAL" options of the "RUNTIME" directive control
the amount of conventional memory initially left free by 386|DOS-Extender.
.np
Part of the conventional memory allocated at initialization is required
by 386|DOS-Extender.
The following is allocated from conventional memory for use by 386|DOS-Extender.
.autonote
.note
A data buffer is allocated and is used to pass data to DOS and BIOS
system functions.
The size allocated is controlled by the "MINIBUF" and "MAXIBUF" options of
the "RUNTIME" directive.
.note
Stack space is allocated and is used for switching between 32-bit
protected mode and real mode.
The size allocated is controlled by the "NISTACK" and "ISTKSIZE"
options of the "RUNTIME" directive.
.note
A call buffer is allocated and is used for passing data on function
calls between 32-bit protected mode and real mode.
The size allocated is controlled by the "CALLBUFS" option of
the "RUNTIME" directive.
.endnote
.np
When a program is loaded by 386|DOS-Extender, memory to hold the entire
program is allocated.
In addition, memory beyond the end of the program is allocated for use
by the program.
By default, all extra memory is allocated when the program is loaded.
It is assumed that any memory not required by the program is freed by
the program.
The amount of memory allocated at the end of the program is controlled
by the "MINDATA" and "MAXDATA" options.
.*
.im wlmemlay
.im wlmem
.*
.helppref
