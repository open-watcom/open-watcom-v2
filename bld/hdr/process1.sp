:segment !CONLY | NOUSING
:segment CNAME
namespace std {
:elsesegment !CONLY
#ifdef __cplusplus
namespace std {
#endif
:endsegment
  _WCRTLINK extern void   abort( void );
  _WCRTLINK extern void   exit( int __status );
  _WCRTLINK extern char   *getenv( const char *__name );
  _WCRTLINK extern int    system( const char *__cmd );
:segment CNAME
}
:elsesegment !CONLY
#ifdef __cplusplus
}
:endsegment
:endsegment
:segment !CNAME & !NOUSING
using std::abort;
using std::exit;
using std::getenv;
using std::system;
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
