.chap Interfacing Visual Basic and &product DLLs
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction to Visual Basic and DLLs
.do end
.*
.np
.ix 'Visual Basic'
.ix 'dynamic link library'
.ix '32-bit DLL'
.ix 'DLL' '32-bit'
This chapter describes how to interface Microsoft Visual Basic 3.0
applications and 32-bit Dynamic Link Libraries (DLLs) created by
&product..
It describes how to write functions for a 32-bit DLL, how to compile
and link them, and how to call these functions from Visual Basic.
.ix '16-bit DLL'
.ix 'DLL' '16-bit'
One of the proposed techniques involves the use of a set of cover
functions in a 16-bit DLL so, indirectly, this chapter also describes
interfacing to 16-bit DLLs.
.np
.ix 'Win386LibEntry'
It is possible to invoke the
.id Win386LibEntry
function (&company's 32-bit function entry point, described below)
directly from Visual Basic.
However, this technique limits the arguments that can be passed to a
32-bit DLL.
The procedure and problems are explained below.
.np
To work around the problem, a 16-bit DLL can be created, that covers
the 32-bit DLL.
Within the 16-bit DLL, we will place cover functions that will call
the corresponding 32-bit function in the 32-bit DLL.
We illustrate the creation of the 16-bit DLL using the 16-bit C
compiler in &company C/C++.
.np
Before we begin our example, there are some important technical issues
to consider.
.np
.ix 'Visual Basic' 'Version 3.0'
The discussion in this chapter assumes that you, the developer, have a
working knowledge of Visual Basic, including how to bring up the
general declarations screen, how to create command buttons, and how to
associate code with command buttons. You must use Visual Basic 3.0 or
later. Visual Basic Version 2.x will not work because of a deficiency
in this product regarding the calling of functions in DLLs.
.np
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
For the purposes of the following discussion, you should have
installed both the 16-bit and 32-bit versions of &product,
as well as version 3.0 or later of Visual Basic.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
For the purposes of the following discussion, you should have
installed the 32-bit version of &product,
as well as version 3.0 or later of Visual Basic.
If you also have the 16-bit &company C compiler, you can use this to
create a 16-bit DLL containing the 16-bit cover functions.
.do end
Ensure that the
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.ev PATH,
.ev INCLUDE
and
.ev WINDOWS_INCLUDE
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.ev PATH
amd
.ev FINCLUDE
.do end
environment variables are defined to include at least the directories
indicated.
We have assumed that &product. is installed in the
.fi c:&pathnam
directory, and Visual Basic is in the
.fi c:\vb
directory:
.code begin
set path=c:&pathnam\binw;c:\vb;c:\dos;c:\windows
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
set include=c:&pathnam.\h
set windows_include=c:&pathnam.\h\win
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
set finclude=c:&pathnam.\src\fortran\win
.do end
.code end
.np
&company's 32-bit DLL supervisor contains a general entry point for
Windows applications to call into called
.id Win386LibEntry.
It also contains 128 specific entry points called
.id DLL1
to
.id DLL128
which correspond to the entry points established via
.id DefineDLLEntry
(the first argument to
.id DefineDLLEntry
is an index number in the range 1 to 128).
All applications call into the 32-bit DLL via these entry points.
They build the necessary stack frame and switch to the 32-bit DLL's
data space.
.np
If you call via
.id Win386LibEntry
then you pass the DLL entry point number or index (1 to 128) as the
last argument.
.id Win386LibEntry
uses this index number to call the appropriate 32-bit DLL routine.
.np
In many languages and programs (such as C and Microsoft Excel),
function calls are very flexible. In other words, a function can be
called with different argument types each time. This is generally
necessary for calling
.id Win386LibEntry
in a 32-bit extended DLL function.
The reason is that this function takes the same arguments as the
function being called, as well as the index number of the called
function. After the 32-bit flat model has been set up,
.id Win386LibEntry
then calls this function. In Visual Basic, once a function is declared
as having certain arguments, it cannot be redeclared. For example,
suppose we have a declaration as follows:
.exam begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Declare Function Win386LibEntry Lib "c:\path\vbdll32.dll"
=> (ByVal v1 As Integer, ByVal v2 As Long, ByVal
=> v3 As Integer, ByVal I As Integer) As Long
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
Declare Function Win386LibEntry Lib "c:\path\vbdll32.dll"
=> (ByVal v1 As Long, ByVal v2 As Long, ByVal
=> v3 As Long, ByVal I As Integer) As Long
.do end
.exam end
.pc
(Note: the => means to continue the statement on the same line.)
In this example, we could only call a function in any 32-bit extended
DLL with
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
a 16-bit integer as the first and third argument, and a 32-bit
integer as the second argument.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
three 32-bit integers as arguments.
.do end
There are three ways to work around this deficiency in Visual Basic:
.autonote
.note
Use the Visual Basic "Alias" attribute to declare
.id Win386LibEntry
differently for each DLL routine.
Reference the different DLL routines using these aliases.
.note
Use the specific entry point, one of
.id DLL1
through
.id DLL128,
corresponding to the DLL routine that you want to call.
Each entry point can be described to take different arguments.  We can
still use the "Alias" attribute to make the link between the name we
use in the Visual Basic function and the name in the 32-bit extended
DLL.  This is the method that we will use in the "Direct Call"
technique discussed below.  It is simpler to use since it requires one
less argument (you don't require the index number).
.note
Use a method which involves calling functions in a 16-bit "cover" DLL
written in a flexible-argument language, which then calls the
functions in the 32-bit DLL.  This is the "Indirect Call" method
discussed below.
.endnote
.*
.section A Working Example
.*
.np
.ix 'Visual Basic' 'example'
The best way to demonstrate these techniques is through an example.
This example consists of a Visual Basic application with 3 push
buttons. The first push button invokes a direct call to a 32-bit DLL
which will display a message window with its arguments, the second
push button invokes an indirect call to the same function through a
16-bit DLL, and the third button exits the Visual Basic application.
.begstep To create a Visual Basic application:
.step Start up a new project folder
from the "File" menu.
.step Select "View Form"
from the "Project" window.
.step Draw three command buttons
on the form by selecting command buttons from the "Toolbox" window.
.step Change the caption on each button.
To do this, highlight the first button.
Then, open the "Properties" window.
Double click on the "Caption window", and change the caption to
"Direct call".
Highlight the second button, and change its caption to "Indirect
call".
Highlight the third, changing the caption to "Exit".
.result
Now, your Visual Basic application should have three push buttons,
"Direct call", "Indirect call", and "Exit".
.step Double click on the "Direct Call" button.
.result
An edit window will pop up. Enter the following code:
.millust begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Sub Command1_Click ()
    Dim var1, var2 As Integer
    Dim varlong, worked As Long

    var1 = 230
    varlong = 215
    var2 = 32
    worked = Add3(var1, varlong, var2)
    Print worked
    worked = Add2(varlong, var2)
    Print worked
End Sub
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
Sub Command1_Click ()
    Dim var1, var2, var3, worked As Long

    var1 = 230
    var2 = 215
    var3 = 32
    worked = Add3(var1, var2, var3)
    Print worked
    worked = Add2(var2, var3)
    Print worked
End Sub
.do end
.millust end
.step Double click on the "Indirect Call" button.
.result
Another edit window will pop up. Enter the following code:
.millust begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Sub Command2_Click ()
    Dim var1, var2 As Integer
    Dim varlong, worked As Long

    var1 = 230
    varlong = 215
    var2 = 32
    worked = Function1( var1, varlong, var2 )
    Print worked
    worked = Function2( varlong, var2 )
    Print worked
End Sub
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
Sub Command2_Click ()
    Dim var1, var2, var3, worked As Long

    var1 = 230
    var2 = 215
    var3 = 32
    worked = Function1(var1, var2, var3)
    Print worked
    worked = Function2(var2, var3)
    Print worked
End Sub
.do end
.millust end
.step Double click on the "Exit" command button
and enter the following code in the pop-up window:
.millust begin
Sub Command3_Click ()
    End
End Sub
.millust end
.step Select "View Code"
from the "Project" window.
To interface these Visual Basic functions to the DLLs, the following
code is needed in the
.millust Object: [general] Proc: [declarations]
section of the code.
This code assumes that
.fi VBDLL32.DLL
and
.fi COVER16.DLL
are in the
.fi c:\path
directory. Modify the pathnames appropriately if this is not the case.
(Note: the => means to continue the statement on the same line.)
.millust begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Declare Function Function1 Lib "c:\path\cover16.dll"
=> (ByVal v1 As Integer, ByVal v2 As Long,
=> ByVal v3 As Integer) As Long

Declare Function Function2 Lib "c:\path\cover16.dll"
=> (ByVal v1 As Long, ByVal v2 As Integer) As Long

Declare Function Add3 Lib "c:\path\vbdll32.dll"
=> Alias "DLL1"
=> (ByVal v1 As Integer, ByVal v2 As Long,
=> ByVal v3 As Integer) As Long

Declare Function Add2 Lib "c:\path\vbdll32.dll"
=> Alias "DLL2"
=> (ByVal v1 As Long, ByVal v2 As Integer) As Long
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
Declare Function Function1 Lib "c:\path\cover16.dll"
=> (ByVal v1 As Long, ByVal v2 As Long, ByVal v3 As Long)
=> As Long

Declare Function Function2 Lib "c:\path\cover16.dll"
=> (ByVal v1 As Long, ByVal v2 As Long) As Long

Declare Function Add3 Lib "c:\path\vbdll32.dll"
=> Alias "DLL1"
=> (ByVal v1 As Long, ByVal v2 As Long, ByVal v3 As Long)
=> As Long

Declare Function Add2 Lib "c:\path\vbdll32.dll"
=> Alias "DLL2"
=> (ByVal v1 As Long, ByVal v2 As Long) As Long
.do end
.millust end
.endstep
.np
Now, when all of the code below is compiled correctly, and the Visual
Basic program is run, the "Direct call" button will call the
.id DLL1
and
.id DLL2
functions directly, aliased as the functions
.id Add3
and
.id Add2
respectively.
The "Indirect call" button will call the 16-bit DLL, which will
then call the 32-bit DLL, for both
.id Function1
and
.id Function2.
To run the Visual Basic program, select "Start" from the "Run" menu.
.*
.section Sample Visual Basic DLL Programs
.*
.np
.ix 'Visual Basic' '16-bit DLL'
.ix 'Visual Basic' '32-bit DLL'
The sample programs provided below are for a 32-bit DLL, and a 16-bit
cover DLL, which will call the two functions contained in the 32-bit
DLL.
.*
.beglevel
.*
.section Source Code for VBDLL32.DLL
.*
.ix 'Visual Basic' '32-bit DLL'
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
/*
 *  VBDLL32.C
 */
#include <stdio.h>
#include <windows.h>    /* required for all Windows applications */

.code break
long FAR PASCAL Add3( short var1, long varlong, short var2 )
{
  char buf[128];

  sprintf( buf, "Add3: var1=%d, varlong=%ld, var2=%d",
                var1, varlong, var2 );
  MessageBox( NULL, buf, "VBDLL32", MB_OK | MB_TASKMODAL );
  return( var1 + varlong + var2 );
}

.code break
long FAR PASCAL Add2( long varlong, short var2 )
{
  char buf[128];

  sprintf( buf, "Add2: varlong=%ld, var2=%d", varlong, var2 );
  MessageBox( NULL, buf, "VBDLL32", MB_OK | MB_TASKMODAL );
  return( varlong + var2 );
}

.code break
#pragma off (unreferenced);
int PASCAL WinMain(HANDLE hInstance, HANDLE x1, LPSTR lpCmdLine, int x2)
#pragma on (unreferenced);
{
  DefineDLLEntry( 1, (void *) Add3, DLL_WORD, DLL_DWORD, DLL_WORD,
                      DLL_ENDLIST );
  DefineDLLEntry( 2, (void *) Add2, DLL_DWORD, DLL_WORD, DLL_ENDLIST );
  return( 1 );
}
.code end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
*$include winapi.fi

* VBDLL32.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 vbdll32 -explicit -d2 -bd -l=win386
* Bind:             wbind vbdll32 -d -n

.code break
*$pragma aux (dll_function) Add3

        integer function Add3( w1, w2, w3 )
        integer w1, w2, w3

        include 'windows.fi'

        character*128 str

        write( str, '(16hDLL 1 arguments:, 3i10, a)' ) w1, w2, w3,
     &                                                 char(0)
        call MessageBox( NULL, str, 'F77 VBDLL32'c, MB_OK )
        Add3 = w1 + w2 + w3

        end

.code break
*$pragma aux (dll_function) Add2

        integer function Add2( w1, w2 )
        integer w1, w2

        include 'windows.fi'

        character*128 str

        write( str, '(16hDLL 2 arguments:, 2i10, a)' ) w1, w2, char(0)
        call MessageBox( NULL, str, 'F77 VBDLL32'c, MB_OK )
        Add2 = w1 + w2

        end

.code break
        integer*2 function FWinMain( hInstance,
     &                               hPrevInstance,
     &                               lpszCmdLine,
     &                               nCmdShow )
        integer*2 hInstance
        integer*2 hPrevInstance
        integer*4 lpszCmdLine
        integer*2 nCmdShow

        include 'windows.fi'

        external Add3, Add2
        integer rc

.code break
        rc = DefineDLLEntry( 1, Add3, DLL_DWORD, DLL_DWORD, DLL_DWORD,
     &                          DLL_ENDLIST )
        if( rc .ne. 0 )then
            FWinMain = 0
            return
        end if
.code break
        rc = DefineDLLEntry( 2, Add2, DLL_DWORD, DLL_DWORD,
     &                          DLL_ENDLIST )
        if( rc .ne. 0 )then
            FWinMain = 0
            return
        end if
.code break
        call MessageBox( NULL, '32-bit DLL started'c,
     &                   'F77 VBDLL32'c, MB_OK )
        FWinMain = 1

        end
.code end
.do end
.*
.section Source code for COVER16.DLL
.*
.np
.ix 'Visual Basic' '16-bit DLL'
The functions in this 16-bit DLL will call the functions in the 32-bit
DLL,
.fi VBDLL32.DLL,
shown above, with the appropriate
.id Win386LibEntry
call for each function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
/*
 *  COVER16.C
 */

#include <stdio.h>
#include <windows.h>    /* required for all Windows applications */

typedef long (FAR PASCAL *FPROC)();

FPROC DLL_1;
FPROC DLL_2;

.code break
long FAR PASCAL __export Function1( short var1,
                                    long var2,
                                    short var3 )
{
    return( (long) DLL_1( var1, var2, var3 ) );
}

.code break
long FAR PASCAL __export Function2( long var1, short var2 )
{
    return( (long) DLL_2( var1, var2 ) );
}

.code break
#pragma off (unreferenced);
BOOL FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                         WORD wHeapSize, LPSTR lpszCmdLine )
#pragma on (unreferenced);
{
    HANDLE hlib;

    /* Do our DLL initialization */
    hlib = LoadLibrary( "vbdll32.dll" );
    if( hlib < 32 ) {
        MessageBox( NULL,
                    "Make sure your PATH contains VBDLL32.DLL",
                    "COVER16", MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    DLL_1 = (FPROC) GetProcAddress( hlib, "DLL1" );
    DLL_2 = (FPROC) GetProcAddress( hlib, "DLL2" );
    return( TRUE );
}
.code end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
/*
 *  COVER16.C
 */

#include <stdio.h>
#include <windows.h>    /* required for all Windows applications */

typedef long (FAR PASCAL *FPROC)();

FPROC DLL_1;
FPROC DLL_2;

.code break
long FAR PASCAL __export Function1( long var1,
                                    long var2,
                                    long var3 )
{
    return( (long) DLL_1( var1, var2, var3 ) );
}

.code break
long FAR PASCAL __export Function2( long var1, long var2 )
{
    return( (long) DLL_2( var1, var2 ) );
}

.code break
#pragma off (unreferenced);
BOOL FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
                         WORD wHeapSize, LPSTR lpszCmdLine )
#pragma on (unreferenced);
{
    HANDLE hlib;

    /* Do our DLL initialization */
    hlib = LoadLibrary( "vbdll32.dll" );
    if( hlib < 32 ) {
        MessageBox( NULL,
                    "Make sure your PATH contains VBDLL32.DLL",
                    "COVER16", MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    DLL_1 = (FPROC) GetProcAddress( hlib, "DLL1" );
    DLL_2 = (FPROC) GetProcAddress( hlib, "DLL2" );
    return( TRUE );
}
.code end
.do end
.*
.endlevel
.*
.section Compiling and Linking the Examples
.*
.np
.ix 'Visual Basic' 'building examples'
To create the 32-bit DLL
.fi VBDLL32.DLL,
type the following at the command line (make sure that
.fi VBDLL32&cxt
is in your current directory):
.millust begin
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
wcl386 vbdll32 -bt=windows -bd -d2 -l=win386
wbind vbdll32 -d -n
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
set finclude=c:\watcom\src\fortran\win
wfl386 vbdll32 -explicit -bd -d2 -l=win386
wbind vbdll32 -d -n
.do end
.millust end
.np
To create the 16-bit DLL
.fi COVER16.DLL,
type the following at the command line (make sure that
.fi COVER16.C
are in your current directory):
.millust begin
wcl cover16 -mc -bt=windows -bd -zu -d2 -l=windows_dll
.millust end
.autonote Notes:
.if '&lang' eq 'FORTRAN 77' .do begin
.note
An object file is provided for
.fi COVER16.C
if you do not have access to the 16-bit &company C compiler.
In this case, the DLL can be generated from the object file using
the following command:
.millust begin
wfl cover16.obj -d2 -l=windows_dll
.millust end
.do end
.note
The "mc" option selects the compact memory model (small code, big
data). The code for 16-bit DLLs must be compiled with one of the big
data models.
.note
The "bd" option indicates that a DLL will be created from the object
files.
.note
The "bt" option selects the "windows" target. This option causes the C
or C++ compiler to generate Windows prologue/epilogue code sequences
which are required for Microsoft Windows applications. It also causes
the compiler to use the
.ev WINDOWS_INCLUDE
environment variable for header file searches.
It also causes the compiler to define the macro
.kwm __WINDOWS__
and, for the 32-bit C or C++ compiler only, the macro
.kwm __WINDOWS_386__
.ct .li .
.note
The "zu" option is used when compiling 16-bit code that is to be placed
in a Dynamic Link Library (DLL) since the SS register points to the
stack segment of the calling application upon entry to the function.
.note
The "d2" option is used to disable optimizations and include debugging
information in the object file and DLL.
The techniques for debugging DLLs are described in the chapter
entitled :HDREF refid='wdll32'..
.endnote
.np
You are now ready to run the Visual Basic application.
