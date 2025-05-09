:segment RESOURCE
#ifndef _WIN32_IE
  #define _WIN32_IE         0x0400
#endif
#ifndef _WIN32_WINDOWS
  #define _WIN32_WINDOWS    0x0400
#endif
:elsesegment
::
:: setup Windows version
::
#ifndef WINVER
  #define WINVER            _WIN32_WINNT
#endif
::
:: setup Windows DDI version
::
:: Windows DDI is supported from Windows 2000
::
:: now it is setup to minimum version NTDDI_NT4 (for Windows NT 4.0) even if 
:: such level is not defined by Microsoft
:: it simplify handling of NTDDI_VERSION and enable 
:: modify DDK for Windows NT 4.0
::
#ifndef NTDDI_VERSION
  #define NTDDI_VERSION     NTDDI_VERSION_FROM_WIN32_WINNT(_WIN32_WINNT)
#endif
::
:: setup Windows IE version
::
#ifndef _WIN32_IE
# if (_WIN32_WINNT <= _WIN32_WINNT_NT4)
#  define _WIN32_IE     _WIN32_IE_IE50
# elif (_WIN32_WINNT <= _WIN32_WINNT_WIN2K)
#  define _WIN32_IE     _WIN32_IE_IE501
# elif (_WIN32_WINNT <= _WIN32_WINNT_WINXP)
#  define _WIN32_IE     _WIN32_IE_IE60
# elif (_WIN32_WINNT <= _WIN32_WINNT_WS03)
#  define _WIN32_IE     _WIN32_IE_WS03
# elif (_WIN32_WINNT <= _WIN32_WINNT_VISTA)
#  define _WIN32_IE     _WIN32_IE_LONGHORN
# elif (_WIN32_WINNT <= _WIN32_WINNT_WIN7)
#  define _WIN32_IE     _WIN32_IE_WIN7
# else
#  define _WIN32_IE     _WIN32_IE_WIN8
# endif
#endif
:endsegment
