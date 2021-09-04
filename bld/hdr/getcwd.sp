:: POSIX getcwd function
::
::                          <unistd.h> (<direct.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern wchar_t    *_wgetcwd( wchar_t *__buf, size_t __size );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern char       *_getcwd( char *__buf, size_t __size );
:elsesegment
:: POSIX
_WCRTLINK extern char       *getcwd( char *__buf, size_t __size );
:endsegment
::
