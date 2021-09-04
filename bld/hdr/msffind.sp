:: MS find I/O functions
::
::                          ??? (<io.h>,<wchar.h>)
:segment WIDE
:: MS wide extension
_WCRTLINK extern __w_intptr_t _wfindfirst( const wchar_t *__filespec, struct _wfinddata_t *__fileinfo );
_WCRTLINK extern __w_intptr_t _wfindfirsti64( const wchar_t *__filespec, struct _wfinddatai64_t *__fileinfo );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern __w_intptr_t _findfirst( const char *__filespec, struct _finddata_t *__fileinfo );
_WCRTLINK extern __w_intptr_t _findfirsti64( const char *__filespec, struct _finddatai64_t *__fileinfo );
:elsesegment
:: POSIX
:endsegment
::
:segment WIDE
:: MS wide extension
_WCRTLINK extern int        _wfindnext( __w_intptr_t __handle, struct _wfinddata_t *__fileinfo );
_WCRTLINK extern int        _wfindnexti64( __w_intptr_t __handle, struct _wfinddatai64_t *__fileinfo );
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _findnext( __w_intptr_t __handle, struct _finddata_t *__fileinfo );
_WCRTLINK extern int        _findnexti64( __w_intptr_t __handle, struct _finddatai64_t *__fileinfo );
:elsesegment
:: POSIX
:endsegment
::
:segment WIDE
:: MS wide extension
:elsesegment MSEXT
:: MS extension
_WCRTLINK extern int        _findclose( __w_intptr_t __handle );
:elsesegment
:: POSIX
:endsegment
::
