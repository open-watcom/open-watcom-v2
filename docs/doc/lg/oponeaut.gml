.*
.*
.option ONEAUTODATA
.*
.np
The "ONEAUTODATA" option specifies that the automatic data segment
(default data segment defined by the group "DGROUP"), for the program
module or Dynamic Link Library (DLL) being created, will be shared by
all instances.
The format of the "ONEAUTODATA" option (short form "ONE") is as
follows.
.mbigbox
    OPTION ONEAUTODATA
.embigbox
.np
The default for a Dynamic Link Library is "ONEAUTODATA" and for a
program module is "MANYAUTODATA".
If you do not want the data area of a DLL to be shared across multiple
applications, then you should specify "OPTION MANYAUTODATA".
.begnote
.note Win16:
Note, however, that this attribute is not supported by Windows 3.x for
16-bit DLLs.
.endnote
.np
You should also see the related section
entitled :HDREF refid='xformat'.
for information on the "INITINSTANCE", "TERMINSTANCE",
"INITGLOBAL", and "TERMGLOBAL" DLL attributes.
