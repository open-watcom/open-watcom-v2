.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="function"  value="function".
:set symbol="functions" value="functions".
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="function"  value="subprogram".
:set symbol="functions" value="subprograms".
.do end
.*
.chap *refid=mdl386 32-bit Memory Models
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'memory models' '32-bit'
This chapter describes the various 32-bit memory models supported by
&cmpname..
Each memory model is distinguished by two properties; the code model
used to implement &function calls and the data model used to reference
data.
.*
.section 32-bit Code Models
.*
.np
There are two code models;
.autopoint
.point
the small code model and
.point
the big code model.
.endpoint
.pc
.ix 'small code model'
.ix 'code models' 'small'
.ix 'near call'
A small code model is one in which all calls to &functions are made
with
.us near calls.
In a near call, the destination address is 32 bits and is relative to
the segment value in segment register CS.
Hence, in a small code model, all code comprising your program,
including library &functions, must be less than 4GB.
.np
.ix 'big code model'
.ix 'code models' 'big'
.ix 'far call'
A big code model is one in which all calls to &functions are made with
.us far calls.
In a far call, the destination address is 48 bits (a 16-bit segment
value and a 32-bit offset relative to the segment value).
This model allows the size of the code comprising your program to
exceed 4GB.
.remark
If your program contains less than 4GB of code, you should use a
memory model that employs the small code model.
This will result in smaller and faster code since near calls are
smaller instructions and are processed faster by the CPU.
.eremark
.*
.section 32-bit Data Models
.*
.np
There are two data models;
.autopoint
.point
the small data model and
.point
the big data model.
.endpoint
.pc
.ix 'small data model'
.ix 'data models' 'small'
A small data model is one in which all references to data are made
with
.us near pointers.
Near pointers are 32 bits; all data references are made relative to
the segment value in segment register DS.
Hence, in a small data model, all data comprising your program must be
less than 4GB.
.np
.ix 'big data model'
.ix 'data models' 'big'
A big data model is one in which all references to data are made with
.us far pointers.
Far pointers are 48 bits (a 16-bit segment value and a 32-bit offset
relative to the segment value).
This removes the 4GB limitation on data size imposed by the small data
model.
However, when a far pointer is incremented, only the offset is
adjusted.
&cmpname assumes that the offset portion of a far pointer will not be
incremented beyond 4GB.
The compiler will assign an object to a new segment if the grouping of
data in a segment will cause the object to cross a segment boundary.
Implicit in this is the requirement that no individual object exceed
4GB.
.remark
If your program contains less than 4GB of data, you should use the
small data model.
This will result in smaller and faster code since references using
near pointers produce fewer instructions.
.eremark
.*
.section Summary of 32-bit Memory Models
.*
.np
As previously mentioned, a memory model is a combination of a code
model and a data model.
The following table describes the memory models supported by
&cmpname..
.ix 'flat memory model'
.ix 'memory models' 'flat'
.ix 'small memory model'
.ix 'memory models' 'small'
.ix 'medium memory model'
.ix 'memory models' 'medium'
.ix 'compact memory model'
.ix 'memory models' 'compact'
.ix 'large memory model'
.ix 'memory models' 'large'
.millust begin
Memory      Code        Data        Default     Default
Model       Model       Model       Code        Data
                                    Pointer     Pointer
--------    --------    --------    --------    --------
.sk 1
flat        small       small       near        near
.sk 1
small       small       small       near        near
.sk 1
medium      big         small       far         near
.sk 1
compact     small       big         near        far
.sk 1
large       big         big         far         far
.millust end
.*
.section Flat Memory Model
.*
.np
.ix 'memory models' 'flat'
In the flat memory model, the application's code and data must total
less than 4GB in size.
Segment registers CS, DS, SS and ES point to the same linear address
space (this does not imply that the segment registers contain the same
value).
That is, a given offset in one segment refers to the same memory
location as that offset in another segment.
Essentially, a flat model operates as if there were no segments.
.*
.section Mixed 32-bit Memory Model
.*
.np
.ix 'mixed memory model'
.ix 'memory models' 'mixed'
A mixed memory model application combines elements from the various
code and data models.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
A mixed memory model application might be characterized as one that
uses the
.kw near
.ct ,
.kw far
.ct , or
.kw huge
keywords when describing some of its &functions or data objects.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
A mixed memory model application might be characterized as one that
includes arrays which are larger than 4GB.
.do end
.np
For example, a medium memory model application that uses some
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
far pointers to data
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
arrays which exceed 4GB in total size
.do end
can be described as a mixed memory model.
In an application such as this, most of the data is in a 4GB segment
(DGROUP) and hence can be referenced with near pointers relative to
the segment value in segment register DS.
This results in more efficient code being generated and better
execution times than one can expect from a big data model.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Data objects outside of the DGROUP segment are described with the
.kw far
keyword.
.do end
.*
.section Linking Applications for the Various 32-bit Memory Models
.*
.np
.ix 'memory models' 'libraries'
Each memory model requires different run-time and floating-point
libraries.
Each library assumes a particular memory model and should be linked
only with modules that have been compiled with the same memory model.
The following table lists the libraries that are to be used to link an
application that has been compiled for a particular memory model.
Currently, only libraries for the flat/small memory model are provided.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
Memory      Run-time    Floating-Point   Floating-Point
Model       Library     Library (80x87)  Library (f-p calls)
----------  ----------  ---------------  -------------------
.if '&target' eq 'QNX' .do begin
.sr letr='r'
.sr lets='s'
flat/small  clib3r.lib    math387r.lib     math3r.lib
            clib3s.lib    math387s.lib     math3s.lib
.if '&lang' eq 'C/C++' .do begin
            plib3r.lib    cplx73r.lib      cplx3r.lib
            plib3s.lib    cplx73s.lib      cplx3s.lib
.do end
.do end
.el .do begin
.sr letr='R'
.sr lets='S'
flat/small  CLIB3R.LIB    MATH387R.LIB     MATH3R.LIB
            CLIB3S.LIB    MATH387S.LIB     MATH3S.LIB
.if '&lang' eq 'C/C++' .do begin
            PLIB3R.LIB    CPLX73R.LIB      CPLX3R.LIB
            PLIB3S.LIB    CPLX73S.LIB      CPLX3S.LIB
.do end
.do end
.code end
.pc
The letter "&letr." or "&lets." which is affixed to the file name
indicates the particular strategy with which the modules in the
library have been compiled.
.begnote
.ix 'C libraries' 'flat'
.ix 'flat model' 'libraries'
.ix 'C libraries' 'small'
.ix 'small model' 'libraries'
.note &letr.
denotes a version of the &product 32-bit libraries which have been
compiled for the "flat/small" memory models using the "3r", "4r" or
"5r" option.
.note &lets.
denotes a version of the &product 32-bit libraries which have been
compiled for the "flat/small" memory models using the "3s", "4s" or
"5s" option.
.endnote
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.ix 'FORTRAN libraries' 'flat'
.ix 'flat model' 'libraries'
.ix 'FORTRAN libraries' 'small'
.ix 'small model' 'libraries'
The following table lists the run-time libraries used by FORTRAN 77 and
the compiler options that cause their use.
.autopoint
.point
The "Library" column specified the library name.
.point
The "Memory model" column indicates the compiler options that specify the
memory model of the library.
.point
The "Floating-point column" indicates the compiler options that specify the
floating-point model of the library.
.point
The "Calling convention" column indicates the compiler option that specifies
the calling convention of the library (register-based or stack-based).
.endpoint
.code begin
Library         Memory      Floating-point        Calling
                model       model                 convention
-------         ------      --------------        ----------
flib.lib        /mf, /ms    /fpc
flibs.lib       /mf, /ms    /fpc                  /sc
flib7.lib       /mf, /ms    /fpi, /fpi87
flib7s.lib      /mf, /ms    /fpi, /fpi87          /sc
clib3r.lib      /mf, /ms    /fpc, /fpi, /fpi87
clib3r.lib      /mf, /ms    /fpc, /fpi, /fpi87    /sc
math387r.lib    /mf, /ms    /fpi, /fpi87
math387s.lib    /mf, /ms    /fpi, /fpi87          /sc
math3r.lib      /mf, /ms    /fpc
math3s.lib      /mf, /ms    /fpc                  /sc
emu387.lib      /mf, /ms    /fpi
noemu387.lib    /mf, /ms    /fpi87
.code end
.do end
.if &e'&dohelp eq 1 .do begin
.   .helppref 32-bit
.do end
.im wmemlay
.if &e'&dohelp eq 1 .do begin
.   .helppref
.do end
