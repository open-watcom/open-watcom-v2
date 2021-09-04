:: MS extension getdcwd function
::
::                          ??? (<direct.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wgetdcwd( int __drive, wchar_t *__buffer, size_t __maxlen );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern char       *_getdcwd( int __drive, char *__buffer, size_t __maxlen );
:elsesegment
:: POSIX
:endsegment
::
