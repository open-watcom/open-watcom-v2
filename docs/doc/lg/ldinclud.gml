.*
.*
.section The @ Directive
.*
.np
.ix 'directives' 'include'
.ix 'include directive'
The "@" directive instructs the &lnkname to process directives from an
alternate source.
The format of the "@" directive is as follows.
.mbigbox
    @directive_var
        or
    @directive_file
.embigbox
.synote
.mnote directive_var
is the name of an environment variable.
The directives specified by the value of
.mono directive_var
will be processed.
.mnote directive_file
is a file specification for the name of a linker directive file.
A file extension of "lnk" is assumed if no file extension is specified.
.esynote
.np
The environment variable approach to specifying linker directives
allows you to specify commonly used directives without having to
specify them each time you invoke the &lnkname..
If the environment variable "wlink" is set as in the following example,
.millust begin
&setcmd wlink=debug watcom all option map, verbose library math
&lnkcmd @wlink
.millust end
.pc
then each time the &lnkname is invoked, full debugging information
will be generated, a verbose map file will be created, and the library
file "math.lib" will be searched for undefined references.
.np
A linker directive file is useful, for example, when the linker input
consists of a large number of object files and you do not want to type
their names on the command line each time you link your program.
Note that a linker directive file can also include other linker
directive files.
.np
Let the file "memos.lnk" be a directive file containing the following
lines.
.millust begin
&syst_drctv
name memos
file memos
file actions
file read
file msg
file prompt
file memmgr
library &pc.termio&pc.screen
library &pc.termio&pc.keyboard
.millust end
.begnote
.note Win16 only:
.ix 'window function'
We must also use the "EXPORT" directive to define the window function.
This is done using the following directive.
.millust begin
export window_function
.millust end
.endnote
.np
Consider the following example.
.exam begin
&sysprompt.&lnkcmd @memos
.exam end
.np
The &lnkname is instructed to process the contents of the directive
file "memos.lnk".
The executable image file will be called "memos.&exeextn".
The following object files will be loaded from the current directory.
.millust begin
memos.&obj
actions.&obj
read.&obj
msg.&obj
prompt.&obj
memmgr.&obj
.millust end
.pc
If any unresolved symbol references remain after all object files have
been processed, the library files "screen.lib" and "keyboard.lib" in
the directory "&pc.termio" will be searched (in the order listed).
.autonote Notes:
.note
In the above example, we did not provide the file extension when the
directive file was specified.
The &lnkname assumes a file extension of "lnk" if none is present.
.note
It is not necessary to list each object file and library with a
separate directive.
The following linker directive file is equivalent.
.millust begin
&syst_drctv
name memos
file memos,actions,read,msg,prompt,memmgr
library &pc.termio&pc.screen,&pc.termio&pc.keyboard
.millust end
.pc
However, if you want to selectively specify what debugging information
should be included, the first style of directive file will be easier
to use.
This is illustrated in the following sample directive file.
.millust begin
&syst_drctv
name memos
debug watcom lines
file memos
debug watcom all
file actions
debug watcom lines
file read
file msg
file prompt
file memmgr
debug watcom
library &pc.termio&pc.screen
library &pc.termio&pc.keyboard
.millust end
.note
Information for a particular directive can span directive files.
This is illustrated in the following sample directive file.
.millust begin
&syst_drctv
file memos, actions, read, msg, prompt, memmgr
file @dbgfiles
library &pc.termio&pc.screen
library &pc.termio&pc.keyboard
.millust end
.pc
The directive file "dbgfiles.lnk" contains, for example, those object
files that are used for debugging purposes.
.endnote
