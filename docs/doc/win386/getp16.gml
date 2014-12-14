.func GetProc16
.synop begin
.if '&lang' eq 'FORTRAN 77' .do begin
c$include 'winapi.fi'
       integer*4 function GetProc16( fcn, type )
       integer*4 fcn, type
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
#include <windows.h>
CALLBACKPTR GetProc16( PROCPTR fcn, long type );
.do end
.synop end
.desc begin
The &func function returns a 16-bit far function pointer suitable for
use as a Windows callback function.
This callback function will invoke the 32-bit far procedure specified
by
.arg fcn
.ct .li .
The types of callback functions that may be allocated are:
.begterm 8
.term GETPROC_CALLBACK
This is the most common form of callback; suitable as the callback routine
for a window.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback has the form:
.millust begin
long FAR PASCAL WProc( HWND, unsigned,
            WORD, LONG );
.millust end
.do end
.term GETPROC_ABORTPROC
This is the callback type used for trapping abort requests when printing.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback has the form:
.millust begin
int FAR PASCAL AbortProc( HDC, WORD );
.millust end
.do end
.term GETPROC_ENUMCHILDWINDOWS
This callback is used with the
.kw EnumChildWindows
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form
.millust begin
BOOL FAR PASCAL EnumChildWindowsFunc(
            HWND, DWORD );
.millust end
.do end
.term GETPROC_ENUMFONTS
This callback type is used with the
.kw EnumFonts
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback has the form:
.millust begin
int FAR PASCAL EnumFontsFunc( LPLOGFONT,
            LPTEXTMETRICS, short, LPSTR );
.millust end
.do end
.term GETPROC_ENUMMETAFILE
This callback is used with the
.kw EnumMetaFile
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL EnumMetaFileFunc( HDC,
            LPHANDLETABLE, LPMETARECORD,
            short, LPSTR );
.millust end
.do end
.term GETPROC_ENUMOBJECTS
This callback is used with the
.kw EnumObjects
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL EnumObjectsFunc( LPSTR, LPSTR );
.millust end
.do end
.term GETPROC_ENUMPROPS_FIXED_DS
This callback is used with the
.kw EnumProps
Windows function, when the fixed data segments callback is needed.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL EnumPropsFunc(
            HWND, LPSTR, HANDLE );
.millust end
.do end
.term GETPROC_ENUMPROPS_MOVEABLE_DS
This callback is used with the
.kw EnumProps
Windows function, when the moveable data segments callback is needed.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL EnumPropsFunc(
            HWND, WORD, PSTR, HANDLE );
.millust end
.do end
.term GETPROC_ENUMTASKWINDOWS
This callback is used with the
.kw EnumTaskWindows
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL EnumTaskWindowsFunc(
            HWND, DWORD );
.millust end
.do end
.term GETPROC_ENUMWINDOWS
This callback is used with the
.kw EnumWindows
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL EnumWindowsFunc( HWND, DWORD );
.millust end
.do end
.term GETPROC_GLOBALNOTIFY
This callback is used with the
.kw GlobalNotify
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL GlobalNotifyFunc( HANDLE );
.millust end
.do end
.term GETPROC_GRAYSTRING
This callback is used with the
.kw GrayString
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL GrayStringFunc(
            HDC, DWORD, short );
.millust end
.do end
.term GETPROC_LINEDDA
This callback is used with the
.kw LineDDA
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
void FAR PASCAL LineDDAFunc(
            short, short, LPSTR );
.millust end
.do end
.term GETPROC_SETRESOURCEHANDLER
This callback is used with the
.kw SetResourceHandler
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL SetResourceHandlerFunc(
            HANDLE, HANDLE, HANDLE );
.millust end
.do end
.term GETPROC_SETTIMER
This callback is used with the
.kw SetTimer
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL SetTimerFunc(
            HWND, WORD, short, DWORD );
.millust end
.do end
.term GETPROC_SETWINDOWSHOOK
This callback is used with the
.kw SetWindowsHook
Windows function.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
The callback function has the form:
.millust begin
int FAR PASCAL SetWindowsHookFunc(
            short, WORD, DWORD );
.millust end
.do end
.term GETPROC_USERDEFINED_x
This callback is used in conjunction with
.kw DefineUserProc16
function to create a callback routine with an arbitrary set of parameters.
Up to 32 user defined callbacks are allowed, they are identified by
using GETPROC_USERDEFINED_1 through GETPROC_USERDEFINED_32.
The user defined callback must be declared as a FAR PASCAL function,
or as a FAR cdecl function.
.endterm
.desc end
.return begin
The &func function returns a 16-bit far pointer to a callback procedure.
This pointer may then be fed to any Windows function that requires a
pointer to a function within the 32-bit program.
Note that the callback function within the 32-bit program must be
declared as
.kw FAR
.ct .li .
.return end
.see begin
.seelist ReleaseProc16
.see end
.if '&lang' eq 'FORTRAN 77' .do begin
.keep 20
.exmp begin
c$include winapi.fi

       integer*4 cbp
       integer*4 lpProcAbout

       ! get a 16-bit callback routine to point at
       ! our About dialogue procedure, then create
       ! the dialogue.
       cbp = GetProc16( About, GETPROC_CALLBACK )
       lpProcAbout = MakeProcInstance( cbp, hInst )
       call DialogBox( hInst,
     &                 'AboutBox'c,
                       hWnd,
                       lpProcAbout )
       call FreeProcInstance( lpProcAbout )
       call ReleaseProc16( cbp )
.exmp end
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.exmp begin
#include <windows.h>

  CALLBACKPTR cbp;
  FARPROC lpProcAbout;
  /*
   * Get a 16-bit callback routine to point at
   * our About dialogue procedure, then create
   * the dialogue.  We use _16 versions of
   * MakeProcInstance, DialogBox, and
   * FreeProcInstance because they do not do
   * any magic work on the callback routines.
   */
  cbp = GetProc16( (PROCPTR) About,
                   GETPROC_CALLBACK );
.exmp break
  lpProcAbout = _16MakeProcInstance( cbp, hInst );
.exmp break
  _16DialogBox( hInst,
             "AboutBox",
             hWnd,
             lpProcAbout );
.exmp break
  _16FreeProcInstance( lpProcAbout );
  ReleaseProc16( cbp );
.exmp end
.do end
.class WIN386
