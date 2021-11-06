:: MS exec function family
::
::                          <unistd.h> ??? (<process.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wexeclpe( const wchar_t *__file, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wexecvpe( const wchar_t *__file, const wchar_t *const __argv[], const wchar_t *const __envp[] );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _execlpe( const char *__file, const char *__arg0, ... );
_WCRTLINK extern int        _execvpe( const char *__file, const char *const __argv[], const char *const __envp[] );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        execlpe( const char *__file, const char *__arg0, ... );
_WCRTLINK extern int        execvpe( const char *__file, const char *const __argv[], const char *const __envp[] );
:endsegment
::
