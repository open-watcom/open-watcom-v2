:: MS specific I/O extension functions
::
_WCRTLINK extern int  _eof( int __handle );
:: MS deprecated alias
_WCRTLINK extern int  eof( int __handle );
::
_WCRTLINK extern long _filelength( int __handle );
_WCRTLINK extern long long _filelengthi64( int __handle );
:: MS deprecated alias
_WCRTLINK extern long filelength( int __handle );
::
_WCRTLINK extern off_t _tell( int __handle );
_WCRTLINK extern __int64  _telli64( int __handle );
:: MS deprecated alias
_WCRTLINK extern off_t tell( int __handle );
::
_WCRTLINK extern int  _chsize( int __handle, long __size );
:: MS deprecated alias
_WCRTLINK extern int  chsize( int __handle, long __size );
::
_WCRTLINK extern int  _setmode( int __handle, int __mode );
:: MS deprecated alias
_WCRTLINK extern int  setmode( int __handle, int __mode );
::
