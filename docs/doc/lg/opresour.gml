.*
.*
.section The RESOURCE Option
.if '&target' ne 'QNX' .do begin
.np
:SF font=9.Formats: &suppsys:eSF.
.do end
.ix 'RESOURCE option'
.ix 'options' 'RESOURCE'
.*
.np
For 16-bit OS/2 executable files and Win16 or Win32 executable files,
the "RESOURCE" option requests the linker to add the specified
resource file to the executable file being generated.
For QNX executable files, the "RESOURCE" option specifies the contents
of the resource record.
.*
.beglevel
.*
.section RESOURCE - OS/2, Win16, Win32 only
.*
.np
.ix 'resource file'
The "RESOURCE" option requests the linker to add the specified
resource file to the executable file that is being generated.
The format of the "RESOURCE" option (short form "RES") is as follows.
.mbigbox
    OPTION RESOURCE[=resource_file]
.embigbox
.synote
.mnote resource_file
is a file specification for the name of the resource file that is to
be added to the executable file.
If no file extension is specified, a file extension of "RES" is
assumed for all but QNX format executables.
.esynote
.np
The "RESOURCE" option cannot be used for 32-bit OS/2 executables.
.*
.section RESOURCE - QNX only
.*
.np
The "RESOURCE" option specifies the contents of the resource record in
QNX executable files.
The format of the "RESOURCE" option (short form "RES") is as follows.
.mbigbox
    OPTION RESOURCE resource_info

    resource_info ::= 'string' | =resource_file
.embigbox
.synote
.mnote resource_file
is a file specification for the name of the resource file.
No file extension is assumed.
.mnote string
is a sequence of characters which is placed in the resource record.
.esynote
.pc
If a resource file is specified, the contents of the resource file are
included in the resource record.
.np
The resource record contains, for example, help information and is
displayed when the following command is executed.
.millust begin
&sysprompt.use <executable>
.millust end
.np
.ix 'usemsg'
QNX also provides the
.bd usemsg
utility to manipulate the resource record of an executable file.
Its use is recommended.
This utility is described in the QNX "Utilities Reference" manual.
.*
.endlevel
