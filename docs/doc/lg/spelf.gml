.*
.*
.helppref ELF:
:set symbol="unique_lbl" value="elf".
:set symbol="opsys" value="ELF".
:set symbol="sysprompt" value="".
:set symbol="exeformat" value="elf".
:set symbol="exefmtup" value="ELF".
:set symbol="form_drctv" value="form elf 'test'".
:set symbol="syst_drctv" value="form elf 'test'".
:set symbol="exeextn" value="exe".
:set symbol="libvarname" value="lib".
:set symbol="tmpvarname" value="tmp".
.*
.im dosfiles
.*
.chap *refid=elfchap The ELF Executable File Format
.*
.im wlintro
.im wlsyntax
.np
where
.bi directive
is any of the following:
.cw `
.begnote $compact
.note ALIAS alias_name=symbol_name{,alias_name=symbol_name}
.note DEBUG dbtype [dblist] | DEBUG [dblist]
.note DISABLE msg_num{,msg_num}
.note ENDLINK
.note EXPORT entry_name {,entry_name}
.note FILE obj_spec{,obj_spec}
.note FORMAT ELF [DLL]
.note IMPORT external_name {,external_name}
.note LANGUAGE lang
.note LIBFILE obj_file{,obj_file}
.note LIBPATH path_name{&ps.path_name}
.note LIBRARY library_file{,library_file}
.note MODFILE obj_file{,obj_file}
.note MODTRACE obj_module{,obj_module}
.note MODULE module_name {,module_name}
.note NAME exe_file
.note OPTION option{,option}
:cmt. where option is one of
.begnote $compact
.note ALIGNMENT=n
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
.note MAXERRORS=n
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
.note PATH path_name{&ps.path_name}
.note REFERENCE symbol_name{,symbol_name}
.note RUNTIME run_option
.note SORT [GLOBAL] [ALPHABETICAL]
.note STARTLINK
.note SYMTRACE symbol_name{,symbol_name}
.note SYSTEM BEGIN system_name {directive} END
.note SYSTEM system_name
.note # comment
.note @ directive_file
.endnote
.cw ;
:CMT. .synote
:CMT. .im objfile
:CMT. .im libfile
:CMT. .im objmod
:CMT. .mnote exe_file
:CMT. is a file specification for the name of the executable file.
:CMT. If no file extension is specified, a file extension of "exe" is
:CMT. assumed.
:CMT. If a Dynamic Link Library file is being generated, a file extension of
:CMT. "dll" is assumed.
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
.im wlmemlay
.*
.helppref
