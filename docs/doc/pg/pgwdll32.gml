.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="winmain"    value="FWinMain".
:set symbol="f16"        value="wfc".
:set symbol="srcdir"     value="&pathnamup.\SAMPLES\FORTRAN\WIN\DLL".
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="winmain"    value="WinMain".
:set symbol="c16"        value="wcc".
:set symbol="c32"        value="wcc386".
:set symbol="srcdir"     value="&pathnamup.\SAMPLES\DLL".
.do end
.*
.chap *refid=wdll32 Windows 32-Bit Dynamic Link Libraries
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction to 32-Bit DLLs
.do end
.*
.np
.ix 'dynamic link library'
.ix '32-bit DLL'
.ix 'DLL' '32-bit'
&product allows the creation of 32-bit Dynamic Link Libraries (DLL).
In fact, 32-bit DLLs are simpler to write than 16-bit DLLs.
A 16-bit DLL runs on the caller's stack, and thus DS != SS.
This creates difficulties in the small and medium memory models
because near pointers to local variables are different from near
pointers to global variables.
The 32-bit DLL runs on its own stack, in the usual flat memory space,
which eliminates these concerns.
.np
There is a special version of the supervisor,
.fi W386DLL.EXT
that performs a similar job to
.fi WIN386.EXT.
However, the 32-bit DLL supervisor is a 16-bit Windows DLL, rather
than a 16-bit Windows application.
.ix '&winmain.'
On the first use of the 32-bit DLL, the DLL supervisor loads the
32-bit DLL and invokes the 32-bit initialization routine (the DLL's
.id &winmain.
routine).
The initialization routine declares all entry points (via
.id DefineDLLEntry
.ct )
and performs any other necessary initialization.
An index number in the range 1 to 128 is used to identify all external
32-bit DLL routines.
.id DefineDLLEntry
is used to assign an index number to each routine, as well as to
identify the arguments.
.np
The DLL supervisor contains a general entry point for Windows
applications to call into called
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
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
These entry points are
.id FAR PASCAL
functions.
.do end
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
From a pseudo-code point of view, the 16-bit supervisor might look
like the following:
.millust begin
DLL1::  set index=1
        invoke 32bitDLLindirect

DLL2::  set index=2
        invoke 32bitDLLindirect
            .
            .
            .
DLL128:: set index=128
         invoke 32bitDLLindirect

Win386LibEntry::
        set index from index_argument
        invoke 32bitDLLindirect

32bitDLLindirect:
        set up stack frame
        switch to 32-bit data space
        call indirect registration_list[ index ]
            .
            .
            .
.millust end
.np
When you are creating a 32-bit DLL, keep in mind that the entry points
you define may be invoked by a 16-bit application as well as a 32-bit
application.
It is for this reason that all far pointers passed to a 32-bit DLL are
16-bit far pointers.
Hence, whenever a pointer is passed as an argument to a 32-bit DLL
entry point and you wish to access the data it points to, you must
convert the pointer appropriately.
.if '&lang' eq 'FORTRAN 77' .do begin
To do this, you must map a dynamically allocatable array to the memory
pointed to by the 16-bit far pointer.
.do end
.*
.section A Sample 32-bit DLL
.*
.np
.ix 'DLL' '32-bit Windows example'
Let us begin our discussion of DLLs by showing the code for a simple
DLL.
The source code for these examples is provided in the
.fi &srcdir.
directory.
We describe how to compile and link the examples in the section
entitled :HDREF refid='dlcreat'..
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
*$include winapi.fi

* WINDLLV.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 windllv -explicit -d2 -bd -l=win386
* Bind:             wbind windllv -d -n

.code break
*$pragma aux (dll_function) Add3

      integer function Add3( w1, w2, w3 )
      integer*4 w1, w2, w3

      include 'windows.fi'

      character*128 str

      write( str, '(16hDLL 1 arguments:, 3i10, a)' ) w1, w2, w3,
     &                                               char(0)
      call MessageBox( NULL, str, 'DLL Function 1'c, MB_OK )
      Add3 = w1 + w2 + w3

      end

.code break
*$pragma aux (dll_function) Add2

      integer function Add2( w1, w2 )
      integer*4 w1, w2

      include 'windows.fi'

      character*128 str

      write( str, '(16hDLL 2 arguments:, 2i10, a)' ) w1, w2, char(0)
      call MessageBox( NULL, str, 'DLL Function 2'c, MB_OK )
      Add2 = w1 + w2

      end

.code break
      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      external Add3, Add2
      integer rc

.code break
      call BreakPoint
      rc = DefineDLLEntry( 1, Add3, DLL_DWORD, DLL_DWORD, DLL_DWORD,
     &                        DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
.code break
      rc = DefineDLLEntry( 2, Add2, DLL_DWORD, DLL_DWORD,
     &                        DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
      call MessageBox( NULL, '32-bit DLL started'c,
     &                 'WINDLLV'c, MB_OK )
      FWinMain = 1

      end
.code end
.np
There are two entry points defined,
.id Add3
(index number 1) and
.id Add2
(index number 2).
.id Add3
has three INTEGER*4 arguments and
.id Add2
has two INTEGER*4 arguments.
The argument lists are described by calling
.id DefineDLLEntry.
All arguments are passed by value.
As previously mentioned, all pointers passed to 32-bit DLLs are 16-bit
far pointers.
Since, by default, FORTRAN 77 passes arguments by reference (a pointer
to the data is passed instead of the actual data),
a level of complexity is introduced since some pointer conversions
must take place when accessing the data pointed to by a 16-bit far
pointer in a 32-bit environment.
We will deal with this problem in a following example.
First, let us deal with passing arguments by value to 32-bit DLLs from
16 and 32-bit Windows applications.
.np
Note that each entry name must be given the
.id dll_function
attribute using an auxiliary pragma.
This alias name is defined in the file
.fi WINAPI.FI.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The code for this DLL can be compiled with the 16-bit compiler to
produce a 16-bit DLL and it can can be compiled with the 32-bit
compiler to produce a 32-bit DLL.
The example illustrates the fundamental differences between the two
types of DLLs.
The 32-bit DLL has a
.id WinMain
routine and the 16-bit DLL has a
.id LibMain
routine.
.exam begin
/*
 *  DLL.C
 */
#include <stdio.h>
#include <windows.h>

#if defined(__386__)/* if we are doing a 32-bit DLL */
  #define DLL_ID "DLL32"
#else               /* else we are doing a 16-bit DLL */
  #define DLL_ID "DLL16"
#endif
.exam break
long FAR PASCAL __export FooMe1(WORD w1, DWORD w2, WORD w3)
{
  char buff[128];

  sprintf( buff, "FooMe1: w1=%hx, w2=%lx, w3=%hx",
           w1, w2, w3 );
  MessageBox( NULL, buff, DLL_ID, MB_OK );
  return( w1 + w2 );
}
.exam break
long FAR PASCAL __export FooMe2( DWORD w1, WORD w2 )
{
  char buff[128];

  sprintf( buff, "FooMe2: w1=%lx, w2=%hx", w1, w2 );
  MessageBox( NULL, buff, DLL_ID, MB_OK );
  return( w1 + 1 );
}
.exam break
#if defined(__386__)/* if we are doing a 32-bit DLL */
long PASCAL WinMain( HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR  lpszCmdLine,
                     int    nCmdShow )
{
  if( DefineDLLEntry( 1, (void *) FooMe1, DLL_WORD,
              DLL_DWORD, DLL_WORD, DLL_ENDLIST )) {
      return( 0 );
  }
  if( DefineDLLEntry( 2, (void *) FooMe2, DLL_DWORD,
              DLL_WORD, DLL_ENDLIST ) ) {
      return( 0 );
  }
  MessageBox( NULL, "32-bit DLL Started", DLL_ID, MB_OK );
  return( 1 );
}
#else               /* else we are doing a 16-bit DLL */
BOOL FAR PASCAL LibMain( HANDLE hInstance,
                         WORD wDataSegment,
                         WORD wHeapSize,
                         LPSTR lpszCmdLine )
{
  #if 0
  /*
    We can't use MessageBox here since static binding is
    used and a message queue has not been created by the
    time DLL16 is loaded.
  */
  MessageBox( NULL, "16-bit DLL Started", DLL_ID, MB_OK );
  #endif
  return( TRUE );
}
#endif
.exam end
.np
To create a 16-bit DLL from this code, the following steps must be
performed.
.exam begin
C>&c16. dll /mc /bt=windows /zu /fo=dll16
C>&lnkcmd. system windows_dll file dll16
C>wlib -n dll16 +dll16.dll
.exam end
.np
To create a 32-bit DLL from this code, the following steps must be
performed.
.exam begin
C>&c32. dll /bt=windows /fo=dll32
C>wlink system win386 file dll32
C>wbind -n -d dll32
.exam end
.np
There are two entry points defined,
.id FooMe1
(index number 1)
and
.id FooMe2
(index number 2).
.id FooMe1
accepts three arguments: a WORD, a DWORD, and a WORD.
.id FooMe2
accepts two arguments: a DWORD and a WORD.
.do end
.np
.id &winmain.
returns zero to notify Windows that the DLL initialization failed,
and returns a one if initialization succeeds.
.np
.id &winmain.
accepts the same arguments as the
.id &winmain.
procedure of a regular Windows program, however, only two arguments
are used.
.id hInstance
is the DLL handle and
.id lpszCmdLine
is the command line passed to the DLL.
.*
.section Calling Functions in a 32-bit DLL from a 16-bit Application
.*
.np
.ix 'DLL' '16-bit calls into 32-bit DLLs'
The following is a 16-bit Windows program that demonstrates how to
call the two routines defined in our DLL example.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin
/*
 *  EXE16.C
 */
#include <stdio.h>
#include <windows.h>

#define Add3 1
#define Add2 2
.exam break
typedef long (FAR PASCAL *FPROC)();
typedef long (FAR PASCAL *FARPROC1)(WORD, DWORD, WORD, int);
typedef long (FAR PASCAL *FARPROC2)(DWORD, WORD, int);

long FAR PASCAL FooMe1( WORD, DWORD, WORD );
long FAR PASCAL FooMe2( DWORD, WORD );
.exam break
int PASCAL WinMain( HANDLE hInstance,
                    HANDLE hPrevInstance,
                    LPSTR  lpszCmdLine,
                    int    nCmdShow )
{
  FPROC fp;
  HANDLE hlib;
  long cb;
  char buff[128];
.exam break
  MessageBox( NULL, "16-bit EXE Started", "EXE16", MB_OK );
.exam break
  /* Do the 16-bit demo using static binding */
  cb = FooMe1( 0x666, 0x77777111, 0x6969 );
  sprintf( buff, "RC1 = %lx", cb );
  MessageBox( NULL, buff, "EXE16", MB_OK );
.exam break
  cb = FooMe2( 0x12345678, 0x8888 );
  sprintf( buff, "RC2 = %lx", cb );
  MessageBox( NULL, buff, "EXE16", MB_OK );
.exam break
  /* Do the 32-bit demo */
  hlib = LoadLibrary( "dll32.dll" );
  fp = (FPROC) GetProcAddress( hlib, "Win386LibEntry" );
.exam break
  cb = (*(FARPROC1)fp)( 0x666, 0x77777111, 0x6969, Add3 );
  sprintf( buff, "RC1 = %lx", cb );
  MessageBox( NULL, buff, "EXE16", MB_OK );
.exam break
  cb = (*(FARPROC2)fp)( 0x12345678, 0x8888, Add2 );
  sprintf( buff, "RC2 = %lx", cb );
  MessageBox( NULL, buff, "EXE16", MB_OK );
.exam break
  return( 0 );
}
.exam end
.np
Note that the last argument of a call to the 32-bit DLL routine is
the index number of the 32-bit DLL routine to use.
To create the 16-bit sample Windows executable from this code, the
following steps must be performed.
.exam begin
C>&c16. exe16 /bt=windows
C>&lnkcmd. system windows file exe16 library dll16
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
*$include winapi.fi

* GEN16V.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl gen16v -explicit -d2 -windows -l=windows
*                              -"op desc '16-bit DLL Test'"

*$pragma aux (dll32_call) indirect_1 \
*            parm( value*4, value*4, value*4 )
*$pragma aux (dll32_call) indirect_2 \
*            parm( value*4, value*4 )

.code break
      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      integer*2 hlib
      integer*4 indirect_1, indirect_2
      integer*4 dll_1, dll_2, cb
      character*128 str

.code break
      hlib = LoadLibrary( 'windllv.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLLV'c,
     &                     'Gen16V'c, MB_OK )
          stop
      endif

.code break
      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

.code break
      cb = indirect_1( 111, 22222, 3333, dll_1 )
      write( str, '(15hDLL 1 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen16V Test 1'c, MB_OK )

.code break
      cb = indirect_2( 4444, 55, dll_2 )
      write( str, '(15hDLL 2 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen16V Test 2'c, MB_OK )

      FWinMain = 0

      end
.code end
.np
The addresses of the routines
.id DLL1
and
.id DLL2
in the 32-bit DLL are obtained and stored in the variables
.id dll_1
and
.id dll_2.
Since the FORTRAN 77 language does not support indirect function
calls, we need a mechanism to call these functions indirectly.
We do this using the two indirect functions called
.id indirect_1
and
.id indirect_2.
These two functions are given the
.id dll32_call
attribute using an auxiliary pragma which is defined in the file
.fi WINAPI.FI.
Note that the last argument of the calls to
.id indirect_1
or
.id indirect_2
is the actual address of the DLL routine.
.np
What you should realize is that the
.id indirect_1
and
.id indirect_2
functions do not really exist.
The code that is generated for statements like the following is really
an indirect call to the function whose address is represented in the
last argument.
.millust begin
cb = indirect_1( 111, 22222, 3333, dll_1 )
cb = indirect_2( 4444, 55, dll_2 )
.millust end
.pc
This is a result of using the
.id dll32_call
auxiliary pragma attribute to describe both
.id indirect_1
and
.id indirect_2.
You can verify this by disassembling the object file that is generated
when this code is compiled.
.*
.section Calling Functions in a 32-bit DLL from a 32-bit Application
.*
.np
.ix 'DLL' '32-bit calls into 32-bit DLLs'
The following is a 32-bit Windows program that demonstrates how to
call the two routines defined in our 32-bit DLL example.
Since this is a 32-bit Windows program, we will use the
.id _Call16
function to call functions in our 32-bit DLL.
Note that we get to the 32-bit DLL functions by going indirectly
through the 16-bit supervisor that forms the "front end" for our
32-bit DLL.
.code begin
*$include winapi.fi

* GEN32V.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 gen32v -explicit -d2 -l=win386
* Bind:             wbind gen32v -n -D "32-bit DLL Test"


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

      integer*2 hlib
      integer*4 dll_1, dll_2, cb
      character*128 str

.code break
      hlib = LoadLibrary( 'windllv.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLLV'c,
     &                     'Gen32V'c, MB_OK )
          stop
      endif

.code break
      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

.code break
      cb = _Call16( dll_1, 'ddd'c, 111, 22222, 3333 )
      write( str, '(15hDLL 1 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen32V Test 1'c, MB_OK )

.code break
      cb = _Call16( dll_2, 'dd'c, 4444, 55 )
      write( str, '(15hDLL 2 returned , i10, a)' ) cb, char(0)
      call MessageBox( NULL, str, 'Gen32V Test 2'c, MB_OK )

      FWinMain = 0

      end
.code end
.np
Note that the first argument of a call to
.id _Call16
is the DLL function address returned by
.id GetProcAddress
and must be a 32-bit argument.
The second argument of a call to
.id _Call16
is a string describing the types of arguments that will be passed to
the DLL function.
.*
.section A Sample 32-bit DLL Using a Structure
.*
.np
.ix 'DLL' 'passing information in a structure'
As previously mentioned, passing pointers from a 16 or 32-bit Windows
application to a 32-bit DLL poses a problem since all pointers are
passed as 16-bit far pointers.
The pointer must be converted from a 16-bit far pointer to a 32-bit
far pointer.
This is achieved by mapping a dynamically allocatable array to each
argument that is passed by reference using the
.kwm LOCATION
specifier of the
.kwm ALLOCATE
statement.
Furthermore, you must specify the
.kwm far
attribute for each such array using the
.id array
pragma.
Since this is cumbersome if you wish to pass many arguments, it is
recommended that a single argument be passed that is actually a
pointer to a structure that contains the actual arguments.
Furthermore, since each call to a DLL routine is made indirectly
through one of
.id Win386LibEntry
or
.id DLL1
through
.id DLL128,
you should also return any values in the same structure since the
return value from any of these functions is only 32-bits wide.
.np
The following example is a 32-bit DLL that receives its arguments
and returns values using a structure.
The source code for these examples is provided in the
.fi &srcdir.
directory.
We describe how to compile and link the examples in the section
entitled :HDREF refid='dlcreat'..
.code begin
*$include winapi.fi

* WINDLL.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 windll -explicit -d2 -bd -l=win386
* Bind:             wbind windll -d -n

.code break
*$pragma aux (dll_function) Add3

      subroutine Add3( arg_list )
      integer*4 arg_list
      structure /argtypes/
          integer     w1
          integer     w2
          integer     w3
          integer     sum
      end structure
      record /argtypes/ args(:)
*$pragma array args far

      include 'windows.fi'

      character*128 str

.code break
      allocate( args(1), location=arg_list )
      write( str, '(16hDLL 1 arguments:, 3i10, a)' ) args(1).w1,
     &                                               args(1).w2,
     &                                               args(1).w3,
     &                                               char(0)
      call MessageBox( NULL, str, 'DLL Function 1'c, MB_OK )
      args(1).sum = args(1).w1 + args(1).w2 + args(1).w3
      deallocate( args )

      end

.code break
*$pragma aux (dll_function) Add2

      subroutine Add2( arg_list )
      integer*4 arg_list
      structure /argtypes/
          real        w1
          real        w2
          real        sum
      end structure
      record /argtypes/ args(:)
*$pragma array args far

      include 'windows.fi'

      character*128 str

.code break
      allocate( args(1), location=arg_list )
      write( str, '(16hDLL 2 arguments:, 2f10.2, a)' ) args(1).w1,
     &                                                 args(1).w2,
     &                                                 char(0)
      call MessageBox( NULL, str, 'DLL Function 2'c, MB_OK )
      args(1).sum = args(1).w1 + args(1).w2
      deallocate( args )

      end

.code break
      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      external Add3, Add2
      integer rc

.code break
      call BreakPoint
      rc = DefineDLLEntry( 1, Add3, DLL_PTR, DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
.code break
      rc = DefineDLLEntry( 2, Add2, DLL_PTR, DLL_ENDLIST )
      if( rc .ne. 0 )then
          FWinMain = 0
          return
      end if
      call MessageBox( NULL, '32-bit DLL started'c,
     &                 'WINDLL'c, MB_OK )
      FWinMain = 1

      end
.code end
.np
The following example is a 16-bit Windows application that passes
arguments to a 32-bit DLL using a structure.
.code begin
*$include winapi.fi

* GEN16.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl gen16 -explicit -d2 -windows -l=windows
*                             -"op desc '16-bit DLL Test'"

*$pragma aux (dll32_call) indirect_1 parm( reference, value*4 )
*$pragma aux (dll32_call) indirect_2 parm( reference, value*4 )

.code break
      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      integer*2 hlib
      integer*4 dll_1, dll_2
      character*128 str

.code break
      structure /args_1/
          integer     w1
          integer     w2
          integer     w3
          integer     sum
      end structure

      structure /args_2/
          real        w1
          real        w2
          real        sum
      end structure

.code break
      record /args_1/ args_1/111, 22222, 3333, 0/
      record /args_2/ args_2/714.3, 35.7, 0.0/

      hlib = LoadLibrary( 'windll.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLL'c,
     &                     'Gen16'c, MB_OK )
          stop
      endif

.code break
      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

.code break
      call indirect_1( args_1, dll_1 )
      write( str, '(15hDLL 1 returned , i10, a)' ) args_1.sum,
     &                                             char(0)
      call MessageBox( NULL, str, 'Gen16 Test 1'c, MB_OK )

.code break
      call indirect_2( args_2, dll_2 )
      write( str, '(15hDLL 2 returned , f10.2, a)' ) args_2.sum,
     &                                               char(0)
      call MessageBox( NULL, str, 'Gen16 Test 2'c, MB_OK )

      FWinMain = 0

      end
.code end
.np
The following example is a 32-bit Windows application that passes
arguments to a 32-bit DLL using a structure.
.code begin
*$include winapi.fi

* GEN32.FOR

* Setup:            set finclude=\WATCOM\src\fortran\win
* Compile and Link: wfl386 gen32 -explicit -d2 -l=win386
* Bind:             wbind gen32 -n -D "32-bit DLL Test"

.code break
      integer*2 function FWinMain( hInstance,
     &                             hPrevInstance,
     &                             lpszCmdLine,
     &                             nCmdShow )
      integer*2 hInstance
      integer*2 hPrevInstance
      integer*4 lpszCmdLine
      integer*2 nCmdShow

      include 'windows.fi'

      integer*2 hlib
      integer*4 dll_1, dll_2, cb
      character*128 str

.code break
      structure /args_1/
          integer     w1
          integer     w2
          integer     w3
          integer     sum
      end structure

      structure /args_2/
          real        w1
          real        w2
          real        sum
      end structure

.code break
      record /args_1/ args_1/111, 22222, 3333, 0/
      record /args_2/ args_2/714.3, 35.7, 0.0/

      hlib = LoadLibrary( 'windll.dll'c )
      if( hlib .lt. 32 ) then
          call MessageBox( NULL, 'Can''t load WINDLL'c,
     &                     'Gen32'c, MB_OK )
          stop
      endif

.code break
      dll_1 = GetProcAddress( hlib, 'DLL1'c )
      dll_2 = GetProcAddress( hlib, 'DLL2'c )

.code break
      cb = _Call16( dll_1, 'p'c, loc(args_1) )
      write( str, '(15hDLL 1 returned , i10, a)' ) args_1.sum,
     &                                             char(0)
      call MessageBox( NULL, str, 'Gen32 Test 1'c, MB_OK )

.code break
      cb = _Call16( dll_2, 'p'c, loc(args_2) )
      write( str, '(15hDLL 2 returned , f10.2, a)' ) args_2.sum,
     &                                               char(0)
      call MessageBox( NULL, str, 'Gen32 Test 2'c, MB_OK )

      FWinMain = 0

      end
.code end
.do end
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.*
.section Writing a 16-bit Cover for the 32-bit DLL
.*
.np
.ix 'DLL' '16-bit cover'
The following is a suggested way to make a 32-bit DLL behave just like
a 16-bit DLL from the point of view of the person trying to use the
DLL.
.np
Create a library of cover functions for each of the entry points.
Each library entry would call the 32-bit DLL using the appropriate
index number.
.np
For example, assume we have 3 functions in our DLL,
.id Initialize,
.id DoStuff,
and
.id Finish.
Assume
.id Initialize
takes an integer,
.id DoStuff
takes an integer and a pointer, and
.id Finish
takes nothing.
We could build a 16-bit library as follows:
.exam begin
#include <windows.h>
typedef long (FAR PASCAL *FPROC)();
extern long FAR PASCAL Win386LibEntry();
FPROC LibEntry = Win386LibEntry;
.exam break
BOOL Initialize( int parm )
{
    return( LibEntry( parm, 1 ) );
}
.exam break
int DoStuff( int parm1, LPVOID parm2 )
{
    return( LibEntry( parm1, parm2, 2 ) );
}
.exam break
void Finish( void )
{
    LibEntry( 3 );
}
.exam end
.do end
.*
.section *refid=dlcreat Creating and Debugging Dynamic Link Libraries
.*
.np
.ix 'DLL' 'creating'
.ix 'DLL' 'debugging'
In the following sections, we will take you through the steps of
compiling, linking, and debugging
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
both 16-bit and
.do end
32-bit Dynamic Link Libraries (DLLs).
.np
We will use example programs that are provided in source-code form in
the &cmpname package.
The files described in this chapter are located in the directory
.fi &srcdir..
The following files are provided:
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:set symbol="gen16"     value="gen16".
:set symbol="gen16u"    value="GEN16".
:set symbol="gen32"     value="gen32".
:set symbol="gen32u"    value="GEN32".
.begnote $setptnt 15
.note GEN16.&langsuffup
is the source code for a generic 16-bit Windows application that calls
functions in a 32-bit Windows DLL.
.note GEN16.LNK
is the linker directive file for linking the 16-bit Windows application.
.note GEN32.&langsuffup
is the source code for a generic 32-bit Windows application that calls
functions in both 16-bit and 32-bit Windows DLLs.
.note GEN32.LNK
is the linker directive file for linking the 32-bit Windows application.
.note DLL16.&langsuffup
is the source code for a simple 16-bit DLL containing one library
routine.
.note DLL16.LNK
is the linker directive file for linking the 16-bit Windows DLL.
.note DLL32.&langsuffup
is the source code for a more complex 32-bit DLL containing three
library routines.
.note DLL32.LNK
is the linker directive file for linking the 32-bit Windows DLL.
.note EXE16.&langsuffup
is the source code for a generic 16-bit Windows application that calls
functions in both 16-bit and 32-bit Windows DLLs.
.note DLL.&langsuffup
is the source code for a DLL containing three library routines.
The source code for this DLL can be used to create both 16-bit and
32-bit DLLs.
.note MAKEFILE
is a makefile for compiling and linking the programs described above.
.endnote
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="gen16"     value="gen16v".
:set symbol="gen16u"    value="GEN16V".
:set symbol="gen32"     value="gen32v".
:set symbol="gen32u"    value="GEN32V".
.begnote $setptnt 15
.note WINDLLV.&langsuffup
is the source code for a simple 32-bit DLL containing two library
routines that use integer arguments to pass information.
.note GEN16V.&langsuffup
is the source code for a generic 16-bit Windows application that calls
functions in the "WINDLLV" 32-bit Windows DLL.
.note GEN32V.&langsuffup
is the source code for a generic 32-bit Windows application that calls
functions in the "WINDLLV" 32-bit Windows DLL.
.note WINDLL.&langsuffup
is the source code for a simple 32-bit DLL containing two library
routines that use structures to pass information.
.note GEN16.&langsuffup
is the source code for a generic 16-bit Windows application that calls
functions in the "WINDLL" 32-bit Windows DLL.
.note GEN32.&langsuffup
is the source code for a generic 32-bit Windows application that calls
functions in the "WINDLL" 32-bit Windows DLL.
.note MAKEFILE
is a makefile for compiling and linking the programs described above.
.endnote
.do end
.*
.beglevel
.*
.section Building the Applications
.*
.np
.ix 'DLL' 'creating'
To create the DLLs and test applications, we will use the WATCOM
&makname utility and the supplied makefile.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin
C>wmake -f makefile
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.exam begin
C>wmake -f makefile
.exam end
.do end
:cmt. .*
:cmt. .section Building the 16-bit Dynamic Link Library
:cmt. .*
:cmt. .np
:cmt. The following commands should be issued to create the 16-bit DLL.
:cmt. .if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:cmt. .exam begin
:cmt. C>&c16. dll16 /bt=windows /mc /zu /d2
:cmt. C>&lnkcmd @dll16
:cmt. .exam end
:cmt. .do end
:cmt. .if '&lang' eq 'FORTRAN 77' .do begin
:cmt. .exam begin
:cmt. C>&f16. dll16 /mc /win /zu /zc /d2
:cmt. C>&lnkcmd @dll16
:cmt. .exam end
:cmt. .do end
:cmt. .np
:cmt. We request the maximum amount of debugging information possible since
:cmt. we are going to show you, later, how to debug a 16-bit DLL.
:cmt. .do end
:cmt. .*
:cmt. .section Building the 32-bit Dynamic Link Library
:cmt. .*
:cmt. .np
:cmt. The following commands should be issued to create the 32-bit DLL.
:cmt. .ix WBIND
:cmt. .if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:cmt. .exam begin
:cmt. C>&c32. dll32 /bt=windows /d2
:cmt. C>&lnkcmd @dll32
:cmt. C>wbind dll32 -d -n
:cmt. .exam end
:cmt. .do end
:cmt. .if '&lang' eq 'FORTRAN 77' .do begin
:cmt. .exam begin
:cmt. C>set finclude=\WATCOM\src\fortran\win
:cmt. C>wfl386 windllv -nowarn -d2 -bd -l=win386
:cmt. C>wbind windllv -d -n
:cmt. .exam end
:cmt. .np
:cmt. The above example assumes the &cmpname package was installed in the
:cmt. "WATCOM" directory on the current disk.
:cmt. .do end
:cmt. .np
:cmt. We request the maximum amount of debugging information possible since
:cmt. we are going to show you, later, how to debug a 32-bit DLL.
:cmt. .*
:cmt. .section Building the 16-bit Generic Windows Application
:cmt. .*
:cmt. .np
:cmt. The following commands should be issued to create the 16-bit generic
:cmt. Windows application.
:cmt. .if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:cmt. .exam begin
:cmt. C>&c16. gen16 /bt=windows
:cmt. C>&lnkcmd. @gen16
:cmt. .exam end
:cmt. .do end
:cmt. .if '&lang' eq 'FORTRAN 77' .do begin
:cmt. .exam begin
:cmt. C>set finclude=\WATCOM\src\fortran\win
:cmt. C>wfl gen16v -nowarn -d2 -windows -l=windows
:cmt.              -"op desc '16-bit DLL Test'"
:cmt. .exam end
:cmt. .np
:cmt. The quoted option should be specified on the same line as the "wfl"
:cmt. command.
:cmt. This option is passed on to the linker.
:cmt. .do end
:cmt. .*
:cmt. .section Building the 32-bit Generic Windows Application
:cmt. .*
:cmt. .np
:cmt. The following commands should be issued to create the 32-bit generic
:cmt. Windows application.
:cmt. .ix WBIND
:cmt. .if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
:cmt. .exam begin
:cmt. C>&c32. gen32 /bt=windows
:cmt. C>&lnkcmd @gen32
:cmt. C>wbind gen32 -n
:cmt. .exam end
:cmt. .do end
:cmt. .if '&lang' eq 'FORTRAN 77' .do begin
:cmt. .exam begin
:cmt. C>set finclude=\WATCOM\src\fortran\win
:cmt. C>wfl386 gen32v -nowarn -d2 -l=win386
:cmt. C>wbind gen32v -n -D "32-bit DLL Test"
:cmt. .exam end
:cmt. .do end
.*
.section Installing the Examples under Windows
.*
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.sr gen='exe16'
.sr genu='EXE16'
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.sr gen='gen32v'
.sr genu='GEN32v'
.do end
.np
.ix 'DLL' 'installing example'
Start up Microsoft Windows 3.x if you have not already done so.
Add the
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.fi EXE16.EXE
file
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.fi GEN16V.EXE
and
.fi GEN32V.EXE
files
.do end
to one of your Window groups using the Microsoft Program
Manager.
.autonote
.note
Select the "New..." entry from the "File" menu of the Microsoft
Windows Program Manager.
.note
Select "Program Item" from the "New Program Object" window and press
the "OK" button.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.note
Enter "DLL Test" as a description for the EXE16 program.
Enter the full path to the EXE16 program as a command line.
.exam begin
Description:    Test
Command Line:   c:\work\dll\exe16.exe
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note
Enter "16-bit DLL Test" as a description for the GEN16V program.
Enter the full path to the GEN16V program as a command line.
.exam begin
Description:    16-bit DLL Test
Command Line:   c:\work\dll\gen16v.exe
.exam end
.note
Enter "32-bit DLL Test" as a description for the GEN32V program.
Enter the full path to the GEN32V program as a command line.
.exam begin
Description:    32-bit DLL Test
Command Line:   c:\work\dll\gen32v.exe
.exam end
.do end
.endnote
.if '&lang' eq 'FORTRAN 77' .do begin
.np
Use a similar procedure to install the
.fi GEN16.EXE
and
.fi GEN32.EXE
programs.
.do end
.*
.section Running the Examples
.*
.np
.ix 'DLL' 'running example'
Start the 16-bit application by double clicking on its icon.
A number of message boxes are presented.
You may wish to compare the output in each message box with the source
code of the program to determine if the correct results are being
obtained.
Click on the "OK" button as each of them are displayed.
.if '&lang' eq 'FORTRAN 77' .do begin
.np
Similarly, start the 32-bit application by double-clicking on its
icon and observe the results.
.do end
.*
.section Debugging a 32-bit DLL
.*
.np
.ix 'DLL' 'debugging example'
The &dbgname can be used to debug a DLL.
To debug a 32-bit DLL, a "breakpoint" instruction must be inserted
into the source code for the DLL at the "&winmain." entry point.
This is done using the "pragma" compiler directive.
We have already added the breakpoint to the source code for the 32-bit
DLL.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exam begin
extern void BreakPoint( void );
#pragma aux BreakPoint = 0xcc;

int PASCAL WinMain( HANDLE hInstance,
                    HANDLE x1,
                    LPSTR lpCmdLine,
                    int x2 )
{
.exam break
  BreakPoint();
  DefineDLLEntry( 1, (void *) Lib1,
                        DLL_WORD,
                        DLL_DWORD,
                        DLL_WORD,
                .
                .
                .
.exam end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.code begin
                .
                .
                .
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
        call BreakPoint
        rc = DefineDLLEntry( 1, Add3, DLL_DWORD, DLL_DWORD, DLL_DWORD,
     &                          DLL_ENDLIST )
        if( rc .ne. 0 )then
            FWinMain = 0
            return
        end if
                .
                .
                .
.code end
.pc
The pragma for "BreakPoint" is defined in the "WINAPI.FI" file.
.do end
.np
Start up Microsoft Windows 3.x if you have not already done so.
Start the debugger by double-clicking on the &dbgname icon.
At the prompt, enter the path specification for the application.
When the debugger has successfully loaded &genu., start execution of
the program.
When the breakpoint is encountered in the 32-bit DLL, the debugger is
re-entered.
The debugger will automatically skip past the breakpoint.
.np
From this point on, you can symbolically debug the 32-bit DLL.
You might, for example, set breakpoints at the start of each DLL
routine to debug each of them as they are called.
.*
.section Summary
.*
.np
.ix 'DLL' 'summary'
Note that the "&winmain." entry point is only called once, at the
start of any application requesting it.
After this, the "&winmain." entry point is no longer called.
You may have to restart Windows to debug this section of code a second
or third time.
.*
.endlevel
