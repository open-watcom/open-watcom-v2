.*
.*
.helppref QNX:
:set symbol="unique_lbl" value="qnx".
:set symbol="opsys" value="QNX".
:set symbol="sysprompt" value="".
:set symbol="exeformat" value="qnx".
:set symbol="exefmtup" value="QNX".
:set symbol="form_drctv" value="form qnx".
:set symbol="syst_drctv" value="system qnx386".
:set symbol="exeextn" value="qnx".
:set symbol="libvarname" value="LIB".
:set symbol="tmpvarname" value="TMPDIR".
.*
.im qnxfiles
.*
.chap *refid=qnxchap The QNX Executable File Format
.*
.im wlintro
.im wlsyntax
.np
where
.bi directive
is any of the following:
.begnote $compact
.note ALIAS symbol_name=symbol_name{,symbol_name=symbol_name}
.note DEBUG dbtype [dblist] | DEBUG [dblist]
.note DISABLE msg_num{,msg_num}
.note ENDLINK
.note FILE obj_spec{,obj_spec}
.note FORMAT QNX [FLAT]
.note LANGUAGE
.note LIBFILE obj_file{,obj_file}
.cw !
.note LIBPATH path_name{&ps.path_name}
.cw ;
.note LIBRARY library_file{,library_file}
.note MODFILE obj_file{,obj_file}
.note MODTRACE obj_spec{,obj_spec}
.note NAME exe_file
.note NEWSEGMENT
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
.note HEAPSIZE=n
.note INCREMENTAL
.note LINEARRELOCS
.note LONGLIVED
.note MANGLEDNAMES
.note MAP[=map_file]
.note MAXERRORS=n
.note NAMELEN=n
.note NODEFAULTLIBS
.note NOEXTENSION
.note NORELOCS
.note OFFSET=n
.note OSNAME='string'
.note PACKCODE=n
.note PACKDATA=n
.note PRIVILEGE=n
.note QUIET
.note REDEFSOK
.note RESOURCE[=resource_file | 'string']
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
.note SEGMENT seg_desc{,seg_desc}
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
:CMT. .if '&target' eq 'QNX' .do begin
:CMT. No file extension is assumed.
:CMT. .do end
:CMT. .el .do begin
:CMT. If no file extension is specified, a file extension of "qnx" is
:CMT. assumed if you are running a DOS, OS/2 or Windows-hosted version of
:CMT. the &lnkname..
:CMT. No file extension is assumed if you are running a QNX-hosted version
:CMT. of the &lnkname..
:CMT. .do end
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
:CMT. .im lnkvalue
:CMT. .esynote
.*
.im wlprompt
.*
.im wlmemlay
.*
.helppref
