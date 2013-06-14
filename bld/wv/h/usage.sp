:segment linkres
:segment qnx
%C  {-options} [:symbol_file] [cmd_line]

cmd_line is one of:
    [//nid] pid
    [//nid] executable {arguments}

options are:
    Console=n           put debugger screen on console number 'n'
    Console=device      or put debugger screen on device 'device'
    LInes=n             set debugger screen to 'n' lines
    COlumns=n           set debugger screen to 'n' columns
    DYnamic=n           set initial dynamic memory to 'n' bytes
    TRap=file           use 'file' as the debugger's trap file
    Invoke=file         invoke 'file' as initial debugger setup file
    NOInvoke            don't invoke any debugger setup file
    NOSymbols           ignore any symbolic information
    NOMouse             ignore any mouse
    NOFpu               ignore any floating point unit in debuggee
    DIp=file            load specified Debug Information Processor
    INITcmd=cmd         process 'cmd' at startup
    REMotefiles         files are from the remote machine by default
    XConfig=string      pass X Windows configuration option to xqsh
:endsegment
:elsesegment
:segment ENGLISH
Usage:   wd [options] [:symbol_file] [cmd_line]

:segment qnx
cmd_line is one of:
    [//nid] pid
    [//nid] executable {arguments}
:elsesegment linux|os2|nt
cmd_line is one of:
    pid (in decimal)            attach to running process
    executable [arguments]      start 'executable' under debugger control
:elsesegment
cmd_line is:
    executable [arguments]
:endsegment

sym_file (optional) is name of file that wd will read symbols from

Options are case insensitive, may be shortened to the letters
listed in uppercase.

:segment qnx|linux
Options:
:elsesegment
Options:            ( /option is also supported )
:endsegment
-Help               print this usage help
-LInes=n            set debugger screen to 'n' lines
-COlumns=n          set debugger screen to 'n' columns
-DYnamic=n          set initial dynamic memory to 'n' bytes
-TRap=file[;arg]    use 'file' as the debugger's trap file and pass it 'arg'
-Invoke=file        invoke 'file' as initial debugger setup file
-NOInvoke           don't invoke any debugger setup file
-NOSYmbols          ignore any symbolic information
-NOMouse            ignore any mouse
-DIp=file           load specified Debug Information Processor
-INItcmd=cmd        process 'cmd' at startup
-REMotefiles        files are accessed on the remote machine by default
-DOwnload           download executable to the remote machine
-LOcalinfo=file     look for symbols in 'file' on local system first
:segment qnx|linux
-NOExports          do not look for exported symbols in shared libraries
:elsesegment
-NOExports          do not look for exported symbols in DLLs
:endsegment
-DEfersymbols       defer symbol loading
-NOSOurcecheck      do not automatically switch to source view when stopping
:segment qnx|linux
-Console=n          put debugger screen on console number 'n'
-Console=device     or put debugger screen on device 'device'
:endsegment
:segment qnx
-XConfig=string     pass X Windows configuration option to xqsh
:endsegment
:segment linux
-XConfig=string     pass X Windows configuration option to xterm
:endsegment
:segment HIDDEN
-CONtinueunexpectedbreak    continue execution after unexpected stops
-POWerbuilder               run in PowerBuilder mode
:endsegment
:elsesegment JAPANESE
Someone please translate the Japanese usage help
:endsegment
:segment IS_RC
.
:endsegment
:endsegment