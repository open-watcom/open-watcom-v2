.*
.*
.dirctv SYSTEM
.*
.np
There are three forms of the "SYSTEM" directive.
.np
The first form of the "SYSTEM" directive (short form "SYS") is called a
system definition directive.
It allows you to associate a set of linker directives with a specified
name called the
.us system name.
.ix 'system name'
This set of linker directives is called a system definition block.
The format of a system definition directive is as follows.
.mbigbox
    SYSTEM BEGIN system_name {directive} END
.embigbox
.synote
.mnote system_name
is a unique system name.
.mnote directive
is a linker directive.
.esynote
.pc
A system definition directive cannot be specified within another
system definition directive.
.np
The second form of the "SYSTEM" directive is called a system deletion
directive.
It allows you to remove the association of a set of linker directives
with a
.us system name.
The format of a system deletion directive is as follows.
.mbigbox
    SYSTEM DELETE system_name
.embigbox
.synote
.mnote system_name
is a defined system name.
.esynote
.np
The third form of the "SYSTEM" directive is as follows.
.mbigbox
    SYSTEM system_name
.embigbox
.synote
.mnote system_name
is a defined system name.
.esynote
.np
When this form of the "SYSTEM" directive is encountered, all directives
specified in the system definition block identified by
.id system_name
will be processed.
.np
Let us consider an example that demonstrates the use of the "SYSTEM"
directive.
The following linker directives define a system called
.us statistics.
.millust begin
system begin statistics
format dos
libpath &pc.libs
library stats, graphics
option stack=8k
end
.millust end
.pc
They specify that a
.us statistics
application is to be created by using the libraries "stats.lib" and
"graphics.lib".
These library files are located in the directory "&pc.libs".
The application requires a stack size of 8k and the specified format
of executable will be generated.
.np
Suppose the linker directives in the above example are contained in
the file "stats.lnk".
If we wish to create a
.us statistics
application, we can issue the following command.
.millust begin
&sysprompt.&lnkcmd @stats system statistics file myappl
.millust end
.np
As demonstrated by the above example, the "SYSTEM" directive can be
used to localize the common attributes that describe a class of
applications.
.np
The system deletion directive can be used to redefine a previously
defined system.
Consider the following example.
.millust begin
system begin at_dos
    libpath %WATCOM%\lib286
    libpath %WATCOM%\lib286\dos
    format dos ^
end
system begin n98_dos
    sys at_dos ^
    libpath %WATCOM%\lib286\dos\n98
end
system begin dos
sys at_dos ^
end
.millust end
.np
If you wish to redefine the definition of the "dos" system,
you can specify the following set of directives.
.millust begin
system delete dos
system begin dos
sys n98_dos ^
end
.millust end
.pc
This effectively redefines a "dos" system to be equivalent to a
"n98_dos" system (NEC PC-9800 DOS), rather than the previously defined
"at_dos" system (AT-compatible DOS).
.np
.ix 'default directive file'
.ix 'wlink.lnk' 'default directive file'
For additional examples on the use of the "SYSTEM" directive, examine
the contents of the
:FNAME.wlink.lnk:eFNAME.
and
:FNAME.wlsystem.lnk:eFNAME.
files.
.*
.im wlinklnk
.*
.beglevel
.*
.section Special System Names
.*
.np
There are two special system names.
When the linker has processed all object files and the executable file
format has not been determined, and a system definition block has not
been processed, the directives specified in the "286" or "386" system
definition block will be processed.
The "386" system definition block will be processed if a 32-bit object
file has been processed.
Furthermore, only a restricted set of linker directives is allowed in
a "286" and "386" system definition block.
They are as follows.
.begbull
.bull
FORMAT
.bull
LIBFILE
.bull
LIBPATH
.bull
LIBRARY
.bull
NAME
.bull
OPTION
.bull
RUNTIME (for Phar Lap executable files only)
.bull
SEGMENT (for OS/2 and QNX executable files only)
.endbull
.*
.endlevel
