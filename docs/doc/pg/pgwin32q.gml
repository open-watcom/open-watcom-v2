.chap 32-bit Extended Windows Application Development
.*
.np
.ix 'Windows 3.x extender' 'questions'
The purpose of this chapter is to anticipate some common questions
about 32-bit Windows application development.
.np
The following topics are discussed in this chapter:
.begbull $compact
.bull
Can you call 16-bit code from a 32-bit Windows application?
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
Can I WinExec another Windows application?
.do end
.bull
How do I add my Windows resources?
.bull
All function pointers passed to Windows must be 16-bit far pointers, correct?
.bull
Why are 32-bit callback routines FAR?
.bull
Why use the _16 API functions?
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
What about pointers in structures?
:cmt. .bull
:cmt. Why isn't memory released to the system when I free it?
:cmt. .bull
:cmt. Why aren't the exit activities performed when I unload a DLL?
.bull
When do I use MK_FP32?
.bull
What is the difference between AllocAlias16 and MK_FP16?
:cmt. .bull
:cmt. How do you open more than 20 files for a 32-bit Windows Process?
.do end
.endbull
.*
.section Can you call 16-bit code from a 32-bit Windows application?
.*
.np
.ix 'Windows 3.x extender' 'calling 16-bit code'
A 32-bit Windows application can make a call to 16-bit code through
the use of the &company
.kw _Call16
or
.kw InvokeIndirectFunction
procedures.
These functions ensure that the &company Windows Supervisor prepares
the stack for the 16-bit call and return to the 32-bit code.
The 32-bit application uses
.kw LoadLibrary
function to bring the 16-bit DLL into memory and then calls the 16-bit
procedures.
To invoke 16-bit procedures, use
.kw GetProcAddress
to get the 16-bit far pointer to the function.
Use the
.kw _Call16
procedure to call the 16-bit function since it is simpler
to use than the
.kw GetIndirectFunctionHandle
.ct ,
.kw InvokeIndirectFunction
.ct , and
.kw FreeIndirectFunctionHandle
sequence.
An example of this process is provided under the
.kw _Call16
Windows library function description.
.np
This method can be used to call any 16-bit Dynamic Link Library (DLL)
procedure or any 32-bit extended DLL procedure from within a 32-bit
application, including DLLs that are available as products through
Independent Software Vendors (ISVs).
:cmt. .np
:cmt.    NOTE:  This section cannot apply to FORTRAN since 9.0 did not
:cmt.    allow you to create 16-bit DLLs in FORTRAN.  This information
:cmt.    will probably be useful for the C version...
:cmt. This method is particularly useful if an application contains
:cmt. sections of code that are "Windows-intensive" and require a fair
:cmt. amount of interaction with Windows, which is a 16-bit environment.
:cmt. From a 32-bit application, each Windows call has the overhead of the
:cmt. switch from 32-bit to 16-bit.
:cmt. This overhead can be avoided by isolating the "Windows-intensive" code
:cmt. into a 16-bit DLL and sending "request packets" to it from the
:cmt. 32-bit application.
:cmt. These "request packets" would be used by the 16-bit DLL to perform the
:cmt. various Windows functions.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section Can I WinExec another Windows application?
.*
.np
.ix 'Windows 3.x extender' 'WinExec'
As far as Windows is concerned, the WinExec was made by a 16-bit
application, and the application specified will be started.
This new application can be a 16-bit application or another 32-bit
application that was implemented with &cmpname.
.*
.do end
.*
.section How do I add my Windows resources?
.*
.np
.ix 'Windows 3.x extender' 'resources'
The
.kw WBIND
utility automatically runs the resource compiler to add the resources
to the 32-bit Windows supervisor (since the supervisor is a 16-bit
Windows application).
Note that resource compiler options may be specified by using the "R"
option of
.kw WBIND
.ct .li .
.*
.section All function pointers passed to Windows must be 16-bit far pointers, correct?
.*
.np
.ix 'Windows 3.x extender' 'function pointers'
All function pointers passed to Windows must be 16-bit far pointers
since no translation is applied to any function pointers passed to
Windows.
Translation is often not possible, since any functions that Windows is
to call back must be exported, and only 16-bit functions can be
exported.
.np
A 16-bit far pointer to a function is obtained in one of two ways:
either Windows gives it to you (via
.kw GetProcAddr
.ct , for example), or you obtain a pointer from the supervisor, via
.kw GetProc16
.ct .li .
.np
Function pointers obtained from Windows may either be fed into other
Windows functions requiring function pointers, or called indirectly
by using
.kw _Call16
or by using the
.kw GetIndirectFunctionHandle
.ct ,
.kw InvokeIndirectFunction
.ct , and
.kw FreeIndirectFunctionHandle
sequence.
.np
The function
.kw GetProc16
returns a 16-bit far pointer to a callback function that Windows can
use.
This callback function will direct control into the desired 32-bit
routine.
.*
.section Why are 32-bit callback routines FAR?
.*
.np
.ix 'Windows 3.x extender' '32-bit callback routines'
The callback routines are declared as FAR so that the compiler will
generate a far return from the procedure.
This is necessary since the 32-bit callback routine is "far" called
from the supervisor.
.np
The callback routine is still "near" in the sense that it lies within
the 32-bit flat address space of the application.
This means that
.kw GetProc16
only needs the offset of the 32-bit callback function in order to
set up the 16-bit procedure to call back correctly.
Thus,
.kw GetProc16
accepts type
.kw PROCPTR
which is in fact only 4 bytes long.
The compiler will provide the offset only, which is, as already
stated, all that is needed.
.*
.section Why use the _16 API functions?
.*
.np
.ix 'Windows 3.x extender' '_16xxx functions'
The regular Windows API functions used in &cmpname automatically
convert any pointers to 16-bit far pointers for use by Windows.
Sometimes, you may have a set of pointers that are 16-bit far pointers
already (e.g., obtained from
.kw GlobalLock
.ct ),
and do not need any conversion.
The "_16..." API functions do not convert pointers, they simply pass
them on directly to Windows.
See the appendix entitled :HDREF refid='winfn16'. for a list of the
"_16..." API functions.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section What about pointers in structures?
.*
.np
.ix 'Windows 3.x extender' 'pointers in structures'
Pointers in structures will be converted if the Windows API function
actually takes a pointer to that structure (i.e., if it is possible
for the supervisor to identify that structure).
There are few functions that accept pointers to structures containing
pointers.
One such function is
.us RegisterClass
which accepts a pointer to a WNDCLASS structure.
.np
If Windows has you passing a pointer to a structure through a 32-bit
integer argument, then it is not possible for the supervisor to identify that
as a pointer that needs conversion.
It is also not possible for the supervisor to convert any
pointers contained in the structure, since it is not aware that it is
a structure (as far as the supervisor is concerned, that data is what
Windows said it was - a 32-bit integer).
In this case, it is necessary to get 16-bit far pointer equivalents
to the 32-bit near pointers that you want to pass.
Use
.kw AllocAlias16
for this.
:cmt. .*
:cmt. .section Why isn't memory released to the system when I free it?
:cmt. .*
:cmt. When GlobalAlloc() is used to allocate memory blocks, the system
:cmt. uses a new segment for the memory.  The subsequent GlobalFree() call
:cmt. releases the entire segment back to the system.
:cmt. .*
:cmt. .section Why aren't the exit activities performed when I unload a DLL?
:cmt. .*
.*
.section When do I use MK_FP32?
.*
.np
.ix 'Windows 3.x extender' 'MK_FP32'
.kw MK_FP32
is used to convert all 16-bit far pointers to 32-bit far pointers that
can be used by your 32-bit application.
For example, to access the memory returned by
.kw GlobalLock
requires the use of
.kw MK_FP32
.ct .li .
To access any pointer passed to you (in a callback routine)
requires the use of
.kw MK_FP32
if you want access to that data in your 32-bit application.
.*
.section What is the difference between AllocAlias16 and MK_FP16?
.*
.np
.ix 'Windows 3.x extender' 'AllocAlias16'
.ix 'Windows 3.x extender' 'MK_FP16'
.kw AllocAlias16
actually gets a new selector that points at the same memory as the 32-bit
near pointer, whereas
.kw MK_FP16
squishes a 32-bit far pointer back into a 16-bit far pointer (i.e., it
reverses
.kw MK_FP32
.ct ).
:cmt. .*
:cmt. .section How do you open more than 20 files for a 32-bit Windows Process?
:cmt. .*
:cmt. See chapter 1 for the section
:cmt. entitled "How can I open more than 20 files at a time?"
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section Tell Me More About Thunking and Aliases
.*
.np
.ix 'Windows 3.x extender' 'thunks'
.ix 'Windows 3.x extender' 'aliases'
Consider the following example.
.code begin
dwAlias = AllocAlias16( pszSomething );
hwnd = CreateWindowEx(
            0L,                     // extendedStyle
            "classname",            // class name
            "",
            WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_HSCROLL|
            WS_BORDER|WS_CAPTION|WS_SYSMENU,
            x, y, 0, 0,             // x, y, cx, cy
            hwndParent,             // hwndParent
            NULL,                   // control ID
            g_app.hinst,            // hInstance
            (void FAR*)dwAlias);    // lpCreateParams

FreeAlias16( dwAlias );
.code end
.np
When I get the
.id lpCreateParams
parameter in
.id WM_CREATE,
I don't get the original
.id dwAlias
but something else which looks like another alias to me.
So the question is: Must the
.id CreateWindowEx
parameter
.id lpCreateParams
be "thunked" or is this done automatically by the supervisor?
.np
Thunks are always created for function pointers. Aliases are always created
for data pointers. There are 3 data pointer parameters in the
.id CreateWindowEx
function call. Aliases are created for all three pointers. The
.id lpCreateParams
argument is a pointer to a struct which contains 3 pointers. Aliases are not
created for the 3 pointers inside the struct. If you need to have this done,
then you will have to create the aliases yourself. If you create aliases for
the parameters to
.id CreateWindowEx,
then you must call the
.id _16CreateWindowEx
function which will not create any aliases.
.np
Here is some further information on thunks (which are created for
function pointers). There is code in the supervisor that
.us trys
(note the word
.us trys
.ct ) to determine if the user has already created a thunk and, if so,
avoids creating a double thunk which will always generate a GPF.
The best policy is to let the supervisor automatically create all
thunks for you unless you have a very specific reason not to, in which
case you should call the _16 version of the function.
.np
Here is some further information on aliases (which are created for
data pointers).
There is no way for the supervisor to determine if a value is a 32-bit
flat pointer or an alias for the pointer.
So if you pass in an alias to the non _16 version of the function, the
supervisor will create an alias for the alias which will end up
pointing to the wrong memory location.
If you are going to create the alias, then you must call the _16
version of the function.
.do end
