struct utimbuf {
    time_t      actime;         /* access time */
    time_t      modtime;        /* modification time */
};
#define _utimbuf    utimbuf

/*
 *  POSIX 1003.1 Prototype
 */
_WCRTLINK extern int utime( const char *__path, const struct utimbuf *__times );
_WCRTLINK extern int _utime( const char *__path, const struct utimbuf *__times );
_WCRTLINK extern int _wutime( const wchar_t *__path, const struct utimbuf *__times );
:segment QNX
_WCRTLINK extern int __futime( int __fildes, const struct utimbuf *__times );
:endsegment
