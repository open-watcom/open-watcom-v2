.if '&target' eq 'QNX' .do begin
. :set symbol="libopt"    value="-l ".
. :set symbol="debugopt"  value="g".
. :set symbol="nameopt"   value="o".
. :set symbol="namesuff"  value=" ".
.do end
.el .do begin
. :set symbol="libopt"    value="".
. :set symbol="debugopt"  value="d2".
. :set symbol="nameopt"   value="fe".
. :set symbol="namesuff"  value="=".
.do end
.if '&target' eq 'QNX' .do begin
. .im CC
.do end
.el .do begin
. .if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
. . .im WCL
. .do end
. .if '&lang' eq 'FORTRAN 77' .do begin
. . .im WFL
. .do end
.do end
.*
.if '&target' ne 'QNX' .do begin
.*
.section &wclvarup16/&wclvarup32 Environment Variables
.*
.np
The
.ev &wclvarup16
environment variable can be used to specify commonly used
.kw &wclcmdup16
options.
The
.ev &wclvarup32
environment variable can be used to specify commonly used
.kw &wclcmdup32
options.
These options are processed before options specified on the command
line.
.ix 'environment variables' '&wclvarup16'
.ix 'environment variables' '&wclvarup32'
.ix '&wclvarup16 environment variable'
.ix '&wclvarup32 environment variable'
.ix '&setcmdup' '&wclvarup16 environment variable'
.ix '&setcmdup' '&wclvarup32 environment variable'
.exam begin 3
&prompt.&setcmd &setdelim.&wclvar16=&sw.d1 &sw.ot&setdelim

&prompt.&setcmd &setdelim.&wclvar32=&sw.d1 &sw.ot&setdelim
.exam end
.pc
The above example defines the default options to be "d1" (include line
number debugging information in the object file), and "ot" (favour
time optimizations over size optimizations).
.np
.ix 'environment string' '= substitute'
.ix 'environment string' '#'
Whenever you wish to specify an option that requires the use of an
"=" character, you can use the "#" character in its place.
This is required by the syntax of the "&setcmdup" command.
.np
Once the appropriate environment variable has been defined, those
options listed become the default each time the
.kw &wclcmdup16
or
.kw &wclcmdup32
command is used.
.np
The
.ev &wclvarup16
environment variable is used by
.kw &wclcmdup16
only.
The
.ev &wclvarup32
environment variable is used by
.kw &wclcmdup32
only.
Both
.kw &wclcmdup16
and
.kw &wclcmdup32
pass the relevant options to the &cmpname compiler and linker.
This environment variable is not examined by the &cmpname compiler
or the linker when invoked directly.
.hint
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT' 'system initialization file'
If you are running DOS and you use the same
.kw &wclcmdup16
or
.kw &wclcmdup32
options all the time, you may find it handy to place the
"&setcmdup &wclvarup16" or
"&setcmdup &wclvarup32"
command in your DOS system initialization file,
.fi AUTOEXEC.BAT.
.ix 'system initialization file' 'CONFIG.SYS'
.ix 'CONFIG.SYS' 'system initialization file'
If you are running OS/2 and you use the same
.kw &wclcmdup16
or
.kw &wclcmdup32
options all the time, you may find it handy to place the
"&setcmdup &wclvarup16" or
"&setcmdup &wclvarup32"
command in your OS/2 system initialization file,
.fi CONFIG.SYS.
.ehint
.do end
.*
.section &wclcmdup16/&wclcmdup32 Command Line Examples
.*
.np
For most small applications, the
.kw &wclcmdup16
or
.kw &wclcmdup32
command will suffice.
We have only scratched the surface in describing the capabilities
of the
.kw &wclcmdup16
and
.kw &wclcmdup32
commands.
The following examples describe the
.kw &wclcmdup16
and
.kw &wclcmdup32
commands in more detail.
.np
Suppose that your application is contained in three files called
.fi apdemo.&langsuff
.ct ,
.fi aputils.&langsuff
.ct , and
.fi apdata.&langsuff..
We can compile and link all three files with one command.
.exam begin 2 ~b1
&prompt.&wclcmd16 &sw.&debugopt apdemo.&langsuff aputils.&langsuff apdata.&langsuff
&prompt.&wclcmd32 &sw.&debugopt apdemo.&langsuff aputils.&langsuff apdata.&langsuff
.exam end
.pc
The executable program will be stored in
.if '&target' eq 'QNX' .do begin
.fi a.out.
.do end
.el .do begin
.fi apdemo&exe
since
.fi apdemo
appeared first in the list.
.do end
Each of the three files is compiled with the "&debugopt" debug option.
Debugging information is included in the executable file.
.np
We can issue a simpler command if the current directory contains only
our three &lang source files.
.exam begin 2 ~b2
&prompt.&wclcmd16 &sw.&debugopt *.&langsuff
&prompt.&wclcmd32 &sw.&debugopt *.&langsuff
.exam end
.pc
.kw &wclcmdup16
or
.kw &wclcmdup32
will locate all files with the "&cxt" filename extension and compile
each of them.
.if '&target' eq 'QNX' .do begin
The default name of the executable file will be
.fi a.out.
Since it is only possible to have one executable with the name
.fi a.out
in a directory,
.do end
.el .do begin
The name of the executable file will depend on which of the &lang
source files is found first.
Since this is a somewhat haphazard approach to naming the executable file,
.do end
.kw &wclcmdup16
and
.kw &wclcmdup32
have an option, "&nameopt", which will allow you to specify the name to
be used.
.exam begin 2 ~b3
&prompt.&wclcmd16 &sw.&debugopt &sw.&nameopt.&namesuff.apdemo *.&langsuff
&prompt.&wclcmd32 &sw.&debugopt &sw.&nameopt.&namesuff.apdemo *.&langsuff
.exam end
.pc
By using the "&nameopt" option, the executable file will always be called
.if '&target' eq 'QNX' .do begin
.fi apdemo&exe..
.do end
.el .do begin
.fi apdemo&exe
regardless of the order of the &lang source files in the directory.
.do end
.np
If the directory contains other &lang source files which are not
part of the application then other tricks may be used to identify a
subset of the files to be compiled and linked.
.exam begin 2 ~b4
&prompt.&wclcmd16 &sw.&debugopt &sw.&nameopt.&namesuff.apdemo ap*.&langsuff
&prompt.&wclcmd32 &sw.&debugopt &sw.&nameopt.&namesuff.apdemo ap*.&langsuff
.exam end
.pc
Here we compile only those &lang source files that begin with the
letters "ap".
.np
In our examples, we have recompiled all the source files each time.
In general, we will only compile one of them and include the object
code for the others.
.exam begin 2 ~b5
&prompt.&wclcmd16 &sw.&debugopt &sw.&nameopt.&namesuff.apdemo aputils.&langsuff ap*&obj
&prompt.&wclcmd32 &sw.&debugopt &sw.&nameopt.&namesuff.apdemo aputils.&langsuff ap*&obj
.exam end
.pc
The source file
.fi aputils.&langsuff
is recompiled and
.fi apdemo&obj
and
.fi apdata&obj
are included when linking the application.
The "&obj" filename extension indicates that this file need not be
compiled.
.exam begin 2 ~b6
&prompt.&wclcmd16 &sw.&nameopt.&namesuff.demo *.&langsuff utility&obj
&prompt.&wclcmd32 &sw.&nameopt.&namesuff.demo *.&langsuff utility&obj
.exam end
.pc
All of the &lang source files in the current directory are compiled
and then linked with
.fi utility&obj
to generate
.fi demo&exe..
.if '&target' ne 'QNX' .do begin
.ix '&setcmdup' '&wclvarup16 environment variable'
.ix '&setcmdup' '&wclvarup32 environment variable'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin 5 ~b7
&prompt.&setcmd &setdelim.&wclvar16=&sw.mm &sw.d1 &sw.ox &sw.k4096&setdelim
&prompt.&wclcmd16 &sw.&nameopt.&namesuff.grdemo gr*.&langsuff &libopt.graph.lib &sw.fd=grdemo

&prompt.&setcmd &setdelim.&wclvar32=&sw.d1 &sw.ox &sw.k4096&setdelim
&prompt.&wclcmd32 &sw.&nameopt.&namesuff.grdemo gr*.&langsuff &libopt.graph.lib &sw.fd=grdemo
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin 5 ~b7
&prompt.&setcmd &setdelim.&wclvar16=&sw.mm &sw.d1 &sw.op &sw.k=4096&setdelim
&prompt.&wclcmd16 &sw.&nameopt.&namesuff.grdemo gr*.&langsuff &libopt.graph.lib &sw.fd=grdemo

&prompt.&setcmd &setdelim.&wclvar32=&sw.d1 &sw.op &sw.k=4096&setdelim
&prompt.&wclcmd32 &sw.&nameopt.&namesuff.grdemo gr*.&langsuff &libopt.graph.lib &sw.fd=grdemo
.exam end
.do end
.pc
All &lang source files beginning with the letters "gr" are compiled
and then linked with
.fi graph.lib
to generate
.fi grdemo&exe
which uses a 4K stack.
The temporary linker directive file that is created by
.kw &wclcmdup16
or
.kw &wclcmdup32
will be kept and renamed to
.fi grdemo.lnk.
.ix 'LIBOS2 environment variable'
.ix '&libvarup environment variable'
.ix '&wclvarup16 environment variable'
.ix '&wclvarup32 environment variable'
.ix '&setcmdup' 'LIBOS2 environment variable'
.ix '&setcmdup' '&libvarup environment variable'
.ix '&setcmdup' '&wclvarup16 environment variable'
.ix '&setcmdup' '&wclvarup32 environment variable'
.ix '&wclcmdup16 options' 'lp'
.ix '&wclcmdup32 options' 'lp'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin 8 ~b8
&prompt.&setcmd libos2=&dr3.&pathnam.&libdir16.\os2;&dr3\os2
&prompt.&setcmd &libvar=&dr3.&pathnam.&libdir16.\dos;&dr3.&pathnam.&libdir16.
&prompt.&setcmd &setdelim.&wclvar16=&sw.mm &sw.lp&setdelim
&prompt.&wclcmd16 grdemo1 &pathnam.&libdir16.\os2\graphp.obj phapi.lib
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin 8 ~b8
&prompt.&setcmd libos2=&dr3.&pathnam.&libdir16.\os2;&dr3\os2
&prompt.&setcmd &libvar=&dr3.&pathnam.&libdir16.\dos
&prompt.&setcmd &setdelim.&wclvar16=&sw.mm &sw.lp&setdelim
&prompt.&wclcmd16 grdemo1 &pathnam.&libdir16.\os2\graphp.obj phapi.lib
.exam end
.do end
.pc
.ix 'GRAPHP.OBJ'
.ix 'Phar Lap' '286 DOS Extender'
.ix 'DOS Extender' 'Phar Lap 286'
.ix 'PHAPI.LIB'
.ix 'OS/2'
The file
.fi grdemo1
is compiled for the medium memory model and then linked with
.fi graphp.obj
and
.fi phapi.lib
to generate
.fi grdemo1&exe
which is to be used with Phar Lap's 286 DOS Extender.
The "lp" option indicates that an OS/2 format executable is to be
created.
The file
.fi graphp.obj
in the directory "&pathnamup.&libdirup16\OS2" contains special
initialization code for Phar Lap's 286 DOS Extender.
The file
.fi phapi.lib
is part of the Phar Lap 286 DOS Extender package.
.ix 'GRAPH.LIB'
The
.ev LIBOS2
environment variable must include the location of the OS/2 libraries
and the
.ev LIB
environment variable must include the location of the DOS libraries
(in order to locate
.fi graph.lib
.ct ).
.ix 'DOSCALLS.LIB'
.ix 'OS/2' 'DOSCALLS.LIB'
The
.ev LIBOS2
environment variable must also include the location of the OS/2 file
.fi doscalls.lib
which is usually "C:\OS2".
.do end
.np
For more complex applications, you should use the "Make" utility.
.if '&target' eq 'QNX' .do begin
See the QNX utilities manual for a description of "Make".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
. .im owcc
.do end
