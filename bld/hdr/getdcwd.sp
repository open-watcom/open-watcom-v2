:: MS getdcwd function
::
::                          ??? (<direct.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wgetdcwd( int __drive, wchar_t *__buffer, __w_size_t __maxlen );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern char       *_getdcwd( int __drive, char *__buffer, __w_size_t __maxlen );
:elsesegment
:: POSIX
:endsegment
::
