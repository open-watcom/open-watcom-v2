:: ISO C strerror function
::
::                          <string.h> (<string.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern wchar_t    *_wcserror( int __errnum );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WCRTLINK extern char       *strerror( int __errnum );
:endsegment
::
