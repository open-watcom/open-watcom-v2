.*
.*
.option MANYAUTODATA
.*
.np
The "MANYAUTODATA" option specifies that a copy of the automatic data
segment (default data segment defined by the group "DGROUP"),
for the program module or Dynamic Link Library (DLL) being created,
is made for each instance.
The format of the "MANYAUTODATA" option (short form "MANY") is as
follows.
.mbigbox
    OPTION MANYAUTODATA
.embigbox
.np
The default for a program module is "MANYAUTODATA" and for a Dynamic
Link Library is "ONEAUTODATA".
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
