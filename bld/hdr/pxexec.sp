:: POSIX exec function family
::
::                          <unistd.h> (<process.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wexecl( const wchar_t *__path, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wexecle( const wchar_t *__path, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wexeclp( const wchar_t *__file, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wexecv( const wchar_t *__path, const wchar_t *const __argv[] );
_WCRTLINK extern int        _wexecve( const wchar_t *__path, const wchar_t *const __argv[], const wchar_t *const __envp[] );
_WCRTLINK extern int        _wexecvp( const wchar_t *__file, const wchar_t *const __argv[] );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _execl( const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        _execle( const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        _execlp( const char *__file, const char *__arg0, ... );
_WCRTLINK extern int        _execv( const char *__path, const char *const __argv[] );
_WCRTLINK extern int        _execve( const char *__path, const char *const __argv[], const char *const __envp[] );
_WCRTLINK extern int        _execvp( const char *__file, const char *const __argv[] );
:elsesegment
:: POSIX
_WCRTLINK extern int        execl( const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        execle( const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        execlp( const char *__file, const char *__arg0, ... );
_WCRTLINK extern int        execv( const char *__path, const char *const __argv[] );
_WCRTLINK extern int        execve( const char *__path, const char *const __argv[], const char *const __envp[] );
_WCRTLINK extern int        execvp( const char *__file, const char *const __argv[] );
:endsegment
::
