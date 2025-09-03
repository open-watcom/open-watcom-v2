:: MS specific I/O extension functions
::
::                          <io.h>
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _eof( int __handle );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        eof( int __handle );
:endsegment
::
::                          <io.h>
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
:segment RDOS
_WCRTLINK extern long long  _filelength( int __handle );
:elsesegment
_WCRTLINK extern long       _filelength( int __handle );
:endsegment
_WCRTLINK extern long long  _filelengthi64( int __handle );
:elsesegment
:segment RDOS
_WCRTLINK extern long long  filelength( int __handle );
:elsesegment
:: MS deprecated
_WCRTLINK extern long       filelength( int __handle );
:endsegment
:endsegment
::
::                          <io.h>
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern off_t      _tell( int __handle );
_WCRTLINK extern long long  _telli64( int __handle );
:elsesegment
:: MS deprecated
_WCRTLINK extern off_t      tell( int __handle );
:endsegment
::
::                          <io.h>
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
:segment RDOS
_WCRTLINK extern int        _chsize( int __handle, long long __size );
:elsesegment
_WCRTLINK extern int        _chsize( int __handle, long __size );
:endsegment
:elsesegment
:segment RDOS
_WCRTLINK extern int        chsize( int __handle, long long __size );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        chsize( int __handle, long __size );
:endsegment
:endsegment
::
::                          <io.h> (<fcntl.h>)
:segment WIDE
:: Wide character version
:elsesegment ANSINAME
:: ANSI name version
_WCRTLINK extern int        _setmode( int __handle, int __mode );
:elsesegment
:: MS deprecated
_WCRTLINK extern int        setmode( int __handle, int __mode );
:endsegment
::
