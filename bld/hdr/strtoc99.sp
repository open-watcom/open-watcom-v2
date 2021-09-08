:: ISO C strto../wcsto... conversion functions
::
::                          <stdlib.h> (<stdlib.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WMRTLINK extern double     wcstod( const wchar_t *, wchar_t ** );
_WMRTLINK extern float      wcstof( const wchar_t *, wchar_t ** );
_WMRTLINK extern long double wcstold( const wchar_t *, wchar_t ** );
_WCRTLINK extern long       wcstol( const wchar_t *, wchar_t **, int );
_WCRTLINK extern long long  wcstoll( const wchar_t *, wchar_t **, int );
_WCRTLINK extern unsigned long wcstoul( const wchar_t *, wchar_t **, int );
_WCRTLINK extern unsigned long long wcstoull( const wchar_t *, wchar_t **, int );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: ISO C
_WMRTLINK extern double     strtod( const char *__nptr, char **__endptr );
_WMRTLINK extern float      strtof( const char *__nptr, char **__endptr );
_WMRTLINK extern long double strtold( const char *__nptr, char **__endptr );
_WCRTLINK extern long       strtol( const char *__nptr, char **__endptr, int __base );
_WCRTLINK extern long long  strtoll( const char *__nptr, char **__endptr, int __base );
_WCRTLINK extern unsigned long strtoul( const char *__nptr, char **__endptr, int __base );
_WCRTLINK extern unsigned long long strtoull( const char *__nptr, char **__endptr, int __base );
:endsegment
::
