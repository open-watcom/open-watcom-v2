.*
.*
.* used by many, empty string for all
:set symbol="sysprompt" value="".
.* used by wldebug, wllib, wloptlib, wlname, wlpath, wlsystem, wlinc (in .if)
:set symbol="exeformat" value="my_os".
.* used by wlfile, wlinc, wllib, wloptlib, wlmodtrc, wlname, wlsymtrc
:set symbol="syst_drctv" value="system my_os".
.* used by wlname, wlinc
:set symbol="exeextn" value="exe".
.* used by wllib, wloptlib
:set symbol="libvarname" value="lib".
.* used by wlmem
:set symbol="tmpvarname" value="tmp".
.*
.if '&target' eq 'QNX' .do begin
.im qnxfiles
.do end
.el .do begin
.im dosfiles
.do end
.*
.chap *refid=genchap Linker Directives and Options
.*
.np
.ix 'general directives/options'
The &lnkname supports a large set of directives and options.
The following sections present these directives and options in
alphabetical order.
.if '&target' ne 'QNX' .do begin
Not all directives and options are supported for all executable
formats.
When a directive or option applies only to a subset of the executable
formats that the linker can generate, the supporting formats are
noted.
In the following example, the notation indicates that the directive
or option is supported for all executable formats.
.exam begin
Formats: All
.exam end
.np
In the following example, the notation indicates that the directive
or option is supported for OS/2, 16-bit Windows and 32-bit
Windows executable formats only.
.exam begin
Formats: OS/2, Win16, Win32
.exam end
.do end
.np
.ix 'directives'
Directives tell the &lnkname how to create your program.
For example, using directives you can tell the &lnkname which object
files are to be included in the program, which library files to search
to resolve undefined references, and the name of the executable file.
.*
.im wlinklnk
.*
.np
.ix 'using environment variables in directives'
It is also possible to use environment variables when specifying a
directive.
For example, if the
.ev LIBDIR
environment variable is defined as follows,
.millust begin
&setcmd libdir=&pc.test
.millust end
.pc
then the linker directive
.millust begin
.if '&target' eq 'QNX' .do begin
library $libdir&pc.mylib
.do end
.el .do begin
library %libdir%&pc.mylib
.do end
.millust end
.pc
is equivalent to the following linker directive.
.millust begin
library &pc.test&pc.mylib
.millust end
.pc
Note that a space must precede a reference to an environment variable.
.np
Many directives can take a list of one or more arguments separated by
commas.
Instead of a comma-delimited list, you can specify a space-separated
list provided the list is enclosed in braces
(e.g., { space delimited list }).
For example, the "FILE" directive can take a list of object file names
as an argument.
.millust begin
file first,second,third,fourth
.millust end
.pc
The alternate way of specifying this is as follows.
.millust begin
file {first second third fourth}
.millust end
.np
Where this comes in handy is in make files, where a list of dependents
is usually a space-delimited list.
.millust begin
OBJS = first second third fourth
    .
    .
    .
    &lnkcmd file {$(objs)}
.millust end
.*
.np
.ix 'notation'
.ix '&lnkcmdup notation'
.ix 'linking notation'
The following notation is used to describe the syntax of linker
directives and options.
.begpoint
.point ABC
All items in upper case are required.
.point [abc]
The item
.us abc
is optional.
.point {abc}
The item
.us abc
may be repeated zero or more times.
.point {abc}+
The item
.us abc
may be repeated one or more times.
.point a|b|c
One of
.us a,
.us b
or
.us c
may be specified.
.point a ::= b
The item
.us a
is defined in terms of
.us b.
.endpoint
.np
Certain characters have special meaning to the linker.
.ix 'apostrophes'
.ix 'special characters'
.ix 'punctuation characters'
.ix 'space character'
.ix 'blanks in file names'
When a special character must appear in a name, you can imbed the
string that makes up the name inside apostrophes (e.g., 'name@8').
This prevents the linker from interpreting the special character in
its usual manner.
This is also true for file or path names that contain spaces (e.g.,
'\program files\software\mylib').
Normally, the linker would interpret a space or blank in a file name
as a separator.
The special characters are listed below:
.millust begin
+-----------+-----------------------+
| Character | Name of Character     |
+-----------+-----------------------+
|           | Blank                 |
| =         | Equals                |
| (         | Left Parenthesis      |
| )         | Right Parenthesis     |
| ,         | Comma                 |
| .         | Period                |
| {         | Left Brace            |
| }         | Right Brace           |
| @         | At Sign               |
| #         | Hash Mark             |
| %         | Percentage Symbol     |
+-----------+-----------------------+
.millust end
.im lnkdir
