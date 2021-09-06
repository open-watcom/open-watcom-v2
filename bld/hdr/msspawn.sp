:: MS spawn function family
::
::                          ??? (<process.h>,<wchar.h>,<stdio.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern int        _wspawnl( int __mode, const wchar_t *__path, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wspawnle( int __mode, const wchar_t *__path, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wspawnlp( int __mode, const wchar_t *__path, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wspawnlpe( int __mode, const wchar_t *__path, const wchar_t *__arg0, ... );
_WCRTLINK extern int        _wspawnv( int __mode, const wchar_t *__path, const wchar_t * const *__argv );
_WCRTLINK extern int        _wspawnve( int __mode, const wchar_t *__path, const wchar_t * const *__argv, const wchar_t * const *__envp );
_WCRTLINK extern int        _wspawnvp( int __mode, const wchar_t *__path, const wchar_t * const *__argv );
_WCRTLINK extern int        _wspawnvpe( int __mode, const wchar_t *__path, const wchar_t * const *__argv, const wchar_t * const *__envp );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _spawnl( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        _spawnle( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        _spawnlp( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        _spawnlpe( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        _spawnv( int __mode, const char *__path, const char * const *__argv );
_WCRTLINK extern int        _spawnve( int __mode, const char *__path, const char * const *__argv, const char * const *__envp );
_WCRTLINK extern int        _spawnvp( int __mode, const char *__path, const char * const *__argv );
_WCRTLINK extern int        _spawnvpe( int __mode, const char *__path, const char * const *__argv, const char * const *__envp );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        spawnl( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        spawnle( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        spawnlp( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        spawnlpe( int __mode, const char *__path, const char *__arg0, ... );
_WCRTLINK extern int        spawnv( int __mode, const char *__path, const char * const *__argv );
_WCRTLINK extern int        spawnve( int __mode, const char *__path, const char * const *__argv, const char * const *__envp );
_WCRTLINK extern int        spawnvp( int __mode, const char *__path, const char * const *__argv );
_WCRTLINK extern int        spawnvpe( int __mode, const char *__path, const char * const *__argv, const char * const *__envp );
:endsegment
::
