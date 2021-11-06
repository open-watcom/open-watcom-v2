:: POSIX directory function family
::
:segment WIDE
:: Wide character version
_WCRTLINK extern WDIR       *_wopendir( const wchar_t * );
_WCRTLINK extern struct _wdirent *_wreaddir( WDIR * );
_WCRTLINK extern void       _wrewinddir( WDIR * );
_WCRTLINK extern int        _wclosedir( WDIR * );
:elsesegment ANSINAME
:: ANSI name version
:elsesegment
:: POSIX
_WCRTLINK extern DIR        *opendir( const char * );
_WCRTLINK extern struct dirent *readdir( DIR * );
_WCRTLINK extern void       rewinddir( DIR * );
_WCRTLINK extern int        closedir( DIR * );
:endsegment
