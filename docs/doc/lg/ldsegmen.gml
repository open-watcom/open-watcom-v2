.*
.*
.dirctv SEGMENT
.*
.np
The "SEGMENT" directive is used to describe the attributes of code and
data segments.
The format of the "SEGMENT" directive (short form "SEG") is as
follows.
.np
:CMT. Force a page break; the table is too big and won't get properly
:CMT. moved to the next page automatically.
.pa
.mbigbox
    SEGMENT seg_desc{,seg_desc}

    seg_desc ::= seg_id {seg_attrs}+

    seg_id ::= 'seg_name' | CLASS 'class_name' | TYPE [CODE | DATA]
OS/2:
    seg_attrs ::= PRELOAD | LOADONCALL
                        | IOPL | NOIOPL
                        | EXECUTEONLY | EXECUTEREAD
                        | READONLY | READWRITE
                        | SHARED | NONSHARED
                        | CONFORMING | NONCONFORMING
                        | PERMANENT | NONPERMANENT
                        | INVALID | RESIDENT
                        | CONTIGUOUS | DYNAMIC
Win32:
    seg_attrs ::= PAGEABLE | NONPAGEABLE
                        | SHARED | NONSHARED

Win16:
    seg_attrs ::= PRELOAD | LOADONCALL
                        | EXECUTEONLY | EXECUTEREAD
                        | READONLY | READWRITE
                        | SHARED | NONSHARED
                        | MOVEABLE | FIXED
                        | DISCARDABLE

VxD:
    seg_attrs ::= PRELOAD | LOADONCALL
                        | IOPL | NOIOPL
                        | SHARED | NONSHARED
                        | DISCARDABLE | NONDISCARDABLE
                        | CONFORMING | NONCONFORMING
                        | RESIDENT

QNX:
    seg_attrs ::= EXECUTEONLY | EXECUTEREAD
                        | READONLY | READWRITE
.embigbox
.synote
.*
.mnote seg_name
is the name of the code or data segment whose attributes are being
specified.
.*
.mnote class_name
is a class name.
The attributes will be assigned to all segments belonging to the
specified class.
.*
.mnote PRELOAD
(short form "PR", OS/2, VxD and Win16 only) specifies that the segment is
loaded as soon as the executable file is loaded.
This is the default.
.*
.mnote LOADONCALL
(short form "LO", OS/2, VxD and Win16 only) specifies that the segment is
loaded only when accessed.
.*
.mnote PAGEABLE
(short form "PAGE", Win32 only) specifies that the segment can be
paged from memory. This is the default.
.*
.mnote NONPAGEABLE
(short form "NONP", Win32 only) specifies that the segment, once
loaded into memory, must remain in memory.
.*
.mnote CONFORMING
(short form "CON", OS/2 and VxD only) specifies that the segment will assume the I/O
privilege of the segment that referenced it.
By default, the segment is "NONCONFORMING".
.*
.mnote NONCONFORMING
(short form "NONC", OS/2 and VxD only) specifies that the segment will not
assume the I/O privilege of the segment that referenced it.
This is the default.
.*
.mnote IOPL
(short form "I", OS/2 and VxD only) specifies that the segment requires I/O
privilege. That is, they can access the hardware directly.
.*
.mnote NOIOPL
(short form "NOI", OS/2 and VxD only) specifies that the segment does not
require I/O privilege.
This is the default.
.*
.mnote PERMANENT
(short form "PERM", OS/2 32-bit only) specifies that the segment is
permanent.
.*
.mnote NONPERMANENT
(short form "NONPERM", OS/2 32-bit only) specifies that the segment is not
permanent.
.*
.mnote INVALID
(short form "INV", OS/2 32-bit only) specifies that the segment is invalid.
.*
.mnote RESIDENT
(short form "RES", OS/2 32-bit and VxD only) specifies that the segment is
resident.
.*
.mnote CONTIGUOUS
(short form "CONT", OS/2 32-bit only) specifies that the segment is
contiguous.
.*
.mnote DYNAMIC
(short form "DYN", OS/2 32-bit only) specifies that the segment is dynamic.
.*
.mnote EXECUTEONLY
(short form "EXECUTEO", OS/2, QNX and Win16 only) specifies that the
segment can only be executed.
This attribute should only be specified for code segments.
This attribute should not be specified if it is possible for the code
segment to contain jump tables which is the case with the &company C,
C++ and FORTRAN 77 optimizing compilers.
.*
.mnote EXECUTEREAD
(short form "EXECUTER", OS/2, QNX and Win16 only) specifies that the
segment can only be executed and read.
This attribute, the default for code segments, should only be
specified for code segments.
This attribute is appropriate for code segments that contain jump
tables as is possible with the &company C, C++ and FORTRAN 77
optimizing compilers.
.*
.mnote READONLY
(short form "READO", OS/2, QNX and Win16 only) specifies that the
segment can only be read.
This attribute should only be specified for data segments.
.*
.mnote READWRITE
(short form "READW", OS/2, QNX and Win16 only) specifies that the
segment can be read and written.
This is the default for data segments.
This attribute should only be specified for data segments.
.*
.mnote SHARED
(short form "SH" ) specifies that a single copy of the segment will be
loaded and will be shared by all processes.
.mnote NONSHARED
(short form "NONS") specifies that a unique copy of the segment will
be loaded for each process.
This is the default.
.*
.mnote MOVEABLE
(short form "MOV", Win16 only) specifies that the segment is moveable.
By default, segments are moveable.
.*
.mnote FIXED
(short form "FIX", Win16 only) specifies that the segment is fixed.
.*
.mnote DISCARDABLE
(short form "DIS", Win16 and VxD only) specifies that the segment is discardable.
By default, segments are not discardable.
.*
.mnote NONDISCARDABLE
(short form "NOND", VxD only) specifies that the segment is not discardable.
By default, segments are not discardable.
.*
.esynote
.remark
Attributes specified for segments identified by a segment name
override attributes specified for segments identified by a class name.
.eremark
