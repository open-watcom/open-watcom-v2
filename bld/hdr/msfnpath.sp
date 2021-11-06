:: MS filename make/split function family
::
::                          ??? (<stdlib.h>,<wchar.h>)
:segment WIDE
:: Wide character version
_WCRTLINK extern void       _wmakepath( wchar_t *__path, const wchar_t *__drive,
                                      const wchar_t *__dir,
                                      const wchar_t *__fname,
                                      const wchar_t *__ext );
_WCRTLINK extern void       _wsplitpath( const wchar_t *__path,
                                         wchar_t *__drive, wchar_t *__dir,
                                         wchar_t *__fname, wchar_t *__ext );
_WCRTLINK extern void       _wsplitpath2( const wchar_t *__inp,
                                          wchar_t *__outp, wchar_t **__drive,
                                          wchar_t **__dir, wchar_t **__fn,
                                          wchar_t **__ext );
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern void       _makepath( char *__path, const char *__drive,
                                    const char *__dir, const char *__fname, const char *__ext );
_WCRTLINK extern void       _splitpath( const char *__path, char *__drive,
                                     char *__dir, char *__fname, char *__ext );
_WCRTLINK extern void       _splitpath2( const char *__inp, char *__outp,
                                      char **__drive, char **__dir,
                                      char **__fn, char **__ext );
:elsesegment
:: MS
:endsegment
::
