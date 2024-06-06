:segment C
Open Watcom C/C++ Version 2.0 - README
--------------------------------------

Welcome to the Open Watcom C/C++ version 2.0 release! This README provides
helpful hints to help you get the software package up and running. For more
detailed information on installation and setup, please refer to online
Getting Started Guide (c_readme.hlp/c_readme.inf).
:elsesegment F77
Open Watcom FORTRAN Version 2.0 - README
----------------------------------------

Welcome to the Open Watcom FORTRAN/77 version 2.0 release! This README
provides helpful hints to help you get the software package up and
running. For more detailed information on installation and setup, please
refer to online Getting Started Guide (f_readme.hlp/f_readme.inf).
:endsegment

===========================================================
Important
===========================================================

Some Open Watcom tools currently do not function properly if installed in a
directory with spaces in the name e.g. "C:\Program Files\WATCOM".
Therefore choose a directory name like C:\WATCOM, D:\WATCOM etc. to install
the tools in.
When you are working on a Linux host, forgive us:
Many Parts in the OpenWatcom Documentation are not adapted for Linux yet,
and there are no graphical tools for Linux. (Patches are welcome).

===========================================================
Environment variables used by Open Watcom
===========================================================

For proper operation the Open Watcom compilers require few environment
variables to be set up correctly. These variables differ slightly depending
on the host platform (DOS, OS/2, Windows, Linux). The common variables are:

 - PATH      - points to directories containing Open Watcom executables
 - WATCOM    - points to the directory where Open Watcom is installed
               and is used by various Open Watcom tools to locate files
 - EDPATH    - points to VI/VIW configuration files
:segment C
 - INCLUDE   - points to directories containing header files used by the
               C/C++ compilers. Target specific variables are also supported.
:elsesegment F77
 - FINCLUDE  - points to directories containing header files used by the
               FORTRAN compilers
:endsegment
 - LIB       - points to directories containing library files; note that
               Open Watcom linker is able to locate Open Watcom runtime
               libraries without this variable
 - ...HELP   - points to the directory on the CDROM drive where help files
               are located. This may minimize hard disk space requirements

DOS specifics

 - PATH      - only needs to point to the binw directory
 - WWINHELP  - points to the directory on the CDROM drive where help files
               are located. This minimizes hard disk space. Note this is not
               needed if the help files are installed on the hard disk

Win16 specifics

 - PATH      - only needs to point to the binw directory
 - WWINHELP  - points to the directory on the CDROM drive where help files
               are located. This minimizes hard disk space. Note this is not
               needed if the help files are installed on the hard disk

Win32 specifics

 - PATH      - must point to binnt and binw directories, in that order
 - WWINHELP  - points to the directory on the CDROM drive where help files
               are located. This minimizes hard disk space. Note this is not
               needed if the help files are installed on the hard disk

Win64 specifics

 - PATH      - must point to binnt64 and binnt directories, in that order
 - WHTMLHELP - points to the directory on the CDROM drive where Windows html
               help files are located. This minimizes hard disk space.
               Note this is not needed if the help files are installed on
               the hard disk.

OS/2 specifics

 - PATH      - must point to binp and binw directories, in that order
 - LIBPATH   - either the BEGINLIBPATH/ENDLIBPATH environment variable or the
               LIBPATH statement in CONFIG.SYS must point to the binp\dll
               subdirectory of Open Watcom in order to let the OS find
               Open Watcom DLLs
 - HELP      - points to binp\help to let Open Watcom GUI tools locate online
               help files
 - BOOKSHELF - points to binp\help to allow the IPF viewer (VIEW.EXE) locate
               Open Watcom online manuals

Linux 32-bit specifics

 - PATH      - must point to binl directory

Linux 64-bit specifics

 - PATH      - must point to binl64 and binl directory, in that order

These environment variables can be either set up in your startup files (which
is the most convenient method if Open Watcom is the only compiler you use)
or you can use simple shell scripts/batch files to set these variables
(useful if you need to switch between compilers, including different versions of
Watcom/Open Watcom compilers).

Below are several sample shell scripts/batch files for setting the
environment variables on various host platforms.
The WATCOM variable must naturally be adjusted according to where Open Watcom
was installed on your system. The LIB environment variable isn't strictly
necessary but you will probably need to set it for any libraries you use,
which are not supplied with the Open Watcom compilers.
:segment C
Similarly you might need to adjust the INCLUDE variable for 3rd party
library include files. (Or use the related target specific *_INCLUDE variable).
:endsegment

DOS BAT file:
---------------------------------------------------------------------------
@ECHO OFF
SET WATCOM=C:\WATCOM
SET PATH=%WATCOM%\BINW;%PATH%
SET EDPATH=%WATCOM%\EDDAT
:segment C
SET INCLUDE=%WATCOM%\H
:endsegment
REM SET LIB=
REM SET WWINHELP=D:\BINW
---------------------------------------------------------------------------

Win16 BAT file:
---------------------------------------------------------------------------
@ECHO OFF
SET WATCOM=C:\WATCOM
SET PATH=%WATCOM%\BINW;%PATH%
SET EDPATH=%WATCOM%\EDDAT
:segment C
SET INCLUDE=%WATCOM%\H;%WATCOM%\H\WIN
:endsegment
REM SET LIB=
REM SET WWINHELP=D:\BINW
---------------------------------------------------------------------------

Win32 BAT file:
---------------------------------------------------------------------------
@ECHO OFF
SET WATCOM=C:\WATCOM
SET PATH=%WATCOM%\BINNT;%WATCOM%\BINW;%PATH%
SET EDPATH=%WATCOM%\EDDAT
:segment C
SET INCLUDE=%WATCOM%\H;%WATCOM%\H\NT
:endsegment
REM SET LIB=
REM SET WWINHELP=D:\BINW
---------------------------------------------------------------------------

Win64 BAT file:
---------------------------------------------------------------------------
@ECHO OFF
SET WATCOM=C:\WATCOM
SET PATH=%WATCOM%\BINNT64;%WATCOM%\BINNT;%PATH%
SET EDPATH=%WATCOM%\EDDAT
:segment C
SET INCLUDE=%WATCOM%\H;%WATCOM%\H\NT
:endsegment
REM SET LIB=
REM SET WHTMLHELP=D:\BINNT\HELP
---------------------------------------------------------------------------

OS/2 CMD file:
---------------------------------------------------------------------------
@ECHO OFF
SET WATCOM=C:\WATCOM
SET PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
SET BEGINLIBPATH=%WATCOM%\BINP\DLL
SET EDPATH=%WATCOM%\EDDAT
:segment C
SET INCLUDE=%WATCOM%\H;%WATCOM%\H\OS2
:endsegment
REM SET LIB=
SET HELP=%WATCOM%\BINP\HELP;%HELP%
SET BOOKSHELF=%WATCOM%\BINP\HELP;%BOOKSHELF%
---------------------------------------------------------------------------

Linux 32-bit shell script:
---------------------------------------------------------------------------
export WATCOM=/usr/bin/watcom
export PATH=$WATCOM/binl:$PATH
export EDPATH=$WATCOM/eddat
:segment C
export INCLUDE=$WATCOM/lh
:endsegment
#export LIB=
---------------------------------------------------------------------------

Linux 64-bit shell script:
---------------------------------------------------------------------------
export WATCOM=/usr/bin/watcom
export PATH=$WATCOM/binl64:$WATCOM/binl:$PATH
export EDPATH=$WATCOM/eddat
:segment C
export INCLUDE=$WATCOM/lh
:endsegment
#export LIB=
---------------------------------------------------------------------------
