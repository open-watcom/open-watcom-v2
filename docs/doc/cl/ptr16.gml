.section The &kwfar16_sp. and &kwisg16_sp. Keywords
.*
.pp
With the 80386 processor, a far pointer consists of a 16-bit
.ix 'pointer' 'selector'
selector
and a 32-bit
.ix 'pointer' 'offset'
offset.
&wc386. also supports a special kind of far pointer which consists
of a 16-bit selector and a 16-bit offset.
These pointers, referred to as
.ix 'pointer' 'far16'
.ul far16
pointers, allow 32-bit code to access code and data running in 16-bit
mode.
.pp
In the OS/2 operating system (version 2.0 or higher), the first 512
megabytes of the 4 gigabyte segment referenced by the DS register is
divided into 8192 areas of 64K bytes each.
A far16 pointer consists of a 16-bit selector referring to one of the
64K byte areas, and a 16-bit offset into that area.
.pp
For compatibility with Microsoft C, &wc386. provides the
.kwix &kwfar16_sp.
.kwfont &kwfar16.
keyword.
A pointer declared as,
.cillust begin
type :MONO. &kwfar16. *:eMONO. name:MSEMI.
.cillust end
.pc
defines an object that is a far16 pointer.
If such a pointer is accessed in the 32-bit environment, the compiler
will generate the necessary code to convert between the far16 pointer
and a "flat" 32-bit pointer.
.pp
For example, the declaration,
.millust begin
char &kwfar16. * bufptr;
.millust end
.pc
declares the object
.mono bufptr
to be a far16 pointer to
.kw char
..ct ..li .
.pp
A function declared as,
.cillust begin
type :MONO. &kwfar16. :eMONO. func:MONO.(:eMONO. parm-list :MONO.):eMONO.:MSEMI.
.cillust end
.pc
declares a 16-bit function.
Any calls to such a function from the 32-bit environment will cause
the compiler to convert any 32-bit pointer parameters
to far16 pointers, and any
.kw int
parameters
from 32 bits to 16 bits.
(In the
16-bit environment, an object of type
.kw int
is only 16 bits.)
Any return value from the function will have its return value converted
in an appropriate manner.
.pp
For example, the declaration,
.code begin
char * &kwfar16. Scan( char * buffer, int buflen, short err );
.code end
.pc
declares the 16-bit function
.mono Scan
..ct ..li .
When this function is called from the 32-bit environment,
the
.mono buffer
parameter will be converted from a flat 32-bit pointer to a far16
pointer
(which, in the 16-bit environment, would be declared as
.mono char&SYSRB.&kwfar.&SYSRB.*
..ct ).
The
.mono buflen
parameter will be converted from a 32-bit integer to a 16-bit integer.
The
.mono err
parameter will be passed unchanged.
Upon returning, the far16 pointer (far pointer in the 16-bit environment)
will be converted to a 32-bit pointer which describes the
equivalent location in the 32-bit address space.
.pp
For compatibility with IBM C Set/2,
&wc386. provides the
.kwix &kwisg16_sp.
.kwfont &kwiseg16.
keyword.
Note that
.kwix &kwisg16_sp.
.kwfont &kwiseg16.
is
.bd not
interchangeable with
.kwix &kwfar16_sp.
.kwfont &kwfar16.
..ct ..li .
.pp
A pointer declared as,
.cillust begin
type :MONO. * &kwiseg16. :eMONO. name:MSEMI.
.cillust end
.pc
defines an object that is a far16 pointer.
Note that the
.kwix &kwisg16_sp.
.kwfont &kwiseg16.
appears on the opposite side of the
.mono *
than the
.kwix &kwfar16_sp.
.kwfont &kwfar16.
keyword described above.
.pp
For example,
.millust begin
char * &kwiseg16. bufptr;
.millust end
.pc
declares the object
.mono bufptr
to be a far16 pointer to
.kw char
(the same as above).
.pp
The
.kwix &kwisg16_sp.
.kwfont &kwiseg16.
keyword may not be used to describe a 16-bit function.
A
.kwpp #pragma
directive must be used.
See the &userguide. for details.
A function declared as,
.cillust begin
type :MONO.* &kwiseg16.:eMONO. func:MONO.(:eMONO. parm-list :MONO.):eMONO.:MSEMI.
.cillust end
.pc
declares a 32-bit function that returns a far16 pointer.
.pp
For example, the declaration,
.code begin
char * &kwiseg16. Scan( char * buffer, int buflen, short err );
.code end
.pc
declares the 32-bit function
.mono Scan
..ct ..li .
No conversion of the parameter list will take place.
The return value is a far16 pointer.
