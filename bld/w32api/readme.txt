Open Watcom W32API Headers and Libraries - README
-------------------------------------------------

This release of Open Watcom includes a new version of the Win32 headers and
libraries.  These headers and libraries were written specifically for Open
Watcom and have no basis in the MinGW headers that were shipped with Open
Watcom 1.7a and earlier.  The goal is to provide headers with more thorough
coverage of the Win32 API, consistency with OW style guidelines, and fewer bugs.
Every effort has been made to preserve compatibility with the MinGW headers.

The headers support compiling to target any version of Windows up through
Windows Vista.  However, to preserve compatibility with existing code, the
default target is Windows 95 or Windows NT 4.0 with Internet Explorer 4.0.
If you wish to use features specific to a more recent version of Windows, you
must define the macros WINVER, _WIN32_WINNT, _WIN32_IE, and NTDDI_VERSION
appropriately, either in your source code or using compiler switches.

Please note that there was a slight discrepancy between the old MinGW headers
and the official Microsoft headers.  The MinGW version of windows.h included
winsock2.h, while the Microsoft version includes winsock.h, as does the new
Open Watcom version.  This may cause compiler errors when compiling code that
includes windows.h followed by winsock2.h when the WIN32_LEAN_AND_MEAN macro
is not defined.  These errors will also occur when compiling such code with
the Microsoft headers.  They can be eliminated by defining WIN32_LEAN_AND_MEAN
to suppress inclusion of winsock.h by windows.h

There are several instances in which the headers that shipped with previous
version of Open Watcom contained undocumented functions that are no longer
available in current versions of Windows.  These functions have been removed
from the headers.  Any code that used these functions should be rewritten to
avoid using them or to access them through GetProcAddress() or those platforms
where they are available.

Documentation for the Win32 API is available over the Internet from Microsoft
at http://msdn.microsoft.com/en-us/library/default.aspx.

