.chap Windows 3.x 32-bit Programming Overview
.*
.np
.ix 'Windows 3.x extender' 'programming notes'
This chapter includes the following topics:
.begbull
.if '&lang' eq 'FORTRAN 77' .do begin
.bull
WINAPI.FI and WINDOWS.FI
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
WINDOWS.H
.do end
.bull
Environment Notes
.bull
Floating-point Emulation
.bull
Multiple Instances
.bull
Pointer Handling
.bull
When To Convert Incoming Pointers
.bull
When To Convert Outgoing Pointers
.bull
SendMessage and SendDlgItemMessage
.bull
GlobalAlloc and LocalAlloc
.bull
Callback Function Pointers
.bull
Window Sub-classing
.bull
Calling 16-bit DLLs
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
Making DLL Calls Transparent
.bull
Far Pointer Manipulation
.do end
.bull
_16 Functions
.endbull
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.section WINAPI.FI
.*
.np
.ix 'WINAPI.FI'
.ix 'WINDOWS.FI'
When developing programs, make sure
.fi WINAPI.FI
is included at the start of all source files and the necessary include
files (particularly
.fi WINDOWS.FI
.ct )
are included in each function or subroutine.
.np
.ix 'Windows API'
It is especially important to get the correct function and argument
typing information for Windows API functions.
.ix 'default type'
Due to the default typing rules of FORTRAN, many Windows API functions
have a default result type of REAL when they may in fact return an
INTEGER or INTEGER*2 result.
By including the appropriate include files, you ensure that this never
happens.
.ix 'WINDLG.FI'
For example, the function
.id CreateDialog
is described in
.fi WINDLG.FI.
as a function returning an INTEGER*2 result.
.exam begin
      external CreateDialog
      integer*2 CreateDialog
.exam end
.np
Failure to specify the correct type of a function will result in code
that looks correct but does not execute correctly.
Similarly, you should make sure that all symbolic constants are
properly defined by including the appropriate include files.
.ix 'WINFONT.FI'
For example, the constant
.id DEFAULT_QUALITY
is described in
.fi WINFONT.FI
as an INTEGER constant whose value is 0.
.exam begin
      integer DEFAULT_QUALITY
      parameter ( DEFAULT_QUALITY = 0 )
.exam end
.np
Without this information,
.id DEFAULT_QUALITY
would be assumed to be a REAL variable and would not have any assigned
value.
.np
.ix 'EXPLICIT option'
The "EXPLICIT" compiler option is useful in this regard.
It requires that all symbols be explicitly typed.
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section WINDOWS.H
.*
.np
.ix 'WINDOWS.H'
When developing programs, make sure
.fi WINDOWS.H
is included as the first include file in all source files.
This header file contains only the following lines:
.millust begin
#ifdef _WINDOWS_16_
#include <win16.h>
#else
#include <_win386.h>
#endif
.millust end
.np
.ix 'WIN16.H'
The file
.fi WIN16.H
is the regular 16-bit Windows header file, and is only conditionally
included for 16-bit Windows applications.
.ix '_WIN386.H'
The file
.fi _WIN386.H
contains all the prototypes and macros for the 32-bit environment, as
well as including and modifying
.fi WIN16.H.
These modifications are changing int to short, and changing the
.kw far
keyword to nothing.
These changes (that ONLY apply to things defined in
.fi WIN16.H
.ct )
cause all integers to be 16-bit integers, and all LP... pointer types
to be near pointers.
.np
.ix 'WINDOWS.H'
Other include files for Windows must be specifically requested by
defining macros before including
.fi WINDOWS.H.
This is required so that the same changes made to the primary Windows
header file will apply to routines declared
in the other header files.
.np
.begnote $compact $setptnt 30
:DTHD.Macro name
:DDHD.File included
.note #define INCLUDE_COMMDLG_H
.ix 'COMMDLG.H'
.fi COMMDLG.H
.note #define INCLUDE_CUSTCNTL_H
.ix 'CUSTCNTL.H'
.fi CUSTCNTL.H
.note #define INCLUDE_DDE_H
.ix 'DDE.H'
.fi DDE.H
.note #define INCLUDE_DDEML_H
.ix 'DDEML.H'
.fi DDEML.H
.note #define INCLUDE_DRIVINIT_H
.ix 'DRIVINIT.H'
.fi DRIVINIT.H
.note #define INCLUDE_LZEXPAND_H
.ix 'LZEXPAND.H'
.fi LZEXPAND.H
.note #define INCLUDE_MMSYSTEM_H
.ix 'MMSYSTEM.H'
.fi MMSYSTEM.H
.note #define INCLUDE_OLE_H
.ix 'OLE.H'
.fi OLE.H
.note #define INCLUDE_PENWIN_H
.ix 'PENWIN.H'
.fi PENWIN.H
.note #define INCLUDE_PENWOEM_H
.ix 'PENWOEM.H'
.fi PENWOEM.H
.note #define INCLUDE_PRINT_H
.ix 'PRINT.H'
.fi PRINT.H
.note #define INCLUDE_SHELLAPI_H
.ix 'SHELLAPI.H'
.fi SHELLAPI.H
.note #define INCLUDE_STRESS_H
.ix 'STRESS.H'
.fi STRESS.H
.note #define INCLUDE_TOOLHELP_H
.ix 'TOOLHELP.H'
.fi TOOLHELP.H
.note #define INCLUDE_VER_H
.ix 'VER.H'
.fi VER.H
.endnote
.do end
.*
.section Environment Notes
.*
.begbull
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
.ix 'Windows API functions' 'Catch'
.ix 'Windows API functions' 'Throw'
The Windows functions
.kw Catch
and
.kw Throw
save only the 16-bit state.
Instead of these functions, use the
.kw setjmp
and
.kw longjmp
functions.
.do end
.bull
The 32-bit Windows Supervisor uses the first 256 bytes of the 32-bit
application's stack to save state information.
If this is corrupted, your application will abnormally terminate.
.bull
The 32-bit Windows Supervisor provides resources for up to 512 callback
routines.
Note that this restriction is only on the maximum number of active callbacks.
.endbull
.*
.section Floating-point Emulation
.*
.ix 'Windows 3.x extender' 'floating-point'
.np
.ix 'WEMU387.386'
The file
.fi WEMU387.386
is included to support floating-point emulation for 32-bit applications
running under Windows.
.ix '386enh'
.ix 'SYSTEM.INI'
This file is installed in the
.mono [386Enh]
section of your
.fi SYSTEM.INI
file.
By using the floating-point emulator, your application can be compiled
with the "fpi87" option to use inline floating-point instructions,
and it will run on a machine without a numeric coprocessor.
.np
.ix 'WDEBUG.386'
Only one of
.fi WEMU387.386
and
.fi WDEBUG.386
may be installed in your
.mono [386Enh]
section.
.ix 'distribution rights'
.fi WEMU387.386
may be distributed with your application.
.*
.section Multiple Instances
.*
.np
.ix 'Windows 3.x extender' 'multiple instances'
Since the 32-bit application resides in a flat memory space, it is NOT
possible to share code with other instances.
This means that you must register new window classes with callbacks
into the new instance's code space.
A simple way of accomplishing this is as follows:
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
      integer*2 function FWINMAIN( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )

      integer*2 hInstance
      integer*2 hPrevInstance
      integer*2 nCmdShow
      integer*4 lpszCmdLine
~b
.code break
      include 'win386.fi'
      include 'wincreat.fi'
      include 'wincurs.fi'
      include 'windefn.fi'
      include 'windisp.fi'
      include 'winmsg.fi'
      include 'winmsgs.fi'
      include 'windtool.fi'
      include 'winutil.fi'
~b
.code break
      external WndProc

      integer*2               hWnd
      record /MSG/            msg
      record /WNDCLASS/       wndclass
      character*14            class
~b
.code break
      wndclass.style = CS_HREDRAW .or. CS_VREDRAW
      wndclass.lpfnWndProc = loc( WndProc )
      wndclass.cbClsExtra = 0
      wndclass.cbWndExtra = 0
      wndclass.hInstance = hInstance
      wndclass.hIcon = NULL_HANDLE
      wndclass.hCursor = LoadCursor( NULL_HANDLE, IDC_ARROW )
      wndclass.hbrBackground = GetStockObject( WHITE_BRUSH )
      wndclass.lpszMenuName = NULL
      write( class, '(''Ellipses'',i5.5,a)' ) hInstance, char(0)
      wndclass.lpszClassName = Loc( class )
      call RegisterClass( wndclass )
~b
.code break
      hWnd = CreateWindow( class,
     &                     'Application'c,
     &                     WS_OVERLAPPEDWINDOW,
     &                     CW_USEDEFAULT,
     &                     0,
     &                     CW_USEDEFAULT,
     &                     0,
     &                     NULL_HANDLE,
     &                     NULL_HANDLE,
     &                     hInstance,
     &                     NULL )
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
int PASCAL WinMain( HANDLE hInstance,
                    HANDLE hPrevInstance;
                    LPSTR lpCmdLine,
                    int nCmdShow );
{
    WNDCLASS  wc;
    HWND      hWnd
    char      class[32];
.millust break

    wc.style = NULL;
    wc.lpfnWndProc = (LPVOID) MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "Menu";
    sprintf( class,"Class%d",hInstance );
    wc.lpszClassName = class;
    RegisterClass( &wc );
.millust break

    hWnd = CreateWindow(
        class,
        "Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
.millust end
.do end
.pc
The variable
.kw class
contains a unique name based on the instance of the application.
.*
.section Pointer Handling
.*
.np
.ix 'Windows 3.x extender' 'pointer handling'
Windows 3.x is a 16-bit operating system.
Function pointers that Windows deals with are 16-bit far pointers,
and any data you communicate to Windows with are 16-bit far pointers.
16-bit far pointers occupy 4 bytes of data, and are capable of
addressing up to 64K.
For data objects larger than 64K, huge pointers are used (a sequence of far
pointers that map out consecutive 64K segments for the data object).
16-bit far pointers are expensive to use due to the overhead of selector
loads (each time you use the pointer, a segment register must have
a value put in it).
16-bit huge pointers are even more expensive: not only is there the
overhead of selector loads, but a run-time call is necessary to perform
any pointer arithmetic.
.np
In a 32-bit flat memory model, such as that of the &cmpname for Windows
environment, all pointers are 32-bit near pointers (occupying 4 bytes of
data as well).
However, these pointers may access objects of up to 4 gigabytes in
size, and there is no selector load overhead.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
All Windows defined pointer types (e.g., LPSTR) are by default near
pointers, not far pointers.
To obtain a far pointer, the far keyword must be explicitly coded,
i.e.,
.mono char far *foo
.ct , rather than
.mono LPSTR foo.
A 32-bit near pointer is the same size as a 16-bit far
pointer, so that all Windows pointers are the same size in the
32-bit flat memory model as they are in the original 16-bit segmented
model.
.do end
.np
For a 32-bit environment to communicate with Windows 3.x, there are
some considerations.
All pointers sent to Windows must be converted from 32-bit near
pointers to 16-bit far pointers.
These conversions are handled by the Supervisor.
.np
It is important to remember that all API functions which accept
pointers (with the exception of functions that accept function
pointers) accept 32-bit near pointers in this 32-bit model.
If you attempt to pass a 32-bit far pointer, the conversion will
not take place correctly.
.np
16-bit far pointers to data may be passed into the API functions, and
the Supervisor will not do any conversion.
.np
Incoming pointers must be converted from 16-bit far pointers to 32-bit
far pointers.
This conversion is a trivial one: the offset portion of the 16-bit far
pointer is extended to 32-bits.
.if '&lang' eq 'FORTRAN 77' .do begin
The pointer conversion will occur automatically when you map a
dynamically allocatable array to the memory pointed to by the 16-bit
pointer using the
.kw LOCATION=
specifier of the
.kw ALLOCATE
statement.
You must also declare the array as
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
Pointers from Windows are by their nature far (that is, the data is
pointed to by its own selector), and must be used as far in the 32-bit
environment.
Of course, conversions are only required if you actually need to
reference the pointer.
.np
Function pointers (i.e., pointers to callback routines) used by
Windows are not converted from 32-bit to 16-bit.
Rather, a 16-bit thunking layer that transfers control from the 16-bit
environment to the 32-bit environment must be used.
This thunking layer is provided by the Supervisor.
.*
.beglevel
.*
.section When To Convert Incoming Pointers
.*
.np
.ix 'Windows 3.x extender' 'pointer conversion'
.if '&lang' eq 'FORTRAN 77' .do begin
Whenever you wish to use a pointer passed to you by Windows, you must
map a dynamically allocatable array to the memory pointed to by the
pointer using the
.kw LOCATION
specifier of the
.kw ALLOCATE
statement.
You must also declare the array as
.kw far
using the
.kw array
pragma.
The pointer conversion will occur automatically.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
Whenever you wish to use a pointer passed to you by Windows, you must
convert it to a 32-bit far pointer.
If you are passed a 16-bit far pointer, the macro
.kw MK_FP32
can be used to convert it to a 32-bit far pointer.
If you are passed a 16-bit near pointer (e.g., from
.kw LocalLock
.ct ), then the macro
.kw MK_LOCAL32
can be used to convert it to a 32-bit far pointer.
.do end
.np
Some places where pointer conversion may be required are:
.* .ix 'LocalLock pointer conversion'
.* .ix 'GlobalLock pointer conversion'
.* .ix 'lParam pointer conversion'
.* .ix 'pointer conversion' 'LocalLock'
.* .ix 'pointer conversion' 'GlobalLock'
.* .ix 'pointer conversion' 'lParam'
.begbull $compact
.bull
LocalLock
.bull
GlobalLock
.bull
the lParam in a window callback routine (if it is a pointer)
.endbull
.*
.section When To Convert Outgoing Pointers
.*
.np
.ix 'Windows 3.x extender' 'pointer conversion'
Typically, there is no need to do any kind of conversions on your
pointers when passing them to Windows.
The Supervisor handles all 32-bit to 16-bit translations for you, in
the case of the regular Windows API functions.
However, if you are passing a 32-bit pointer to some other 16-bit
application in the Windows environment, then pointer conversions must
by done.
There are two types of "outgoing" pointers: data pointers and function
pointers.
.np
Function pointers (to callback routines) must have a thunking layer
provided, using the
.kw GetProc16
function (this is explained in detail in a later section).
.np
Data pointers can be translated from 32-bit to 16-bit using the
.kw AllocAlias16
and
.kw AllocHugeAlias16
functions.
These functions create 16-bit far pointers that have the same linear
address as the 32-bit near pointer that was converted.
.np
It is important to remember that when passing a pointer to a data
structure in this fashion, any pointers in the data structure must
also be converted.
.np
The Supervisor will convert any pointers that it knows about; but
there are some complications created by the fact that Windows allows
you to pass pointers in functions that are prototyped to take a long
integer.
.np
The Windows API functions
.kw SendMessage
and
.kw SendDlgItemMessage
rely on other fields determining the nature of the long data item that
they accept; this is discussed in detail in the next section.
.beglevel
.*
.section SendMessage and SendDlgItemMessage
.*
.np
.kw SendMessage
and
.kw SendDlgItemMessage
have special cover functions that determine when the
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
long
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
32-bit
.do end
integer is really a pointer and needs to be converted.
These cover functions are used automatically, unless the macro
.kw NOCOVERSENDS
is defined before including
.if '&lang' eq 'FORTRAN 77' .do begin
.ix 'WINAPI.FI'
.fi WINAPI.FI
as in the following example.
.millust begin
*$define NOCOVERSENDS
*$include winapi.fi
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.ix 'WINDOWS.H'
.fi WINDOWS.H
as in the following example.
.millust begin
#define NOCOVERSENDS
#include <windows.h>
.millust end
.do end
.np
.kw SendMessage
and
.kw SendDlgItemMessage
will do pointer conversions automatically using
.kw AllocAlias16
and
.kw FreeAlias16
(unless
.kw NOCOVERSENDS
is defined) for the following message types:
.begbull $compact
.bull
combo boxes (CB_ messages)
.bull
edit controls (EM_ messages)
.bull
list boxes (LB_ messages)
.bull
certain windows messages (WM_ messages);
.endbull
.np
The messages that are intercepted by the cover functions for
.kw SendMessage
and
.kw SendDlgItemMessage
are:
.millust begin
CB_ADDSTRING       CB_DIR             CB_FINDSTRING
CB_FINDSTRINGEXACT CB_GETLBTEXT       CB_INSERTSTRING
CB_SELECTSTRING

EM_GETLINE         EM_GETRECT         EM_REPLACESEL
EM_SETRECT         EM_SETRECTNP       EM_SETTABSTOPS

LB_ADDSTRING       LB_DIR             LB_FINDSTRING
LB_FINDSTRINGEXACT LB_GETITEMRECT     LB_GETSELITEMS
LB_GETTEXT         LB_INSERTSTRING    LB_SELECTSTRING
LB_SETTABSTOPS

WM_MDICREATE       WM_NCCALCSIZE
.millust end
.np
Note that for
.kw SendMessage
and
.kw SendDlgItemMessage
.ct , some of the messages may NOT require pointer conversion:
.begbull
.bull
CB_ADDSTRING, CB_FINDSTRING, CB_FINDSTRINGEXACT, CB_INSERTSTRING
will not need a conversion if the combo box was created
as owner-draw style without CBS_HASSTRINGS style.
.bull
LB_ADDSTRING, LB_FINDSTRING, LB_FINDSTRINGEXACT, LB_INSERTSTRING
will not need a conversion if the list box was created
as owner-draw style without LBS_HASSTRINGS style.
.endbull
.np
The macro
.kw NOCOVERSENDS
should be defined in modules where messages like these are being sent.
With these messages, the lParam data item does not contain a pointer,
and the automatic pointer conversion would be incorrect.
By doing
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
*$define NOCOVERSENDS
*$include winapi.fi
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#define NOCOVERSENDS
#include "windows.h"
.millust end
.do end
.pc
modules that send messages like the above will not have the pointer
conversion performed.
.endlevel
.*
.section GlobalAlloc and LocalAlloc
.*
.np
The functions
.kw GlobalAlloc
and
.kw LocalAlloc
are the typical way of allocating memory in the 16-bit Windows
environment.
In the 32-bit environment, there is no need to use these functions.
The only time
.kw GlobalAlloc
is needed is when allocating shared memory, i.e.,
.kw GMEM_DDESHARE
.ct .li .
.if '&lang' eq 'FORTRAN 77' .do begin
.np
The
.kw ALLOCATE
and
.kw DEALLOCATE
statements can be used to allocate memory from
your 32-bit near heap.
By allocating memory in this way, you may create data objects as large
as the enhanced mode Windows memory manager will permit.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
The C runtime functions
.kw malloc
and
.kw free
manipulate your 32-bit near heap for you.
By using these functions to allocate memory, you may create data
objects as large as the enhanced mode Windows memory manager will
permit.
.do end
.*
.section Callback Function Pointers
.*
.np
To access a callback function, an instance of it must be created using
.kw MakeProcInstance
.ct .li .
This creates a "thunk" (a special piece of code) that automatically
puts the application's data segment into the AX register, and then
calls the specified callback function.
.np
In Windows 3.x, it is not possible to do a
.kw MakeProcInstance
directly on a 32-bit callback routine, since Windows 3.x does not understand
32-bit applications.
Therefore, it is necessary to use a 16-bit callback routine that
passes control to the 32-bit callback routine.
This 16-bit callback routine is automatically created by the Supervisor
when using any of the standard Windows API functions that accept a
callback routine.
.np
The 16-bit callback routine for a 32-bit application is a special
layer that transfers the parameters from a 16-bit stack to the 32-bit
stack, and then passes control to 32-bit code.
These 16-bit callback routines are found in the Supervisor.
The function
.kw GetProc16
provides pointers to these 16-bit callback routines.
.np
However, it is not often necessary to use the
.kw GetProc16
function to obtain a 16-bit/32-bit callback interface function.
.np
In the general case, one would have to write code as follows:
.if '&lang' eq 'FORTRAN 77' .do begin
.millust begin
      integer*4 pCb, fpProc

      pCb = GetProc16( A_Function, GETPROC_callbacktype )
      fpProc = MakeProcInstance( pCb, hInstance )

*     do stuff

      call Do_it( ..., fpProc, ... )

*     do more stuff

      call FreeProcInstance( fpProc )
      call ReleaseProc16( pCb )
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#define NOAUTOPROCS
#include <windows.h>

CALLBACKPTR     pCb;
FARPROC         fpProc;

pCb = GetProc16( A_Function, GETPROC_callbacktype );
fpProc = MakeProcInstance( pCb, hInstance );

/* do stuff */

Do_it( ..., fpProc, ... );

/* do more stuff */

FreeProcInstance( fpProc );
ReleaseProc16( pCb );
.millust end
.do end
.np
It is not necessary to use this general code in the case of the
regular Windows API functions.
The following functions will automatically allocate the correct
16-bit/32-bit callback interface functions:
.begbull $compact
.bull
ChooseColor
.bull
ChooseFont
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
CorrectWriting
.do end
.bull
CreateDialog
.bull
CreateDialogIndirect
.bull
CreateDialogIndirectParam
.bull
CreateDialogParam
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
DdeInitialize
.do end
.bull
DialogBox
.bull
DialogBoxIndirect
.bull
DialogBoxIndirectParam
.bull
DialogBoxParam
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
DictionarySearch
.do end
.bull
EnumChildWindows
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
EnumFontFamilies
.do end
.bull
EnumFonts
.bull
EnumMetaFile
.bull
EnumObjects
.bull
EnumProps
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
EnumSymbols
.do end
.bull
EnumTaskWindows
.bull
EnumWindows
.bull
Escape (SETABORTPROC option)
.bull
FindText
.bull
GetOpenFileName
.bull
GetSaveFileName
.bull
GlobalNotify
.bull
GrayString
.bull
LineDDA
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
mciSetYieldProc
.bull
mmioInstallIOProc
.bull
NotifyRegister
.do end
.bull
PrintDlg
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
ProcessWriting
.bull
Recognize
.bull
RecognizeData
.do end
.bull
RegisterClass
.bull
ReplaceText
.bull
SetClassLong (GCL_WNDPROC option)
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
SetPenHook
.do end
.bull
SetResourceHandler
.bull
SetTimer
.bull
SetWindowLong (GWL_WNDPROC option)
.bull
SetWindowsHook
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
SetWindowsHookEx
.bull
TrainInk
.do end
.endbull
.np
As well, the following functions are covered to provide support
for automatic creation of 16-bit callback routines:
.begbull $compact
.bull
FreeProcInstance
.bull
MakeProcInstance
.bull
UnhookWindowsHook
.endbull
.np
If you need to get a callback that is not used by one of the above
functions, then you must code the general case.
Typically, this is required when a DLL needs a callback routine.
In modules where this is necessary, you define the macro
.kw NOAUTOPROCS
before you include
.if '&lang' eq 'FORTRAN 77' .do begin
.fi WINAPI.FI
as in the following example.
.millust begin
*$define NOAUTOPROCS
*$include winapi.fi
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.fi WINDOWS.H
as in the following example.
.millust begin
#define NOAUTOPROCS
#include <windows.h>
.millust end
.do end
.np
Be careful of the following when using
.kw NOAUTOPROCS
.ct .li .
.autonote
.note
The call to
.kw MakeProcInstance
and
.kw FreeProcInstance
for the callback function occurs in a module with
.kw NOAUTOPROCS
defined.
.note
No Windows API functions (listed above) are used in the module with
.kw NOAUTOPROCS
defined.
If they are, you must code the general case to use them.
.endnote
.np
Note that
.kw NOAUTOPROCS
is in effect on a module-to-module basis only.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
You can avoid using
.kw NOAUTOPROCS
on a call-by-call basis, if you do the following:
.millust begin
#undef <function>
<function>
Note: re-defining is only needed if you want to
      use a covered version of the function later on.
#define <function> _Cover_<function>
.millust end
.np
For example:
.millust begin
{
#undef SetWindowsHook
#undef MakeProcInstance

    FARPROC fp,oldfp;
    CALLBACKPTR cbp;

.millust break
    cbp = GetProc16( CallbackHook, GETPROC_CALLBACK );
    fp = MakeProcInstance( cbp, hInstance );
    oldfp = SetWindowsHook( WH_CALLWNDPROC, fp );

}
.millust end
.np
This allows you to add general case code in the same module, without having
to break the module into two parts.
.do end
.np
RegisterClass automatically does a GetProc16 for the callback function,
unless the macro NOCOVERRC is specified before including
.if '&lang' eq 'FORTRAN 77' .do begin
.fi WINAPI.FI
as in the following example.
.millust begin
*$define NOCOVERRC
*$include winapi.fi
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.fi WINDOWS.H
as in the following example.
.millust begin
#define NOCOVERRC
#include <windows.h>
.millust end
.do end
.beglevel
.*
.section Window Sub-classing
.*
.np
Sub-classing a Windows control in the 32-bit environment is
straightforward.
In fact, the code is identical to the code used in the 16-bit
environment.
A simple example is:
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
*$include winapi.fi
*$pragma aux (callback) SubClassProc parm( value, value, value, value )

      integer*4 function SubClassProc( hWnd, msg, wp, lp )
      integer*2 hWnd
      integer*2 msg
      integer*2 wp
      integer*4 lp
~b
.code break
      include 'windows.fi'

      common fpOldProc
      integer*4 fpOldProc

      ! code for sub-classing here

      SubClassProc = CallWindowProc( fpOldProc, hWnd, msg, wp, lp )

      end
~b
.code break
      program SubClassDemo
      integer*2 hControl
      common fpOldProc
      integer*4 fpOldProc
      integer*4 fp;

      include 'windows.fi'

      integer*4 SubClassProc
      external SubClassProc
      integer*4 ProgramInstance
      external ProgramInstance
~b
.code break
      ! assume hControl gets created in here

      fpOldProc = GetWindowLong( hControl, GWL_WNDPROC )
      fp = MakeProcInstance( SubClassProc, ProgramInstance )
      call SetWindowLong( hControl, GWL_WNDPROC, fp )

      ! set it back
      call SetWindowLong( hControl, GWL_WNDPROC, fpOldProc )
      call FreeProcInstance( fp )

      end
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.code begin
FARPROC fpOldProc;

long FAR PASCAL SubClassProc( HWND hWnd,
                              unsigned message,
                              WORD wParam,
                              LONG lParam )
{
    /*
     * code for sub-classing here
     */
    return( CallWindowProc( fpOldProc, hWnd, message,
                            wParam, lParam ) );
}
~b
.code break
void SubClassDemo( void )
{
    HWND                hControl;
    FARPROC             fp;
    extern HANDLE       ProgramInstance;

    /* assume hControl gets created in here */

    fpOldProc = (FARPROC) GetWindowLong( hControl, GWL_WNDPROC );
    fp = MakeProcInstance( SubClassProc, ProgramInstance );
    SetWindowLong( hControl, GWL_WNDPROC, (LONG) fp );

    /* set it back */
    SetWindowLong( hControl, GWL_WNDPROC, (LONG) fpOldProc );
    FreeProcInstance( fp );
}
.code end
.do end
.np
Note that
.kw SetWindowLong
is covered to recognize
.kw GWL_WNDPROC
and automatically
creates a 16-bit callback for the 32-bit callback.
When replacing the callback routine with the original 16-bit routine,
the covered version of
.kw SetWindowLong
recognizes that the function is not a 32-bit callback, and so passes
the pointer right through to Windows unchanged.
.endlevel
.endlevel
.*
.section Calling 16-bit DLLs
.*
.np
A 16-bit function in a DLL can be called using the
.kw _Call16
function.
The first argument to
.kw _Call16
is the address of the 16-bit function.
This address is usually obtained by calling
.kw GetProcAddress
with the name of the desired function.
The second argument to
.kw _Call16
is a string identifying the types of the parameters to be passed to
the 16-bit function.
.begnote $compact
:DTHD.Character
:DDHD.Parameter Type
.note c
call a 'cdecl' function as opposed to a 'pascal' function
(if specified, it must be listed first)
.note b
unsigned BYTE
.note w
16-bit WORD
.note d
32-bit DWORD
.note f
double precision floating-point
.note p
32-bit flat pointer (converted to 16:16 far pointer)
.endnote
.np
The 16-bit function must use either the
.kw PASCAL
or
.kw CDECL
calling convention.
.kw PASCAL
calling convention is the default.
If the function uses the
.kw CDECL
calling convention, then you must specify the letter "c" as the
first character of the argument type string.
.np
Pointer types will automatically be converted from 32-bit near
pointers to 16-bit far pointers before the function is invoked.
Note that this pointer is only valid over the period of the call;
after control returns to the 32-bit application, the 16-bit pointer
created by the Supervisor is no longer valid.
.np
The return value from
.kw _Call16
is a
.kw DWORD
.ct .li .
.np
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
*$include winapi.fi

      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      integer*2 hDrv, hWnd
      integer*4 lpfn, cb

      hDrv = LoadLibrary( 'your.dll'c )
      if( hDrv .lt. 32 )then
          return
      end if
.code break
      lpfn = GetProcAddress( hDrv, 'ExtDeviceMode'c )
      if( lpfn .eq. 0 )then
           return
      end if
.code break
      ! Invoke the function.
      cb = _Call16( lpfn, 'wwdppddw'c,
     &              hWnd, hDrv, NULL,
     &              'POSTSCRIPT PRINTER'c,
     &              'LPT1'c,
     &              NULL, NULL, 0 )
.code end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.millust begin
#include <windows.h>
HANDLE hDrv;
FARPROC lpfn;
int cb;
.millust break
    if( (hDrv = LoadLibrary ("foo.dll")) < 32 )
        return FALSE;
    if( !(lpfn = GetProcAddress (hDrv, "ExtDeviceMode")) )
        return FALSE;
.millust break
    /*
     * now, invoke the function
     */
    cb = (WORD) _Call16(
           lpfn,                // address of function
           "wwdppddw",          // parameter type info
           hwnd,                // parameters ...
           hDrv,
           NULL,
           "POSTSCRIPT PRINTER",
           "LPT1",
           NULL,
           NULL,
           0
            );
.millust end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.beglevel
.*
.section Making DLL Calls Transparent
.*
.np
This section gives an example of how to make your source code look
as if you are calling the 16-bit DLL directly.
.np
Assume there are 3 functions that you want to call in the 16-bit DLL,
with prototypes as follows:
.millust begin
HWND FAR PASCAL Initialize( WORD start_code );
BOOL FAR PASCAL DoStuff( HWND win_hld, HDC win_dc );
void FAR PASCAL Finish( void );
.millust end
.np
A fragment from the header file that you would include in your 32-bit
application would be as follows:
.millust begin
extern FARPROC InitializeAddr;
extern FARPROC DoStuffAddr;
extern FARPROC FinishAddr;
#define Initialize( start_code ) \
  _Call16( InitializeAddr, "w", (WORD)start_code )
#define DoStuff( win_hld, data ) \
  _Call16( DoStuffAddr, "wp", (HWND)win_hld, (LPVOID)data )
#define Finish( void )  _Call16( FinishAddr, "" )
.millust end
.np
The header file fragment gives external references for the
function addresses for each function, and sets up macros do a
.kw _Call16
for each of the functions.
.np
At start up, you would call the following function:
.millust begin
/*
 * LoadDLL - get DLL ready for 32-bit use
 */
BOOL LoadDLL( void )
{
    HANDLE  dll;

    dll = LoadLibrary( "chart.dll" );
    if( dll < 32 ) return( FALSE);
.millust break

    InitializeAddr = GetProcAddress( dll, "Initialize" );
    DoStuffAddr = GetProcAddress( dll, "DoStuff" );
    FinishAddr = GetProcAddress( dll, "Finish" );
    return( TRUE );
}
.millust end
.np
This function loads the 16-bit DLL and gets the addresses
for all of the entry points in the DLL.
By including the header file with all the macros in it, you can code
calls to the DLL functions as if you were calling the functions
directly.
For example:
.millust begin
#include <windows.h>
#include "fragment.h"
char *data = "the data";

void TestDLL( void )
{
    HWND res;
.millust break

    if( !LoadDLL() ) {
        MessageBox( NULL, "Could not load DLL",
                    "Error", MB_OK );
        return;
    }
.millust break

    res = Initialize( 1 );
    DoStuff( res, data );
    Finish();
}
.millust end
.endlevel
.*
.section Far Pointer Manipulation
.*
.np
The following C library functions are available for manipulating far
data.
These are useful when using pointers obtained by
.kw MK_FP32
and
.kw MK_LOCAL32
.ct .li .
.np
Memory manipulation:
.* .ix '_fmemccpy'
.* .ix '_fmemchr'
.* .ix '_fmemcmp'
.* .ix '_fmemcpy'
.* .ix '_fmemicmp'
.* .ix '_fmemmove'
.* .ix '_fmemset'
.* .ix 'far data' '_fmemccpy'
.* .ix 'far data' '_fmemchr'
.* .ix 'far data' '_fmemcmp'
.* .ix 'far data' '_fmemcpy'
.* .ix 'far data' '_fmemicmp'
.* .ix 'far data' '_fmemmove'
.* .ix 'far data' '_fmemset'
.begbull $compact
.bull
_fmemccpy
.bull
_fmemchr
.bull
_fmemcmp
.bull
_fmemcpy
.bull
_fmemicmp
.bull
_fmemmove
.bull
_fmemset
.endbull
.np
String manipulation:
.* .ix '_fstrcat'
.* .ix '_fstrchr'
.* .ix '_fstrcmp'
.* .ix '_fstrcpy'
.* .ix '_fstrcspn'
.* .ix '_fstricmp'
.* .ix '_fstrlen'
.* .ix '_fstrlwr'
.* .ix '_fstrncat'
.* .ix '_fstrncmp'
.* .ix '_fstrncpy'
.* .ix '_fstrnicmp'
.* .ix '_fstrnset'
.* .ix '_fstrpbrk'
.* .ix '_fstrrchr'
.* .ix '_fstrrev'
.* .ix '_fstrset'
.* .ix '_fstrspn'
.* .ix '_fstrtok'
.* .ix '_fstrupr'
.* .ix 'far data' '_fstrcat'
.* .ix 'far data' '_fstrchr'
.* .ix 'far data' '_fstrcmp'
.* .ix 'far data' '_fstrcpy'
.* .ix 'far data' '_fstrcspn'
.* .ix 'far data' '_fstricmp'
.* .ix 'far data' '_fstrlen'
.* .ix 'far data' '_fstrlwr'
.* .ix 'far data' '_fstrncat'
.* .ix 'far data' '_fstrncmp'
.* .ix 'far data' '_fstrncpy'
.* .ix 'far data' '_fstrnicmp'
.* .ix 'far data' '_fstrnset'
.* .ix 'far data' '_fstrpbrk'
.* .ix 'far data' '_fstrrchr'
.* .ix 'far data' '_fstrrev'
.* .ix 'far data' '_fstrset'
.* .ix 'far data' '_fstrspn'
.* .ix 'far data' '_fstrtok'
.* .ix 'far data' '_fstrupr'
.begbull $compact
.bull
_fstrcat
.bull
_fstrchr
.bull
_fstrcmp
.bull
_fstrcpy
.bull
_fstrcspn
.bull
_fstricmp
.bull
_fstrlen
.bull
_fstrlwr
.bull
_fstrncat
.bull
_fstrncmp
.bull
_fstrncpy
.bull
_fstrnicmp
.bull
_fstrnset
.bull
_fstrpbrk
.bull
_fstrrchr
.bull
_fstrrev
.bull
_fstrset
.bull
_fstrspn
.bull
_fstrtok
.bull
_fstrupr
.endbull
.do end
.*
.section _16 Functions
.*
.np
Every Windows API function that accepts a pointer has a corresponding
_16 function.
The _16 version of the function will not convert any of the pointers
that it accepts; it will assume that all pointers are 16-bit far
pointers already.
This applies to both data and function pointers.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
Some sample code demonstrating the use for this is:
.millust begin
void ReadEditBuffer( char *fname, HWND hwndEdit )
{
    int       file;
    HANDLE    hText;
    char far *flpData;
    LPSTR     lpData;
    WORD      filelen;
.millust break
    /*
     * no error checking is performed; we just
     * assume everything works for this example.
     */
    file = _lopen( fname, 0);
    filelen = _llseek( file, 0L, 2 );

.millust break
    hText = (HANDLE) SendMessage( hwndEdit, EM_GETHANDLE,
                                  0, 0L );
    LocalReAlloc( hText, filelen+1, LHND );
    flpData = MK_LOCAL32( LocalLock( hText ) );
    lpData = (LPSTR) MK_FP16( flpB );
    _16_lread( file, lpData, filelen );
    _lclose( file );

}
.millust end
.np
This example reads the contents of a file into the buffer of an edit
window.
Because the edit window's memory is located in the local heap, which
is the Supervisor's heap, the
.kw MK_LOCAL32
function is needed to access the data.
The
.kw MK_FP16
macro compresses the 32-bit far pointer into a 16-bit far pointer,
which can then be used by the _16_lread function.
.do end
