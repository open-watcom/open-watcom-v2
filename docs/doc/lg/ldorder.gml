.*
.*
.dirctv ORDER
.*
.np
The "ORDER" directive is used to specify the order in which classes are
placed into the output image, and the order in which segments are linked
within a class. The directive can optionally also specify the starting
address of a class or segment, control whether the segment appears in the
output image, and facilitate copying of data from one segment to another.
The "ORDER" Directive is primarily intended for embedded (ROMable) targets
that do not run under an operating system, or for other special purpose
applications. The format of the "ORDER" directive (short form "ORD")
is as follows.
.mbigbox
    ORDER {CLNAME class_name [class_options]}+
~b
    class_options ::= [SEGADDR=n][OFFSET=n][copy_option][NOEMIT]{seglist}
    copy_option ::= [COPY source_class_name]
    seglist := {SEGMENT seg_name [SEGADDR=n][OFFSET=n][NOEMIT]}+
.embigbox
.synote
.*
.im lnkvalue
.*
.mnote class_name
is the name of a class defined in one or more object files. If the class
is not defined in an object file, the
.sy class_name
and all associated options are ignored. Note that the "ORDER" directive does
.us not
create classes or segments. Classes specified with "CLNAME" keywords will be
placed in the output image in the order listed. Any classes that are not
listed will be placed after the listed ones.
.*
.mnote SEGADDR=n
(short form "SEGA")
specifies the segment portion of the starting address of the class or segment
in the output image. It is combined with "OFFSET" to represent a unique
linear address. "SEGADDR" is only valid for segmented formats. Its use in
other contexts is undefined. The "HSHIFT" value affects how the segment
value is converted to a linear address.
.*
.mnote OFFSET=n
(short form "OFF")
specifies the offset portion of the starting address of the class or segment
in the output image. It is combined with "SEGADDR" to represent a unique
linear address. Offset is limited to a range of 0 to 65535 in segmented
architectures, but can be a larger value for non-segmented architectures,
up to the limits of the architecture.
.np
When "SEGADDR" and/or "OFFSET" are specified, the location counter used to
generate the executable is advanced to that address. Any gaps are filled with
the "FILLCHAR" value, except for HEX output format, in which case they are
simply skipped. If the location counter is already beyond the specified
location, an error message is generated. This would likely be the result of
having specified classes or segments in incorrect order, or not providing
enough room for preceding ones. Without the "SEGADDR" and "OFFSET" options,
classes and segments are placed in the executable consecutively, possibly
with a small gap in between if required by the alignment specified for the
class. If "SEGADDR" is specified without corresponding "OFFSET", the offset
portion of the address defaults to 0.
.*
.mnote COPY
(short form "CO")
indicates that the data from the segment named
.sy source_class_name
is to be used in this segment.
.*
.mnote NOEMIT
(short form "NOE")
indicates that the data in this segment should not be placed in the executable.
.*
.mnote SEGMENT
indicates the order of segments within a class, and possibly other options
associated with that segment. Segments listed are placed in the executable
in the order listed. They must be part of the class just named. Any segments
in that class not listed will follow the last listed segment. The segment
options are a subset of the class options and conform to the same
specifications.
.esynote
.*
.*
In ROM-based applications it is often necessary to:
.begbull
.*
.bull
Fix the program location
.bull
Separate code and data to different fixed parts of memory
.bull
Place a copy of initialized data in ROM (usually right after the code)
.bull
Prevent the original of the initialized data from being written to the loadfile, since it resides in RAM and cannot be saved there.
.endbull
.*
The "ORDER" directive caters for these requirements. Classes can be placed
in the executable in a specific order, with absolute addresses specified
for one or more classes, and segments within a class can be forced into a
specified order with absolute addresses specified for one or more of them.
Initialized data can be omitted at its target address, and a copy included
at a different address.
.np
Following is a sample "ORDER" directive for an embedded target (AM186ER). The
bottom 32K of memory is RAM for data. A DGROUP starting address of 0x80:0 is
required. The upper portion of memory is FLASH ROM. Code starts at address
0xD000:0. The initialized data from DGROUP is placed immediately after the
code.
.*
.millust begin
order clname BEGDATA NOEMIT segaddr=0x80 segment _NULL segment _AFTERNULL
      clname DATA NOEMIT segment _DATA
      clname BSS
      clname STACK
      clname START segaddr=0xD000
      clname CODE segment BEGTEXT segment _TEXT
      clname ROMDATA COPY BEGDATA
      clname ROMDATAE
.millust end
.*
DGROUP consists of classes "BEGDATA", "DATA", "BSS", "BSS2" and "STACK".
Note that these are marked "NOEMIT" (except for the BSS classes and STACK
which are not initialized,
and therefore have no data in them anyway) to prevent data from being
placed in the loadfile at 0x80:0. The first class of DGROUP is given the
fixed starting segment address of 0x80 (offset is assumed to be 0).
The segments "_NULL", "_AFTERNULL" and "_DATA" will be allocated consecutively
in that order, and because they are part of DGROUP, will all share the same
segment portio of the address, with offsets adjusted accordingly.
.np
The code section consists of classes "START" and "CODE". These are placed
beginning at 0xD000:0. "START" contains only one segment, which will be
first. It will have a CS value of 0xD000. Code has two segments, "BEGTEXT"
and "_TEXT" which will be placed after "START", in that order, and packed into
a single CS value of their own (perhaps 0xD001 in this example), unless they
exceed 64K in size, which should not be the case if the program was compiled
using the small memory model.
.np
The classes "ROMDATA" and "ROMDATAE" were created in assembly with one segment
each and no symbols or data in them. The class names can be used to identify
the beginning and end of initialized data so it can be copied to RAM by the
startup code.
.np
The "COPY" option actually works at the group level, because that is the way
it is generally needed. The entire data is in DGROUP. "ROMDATA" will be placed
in a group of its own called "AUTO". (Note: each group mentioned in the map
file under the name "AUTO" is a separate group. They are not combined or
otherwise related in any way, other than they weren't explicitly created by
the programmer, compiler or assembler, but rather automatically created by
the linker in the course of its work.) Therefore there is a unique group
associated with this class. The "COPY" option finds the group associated with
"BEGDATA" and copies all the object data from there to "ROMDATA". Specifically,
it places a copy of this data in the executable at the location assigned to
"ROMDATA", and adjusts the length of "ROMDATA" to account for this. All symbol
references to this data are to its execution address (0x80:0), not where it
ended up in the executable (for instance 0xD597:0). The starting address of
"ROMDATAE" is also adjusted to account for the data assigned to "ROMDATA".
That way, the program can use the symbol "ROMDATAE" to identify the end of the
copy of DGROUP. It is also necessary in case more than one "COPY" class
exists consecutively, or additional code or data need to follow it.
.np
It should also be noted that the "DOSSEG" option (whether explicitly given
to the linker, or passed in an object file) performs different class and
segment ordering. If the "ORDER" directive is used, it overrides the "DOSSEG"
option, causing it to be ignored.
