.chap The &company 32-bit Windows 3.x Extender
.*
.np
.ix 'Windows 3.x extender'
&product contains the necessary tools and libraries to create 32-bit
applications for Windows 3.x.
Using &product gives the programmer the benefits of a 32-bit flat
memory model and access to the full Windows API (along with the usual
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&lang
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
FORTRAN 77 and C
.do end
library functions).
.np
The general model of the environment is as follows:
The 32-bit flat memory model program is linked against a special 32-bit
Windows library.
This library contains the necessary information to invoke special
16-bit functions, which lie in the supervisor
.fi (WIN386.EXT).
The 32-bit program is then bound (using
.fi WBIND.EXE)
with the supervisor to create a Windows executable.
At the same time as the 32-bit program is being bound, the resource
compiler is run on the supervisor, and all the resources for the
application are placed there.
When the application is started, the supervisor obtains the 32-bit
memory, relocates the 32-bit application into the memory, and invokes
the 32-bit application.
.np
All Windows functions are invoked from the supervisor, and all
callback routines lie within the supervisor.
The local heap resides within the supervisor as well.
.np
If you are starting from a 16-bit Windows application,
most of the code will not change when you port it to the 32-bit Windows
environment.
However, because of the nature of the Windows API and its implicit
dependencies on a 16-bit environment, some source changes are
necessary.
These source changes are minimal, and are backwards compatible with
the 16-bit environment.
.*
.section Pointers
.*
.np
.ix 'Windows 3.x extender' 'pointers'
Throughout this document, there will be references to both
.kw near
and
.kw far
.ct , and
.kw 16-bit
and
.kw 32-bit
pointers.
Since this can rapidly become confusing, some initial explanations
will be given here.
.np
.ix 'pointers' 'far'
A
.kw far pointer
is a pointer that is composed of both a selector and an offset.
A selector determines a specific region of memory, and the offset is
relative to the start of this region.
.ix 'pointers' 'near'
A
.kw near pointer
is a pointer that has an offset only, the selector is automatically
assumed by the CPU.
.np
The problem with far pointers is the selector overhead.
Using a far pointer is much more expensive than using a near pointer.
This is the advantage of the 32-bit flat memory model - all pointers
within the program are near, and yet you can address up to 4 gigabytes
of memory.
.np
.ix 'pointers' '16-bit'
A
.kw 16-bit near pointer
occupies 2 bytes of memory (i.e., the offset is 16 bits long).
This pointer can reference up to 64K of data.
.np
A
.kw 16-bit far pointer
occupies 4 bytes of memory.
There is a 16-bit selector and a 16-bit offset.
This pointer can reference up to 64K of data.
.np
.ix 'pointers' '32-bit'
A
.kw 32-bit near pointer
occupies 4 bytes of memory (i.e., the offset is 32 bits long).
This pointer can reference up to 4 gigabytes of data.
.np
A
.kw 32-bit far pointer
occupies 6 bytes of memory.
There is a 16-bit selector and a 32-bit offset.
This pointer can reference up to 4 gigabytes of data.
.np
Windows, in general, uses 16-bit far pointers to pass information around.
These 16-bit far pointers can also be used by a 32-bit Windows application.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Using a special macro,
.kw MK_FP32
.ct , the offset of the 16-bit far pointer is extended from 16 bits to 32 bits,
and the pointer becomes a 32-bit far pointer.
The 32-bit far pointer is then used by the application to access the
data (note that offsets still must be less than 64K, since the
selector is still for a 64K data area).
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
The conversion from a 16-bit pointer to a 32-bit pointer will occur
automatically when you map a dynamically allocatable array to the
memory pointed to by the 16-bit pointer using the
.kw LOCATION=
specifier of the
.kw ALLOCATE
statement.
You must also declare the allocatable array as
.kw far
using the
.kw array
pragma.
The syntax for the
.kw array
pragma is:
.millust begin
$*pragma array ARRAY_NAME far
.millust end
.pc
where
.id ARRAY_NAME
is the array name.
.do end
.*
.section Implementation Overview
.*
.np
.ix 'Windows 3.x extender' 'overview'
This section provides an overview of the issues that require
consideration when creating a 32-bit Windows application for a 16-bit
Windows environment.
.np
First, all modules have to be recompiled for the 32-bit flat memory
model with a compiler capable of generating 32-bit instructions.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Many Windows API functions take
.kw int
as a parameter.
This
.kw int
is from the 16-bit world, and is 2 bytes long.
In the 32-bit world, this
.kw int
becomes 4 bytes long.
Since Windows is only expecting two bytes of data, all occurrences of
.kw int
have to be changed to
.kw short
in
.fi WINDOWS.H.
.do end
.np
Pointers to data passed to Windows are all far pointers.
We will be passing pointers to data in our 32-bit flat address space,
and these pointers are near pointers.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
By simply getting rid of all
.kw far
keywords in
.fi WINDOWS.H,
all pointers will now be passed as 32-bit near
pointers.
.do end
As well, notice that these 32-bit near pointers are the same size as
as their 16-bit far pointer counterparts (4 bytes).
This is good, since all data structures containing pointers will
remain the same size.
.np
Windows cannot be called from 32-bit code on a 32-bit stack.
This means that in order to call the API functions, it is necessary to
write a set of cover functions that will accept the parameters, switch
into a 16-bit environment, and then call Windows.
There is another issue, though.
Windows only understands 16-bit pointers, so before calling Windows,
all pointers being passed to Windows must be converted to 16-bit far pointers.
.np
It turns out that Windows can also call back to your application.
Windows can only call 16-bit code, though, so there is a need for
a bridge from the 16-bit side to the 32-bit side.
It is necessary to allocate 16-bit call back routines that can be
passed to Windows.
These call back routines will then switch into the 32-bit environment
and call whatever 32-bit function is required.
The 32-bit call back has to be declared as a far function, since it
is necessary to issue a far call to enter it from the 16-bit side.
If it is a far function, then the compiler will generate the appropriate
code for it to return from the far call.
.np
Once Windows calls you back, it can hand you 16-bit far pointers in
a long (4 byte) parameter.
This pointer can only be used in the 32-bit environment if it is a
32-bit far pointer, not a 16-bit far pointer.
The conversion is simple: the 16-bit offset is extended to a 32-bit offset
(the high word is zeroed out).
Any far pointer that Windows hands to you must be converted in this
way.
.if '&lang' eq 'FORTRAN 77' .do begin
This conversion is performed automatically when a dynamically
allocatable array is mapped to a 16-bit far pointer using the
.kw LOCATION
specifier of the &product
.kw ALLOCATE
statement and the
.kw array
pragma.
The syntax for the
.kw array
pragma is:
.millust begin
$*pragma array ARRAY_NAME far
.millust end
.pc
where
.id ARRAY_NAME
is the array name.
.exam begin
        subroutine DLLSUB( arg_list )

        structure /argtypes/
            integer     w1
            integer     w2
            integer     w3
            integer     sum
        end structure
        record /argtypes/ args(:)
*$pragma array args far
        integer*4 arg_list

        allocate( args(1), location=arg_list )
.exam end
.np
In the preceding example,
.id arg_list
is a 16-bit far pointer to a structure with the elements described by
the
.id argtypes
structure.
The allocatable array
.id args
is described as
.kw far
using the
.kw array
pragma.
.do end
.np
Sometimes, a Windows application wants to call a procedure in a DLL.
The procedure address is a 16-bit far pointer.
It is not possible to issue an indirect call to this address from the
32-bit environment, so some sort of interface is needed.
This interface would switch into the 16-bit environment, and then call
the 16-bit function.
.np
These issues, along with other minor items, are handled by &product,
and are discussed in more technical detail in later sections.
.*
.section System Structure
.*
.ix 'Windows 3.x extender' 'structure'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:cmt.    :fig place=inline frame=box.
:cmt.    :graphic file='drawing.eps' scale=84 depth='5.00i' xoff='-0.1i' yoff='-0.6i'
:cmt.    :figcap.&product Structure
:cmt.    :efig.
.figure *depth='4.00' *scale=100 *file='struct' WIN386 Structure
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:cmt.    :fig place=inline frame=box.
:cmt.    :graphic file='fdraw.eps' scale=84 depth='5.00i' xoff='-0.1i' yoff='-0.6i'
:cmt.    :figcap.&product Structure
:cmt.    :efig.
.figure *depth='4.00' *scale=100 *file='fstruct' WIN386 Structure
.do end
:cmt.    :fig place=inline frame=box.
:cmt.    :graphic file='draw2.eps' scale=67 depth='2.20i' xoff='-0.05i' yoff='-0.5i'
:cmt.    :figcap.32-bit Application Structure
:cmt.    :efig.
.figure *depth='1.45' *scale=100 *file='struct2' 32-bit Application Structure
.*
.section System Overview
.*
.ix 'Windows 3.x extender' 'components'
.begbull
.bull
.fi WIN386.EXT
is the key component of a 32-bit Windows application.
It is a 16-bit Windows application which contains:
.begbull $compact
.bull
All application resources.
.bull
A 16-bit local heap.
.bull
A 16-bit stack.
.endbull
.np
.bull
.fi W386DLL.EXT
is similar to
.fi WIN386.EXT,
only it provides a DLL interface.
.np
.fi WIN386.EXT
is bound to your 32-bit application to create a 32-bit application
that will run under Windows 3.x.
.fi WIN386.EXT
provides the following functionality:
.begbull
.bull
supervisor to bring the 32-bit application into memory and start it
running.
.bull
"glue" functions to connect to Windows for both API and DOS
functionality.
This interface is designed to transparently set up the calling
functions' pointers and parameters to their 16-bit counterparts.
.bull
"glue-back" functions to allow Windows to call back 32-bit routines.
.bull
special code to allow debugging of 32-bit applications.
.endbull
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
.fi WINDOWS.H
has been specially modified for use in the 32-bit Windows
environment.  As well, it contains all special definitions for
32-bit applications.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
A number of files with file extension
.fi &hxt
are located in the
.fi &pathnamup.&pc.SRC&pc.FORTRAN&pc.WIN
directory.
The file
.fi WINAPI.FI
describes the calling convention of each Windows API function.
Other files define Windows constants and data structures.
.do end
.bull
.fi WIN386.LIB
contains all the necessary library functions to connect to the 32-bit
supervisor
.fi WIN386.EXT.
All Windows API calls and &product library DOS calls are found here.
.bull
The standard
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&lang
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
FORTRAN 77 and C
.do end
library functions, specially modified to run in the 32-bit
environment, are located in the
.fi &pathnamup.&libdirup32.&pc.WIN
directory.
.bull
.fi WBIND.EXE
merges your 32-bit executable and the appropriate Supervisor
into a single executable.
.endbull
.*
.section Steps to Obtaining a 32-bit Application
.*
.np
.ix 'Windows 3.x extender' 'creating applications'
The following is an overview of the procedure for creating
a 32-bit Windows Application:
.autopoint
.point
If you are starting with a 16-bit Windows application, you must adapt
your source code to the 32-bit environment.
.point
You must compile the application using a 32-bit compiler.
.point
You must link the application with the 32-bit libraries.
.point
You must bind the 32-bit application with the 32-bit supervisor.
.point
You can then run and/or debug the application.
.endpoint
