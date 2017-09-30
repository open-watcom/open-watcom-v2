:: we keep the name '__iobuf' in the global namespace.
:segment DECLARE_STRUCT
#ifndef __OBSCURE_STREAM_INTERNALS
struct __stream_link;
struct __iobuf {
    unsigned char        *_ptr;         /* next character position */
    int                   _cnt;         /* number of characters left */
    struct __stream_link *_link;        /* location of associated struct */
    unsigned              _flag;        /* mode of file access */
    int                   _handle;      /* file handle */
    unsigned              _bufsize;     /* size of buffer */
    unsigned short        _ungotten;    /* used by ungetc and ungetwc */
:segment QNX
    struct __iobuf       *_unused;      /* spare entry */
:endsegment
};
#define ___IOBUF_DEFINED
#endif

:endsegment
#ifndef ___IOBUF_DEFINED
#define ___IOBUF_DEFINED
 struct __iobuf;
#endif
:segment CNAME
#ifndef _STDFILE_DEFINED
#define _STDFILE_DEFINED
 #define _STDFILE_DEFINED_
 namespace std {
  typedef struct __iobuf FILE;
 }
 typedef std::FILE __w_FILE;
#endif
:elsesegment
#ifndef _FILE_DEFINED
#define _FILE_DEFINED
 #define _FILE_DEFINED_
 typedef struct __iobuf FILE;
 typedef FILE __w_FILE;
#endif
:endsegment
