.*
.*
.helppref RAW:
:set symbol="unique_lbl" value="raw".
:set symbol="opsys" value="RAW".
:set symbol="sysprompt" value="".
:set symbol="exeformat" value="raw".
:set symbol="exefmtup" value="RAW".
:set symbol="form_drctv" value="form RAW".
:set symbol="exeextn" value="bin".
:set symbol="libvarname" value="lib".
:set symbol="tmpvarname" value="tmp".
.*
.im dosfiles
.*
.chap *refid=rawchap The RAW File Format
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
.note FORMAT RAW [BIN | HEX]
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
.note MAXERRORS=n
.note NAMELEN=n
.note NODEFAULTLIBS
.note NOEXTENSION
.note OFFSET=n
.note OSNAME='string'
.note QUIET
.note REDEFSOK
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
:CMT. is a file specification for the name of the output file.
:CMT. If no file extension is specified, a file extension of "bin" is assumed
:CMT. unless a "HEX" image file is being generated, in which case a file
:CMT. extension of "hex" is assumed.
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
.im ms2wlink
.*
.helppref
