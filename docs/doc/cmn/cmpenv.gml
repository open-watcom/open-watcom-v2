.if '&lang' eq 'C/C++' .do begin
.*
.section &cvarup16/&pvarup16/&cvarup32/&pvarup32 Environment Variables
.*
.sr cname=&cmpcname
.do end
.el .do begin
.*
.section &cvarup16/&cvarup32 Environment Variables
.*
.sr cname=&cmpname
.do end
.*
.np
The
.ev &cvarup16
and
.ev &cvarup32
environment variables can be used to specify commonly used &cname
options.
.if '&lang' eq 'C/C++' .do begin
The
.ev &pvarup16
and
.ev &pvarup32
environment variables can be used to specify commonly used &cmppname
options.
.do end
These options are processed before options specified on the command
line.
.ix '&setcmdup' '&cvarup16 environment variable'
.ix '&setcmdup' '&cvarup32 environment variable'
.if '&cmpclass' eq 'load-n-go' .do begin
.exam begin 1
&prompt.&setcmd &setdelim.&cvar16=&sw.noext &sw.nowarn&setdelim
&prompt.&setcmd &setdelim.&cvar32=&sw.noext &sw.nowarn&setdelim
.exam end
.pc
The above example defines the default options to be "noext" (do not issue
extension messages), and "nowarn" (do not issue warning messages).
.do end
.el .do begin
.exam begin 2
&prompt.&setcmd &setdelim.&cvar16=&sw.d1 &sw.ot&setdelim
&prompt.&setcmd &setdelim.&cvar32=&sw.d1 &sw.ot&setdelim
.if '&lang' eq 'C/C++' .do begin
&prompt.&setcmd &setdelim.&pvar16=&sw.d1 &sw.ot&setdelim
&prompt.&setcmd &setdelim.&pvar32=&sw.d1 &sw.ot&setdelim
.do end
.exam end
.pc
The above example defines the default options to be "d1" (include line
number debugging information in the object file), and "ot" (favour
time optimizations over size optimizations).
.do end
.if '&target' ne 'QNX' .do begin
.np
.ix 'environment string' '= substitute'
.ix 'environment string' '#'
Whenever you wish to specify an option that requires the use of an
"=" character, you can use the "#" character in its place.
This is required by the syntax of the "&setcmdup" command.
.do end
.np
Once the
.ev &cvarup16
or
.ev &cvarup32
environment variable has been defined, those options listed become the
default each time the 16-bit or 32-bit &cname compiler is used.
The
.ev &cvarup16
environment variable is used by the 16-bit &cname compiler only.
The
.ev &cvarup32
environment variable is used by the 32-bit &cname compiler only.
The &cname compiler command line can be used to override any options
specified in the environment string.
.*
.if '&lang' eq 'C/C++' .do begin
.np
Once the
.ev &pvarup16
or
.ev &pvarup32
environment variable has been defined, those options listed become the
default each time the 16-bit or 32-bit &cmppname compiler is used.
The
.ev &pvarup16
environment variable is used by the 16-bit &cmppname compiler only.
The
.ev &pvarup32
environment variable is used by the 32-bit &cmppname compiler only.
The &cmppname compiler command line can be used to override any
options specified in the environment string.
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.if '&target' ne 'QNX' .do begin
.np
These environment variables are not examined by &wclcmdup16 or
&wclcmdup32, the &wclname utilities.
Since &wclcmdup16 passes the relevant options found in the
.ev &wclvarup16
environment variable to the compiler command line, the
.ev &wclvarup16
environment variable options take precedence over the
.ev &cvarup16
.if '&lang' eq 'C/C++' .do begin
and
.ev &pvarup16
.do end
environment variable options.
Since &wclcmdup32 passes the relevant options found in the
.ev &wclvarup32
environment variable to the compiler command line, the
.ev &wclvarup32
environment variable options take precedence over the
.ev &cvarup32
.if '&lang' eq 'C/C++' .do begin
and
.ev &pvarup32
.do end
environment variable options.
.do end
.do end
.keep 18
.hint
.if '&target' eq 'QNX' .do begin
.ix 'user initialization file'
If you use the same compiler options all the time, you may find it
handy to define the environment variable in your user initialization
file.
.do end
.el .do begin
.ix 'system initialization file' 'AUTOEXEC.BAT'
.ix 'AUTOEXEC.BAT' 'system initialization file'
If you are running DOS and you use the same compiler options all the
time, you may find it handy to define the environment variable in your
DOS system initialization file,
.fi AUTOEXEC.BAT.
.np
.ix 'system initialization' 'Windows NT'
.ix 'Windows NT' 'system initialization'
If you are running Windows NT, use the "System" icon in the
.bd Control Panel
to define environment variables.
.np
.ix 'system initialization file' 'CONFIG.SYS'
.ix 'CONFIG.SYS' 'system initialization file'
If you are running OS/2 and you use the same compiler options all the
time, you may find it handy to define the environment variable in your
OS/2 system initialization file,
.fi CONFIG.SYS.
.do end
.ehint
