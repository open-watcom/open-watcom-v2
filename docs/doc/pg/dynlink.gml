:set symbol="oprompt"   value=&prompt.
.if '&targetos' eq 'OS/2 2.x' .do begin
:set symbol="prompt"    value="[C:\]".
:set symbol="tgtosname" value="os2v2".
.do end
.el .do begin
:set symbol="prompt"    value="C:\>".
:set symbol="tgtosname" value="nt".
.do end
:set symbol="tgtopts"   value="".
.*
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="dll_init"  value="__fdll_initialize_".
:set symbol="dll_term"  value="__fdll_terminate_".
:set symbol="thrd_init"  value="__fthrd_initialize_".
:set symbol="thrd_term"  value="__fthrd_terminate_".
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&targetos' eq 'Windows NT' .do begin
:set symbol="tgtopts"   value="&sw.bm ".
.do end
:set symbol="dll_init"  value="__dll_initialize".
:set symbol="dll_term"  value="__dll_terminate".
:set symbol="thrd_init"  value="__thrd_initialize".
:set symbol="thrd_term"  value="__thrd_terminate".
.do end
.*
.chap &targetos Dynamic Link Libraries
.*
.np
.ix 'dynamic link libraries' '&targetos.'
.ix 'DLL' '&targetos.'
A dynamic link library, like a standard library, is a library of
functions.
.ix 'static linking'
When an application uses functions from a standard library, the
library functions referenced by the application become part of the
executable module.
This form of linking is called static linking.
.ix 'dynamic linking'
When an application uses functions from a dynamic link library, the
library functions referenced by the application are not included in
the executable module.
Instead, the executable module contains references to these functions
which are resolved when the application is loaded.
This form of linking is called dynamic linking.
.np
Let us consider some of the advantages of using dynamic link libraries
over standard libraries.
.autonote
.note
Functions in dynamic link libraries are not linked into your program.
Only references to the functions in dynamic link libraries are placed
in the program module.
.ix 'import definitions'
These references are called import definitions.
As a result, the linking time is reduced and disk space is saved.
If many applications reference the same dynamic link library, the
saving in disk space can be significant.
.note
Since program modules only reference dynamic link libraries and do not
contain the actual executable code, a dynamic link library can be
updated without re-linking your application.
When your application is executed, it will use the updated version of
the dynamic link library.
.note
Dynamic link libraries also allow sharing of code and data between the
applications that use them.
If many applications that use the same dynamic link library are
executing concurrently, the sharing of code and data segments improves
memory utilization.
.endnote
.*
.section Creating Dynamic Link Libraries
.*
.np
.ix 'dynamic link library creation' '&targetos.'
.ix 'DLL creation' '&targetos.'
Once you have developed the source for a library of functions, a
number of steps are required to create a dynamic link library
containing those functions.
.np
First, you must compile your source using the "bd" compiler option.
This option tells the compiler that the module you are compiling is
part of a dynamic link library.
Once you have successfully compiled your source, you must create a
linker directive file that describes the attributes of your dynamic
link library.
The following lists the most common linker directives required to
create a dynamic link library.
.autopoint
.point
The "SYSTEM" directive is used to specify that a dynamic link library
is to be created.
.point
The "EXPORT" directive is used to to specify which functions in the
dynamic link library are to be exported.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.begnote
.*
.note Specifying exports in the source code
.np
.if '&targetos' eq 'Windows NT' .do begin
The "EXPORT" directive need not be used when the symbols to be exported
are declared with the
.id __declspec( dllexport )
modifier in the source code. Such symbols are exported automatically,
through special records inserted into the object files by the compiler.
.if '&lang' eq 'C/C++' .do begin
.note Exporting C++ symbols and classes
.np
Symbols exported via the "EXPORT" directive have to be entered in their
mangled form. This makes it rather awkward to export C++ functions, classes
or global objects. These symbols also often reference other compiler-generated
symbols (invisible to the user) that need be exported together with the
class/object. Using the
.id __declspec( dllexport )
method of exporting symbols is the preferred solution.
.do end
.do end
.if '&targetos' eq 'OS/2 2.x' .do begin
The "EXPORT" directive need not be used when the symbols to be exported
are declared with the
.id __export
type qualifier in the source code. Such symbols are exported automatically,
through special records inserted into the object files by the compiler.
.do end
.np
.endnote
.do end
.point
The "OPTION" directive is used to specify attributes such as the name of
the dynamic link library and how to allocate the automatic data segment
when the dynamic link library is referenced.
.point
The "SEGMENT" directive is used to specify attributes of segments.
For example, a segment may be read-only or read-write.
.endpoint
.np
Once the dynamic link library is created, you must allow access to the
dynamic link library to client applications that wish to use it.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&targos' eq 'Windows NT' .do begin
The client application should to be compiled so that each symbol that will
be imported from a dynamic link library is declared with
.id __declspec( dllimport )
modifier in the source code. This is highly recommended for imported
functions and required for imported variables or classes.
The linker must also be told where the symbols should be imported from.
.do end
.do end
This can be done by creating an import library for the dynamic link
library or creating a linker directive file that contains "IMPORT"
directives for each of the entry points in the dynamic link library.
.*
.section Creating a Sample Dynamic Link Library
.*
.np
.ix 'dynamic link library sample' '&targetos.'
.ix 'DLL sample' '&targetos.'
Let us now create a dynamic link library using the following example.
.if '&lang' eq 'FORTRAN 77' .do begin
The source code for this example can be found in
.if '&targetos' eq 'OS/2 2.x' .do begin
.fi &pathnam.\samples\fortran\os2\dll.
.do end
.el .do begin
.fi &pathnam.\samples\fortran\win\dll.
.do end
Unlike applications developed in the C or C++ language, the &lang
developer must not provide a
.kw LibMain
entry point.
This entry point is already defined in the &product run-time
libraries.
The run-time system's
.kw LibMain
provides for the proper initialization of the &lang run-time system
and includes hooks to call developer-written process attach/detach and
thread attach/detach routines.
These routines are optional but we show skeleton versions in the
following example so that you can develop your own if required.
.code begin
* DLLSAMP.FOR

      integer function __fdll_initialize_()
*     Called from LibMain during "DLL PROCESS ATTACH"

*     do process initialization
      print *, 'Hi from process attach'

*     returning 0 indicates failure
      __fdll_initialize_ = 1
      return
      end

.if '&targetos' eq 'Windows NT' .do begin
.code break
      integer function __fthrd_initialize_()
*     Called from LibMain during "DLL THREAD ATTACH"

*     do thread initialization
      print *, 'Hi from thread attach'

*     returning 0 indicates failure
      __fthrd_initialize_ = 1
      return
      end

.code break
      integer function __fthrd_terminate_()
*     Called from LibMain during "DLL THREAD DETACH"

*     do thread cleanup
      print *, 'Hi from thread detach'

*     returning 0 indicates failure
      __fthrd_terminate_ = 1
      return
      end

.do end
.code break
      integer function __fdll_terminate_()
*     Called from LibMain during "DLL PROCESS DETACH"

*     do process cleanup
      print *, 'Hi from process detach'

*     returning 0 indicates failure
      __fdll_terminate_ = 1
      return
      end

.code break
      subroutine dll_entry_1()
      print *, 'Hi from dll entry #1'
      end

      subroutine dll_entry_2()
      print *, 'Hi from dll entry #2'
      end
.code end
.if '&targetos' eq 'Windows NT' .do begin
.np
Here are some explanatory notes on this example.
.begnote
.notehd1 Function
.notehd2 Description
.*
.note __FDLL_INITIALIZE_
.*
This function is called when the DLL is attaching to the address space
of the current process as a result of the process starting up or as a
result of a call to
.kw LoadLibrary
.ct .li .
A DLL can use this opportunity to initialize any instance data.
.np
During initial process startup or after a call to
.kw LoadLibrary
.ct , the operating system scans the list of loaded DLLs for the process.
For each DLL that has not already been called with the
.id DLL_PROCESS_ATTACH
value, the system calls the DLL's
.kw LibMain
entry-point (in the &product run-time system).
This call is made in the context of the thread that caused the process
address space to change, such as the primary thread of the process or
the thread that called
.kw LoadLibrary
.ct .li .
.*
.note __FTHRD_INITIALIZE_
.*
This function is called when the current process is creating a new
thread.
When this occurs, the system calls the
.kw LibMain
entry-point (in the &product run-time system) of all DLLs currently
attached to the process.
The call is made in the context of the new thread.
DLLs can use this opportunity to initialize thread specific data.
A thread calling the DLL's
.id LibMain
with the
.id DLL_PROCESS_ATTACH
value does not call
.kw LibMain
with the
.id DLL_THREAD_ATTACH
value.
Note that
.kw LibMain
is called with this value only by threads created after the DLL is
attached to the process.
When a DLL is attached by
.kw LoadLibrary
.ct , existing threads do not call the
.kw LibMain
entry-point of the newly loaded DLL.
.*
.note __FTHRD_TERMINATE_
.*
This function is called when a thread is exiting normally.
The DLL uses this opportunity to do thread specific termination such
as closing files that were opened by the thread.
The operating system calls the
.kw LibMain
entry-point (in the &product run-time system) of all currently loaded
DLLs with this value.
The call is made in the context of the exiting thread.
There are cases in which
.kw LibMain
is called for a terminating thread even if the DLL never attached to
the thread.
For example,
.kw LibMain
is never called with the
.id DLL_THREAD_ATTACH
value in the context of the thread in either of these two situations:
.begbull
.bull
The thread was the initial thread in the process, so the system called
.kw LibMain
with the
.id DLL_PROCESS_ATTACH
value.
.bull
The thread was already running when a call to the
.kw LoadLibrary
function was made, so the system never called
.kw LibMain
for it.
.endbull
.*
.note __FDLL_TERMINATE_
.*
This function is called when the DLL is detaching from the address
space of the calling process as a result of either a normal
termination or
of a call to
.kw FreeLibrary
.ct .li .
When a DLL detaches from a process as a result of process termination
or as a result of a call to
.kw FreeLibrary
.ct , the operating system does not call the DLL's
.kw LibMain
with the
.id DLL_THREAD_DETACH
value for the individual threads of the process.
The DLL is only given
.id DLL_PROCESS_DETACH
notification.
DLLs can take this opportunity to clean up all resources for all
threads attached and known to the DLL.
.*
.note Note:
.*
These functions return 1 if initialization succeeds or 0 if
initialization fails.
Subsequently, this value will be returned by the run-time system's
.id LibMain
function.
.np
If the return value is 0 when
.kw LibMain
is called because the process uses the
.kw LoadLibrary
function,
.kw LoadLibrary
returns NULL.
.np
If the return value is 0 when
.kw LibMain
is called during process initialization, the process terminates with
an error.
.*
.note DLL_ENTRY_1, DLL_ENTRY_2
.*
These are sample DLL entry points that we will call from our simple
test program.
.endnote
.np
Some further explanation and an example are provided in a later
section.
.do end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
#include <stdio.h>
.if '&targetos' eq 'Windows NT' .do begin
#include <windows.h>

#if defined(__cplusplus)
#define EXPORTED extern "C" __declspec( dllexport )
#else
#define EXPORTED __declspec( dllexport )
#endif

DWORD TlsIndex; /* Global Thread Local Storage index */

/* Error checking should be performed in following code */

.code break
BOOL APIENTRY LibMain( HANDLE hinstDLL,
                       DWORD  fdwReason,
                       LPVOID lpvReserved )
{
    switch( fdwReason ) {
    case DLL_PROCESS_ATTACH:
        /* do process initialization */

        /* create TLS index */
        TlsIndex = TlsAlloc();
        break;

.code break
    case DLL_THREAD_ATTACH:
        /* do thread initialization */

        /* allocate private storage for thread */
        /* and save pointer to it */
        TlsSetValue( TlsIndex, malloc(200) );
        break;

.code break
    case DLL_THREAD_DETACH:
        /* do thread cleanup */

        /* get the TLS value and free associated memory */
        free( TlsGetValue( TlsIndex ) );
        break;

.code break
    case DLL_PROCESS_DETACH:
        /* do process cleanup */

        /* free TLS index */
        TlsFree( TlsIndex );
        break;
    }
    return( 1 );        /* indicate success */
    /* returning 0 indicates initialization failure */
}

.code break
EXPORTED void dll_entry_1( void )
{
    printf( "Hi from dll entry #1\n" );
}

.code break
EXPORTED void dll_entry_2( void )
{
    printf( "Hi from dll entry #2\n" );
}
.do end
.el .if '&targetos' eq 'OS/2 2.x' .do begin
#include <os2.h>

#if defined(__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

unsigned APIENTRY LibMain( unsigned hmod, unsigned termination )
{
    if( termination ) {
        /* DLL is detaching from process */
    } else {
        /* DLL is attaching to process */
    }
    return( 1 );
}

.code break
EXTERNC void dll_entry_1( void )
{
    printf( "Hi from dll entry #1\n" );
}

.code break
EXTERNC void dll_entry_2( void )
{
    printf( "Hi from dll entry #2\n" );
}
.do end
.code end
.if '&targetos' eq 'Windows NT' .do begin
.begnote Arguments:
.*
.note hinstDLL
.*
This is a handle for the DLL.
It can be used as a argument to other functions such as
.kw GetModuleFileName
.ct .li .
.*
.note fdwReason
.*
This argument indicates why
.kw LibMain
is being called.
It can have one of the following values:
.*
.begnote
.notehd1 Value
.notehd2 Meaning
.*
.note DLL_PROCESS_ATTACH
.*
This value indicates that the DLL is attaching to the address space of
the current process as a result of the process starting up or as a
result of a call to
.kw LoadLibrary
.ct .li .
A DLL can use this opportunity to initialize any instance data or to
use the
.kw TlsAlloc
function to allocate a Thread Local Storage (TLS) index.
.np
During initial process startup or after a call to
.kw LoadLibrary
.ct , the operating system scans the list of loaded DLLs for the process.
For each DLL that has not already been called with the
.id DLL_PROCESS_ATTACH
value, the system calls the DLL's
.kw LibMain
entry-point.
This call is made in the context of the thread that caused the process
address space to change, such as the primary thread of the process or
the thread that called
.kw LoadLibrary
.ct .li .
.*
.note DLL_THREAD_ATTACH
.*
This value indicates that the current process is creating a new
thread.
When this occurs, the system calls the
.kw LibMain
entry-point of all DLLs currently attached to the process.
The call is made in the context of the new thread.
DLLs can use this opportunity to initialize a Thread Local Storage
(TLS) slot for the thread.
A thread calling the DLL's
.id LibMain
with the
.id DLL_PROCESS_ATTACH
value does not call
.kw LibMain
with the
.id DLL_THREAD_ATTACH
value.
Note that
.kw LibMain
is called with this value only by threads created after the DLL is
attached to the process.
When a DLL is attached by
.kw LoadLibrary
.ct , existing threads do not call the
.kw LibMain
entry-point of the newly loaded DLL.
.*
.note DLL_THREAD_DETACH
.*
This value indicates that a thread is exiting normally.
If the DLL has stored a pointer to allocated memory in a TLS slot, it
uses this opportunity to free the memory.
The operating system calls the
.kw LibMain
entry-point of all currently loaded DLLs with this value.
The call is made in the context of the exiting thread.
There are cases in which
.kw LibMain
is called for a terminating thread even if the DLL never attached to
the thread.
For example,
.kw LibMain
is never called with the
.id DLL_THREAD_ATTACH
value in the context of the thread in either of these two situations:
.begbull
.bull
The thread was the initial thread in the process, so the system called
.kw LibMain
with the
.id DLL_PROCESS_ATTACH
value.
.bull
The thread was already running when a call to the
.kw LoadLibrary
function was made, so the system never called
.kw LibMain
for it.
.endbull
.*
.note DLL_PROCESS_DETACH
.*
This value indicates that the DLL is detaching from the address space
of the calling process as a result of either a normal termination or
of a call to
.kw FreeLibrary
.ct .li .
The DLL can use this opportunity to call the
.kw TlsFree
function to free any TLS indices allocated by using
.kw TlsAlloc
and to free any thread local data.
When a DLL detaches from a process as a result of process termination
or as a result of a call to
.kw FreeLibrary
.ct , the operating system does not call the DLL's
.kw LibMain
with the
.id DLL_THREAD_DETACH
value for the individual threads of the process.
The DLL is only given
.id DLL_PROCESS_DETACH
notification.
DLLs can take this opportunity to clean up all resources for all
threads attached and known to the DLL.
.endnote
.*
.note lpvReserved
.*
This argument specifies further aspects of DLL initialization and
cleanup.
If
.id fdwReason
is
.id DLL_PROCESS_ATTACH,
.id lpvReserved
is NULL for dynamic loads and non-NULL for static loads.
If
.id fdwReason
is
.id DLL_PROCESS_DETACH,
.id lpvReserved
is NULL if
.kw LibMain
has been called by using
.kw FreeLibrary
and non-NULL if
.kw LibMain
has been called during process termination.
.*
.note Return Value
.*
When the system calls the
.id LibMain
function with the
.id DLL_PROCESS_ATTACH
value, the function returns TRUE (1) if initialization succeeds or
FALSE (0) if initialization fails.
.np
If the return value is FALSE (0) when
.kw LibMain
is called because the process uses the
.kw LoadLibrary
function,
.kw LoadLibrary
returns NULL.
.np
If the return value is FALSE (0) when
.kw LibMain
is called during process initialization, the process terminates with
an error.
To get extended error information, call
.kw GetLastError
.ct .li .
.np
When the system calls
.kw LibMain
with any value other than
.id DLL_PROCESS_ATTACH,
the return value is ignored.
.endnote
.do end
.el .if '&targetos' eq 'OS/2 2.x' .do begin
.np
32-bit OS/2 DLLs can include a
.kw LibMain
entry point when you are using the &product run-time libraries.
.begnote Arguments:
.*
.note hmod
.*
This is a handle for the DLL.
.*
.note termination
.*
A 0 value indicates that the DLL is attaching to the address space of
the current process as a result of the process starting up or as a
result of a call to
.kw DosLoadModule
.ct .li .
A DLL can use this opportunity to initialize any instance data.
.np
A non-zero value indicates that the DLL is detaching from the address
space of the calling process as a result of either a normal
termination or of a call to
.kw DosFreeModule
.ct .li .
.*
.note Return Value
.*
The
.id LibMain
function returns 1 if initialization succeeds or 0 if initialization fails.
.np
If the return value is 0 when
.kw LibMain
is called because the process uses the
.kw DosLoadModule
function,
.kw DosLoadModule
returns an error.
.np
If the return value is 0 when
.kw LibMain
is called during process initialization, the process terminates with
an error.
.endnote
.do end
.do end
.np
Assume the above example is contained in the file
.fi dllsamp.&langsuff..
We can compile the file using the following command.
Note that we must specify the "bd" compiler option.
.millust begin
&prompt.&ccmd32 &sw.bd dllsamp
.millust end
.np
Before we can link our example, we must create a linker directive file
that describes the attributes and entry points of our dynamic link
library.
The following is a linker directive file, called
.fi dllsamp.lnk,
that can be used to create the dynamic link library.
.millust begin
.if '&targetos' eq 'OS/2 2.x' .do begin
system os2v2 dll initinstance terminstance
option manyautodata
.do end
.el .do begin
system nt_dll initinstance terminstance
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
export DLL_ENTRY_1
export DLL_ENTRY_2
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
export dll_entry_1_
export dll_entry_2_
.do end
file dllsamp
.millust end
.autonote Notes:
.note
The "SYSTEM" directive specifies that we are creating
.if '&targetos' eq 'OS/2 2.x' .do begin
a 32-bit OS/2
.do end
.el .do begin
a Windows NT
.do end
dynamic link library.
.note
.if '&targetos' eq 'OS/2 2.x' .do begin
The "MANYAUTODATA" option specifies that the automatic data segment is
allocated for every instance of the dynamic link library.
This option must be specified only for a dynamic link library that
uses the &product run-time libraries.
If the &product run-time libraries are not used, this option is not
required.
Our example does use the &product run-time libraries so we must
specify the "MANYAUTODATA" option.
.np
As was just mentioned, when a dynamic link library uses the &product
run-time libraries, an automatic data segment is created each time a
process accesses the dynamic link library.
.do end
.el .do begin
When a dynamic link library uses the &product
run-time libraries, an automatic data segment is created each time a
new process accesses the dynamic link library.
.do end
For this reason, initialization code must be executed when a process
accesses the dynamic link library for the first time.
To achieve this, "INITINSTANCE" must be specified in the "SYSTEM"
directive.
Similarly, "TERMINSTANCE" must be specified so that the termination
code is executed when a process has completed its access to the
dynamic link library.
If the &product run-time libraries are not used, these options are not
required.
.note
The "EXPORT" directive specifies the entry points into the dynamic
link library.
.if '&lang' eq 'FORTRAN 77' .do begin
Note that in &product, names of all symbols are uppercased. Regardless
of the case used in source files, linker directives must use uppercased
symbol names. The linker is case sensitive by default, although the
"OP NOCASEEXACT" directive may be used to override this.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Note that the names specified in the "EXPORT" directive are appended
with an underscore.
This is the default naming convention used when compiling using the
register-based calling convention.
No underscore is required when compiling using the stack-based calling
convention.
.do end
.endnote
.np
We can now create our dynamic link library by issuing the following
command.
.millust begin
&prompt.&lnkcmd @dllsamp
.millust end
.pc
A file called
.fi dllsamp.dll
will be created.
.*
.section Using Dynamic Link Libraries
.*
.np
.ix 'dynamic link library access' '&targetos.'
.ix 'DLL access' '&targetos.'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&targetos' eq 'Windows NT' .do begin
.np
It is assumed that all symbols imported by a client application were
declared with a
.id __declspec( dllimport )
modifier when the client application was compiled. At the link stage
we have to tell the linker which dynamic libraries the client
application should link to.
.do end
.do end
Once we have created a dynamic link library, we must allow other
applications to access the functions available in the dynamic link
library.
There are two ways to achieve this.
.np
The first method is to create a linker directive file which contains
an "IMPORT" directive for all entry points in the dynamic link
library.
The "IMPORT" directive provides the name of the entry point and the
name of the dynamic link library.
When creating an application that references a function in the dynamic
link library, this linker directive file would be included as part of
the linking process that created the application.
.np
The second method is to use import libraries.
.ix 'import library'
An import library is a standard library that is created from a dynamic
link library by using the &libname..
It contains object modules that describe the entry points in a dynamic
link library.
The resulting import library can then be specified in a "LIBRARY"
directive in the same way one would specify a standard library.
.np
Using an import library is the preferred method of providing
references to functions in dynamic link libraries.
When a dynamic link library is modified, typically the import library
corresponding to the modified dynamic link library is updated to
reflect the changes.
Hence, any directive file that specifies the import library in a
"LIBRARY" directive need not be modified.
However, if you are using "IMPORT" directives, you may have to modify
the "IMPORT" directives to reflect the changes in the dynamic link
library.
.np
Let us create an import library for our sample dynamic link library we
created in the previous section.
We do this by issuing the following command.
.millust begin
&prompt.&libcmd dllsamp +dllsamp.dll
.millust end
.np
A standard library called
.fi dllsamp.lib
will be created.
.np
Suppose the following sample program, contained in the file
.fi dlltest.&langsuff,
calls the functions from our sample dynamic link library.
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
* DLLTEST.FOR

      call dll_entry_1()
      call dll_entry_2()
      end
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
#include <stdio.h>
.if '&targetos' eq 'Windows NT' .do begin
#include <process.h>
.do end

.if '&targetos' eq 'Windows NT' .do begin
#if defined(__cplusplus)
#define IMPORTED extern "C" __declspec( dllimport )
#else
#define IMPORTED __declspec( dllimport )
#endif

IMPORTED void dll_entry_1( void );
IMPORTED void dll_entry_2( void );
.do end
.if '&targetos' eq 'OS/2 2.x' .do begin
#if defined(__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void dll_entry_1( void );
EXTERNC void dll_entry_2( void );
.do end

.code break
.if '&targetos' eq 'Windows NT' .do begin
#define STACK_SIZE      8192

static void thread( void *arglist )
{
    printf( "Hi from thread\n" );
    _endthread();
}

.code break
.do end
int main( void )
{
.if '&targetos' eq 'Windows NT' .do begin
    unsigned long   tid;

    dll_entry_1();
    tid = _beginthread( thread, STACK_SIZE, NULL );
.do end
.el .do begin

    dll_entry_1();
.do end
    dll_entry_2();
    return( 0 );
}
.code end
.do end
.np
We can compile and link our sample application by issuing the
following command.
.millust begin
&prompt.&wclcmd32 &tgtopts.&sw.l=&tgtosname dlltest dllsamp.lib
.millust end
.np
If we had created a linker directive file of "IMPORT" directives
instead of an import library for the dynamic link library, the linker
directive file, say
.fi dllimps.lnk,
would be as follows.
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
import DLL_ENTRY_1 dllsamp
import DLL_ENTRY_2 dllsamp
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
import dll_entry_1_ dllsamp
import dll_entry_2_ dllsamp
.millust end
.np
Note that the names specified in the "IMPORT" directive are appended
with an underscore.
This is the default naming convention used when compiling using the
register-based calling convention.
No underscore is required when compiling using the stack-based calling
convention.
.do end
.np
To compile and link our sample application, we would issue the
following command.
.millust begin
&prompt.&wclcmd32 &tgtopts.&sw.l=&tgtosname dlltest &sw."@dllimps"
.millust end
.*
.section The Dynamic Link Library Data Area
.*
.np
The &product 32-bit run-time library does not support the general case
operation of DLLs in an execution environment where there is only one
instance of the DATA segment (DGROUP) for that DLL.
.np
There are two cases that can lead to a DLL executing with only one
instance of the DGROUP.
.autonote
.note
DLLs linked for 32-bit OS/2 without the MANYAUTODATA option.
.note
DLLs linked for the Win32 API and executing under Win32s.
.endnote
.np
In these cases the run-time library startup code detects that there is
only one instance of the DGROUP when a second process attempts to
attach to the DLL.
At that point, it issues a diagnostic for the user and then notifies
the operating system that the second process cannot attach to the DLL.
.np
Developers who require DLLs to operate when there is only one instance
of the DGROUP can suppress the function which issues the diagnostic
and notifies the operating system that the second process cannot
attach to the DLL.
.np
Doing so requires good behaviour on the part of processes attaching to
the DLL.
This good behaviour consists primarily of ensuring that the first
process to attach to the DLL is also the last process to detach from
the DLL thereby ensuring that the DATA segment is not released back
to the free memory pool.
.np
To suppress the function which issues the diagnostic and notifies the
operating system that the second process cannot attach to the DLL, the
developer must provide a replacement entry point with the following
prototype:
.millust begin
int __disallow_single_dgroup( int );
.millust end
.np
This function should return zero to indicate that the detected single
copy of the DATA segment is allowed.
.*
.if '&targetos' eq 'OS/2 2.x' or '&lang' eq 'FORTRAN 77' .do begin
.section Dynamic Link Library Initialization/Termination
.*
.np
.ix 'dynamic link library initialization' '&targetos.'
.ix 'DLL initialization' '&targetos.'
.ix 'initialization' '&targetos dynamic link library'
.ix 'dynamic link library termination' '&targetos.'
.ix 'DLL termination' '&targetos.'
.ix 'termination' '&targetos dynamic link library'
Each dynamic link library (DLL) has an initialization and termination
routine associated with it.
The initialization routine can either be called the first time any
process accesses the DLL ("INITGLOBAL" is specified at link time) or
each time a process accesses the DLL ("INITINSTANCE" is specified at
link time).
Similarly, the termination routine can either be called when all
processes have completed their access of the DLL ("TERMGLOBAL" is
specified at link time) or each time a process completes its access of
the DLL ("TERMINSTANCE" is specified at link time).
.np
For a DLL that uses the &lang run-time libraries, initialization and
termination of the &lang run-time environment is performed
automatically.
It is also possible for a DLL to do its own special initialization and
termination process.
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
The &lang run-time environment provides two methods for calling
user-written DLL initialization and termination code.
.autonote
.note
If you provide your own version of
.id LibMain
then it will be called for initialization and termination.
The use of
.id LibMain
is described earlier in this chapter.
.note
If you do not provide your own version of
.id LibMain
then a default version is linked in from the library.
This version will call
.ix '&dll_init'
.id &dll_init
for DLL initialization and
.ix '&dll_term'
.id &dll_term
for DLL termination.
Default stub versions of these two routines are included in the
run-time library.
If you wish to perform additional initialization/termination that is
specific to your dynamic link library, you may write your own
versions of these routines.
.endnote
.do end
.*
.el .do begin
.np
The &lang run-time environment provides a method for calling
user-written DLL initialization and termination code.
The
.ix '&dll_init'
.id &dll_init
routine is called for DLL process initialization.
.if '&targetos' eq 'Windows NT' .do begin
The
.ix '&thrd_init'
.id &thrd_init
routine is called for DLL thread initialization.
The
.ix '&thrd_term'
.id &thrd_term
routine is called for DLL thread termination.
.do end
The
.ix '&dll_term'
.id &dll_term
routine is called for DLL process termination.
Default stub versions of these routines are included in the
run-time library.
If you wish to perform additional initialization/termination
processing that is specific to your dynamic link library, you may
write your own versions of these routines.
.do end
.*
.if '&targetos' eq 'OS/2 2.x' .do begin
.np
Once the &lang run-time environment is initialized, the routine
.id &dll_init
is called.
After the &lang run-time environment is terminated, the routine
.id &dll_term
is called.
This last point is important since it means that you cannot do any
run-time calls in the termination routine.
.do end
.*
.el .do begin
.np
When a process first attaches to the DLL, the &lang run-time
environment is initialized and then the routine
.id &dll_init
is called.
When a thread is started, the routine
.id &thrd_init
is called.
When a thread is terminated, the routine
.id &thrd_term
is called.
When the main process relinquishes the DLL,
the routine
.id &dll_term
is called and then the &lang run-time environment is terminated,
.do end
.*
.np
The initialization and termination routines return an integer.
A value of 0 indicates failure; a value of 1 indicates success.
The following example illustrates sample initialization/termination
routines.
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
* DLLINIT.FOR

      integer function __fdll_initialize_()
.if '&targetos' eq 'Windows NT' .do begin
      integer __fthrd_initialize_, __fthrd_terminate_
.do end
      integer __fdll_terminate_, dll_entry

      integer WORKING_SIZE
      parameter ( WORKING_SIZE = 16*1024 )
      integer ierr, WorkingStorage
      dimension WorkingStorage(:)

.code break
      allocate( WorkingStorage(WORKING_SIZE), stat=ierr )
      if( ierr .eq. 0 )then
          __fdll_initialize_ = 1
      else
          __fdll_initialize_ = 0
      endif
      return

.if '&targetos' eq 'Windows NT' .do begin
.code break
      entry __fthrd_initialize_()
      __fthrd_initialize_ = 1
      return

.code break
      entry __fthrd_terminate_()
      __fthrd_terminate_ = 1
      return

.do end
.code break
      entry __fdll_terminate_()
* Note: no run-time calls allowed under OS/2 Warp
      deallocate( WorkingStorage )
      __fdll_terminate_ = 1
      return

.code break
      entry dll_entry()
      ! use WorkingStorage
      return

      end
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
#include <stdlib.h>

#define WORKING_SIZE (64 * 1024)

char *WorkingStorage;

#if defined(__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

void __dll_finalize( void );

EXTERNC int __dll_initialize( void )
{
    WorkingStorage = malloc( WORKING_SIZE );
    if( WorkingStorage == NULL ) return( 0 );
    atexit( __dll_finalize );
    return( 1 );
}

void __dll_finalize( void )
{
    free( WorkingStorage );
}

EXTERNC int __dll_terminate( void )
{
    return( 1 );
}

EXTERNC void dll_entry( void )
{
    /* use array WorkingStorage */
}
.code end
.do end
.np
In the above example, the process initialization routine allocates
storage that the dynamic link library needs, the routine
.id dll_entry
uses the storage, and the process termination routine frees the
storage allocated in the initialization routine.
.do end
.*
:set symbol="prompt"     value=&oprompt.
