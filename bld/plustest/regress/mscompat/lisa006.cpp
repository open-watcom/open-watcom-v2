//Bug: used to core dump
//Status: fixed in 10.6 and 11.0
//Result: gives ambiguity error message
//        Microsoft also calls it ambiguous
extern "C" {
extern "C" {
typedef long char wchar_t;
typedef wchar_t wint_t;
typedef wchar_t wctype_t;
   extern int	isalnum(int);
   extern int	isalpha(int);
   extern int	iscntrl(int);
   extern int	isdigit(int);
   extern int	isgraph(int);
   extern int	islower(int);
   extern int	isprint(int);
   extern int	ispunct(int);
   extern int	isspace(int);
   extern int	isupper(int);
   extern int	isxdigit(int);
   extern int	isleadbyte(int);
   extern int	tolower(int);
   extern int	_tolower(int);
   extern int	toupper(int);
   extern int	_toupper(int);
   extern int	__iscsymf(int);
   extern int	__iscsym(int);
   extern int	iswalpha(wint_t);
   extern int	iswalnum(wint_t);
   extern int	iswascii(wint_t);
   extern int	iswcntrl(wint_t);
   extern int	iswprint(wint_t);
   extern int	iswgraph(wint_t);
   extern int	iswdigit(wint_t);
   extern int	iswxdigit(wint_t);
   extern int	iswlower(wint_t);
   extern int	iswupper(wint_t);
   extern int	iswspace(wint_t);
   extern int	iswpunct(wint_t);
   extern wctype_t wctype(const char *);
   extern int	iswctype(wint_t,wctype_t);
   extern wint_t	towlower(wint_t);
   extern wint_t	towupper(wint_t);
    extern int	isascii(int);
    extern int	__isascii(int);
    extern const char  __near  _IsTable[257];
};
extern "C" {
typedef unsigned size_t;
   extern void *memchr( const void *__s, int __c, size_t __n );
   extern int memcmp( const void *__s1, const void *__s2, size_t __n );
   extern void *memcpy( void *__s1, const void *__s2, size_t __n );
   extern void *memmove( void *__s1, const void *__s2, size_t __n );
extern void *memset( void *__s, int __c, size_t __n );
   extern char *strcat( char *__s1, const char *__s2 );
   extern char *strchr( const char *__s, int __c );
extern int strcmp( const char *__s1, const char *__s2 );
   extern int strcoll( const char *__s1, const char *__s2 );
   extern int _stricoll( const char *__s1, const char *__s2 );
   extern int _strncoll( const char *__s1, const char *__s2, size_t __n );
   extern int _strnicoll( const char *__s1, const char *__s2, size_t __n );
   extern size_t strxfrm( char *__s1, const char *__s2, size_t __n );
   extern char *strcpy( char *__s1, const char *__s2 );
   extern size_t strcspn( const char *__s1, const char *__s2 );
   extern char *strerror( int __errnum );
   extern size_t strlen( const char *__s );
   extern char *strncat( char *__s1, const char *__s2, size_t __n );
   extern int strncmp( const char *__s1, const char *__s2, size_t __n );
   extern char *strncpy( char *__s1, const char *__s2, size_t __n );
   extern char *strpbrk( const char *__s1, const char *__s2 );
   extern char *strrchr( const char *__s, int __c );
   extern size_t strspn( const char *__s1, const char *__s2 );
   extern char *strspnp( const char *__s1, const char *__s2 );
   extern char *strstr( const char *__s1, const char *__s2 );
   extern char *strtok( char *__s1, const char *__s2 );
   extern void  __far  *_fmemccpy( void  __far  *__s1, const void  __far  *__s2, int __c, size_t __n );
   extern void  __far  *_fmemchr( const void  __far  *__s, int __c, size_t __n );
   extern void  __far  *_fmemcpy( void  __far  *__s1, const void  __far  *__s2, size_t __n );
   extern void  __far  *_fmemmove( void  __far  *__s1, const void  __far  *__s2, size_t __n );
   extern void  __far  *_fmemset( void  __far  *__s, int __c, size_t __n );
   extern int _fmemcmp( const void  __far  *__s1, const void  __far  *__s2, size_t __n );
   extern int _fmemicmp( const void  __far  *__s1, const void  __far  *__s2, size_t __n );
   extern char  __far  *_fstrcat( char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrchr( const char  __far  *__s, int __c );
   extern int _fstrcmp( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrcpy( char  __far  *__s1, const char  __far  *__s2 );
   extern size_t _fstrcspn( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrdup( const char  __far  *__string );
   extern int _fstricmp( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrncat( char  __far  *__s1, const char  __far  *__s2, size_t __n );
   extern size_t _fstrlen( const char  __far  *__s );
   extern char  __far  *_fstrlwr( char  __far  *__string );
   extern int _fstrncmp( const char  __far  *__s1, const char  __far  *__s2, size_t __n );
   extern char  __far  *_fstrncpy( char  __far  *__s1, const char  __far  *__s2, size_t __n );
   extern int _fstrnicmp( const char  __far  *__s1, const char  __far  *__s2, size_t __n );
   extern char  __far  *_fstrnset( char  __far  *__string, int __c, size_t __len );
   extern char  __far  *_fstrpbrk( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrrchr( const char  __far  *__s, int __c );
   extern char  __far  *_fstrrev( char  __far  *__string );
   extern char  __far  *_fstrset( char  __far  *__string, int __c );
   extern size_t _fstrspn( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrspnp( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrstr( const char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrtok( char  __far  *__s1, const char  __far  *__s2 );
   extern char  __far  *_fstrupr( char  __far  *__string );
   extern void movedata( unsigned __srcseg, unsigned __srcoff, unsigned __tgtseg, unsigned __tgtoff, unsigned __len );
   extern void *memccpy( void *__s1, const void *__s2, int __c, size_t __n );
   extern int memicmp( const void *__s1, const void *__s2, size_t __n );
   extern int _memicmp( const void *__s1, const void *__s2, size_t __n );
   extern int strcmpi( const char *__s1, const char *__s2 );
   extern char *strdup( const char *__string );
   extern char *_strdup( const char *__string );
   extern int stricmp( const char *__s1, const char *__s2 );
   extern int _stricmp( const char *__s1, const char *__s2 );
   extern char *strlwr( char *__string );
   extern char *_strlwr( char *__string );
   extern int strnicmp( const char *__s1, const char *__s2, size_t __n );
   extern int _strnicmp( const char *__s1, const char *__s2, size_t __n );
   extern char *strnset( char *__string, int __c, size_t __len );
   extern char *strrev( char *__string );
   extern char *_strrev( char *__string );
   extern char *strset( char *__string, int __c );
   extern char *strupr( char *__string );
   extern char *_strupr( char *__string );
   extern size_t wcslen( const wchar_t * );
   extern wchar_t *_wcsnset( wchar_t *, int, size_t );
   extern wchar_t *wcsnset( wchar_t *, int, size_t );
   extern size_t wcsspn( const wchar_t *, const wchar_t * );
   extern wchar_t *_wcsspnp( const wchar_t *__s1, const wchar_t *__s2 );
   extern size_t wcscspn( const wchar_t *, const wchar_t * );
   extern wchar_t *wcsncat( wchar_t *, const wchar_t *, size_t );
   extern int wcsncmp( const wchar_t *, const wchar_t *, size_t );
   extern wchar_t *wcsncpy( wchar_t *, const wchar_t *, size_t );
   extern int _wcsnicmp( const wchar_t *, const wchar_t *, size_t );
   extern int wcsnicmp( const wchar_t *, const wchar_t *, size_t );
   extern wchar_t *wcschr( const wchar_t *, wint_t );
   extern wchar_t *_wcsdup( const wchar_t * );
   extern wchar_t *wcsdup( const wchar_t * );
   extern wchar_t *wcscat( wchar_t *, const wchar_t * );
   extern wchar_t *_wcslwr( wchar_t * );
   extern wchar_t *wcslwr( wchar_t * );
   extern wchar_t *_wcsupr( wchar_t * );
   extern wchar_t *wcsupr( wchar_t * );
   extern wchar_t *wcscpy( wchar_t *, const wchar_t * );
   extern int wcscmp( const wchar_t *, const wchar_t * );
   extern int _wcsicmp( const wchar_t *, const wchar_t * );
   extern int wcsicmp( const wchar_t *, const wchar_t * );
   extern int wcscoll( const wchar_t *__s1, const wchar_t *__s2 );
   extern int _wcsicoll( const wchar_t *__s1, const wchar_t *__s2 );
   extern int _wcsncoll( const wchar_t *__s1, const wchar_t *__s2, size_t __n );
   extern int _wcsnicoll( const wchar_t *__s1, const wchar_t *__s2, size_t __n );
   extern size_t wcsxfrm( wchar_t *__s1, const wchar_t *__s2, size_t __n );
   extern wchar_t *wcsstr( const wchar_t *, const wchar_t * );
   extern wchar_t *wcspbrk( const wchar_t *, const wchar_t * );
   extern wchar_t *wcstok( wchar_t *, const wchar_t *, wchar_t ** );
   extern wchar_t *_wcsrev( wchar_t * );
   extern wchar_t *wcsrev( wchar_t * );
   extern wchar_t *_wcsset( wchar_t *, wchar_t );
   extern wchar_t *wcsset( wchar_t *, wchar_t );
   extern wchar_t *wcsrchr( const wchar_t *, wint_t );
   extern int wcscmpi( const wchar_t *, const wchar_t * );
   extern wchar_t *wcserror( int );
   extern void *_wmemccpy( void *__s1, const void *__s2, int __c, size_t __n );
   extern void *_wmemchr( const void *, int, size_t );
   extern int _wmemcmp( const void *, const void *, size_t );
   extern void *_wmemcpy( void *, const void *, size_t );
   extern int _wmemicmp( const void *__s1, const void *__s2, size_t __n );
   extern void *_wmemmove( void *, const void *, size_t );
   extern size_t _ustrlen( const wchar_t * );
   extern wchar_t *_ustrnset( wchar_t *, int, size_t );
   extern size_t _ustrspn( const wchar_t *, const wchar_t * );
   extern size_t _ustrcspn( const wchar_t *, const wchar_t * );
   extern wchar_t *_ustrncat( wchar_t *, const wchar_t *, size_t );
   extern int _ustrncmp( const wchar_t *, const wchar_t *, size_t );
   extern wchar_t *_ustrncpy( wchar_t *, const wchar_t *, size_t );
   extern int _ustrnicmp( const wchar_t *, const wchar_t *, size_t );
   extern wchar_t *_ustrchr( const wchar_t *, int );
   extern wchar_t *_ustrdup( const wchar_t * );
   extern wchar_t *_ustrcat( wchar_t *, const wchar_t * );
   extern wchar_t *_ustrlwr( wchar_t * );
   extern wchar_t *_ustrupr( wchar_t * );
   extern wchar_t *_ustrcpy( wchar_t *, const wchar_t * );
   extern int _ustrcmp( const wchar_t *, const wchar_t * );
   extern int _ustricmp( const wchar_t *, const wchar_t * );
   extern int _ustrcoll( const wchar_t *__s1, const wchar_t *__s2 );
   extern wchar_t *_ustrstr( const wchar_t *, const wchar_t * );
   extern wchar_t *_ustrpbrk( const wchar_t *, const wchar_t * );
   extern wchar_t *_ustrtok( wchar_t *, const wchar_t * );
   extern wchar_t *_ustrrev( wchar_t * );
   extern wchar_t *_ustrset( wchar_t *, wchar_t );
   extern wchar_t *_ustrrchr( const wchar_t *, int );
   extern int _ustrcmpi( const wchar_t *, const wchar_t * );
   extern wchar_t *_ustrerror( int );
};
extern "C" {
typedef int ptrdiff_t;
    extern int *__threadid(void);  
};
extern "C" {
  #pragma pack(__push,1);
typedef struct	{
	int	quot;
	int	rem;
} div_t;
typedef struct	{
	long	quot;
	long	rem;
} ldiv_t;
   extern void	 abort( void );
   extern int	 abs( int __j );
	  extern int	 atexit( register void ( *__func )( void ) );
   extern double	 atof( const char *__nptr );
   extern int	 atoi( const char *__nptr );
   extern long int atol( const char *__nptr );
   extern void	*bsearch( const void *__key, const void *__base, 
				  size_t __nmemb, size_t __size, 
				  int (*__compar)(const void *__pkey, 
				  		  const void *__pbase) );
   extern void	 break_on( void );
   extern void	 break_off( void );
   extern void	*calloc( size_t __n, size_t __size );
   extern div_t	 div( int __numer, int __denom );
   extern void	 exit( int __status );
   extern void	 free( void *__ptr );
   extern char	*getenv( const char *__name );
   extern long int labs( long int __j );
extern ldiv_t ldiv( long int __numer, long int __denom );
   extern void	*malloc( size_t __size );
   extern int	 mblen( const char *__s, size_t __n );
   extern size_t	 mbstowcs( wchar_t *__pwcs, const char *__s, 
				   size_t __n );
   extern int	 mbtowc( wchar_t *__pwc, const char *__s, size_t __n );
   extern size_t	 wcstombs( char *__s, const wchar_t *__pwcs, 
				   size_t __n );
   extern int	 wctomb( char *__s, wchar_t __wchar );
   extern void	 qsort( void *__base, size_t __nmemb, size_t __size,
				int (*__compar)( const void *, const void * ) );
   extern int	 rand( void );
   extern void	*realloc( void *__ptr, size_t __size );
   extern void	 srand( unsigned int __seed );
   extern double	 strtod( const char *__nptr, char **__endptr );
   extern long int strtol( const char *__nptr, char **__endptr, 
				 int __base );
   extern unsigned long strtoul( const char *__nptr, char **__endptr, 
				       int __base );
   extern int	 system( const char *__string );
   extern void	 _exit( int __status );
   extern char	*ecvt( double __val, int __ndig, int *__dec, 
			       int *__sign );
   extern char	*_ecvt( double __val, int __ndig, int *__dec, 
			       int *__sign );
   extern char	*fcvt( double __val, int __ndig, int *__dec, 
			       int *__sign );
   extern char	*_fcvt( double __val, int __ndig, int *__dec, 
			        int *__sign );
   extern char	*_fullpath( char *__buf, const char *__path, 
			            size_t __size );
   extern char	*gcvt( double __val, int __ndig, char *__buf );
   extern char	*_gcvt( double __val, int __ndig, char *__buf );
   extern char	*itoa( int __value, char *__buf, int __radix );
   extern char	*_itoa( int __value, char *__buf, int __radix );
extern unsigned long _lrotl( unsigned long __value, unsigned int __shift );
extern unsigned long _lrotr( unsigned long __value, unsigned int __shift );
   extern char	*ltoa( long int __value, char *__buf, int __radix );
   extern char	*_ltoa( long int __value, char *__buf, int __radix );
   extern void	 _makepath( char *__path, const char *__drive,
				    const char *__dir, const char *__fname,
				    const char *__ext );
   extern unsigned int _rotl( unsigned int __value, unsigned int __shift );
   extern unsigned int _rotr( unsigned int __value, unsigned int __shift );
   extern int	  _watoi( const wchar_t * );
   extern long int _watol( const wchar_t * );
   extern wchar_t *_witoa( int, wchar_t *, int );
   extern wchar_t *_wltoa( long int, wchar_t *, int );
   extern wchar_t *_wutoa( unsigned int, wchar_t *, int );
   extern wchar_t *_wultoa( unsigned long int, wchar_t *, int );
   extern int	  _wtoi( const wchar_t * );
   extern long int _wtol( const wchar_t * );
   extern wchar_t *_itow( int, wchar_t *, int );
   extern wchar_t *_ltow( long int, wchar_t *, int );
   extern wchar_t *_utow( unsigned int, wchar_t *, int );
   extern wchar_t *_ultow( unsigned long int, wchar_t *, int );
   extern double	 _watof( const wchar_t * );
   extern long int wcstol( const wchar_t *, wchar_t **, int );
   extern double	 wcstod( const wchar_t *, wchar_t ** );
   extern unsigned long int wcstoul( const wchar_t *, wchar_t **, int );
   extern wchar_t *_atouni( wchar_t *, const char * );
   extern wchar_t *_wfullpath( wchar_t *, const wchar_t *, size_t );
   extern void	  _wmakepath( wchar_t *__path, const wchar_t *__drive,
				      const wchar_t *__dir,
				      const wchar_t *__fname,
				      const wchar_t *__ext );
   extern int _wcsicmp( const wchar_t *, const wchar_t * );
   extern wchar_t *_wcsdup( const wchar_t * );
   extern int _wcsnicmp( const wchar_t *, const wchar_t *, size_t );
   extern wchar_t *_wcslwr( wchar_t * );
   extern wchar_t *_wcsupr( wchar_t * );
   extern wchar_t *_wcsrev( wchar_t * );
   extern wchar_t *_wcsset( wchar_t *, wchar_t );
   extern wchar_t *_wcsnset( wchar_t *, int, size_t );
   extern wchar_t *  _wgetenv( const wchar_t *__name );
   extern int	    _wsetenv( const wchar_t *__name,
				      const wchar_t *__newvalue, 
				      int __overwrite );
   extern int	    _wputenv( const wchar_t *__env_string );
   extern void	    _wsearchenv( const wchar_t *__name,
					 const wchar_t *__env_var,
					 wchar_t *__buf );
   extern void	    _wsplitpath2( const wchar_t *__inp,
					  wchar_t *__outp, wchar_t **__drive,
					  wchar_t **__dir, wchar_t **__fn,
					  wchar_t **__ext );
   extern void	    _wsplitpath( const wchar_t *__path,
					 wchar_t *__drive, wchar_t *__dir,
					 wchar_t *__fname, wchar_t *__ext );
   extern int	    _wsystem( const wchar_t *__cmd );
   extern int	 putenv( const char *__string );
   extern void	 _searchenv( const char *__name, const char *__env_var,
				     char *__buf );
   extern void	 _splitpath2( const char *__inp, char *__outp,
		    		      char **__drive, char **__dir, 
				      char **__fn, char **__ext );
   extern void	 _splitpath( const char *__path, char *__drive,
		    		     char *__dir, char *__fname, char *__ext );
   extern void	 swab( char *__src, char *__dest, int __num );
   extern char	*ultoa( unsigned long int __value, char *__buf, 
				int __radix );
   extern char	*utoa( unsigned int __value, char *__buf, int __radix );
   extern int	  _uatoi( const wchar_t * );
   extern long int _uatol( const wchar_t * );
   extern wchar_t *_uitoa( int, wchar_t *, int );
   extern wchar_t *_ultoa( long int, wchar_t *, int );
   extern wchar_t *_uutoa( unsigned int, wchar_t *, int );
   extern wchar_t *_uultoa( unsigned long int, wchar_t *, int );
   extern long int _ustrtol( const wchar_t *, wchar_t **, int );
   extern double	  _uatof( const wchar_t * );
   extern double	  _ustrtod( const wchar_t *, wchar_t ** );
   extern unsigned long int _ustrtoul( const wchar_t *, wchar_t **, int );
   extern wchar_t *_uecvt( double, int, int *, int * );
   extern wchar_t *_ufcvt( double, int, int *, int * );
   extern wchar_t *_ugcvt( double, int, wchar_t * );
   extern wchar_t *_atouni( wchar_t *, const char * );
   extern wchar_t *_ugetenv( const wchar_t * );
   extern wchar_t *_u_fullpath( wchar_t *, const wchar_t *, size_t );
   extern int	  _usystem( const wchar_t * );
   extern void	  _u_makepath( wchar_t *, const wchar_t *, 
				       const wchar_t *, const wchar_t *, 
				       const wchar_t * );
   extern void	  _u_splitpath( const wchar_t *, wchar_t *,  wchar_t *,
		      		        wchar_t *,  wchar_t * );
   extern void	  _u_splitpath2( const wchar_t *, wchar_t *, wchar_t **,
		       			 wchar_t **, wchar_t **, wchar_t ** );
   extern char ** __near  environ;	 
   extern wchar_t ** __near  _wenviron;  
   extern int _fileinfo;		 
   extern int  (*__get_errno_ptr()) ;
   extern int		     (*__get_doserrno_ptr()) ;   
   extern unsigned  __near     _psp;	 
   extern unsigned char  __near  _osmode;	 
   extern int  __near 	    _fmode; 	 
   extern char *		    sys_errlist[]; 
   extern int  __near 	    sys_nerr;	 
   extern unsigned  __near 	    __minreal;	 
   extern unsigned long  __near  __win_alloc_flags;  
   extern unsigned long  __near  __win_realloc_flags; 
   extern unsigned char  __near  _osmajor;	 
   extern unsigned char  __near  _osminor;	 
   extern unsigned short  __near  _osbuild;	 
   extern unsigned  __near 	_amblksiz;	 
   extern void  __near  *__brk(unsigned __new_brk_value);
   extern void  __near  *sbrk(int __increment);
 typedef void (*onexit_t)();
   extern onexit_t onexit(onexit_t __func);
#pragma pack(__pop);
};
}
struct __WATCOM_exception {
};
class __lock;
typedef long streampos;
typedef long streamoff;
enum {
    DEFAULT_PUTBACK_SIZE = 4,
    DEFAULT_MAINBUF_SIZE = 512,
    DEFAULT_BUF_SIZE     = DEFAULT_MAINBUF_SIZE + DEFAULT_PUTBACK_SIZE
};
class    __WATCOM_ios;
class    istream;
class    ostream;
class    streambuf;
  #pragma pack(__push,1);
class    ios {
public:
    enum io_state {                      
        goodbit = 0x00,                  
        badbit  = 0x01,                  
        failbit = 0x02,                  
        eofbit  = 0x04                   
    };
    typedef int iostate;
    enum open_mode {                     
        in        = 0x0001,              
        out       = 0x0002,              
        atend     = 0x0004,              
        append    = 0x0008,              
        truncate  = 0x0010,              
        nocreate  = 0x0020,              
        noreplace = 0x0040,              
        text      = 0x0080,              
        binary    = 0x0100,              
        app       = append,              
        ate       = atend,
        trunc     = truncate
    };
    typedef int openmode;
    enum seek_dir {                      
	beg	  = 0x00,                
	cur	  = 0x01,                
        end       = 0x02                 
    };
    typedef int seekdir;
    enum fmt_flags {                     
        skipws     = 0x0001,             
        left       = 0x0002,             
        right      = 0x0004,             
        internal   = 0x0008,             
        dec        = 0x0010,             
        oct        = 0x0020,             
        hex        = 0x0040,             
        showbase   = 0x0080,             
        showpoint  = 0x0100,             
        uppercase  = 0x0200,             
        showpos    = 0x0400,             
        scientific = 0x0800,             
        fixed      = 0x1000,             
        unitbuf    = 0x2000,             
        stdio      = 0x4000,             
        basefield  = dec | oct | hex,
        adjustfield= left | right | internal,
        floatfield = scientific | fixed,
    };
    typedef long fmtflags;
    class failure : public __WATCOM_exception {
	iostate __cause;
    public:
	failure( iostate );		 
	iostate cause() const;		 
    };
    ios( streambuf *__sb );
    ios( ios const & );
    virtual ~ios();
    ios &operator = ( ios const & );
        operator void * () const;
    int operator !      () const;
    ostream   *tie( ostream *__ostrm );
    ostream   *tie() const;
    streambuf *rdbuf() const;
    iostate    rdstate() const;
    iostate    clear( iostate __state = 0 );
    int        good() const;
    int        bad()  const;
    int        fail() const;
    int        eof()  const;
    iostate    exceptions( iostate __enable );
    iostate    exceptions() const;
    fmtflags   setf( fmtflags __onbits, fmtflags __mask );
    fmtflags   setf( fmtflags __onbits );
    fmtflags   unsetf( fmtflags __offbits );
    fmtflags   flags( fmtflags __bits );
    fmtflags   flags() const;
    char       fill( char __fillchar );
    char       fill() const;
    int        precision( int __precision );
    int        precision() const;
    int        width( int __width );
    int        width() const;
    long      &iword( int __index );
    void     *&pword( int __index );
    static void sync_with_stdio( void ) {};	 
    static fmtflags bitalloc();
    static int      xalloc();
    __lock *__i_lock;	      
    static __lock *__x_lock;  
protected:
    ios();
    void init( streambuf *__sb );
    void setstate( ios::iostate __orbits );
private:
    streambuf *__strmbuf;
    ostream   *__tied_stream;
    long       __format_flags;
    int        __error_state;
    int        __enabled_exceptions;
    int        __float_precision;
    int        __field_width;
    void      *__xalloc_list;
    char       __fill_character;
    static int       __xalloc_index;
    static fmtflags  __last_format_flag;
    friend class __WATCOM_ios;
    int : 0;
};
#pragma pack(__pop);
inline streambuf *ios::rdbuf() const {
    return( __strmbuf );
}
inline ios::iostate ios::rdstate() const {
    return( __error_state );
}
inline int ios::good() const {
    return( __error_state == 0 );
}
inline int ios::bad() const {
    return( __error_state & ios::badbit );
}
inline int ios::fail() const {
    return( __error_state & (ios::failbit|ios::badbit) );
}
inline int ios::eof() const {
    return( __error_state & ios::eofbit );
}
inline ios::iostate ios::exceptions() const {
    return( __enabled_exceptions );
}
inline ios::operator void * () const {
    return( (void *) (fail()==0) );
}
inline int ios::operator ! () const {
    return( fail() );
}
inline ios::fmtflags ios::setf( ios::fmtflags __onbits ) {
      ;
    ios::fmtflags __old_flags = __format_flags;
    __format_flags           |= __onbits;
    return( __old_flags );
}
inline ios::fmtflags ios::setf( ios::fmtflags __onbits, ios::fmtflags __mask ) {
      ;
    ios::fmtflags __old_flags = __format_flags;
    __format_flags           &= ~__mask;
    __format_flags           |= __onbits & __mask;
    return( __old_flags );
}
inline ios::fmtflags ios::unsetf( ios::fmtflags __offbits ) {
      ;
    ios::fmtflags __old_flags = __format_flags;
    __format_flags           &= ~__offbits;
    return( __old_flags );
}
inline ios::fmtflags ios::flags( ios::fmtflags __flags ) {
      ;
    ios::fmtflags __old_flags = __format_flags;
    __format_flags            = __flags;
    return( __old_flags );
}
inline ios::fmtflags ios::flags() const {
    return( __format_flags );
}
inline char ios::fill( char __fillchar ) {
      ;
    char __old_fill  = __fill_character;
    __fill_character = __fillchar;
    return( __old_fill );
}
inline char ios::fill() const {
    return( __fill_character );
}
inline int ios::precision( int __precision ) {
      ;
    int __old_precision = __float_precision;
    __float_precision   = __precision;
    return( __old_precision );
}
inline int ios::precision() const {
    return( __float_precision );
}
inline int ios::width( int __width ) {
      ;
    int __old_width = __field_width;
    __field_width   = __width;
    return( __old_width );
}
inline int ios::width() const {
    return( __field_width );
}
inline ostream *ios::tie( ostream *__ostrm ) {
      ;
    ostream *__old_tie = __tied_stream;
    __tied_stream      = __ostrm;
    return( __old_tie );
}
inline ostream *ios::tie() const {
    return( __tied_stream );
}
  #pragma pack(__push,1);
class    streambuf {
public:
    int  in_avail() const;
    int  out_waiting() const;
    int  snextc();
    int  sgetn( char *__buf, int __len );
    int  speekc();
    int  sgetc();
    int  sgetchar();
    int  sbumpc();
    void stossc();
    int  sputbackc( char __c );
    int  sputc( int __c );
    int  sputn( char const *__buf, int __len );
    void dbp();
    virtual int        do_sgetn( char *__buf, int __len );
    virtual int        do_sputn( char const *__buf, int __len );
    virtual int        pbackfail( int __ignored );
    virtual int        overflow( int =  (-1)  ) = 0;
    virtual int        underflow() = 0;
    virtual streambuf *setbuf( char *__buf, int __len );
    virtual streampos  seekoff( streamoff     __ignored1,
                                ios::seekdir  __ignored2,
                                ios::openmode __ignored3 = ios::in | ios::out );
    virtual streampos  seekpos( streampos     __position,
                                ios::openmode __mode = ios::in | ios::out );
    virtual int        sync();
    __lock *__b_lock;	     
protected:
    streambuf();
    streambuf( char *__buf, int __len );
    virtual ~streambuf();
    int   allocate();
    char *base() const;
    char *ebuf() const;
    int   blen() const;
    void  setb( char *__buf, char *__endbuf, int __autodelete = 0 );
    char *eback() const;
    char *gptr() const;
    char *egptr() const;
    void  gbump( streamoff __offset );
    void  setg( char *__eback, char *__gptr, char *__egptr );
    char *pbase() const;
    char *pptr() const;
    char *epptr() const;
    void  pbump( streamoff __offset );
    void  setp( char *__pptr, char *__epptr );
    int   unbuffered( int __unbuffered );
    int   unbuffered() const;
    virtual int doallocate();
private:
    streambuf( streambuf & );
    void operator = ( streambuf & );
    char *__reserve_base;
    char *__reserve_end;
    char *__get_base;
    char *__get_end;
    char *__get_ptr;
    char *__put_base;
    char *__put_end;
    char *__put_ptr;
    unsigned  __unbuffered_state : 1;
    unsigned  __delete_reserve   : 1;
};
#pragma pack(__pop);
inline char *streambuf::base() const {
    return( __reserve_base );
}
inline char *streambuf::ebuf() const {
    return( __reserve_end );
}
inline int streambuf::blen() const {
      ;
    return( (int)(__reserve_end - __reserve_base) );
}
inline int streambuf::allocate() {
      ;
    return( (base() !=  0  || unbuffered()) ?  0 
                                             : doallocate() );
}
inline int streambuf::unbuffered() const {
    return( __unbuffered_state );
}
inline int streambuf::unbuffered( int __unbuffered ) {
      ;
    int __old_unbuffered = __unbuffered_state;
    __unbuffered_state   = (char)(__unbuffered ? 1 : 0);
    return( __old_unbuffered );
}
inline char * streambuf::eback() const {
    return( __get_base );
}
inline char * streambuf::gptr() const {
    return( __get_ptr );
}
inline char * streambuf::egptr() const {
    return( __get_end );
}
inline void streambuf::gbump( streamoff __offset ) {
      ;
    __get_ptr += __offset;
}
inline void streambuf::setg( char *__eback, char *__gptr, char *__egptr ) {
      ;
    __get_base = __eback;
    __get_ptr  = __gptr;
    __get_end  = __egptr;
}
inline char * streambuf::pbase() const {
    return( __put_base );
}
inline char * streambuf::pptr() const {
    return( __put_ptr );
}
inline char * streambuf::epptr() const {
    return( __put_end );
}
inline void streambuf::pbump( streamoff __offset ) {
      ;
    __put_ptr += __offset;
}
inline void streambuf::setp( char *__pptr, char *__epptr ) {
      ;
    __put_base = __pptr;
    __put_ptr  = __pptr;
    __put_end  = __epptr;
}
inline int streambuf::in_avail() const {
      ;
    return( (int)(__get_end - __get_ptr) );
}
inline int streambuf::sgetchar() {
      ;
    return( (__get_ptr >= __get_end) && (underflow() ==  (-1) ) ?  (-1) 
                                                             : *__get_ptr++ );
}
inline int streambuf::sbumpc() {
    return( sgetchar() );
}
inline int streambuf::snextc() {
      ;
    return( (__get_ptr) && (++__get_ptr < __get_end) ? *__get_ptr
                                                     : underflow() );
}
inline int streambuf::speekc() {
      ;
    return( (__get_ptr < __get_end) ? *__get_ptr
                                    : underflow() );
}
inline int streambuf::sgetc() {
    return( speekc() );
}
inline int streambuf::sgetn( char *__buf, int __len ) {
      ;
    if( __len < (int)(__get_end - __get_ptr) ) {
	::memcpy( __buf, __get_ptr, __len );
	gbump( __len );
	return( __len );
    }
    return( do_sgetn( __buf, __len ) );
}
inline void streambuf::stossc() {
      ;
    if( (__get_ptr < __get_end) || (underflow() !=  (-1) ) ) {
        ++__get_ptr;
    }
}
inline int streambuf::out_waiting() const {
      ;
    return( (int)(__put_ptr - __put_base) );
}
inline int streambuf::sputbackc( char __c ) {
      ;
    return( (__get_ptr > __get_base) ? (*--__get_ptr = __c)
                                     : pbackfail( __c ) );
}
inline int streambuf::sputc( int __c ) {
      ;
    return( (__put_ptr >= __put_end) ? overflow( __c )
                                     : (*__put_ptr++ = (char)__c) );
}
inline int streambuf::sputn( char const *__buf, int __len ) {
      ;
    if( __len < (int)(__put_end - __put_ptr) ) {
	::memcpy( __put_ptr, __buf, __len );
	pbump( __len );
	return( __len );
    }
    return( do_sputn( __buf, __len ) );
}
  #pragma pack(__push,1);
class    istream : virtual public ios {
public:
    istream( streambuf *__sb );
    istream( istream const &__istrm );
    virtual ~istream();
    istream &operator = ( streambuf * __sb );
    istream &operator = ( istream const &__istrm );
    istream &operator >> (           char * __buf );
    istream &operator >> (    signed char * __buf );
    istream &operator >> (  unsigned char * __buf );
    istream &operator >> (           char & __c );
    istream &operator >> (    signed char & __c );
    istream &operator >> (  unsigned char & __c );
    istream &operator >> (   signed short & __i );
    istream &operator >> ( unsigned short & __i );
    istream &operator >> (     signed int & __i );
    istream &operator >> (   unsigned int & __i );
    istream &operator >> (    signed long & __i );
    istream &operator >> (  unsigned long & __i );
    istream &operator >> (          float & __f );
    istream &operator >> (         double & __f );
    istream &operator >> (    long double & __f );
    istream &operator >> (      streambuf * __sb );
    istream &operator >> ( ios &(*__f)( ios & ) );
    istream &operator >> ( istream &(*__f)( istream & ) );
    int        ipfx( int __noskipws = 0 );
    void       isfx();
    int        get();
    istream   &get(          char *__buf, int __len, char __delim = '\n' );
    istream   &get(   signed char *__buf, int __len, char __delim = '\n' );
    istream   &get( unsigned char *__buf, int __len, char __delim = '\n' );
    istream   &get(          char &__c );
    istream   &get(   signed char &__c );
    istream   &get( unsigned char &__c );
    istream   &get( streambuf &__sb, char __delim = '\n' );
    istream   &getline(          char *__buf, int __len, char __delim = '\n' );
    istream   &getline(   signed char *__buf, int __len, char __delim = '\n' );
    istream   &getline( unsigned char *__buf, int __len, char __delim = '\n' );
    istream   &ignore( int __num = 1, int __delim =  (-1)  );
    istream   &read(          char *__buf, int __len );
    istream   &read(   signed char *__buf, int __len );
    istream   &read( unsigned char *__buf, int __len );
    istream   &seekg( streampos __position );
    istream   &seekg( streamoff __offset, ios::seekdir __direction );
    istream   &putback( char __c );
    streampos  tellg();
    int        gcount() const;
    int        peek();
    int        sync();
protected:
    istream();
    void     eatwhite();
    istream &do_get( char &__c );
    istream &do_rshift( char &__c );
    istream &do_read( char *__buf, int __len );
    int	     ipfx0( void );
    int	     ipfx1( void );
    int      do_ipfx( int __noskipws );
private:
    int __last_read_length;
};
#pragma pack(__pop);
inline istream &istream::operator >> ( signed char *__buf ) {
    return( *this >> (char *) __buf );
}
inline istream &istream::operator >> ( unsigned char *__buf ) {
    return( *this >> (char *) __buf );
}
inline istream &istream::operator >> ( signed char &__c ) {
    return( *this >> (char &) __c );
}
inline istream &istream::operator >> ( unsigned char &__c ) {
    return( *this >> (char &) __c );
}
inline istream &istream::get( signed char *__buf, int __len, char __delim ) {
    return( get( (char *)__buf, __len, __delim ) );
}
inline istream &istream::get( unsigned char *__buf, int __len, char __delim ) {
    return( get( (char *)__buf, __len, __delim ) );
}
inline istream &istream::get( signed char &__c ) {
    return( get( (char &) __c ) );
}
inline istream &istream::get( unsigned char &__c ) {
    return( get( (char &) __c ) );
}
inline istream &istream::getline( signed char *__buf, int __len,
    char __delim ) {
    return( getline( (char *)__buf, __len, __delim ) );
}
inline istream &istream::getline( unsigned char *__buf, int __len,
    char __delim ) {
    return( getline( (char *)__buf, __len, __delim ) );
}
inline istream &istream::read( signed char *__buf, int __len ) {
    return( read( (char *) __buf, __len ) );
}
inline istream &istream::read( unsigned char *__buf, int __len ) {
    return( read( (char *) __buf, __len ) );
}
inline int istream::ipfx0( void ) {
      ;
    return( ((flags()&ios::skipws) || !good() || tie()) ? do_ipfx( 0 ) : 1);
}
inline int istream::ipfx1( void ) {
      ;
    return( (!good() || tie()) ? do_ipfx( 1 ) : 1);
}
inline void istream::isfx() {
}
inline int istream::gcount() const {
    return( __last_read_length );
}
  #pragma pack(__push,1);
class    ostream : virtual public ios {
public:
    ostream( streambuf *__sb );
    ostream( ostream const &__ostrm );
    virtual ~ostream();
    ostream &operator = ( streambuf *__sb );
    ostream &operator = ( ostream const &__ostrm );
    ostream &operator << (                char  __c );
    ostream &operator << (         signed char  __c );
    ostream &operator << (       unsigned char  __c );
    ostream &operator << (        signed short  __s );
    ostream &operator << (      unsigned short  __s );
    ostream &operator << (          signed int  __i );
    ostream &operator << (        unsigned int  __i );
    ostream &operator << (         signed long  __l );
    ostream &operator << (       unsigned long  __l );
    ostream &operator << (               float  __f );
    ostream &operator << (              double  __f );
    ostream &operator << (         long double  __f );
    ostream &operator << (                void *__p );
    ostream &operator << (           streambuf *__sb );
    ostream &operator << (          char const *__buf );
    ostream &operator << (   signed char const *__buf );
    ostream &operator << ( unsigned char const *__buf );
    ostream &operator << ( ostream &(*__f)( ostream & ) );
    ostream &operator << ( ios &(*__f)( ios & ) );
    int        opfx();
    void       osfx();
    ostream   &put(          char __c );
    ostream   &put(   signed char __c );
    ostream   &put( unsigned char __c );
    ostream   &write(          char const *__buf, int __len );
    ostream   &write(   signed char const *__buf, int __len );
    ostream   &write( unsigned char const *__buf, int __len );
    ostream   &flush();
    ostream   &seekp( streampos __position );
    ostream   &seekp( streamoff __offset, ios::seekdir __direction );
    streampos  tellp();
protected:
    ostream();
    ostream &__outfloat( long double const & );
    ostream &do_lshift( char __c);
    int	     do_opfx();
};
#pragma pack(__pop);
inline ostream &ostream::operator << ( signed char __c ) {
    return( *this << (char) __c );
}
inline ostream &ostream::operator << ( unsigned char __c ) {
    return( *this << (char) __c );
}
inline ostream &ostream::operator << ( signed short __s ) {
    return( *this << (signed long) __s );
}
inline ostream &ostream::operator << ( unsigned short __s ) {
    return( *this << (unsigned long) __s );
}
inline ostream &ostream::operator << ( signed int __i ) {
    return( *this << (signed long) __i );
}
inline ostream &ostream::operator << ( unsigned int __i ) {
    return( *this << (unsigned long) __i );
}
inline ostream &ostream::operator << ( float __f ) {
    return( __outfloat( (long double)__f ) );
}
inline ostream &ostream::operator << ( double __f ) {
    return( __outfloat( (long double)__f ) );
}
inline ostream &ostream::operator << ( long double __f ) {
    return( __outfloat( __f ) );
}
inline ostream &ostream::operator << ( signed char const *__buf ) {
    return( *this << (char const *) __buf );
}
inline ostream &ostream::operator << ( unsigned char const *__buf ) {
    return( *this << (char const *) __buf );
}
inline ostream &ostream::put( signed char __c ) {
    return( put( (char) __c ) );
}
inline ostream &ostream::put( unsigned char __c ) {
    return( put( (char) __c ) );
}
inline ostream &ostream::write( signed char const *__buf, int __len ) {
    return( write( (char const *) __buf, __len ) );
}
inline ostream &ostream::write( unsigned char const *__buf, int __len ) {
    return( write( (char const *) __buf, __len ) );
}
inline int ostream::opfx() {
      ;
    if( !good() ) {
        return 0;
    } else if( tie() || ( flags() & ios::stdio ) ) {
        return do_opfx();
    } else {
        return 1;
    }
}
inline void ostream::osfx() {
      ;
    if( flags() & ios::unitbuf ) {
        flush();
    }
}
  #pragma pack(__push,1);
class    iostream : public istream, public ostream {
public:
    iostream( streambuf *__sb );
    iostream( ios const &__strm );
    virtual ~iostream();
    iostream & operator = ( streambuf *__sb );
    iostream & operator = ( ios const &__strm );
protected:
    iostream();
};
#pragma pack(__pop);
   extern ios &dec( ios & );
   extern ios &hex( ios & );
   extern ios &oct( ios & );
   extern istream &   ws( istream & );
   extern ostream & endl( ostream & );
   extern ostream & ends( ostream & );
   extern ostream &flush( ostream & );
    extern istream  __near  cin;
    extern ostream  __near  cout;
    extern ostream  __near  cerr;
    extern ostream  __near  clog;
template <class T1, class T2>
inline  int  operator!=(const T1& x, const T2& y) {
    return !(x == y);
}
template <class T1, class T2>
inline  int  operator>(const T1& x, const T2& y) {
    return y < x;
}
template <class T1, class T2>
inline  int  operator<=(const T1& x, const T2& y) {
    return !(y < x);
}
template <class T1, class T2>
inline  int  operator>=(const T1& x, const T2& y) {
    return !(x < y);
}
template <class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};
template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};      
template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x + y; }
};
template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x - y; }
};
template <class T>
struct times : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x * y; }
};
template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x / y; }
};
template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x % y; }
};
template <class T>
struct negate : public unary_function<T, T> {
    T operator()(const T& x) const { return -x; }
};
template <class T>
struct equal_to : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x == y; }
};
template <class T>
struct not_equal_to : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x != y; }
};
template <class T>
struct greater : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x > y; }
};
template <class T>
struct less : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x < y; }
};
template <class T>
struct greater_equal : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x >= y; }
};
template <class T>
struct less_equal : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x <= y; }
};
template <class T>
struct logical_and : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x && y; }
};
template <class T>
struct logical_or : public binary_function<T, T,  int > {
     int  operator()(const T& x, const T& y) const { return x || y; }
};
template <class T>
struct logical_not : public unary_function<T,  int > {
     int  operator()(const T& x) const { return !x; }
};
template <class Predicate>
class unary_negate : public unary_function<Predicate::argument_type,  int > {
protected:
    Predicate pred;
public:
    unary_negate(const Predicate& x) : pred(x) {}
     int  operator()(const argument_type& x) const { return !pred(x); }
};
template <class Predicate>
unary_negate<Predicate> not1(const Predicate& pred) {
    return unary_negate<Predicate>(pred);
}
template <class Predicate> 
class binary_negate 
   : public binary_function<Predicate::first_argument_type,
			     Predicate::second_argument_type,  int > {
protected:
    Predicate pred;
public:
    binary_negate(const Predicate& x) : pred(x) {}
     int  operator()(const first_argument_type& x, 
		    const second_argument_type& y) const {
	return !pred(x, y); 
    }
};
template <class Predicate>
binary_negate<Predicate> not2(const Predicate& pred) {
    return binary_negate<Predicate>(pred);
}
template <class Operation> 
class binder1st : public unary_function<Operation::second_argument_type,
					Operation::result_type> {
protected:
    Operation op;
    Operation::first_argument_type value;
public:
    binder1st(const Operation& x, const Operation::first_argument_type& y)
	: op(x), value(y) {}
    result_type operator()(const argument_type& x) const {
	return op(value, x); 
    }
};
template <class Operation, class T>
binder1st<Operation> bind1st(const Operation& op, const T& x) {
    return binder1st<Operation>(op, Operation::first_argument_type(x));
}
template <class Operation> 
class binder2nd : public unary_function<Operation::first_argument_type,
					Operation::result_type> {
protected:
    Operation op;
    Operation::second_argument_type value;
public:
    binder2nd(const Operation& x, const Operation::second_argument_type& y) 
	: op(x), value(y) {}
    result_type operator()(const argument_type& x) const {
	return op(x, value); 
    }
};
template <class Operation, class T>
binder2nd<Operation> bind2nd(const Operation& op, const T& x) {
    return binder2nd<Operation>(op, Operation::second_argument_type(x));
}
template <class Operation1, class Operation2>
class unary_compose : public unary_function<Operation2::argument_type,
                                            Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
public:
    unary_compose(const Operation1& x, const Operation2& y) : op1(x), op2(y) {}
    result_type operator()(const argument_type& x) const {
	return op1(op2(x));
    }
};
template <class Operation1, class Operation2>
unary_compose<Operation1, Operation2> compose1(const Operation1& op1, 
					       const Operation2& op2) {
    return unary_compose<Operation1, Operation2>(op1, op2);
}
template <class Operation1, class Operation2, class Operation3>
class binary_compose : public unary_function<Operation2::argument_type,
                                             Operation1::result_type> {
protected:
    Operation1 op1;
    Operation2 op2;
    Operation3 op3;
public:
    binary_compose(const Operation1& x, const Operation2& y, 
		   const Operation3& z) : op1(x), op2(y), op3(z) { }
    result_type operator()(const argument_type& x) const {
	return op1(op2(x), op3(x));
    }
};
template <class Operation1, class Operation2, class Operation3>
binary_compose<Operation1, Operation2, Operation3> 
compose2(const Operation1& op1, const Operation2& op2, const Operation3& op3) {
    return binary_compose<Operation1, Operation2, Operation3>(op1, op2, op3);
}
template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result> {
protected:
    Result (*ptr)(Arg);
public:
    pointer_to_unary_function() {}
    pointer_to_unary_function(Result (*x)(Arg)) : ptr(x) {}
    Result operator()(Arg x) const { return ptr(x); }
};
template <class Arg, class Result>
pointer_to_unary_function<Arg, Result> ptr_fun(Result (*x)(Arg)) {
    return pointer_to_unary_function<Arg, Result>(x);
}
template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result> {
protected:
    Result (*ptr)(Arg1, Arg2);
public:
    pointer_to_binary_function() {}
    pointer_to_binary_function(Result (*x)(Arg1, Arg2)) : ptr(x) {}
    Result operator()(Arg1 x, Arg2 y) const { return ptr(x, y); }
};
template <class Arg1, class Arg2, class Result>
pointer_to_binary_function<Arg1, Arg2, Result> 
ptr_fun(Result (*x)(Arg1, Arg2)) {
    return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}
template <class T1, class T2>
struct pair {
    T1 first;
    T2 second;
    pair() : first(T1()), second(T2()) {}
    pair(const T1& a, const T2& b) : first(a), second(b) {}
    friend void destroy(pair<T1, T2>* p) {
      p->~pair();
    }
};
template <class T1, class T2>
inline  int  operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) { 
    return x.first == y.first && x.second == y.second; 
}
template <class T1, class T2>
inline  int  operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) { 
    return x.first < y.first || (!(y.first < x.first) && x.second < y.second); 
}
template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y) {
    return pair<T1, T2>(x, y);
}
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag {};
struct bidirectional_iterator_tag {};
struct random_access_iterator_tag {};
template <class T, class Distance> struct input_iterator {};
struct output_iterator {};
template <class T, class Distance> struct forward_iterator {};
template <class T, class Distance> struct bidirectional_iterator {};
template <class T, class Distance> struct random_access_iterator {};
template <class T, class Distance> 
inline input_iterator_tag 
iterator_category(const input_iterator<T, Distance>&) {
    return input_iterator_tag();
}
inline output_iterator_tag iterator_category(const output_iterator&) {
    return output_iterator_tag();
}
template <class T, class Distance> 
inline forward_iterator_tag
iterator_category(const forward_iterator<T, Distance>&) {
    return forward_iterator_tag();
}
template <class T, class Distance> 
inline bidirectional_iterator_tag
iterator_category(const bidirectional_iterator<T, Distance>&) {
    return bidirectional_iterator_tag();
}
template <class T, class Distance> 
inline random_access_iterator_tag
iterator_category(const random_access_iterator<T, Distance>&) {
    return random_access_iterator_tag();
}
template <class T>
inline random_access_iterator_tag iterator_category(const T*) {
    return random_access_iterator_tag();
}
template <class T, class Distance> 
inline T* value_type(const input_iterator<T, Distance>&) {
    return (T*)(0); 
}
template <class T, class Distance> 
inline T* value_type(const forward_iterator<T, Distance>&) {
    return (T*)(0);
}
template <class T, class Distance> 
inline T* value_type(const bidirectional_iterator<T, Distance>&) {
    return (T*)(0);
}
template <class T, class Distance> 
inline T* value_type(const random_access_iterator<T, Distance>&) {
    return (T*)(0);
}
template <class T>
inline T* value_type(const T*) { return (T*)(0); }
template <class T, class Distance> 
inline Distance* distance_type(const input_iterator<T, Distance>&) {
    return (Distance*)(0);
}
template <class T, class Distance> 
inline Distance* distance_type(const forward_iterator<T, Distance>&) {
    return (Distance*)(0);
}
template <class T, class Distance> 
inline Distance* 
distance_type(const bidirectional_iterator<T, Distance>&) {
    return (Distance*)(0);
}
template <class T, class Distance> 
inline Distance* 
distance_type(const random_access_iterator<T, Distance>&) {
    return (Distance*)(0);
}
template <class T>
inline ptrdiff_t* distance_type(const T*) { return (ptrdiff_t*)(0); }
template <class Container>
class back_insert_iterator : public output_iterator {
protected:
    Container& container;
public:
    back_insert_iterator(Container& x) : container(x) {}
    back_insert_iterator<Container>&
    operator=(const Container::value_type& value) { 
	container.push_back(value);
	return *this;
    }
    back_insert_iterator<Container>& operator*() { return *this; }
    back_insert_iterator<Container>& operator++() { return *this; }
    back_insert_iterator<Container>& operator++(int) { return *this; }
};
template <class Container>
back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}
template <class Container>
class front_insert_iterator : public output_iterator {
protected:
    Container& container;
public:
    front_insert_iterator(Container& x) : container(x) {}
    front_insert_iterator<Container>&
    operator=(const Container::value_type& value) { 
	container.push_front(value);
	return *this;
    }
    front_insert_iterator<Container>& operator*() { return *this; }
    front_insert_iterator<Container>& operator++() { return *this; }
    front_insert_iterator<Container>& operator++(int) { return *this; }
};
template <class Container>
front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}
template <class Container>
class insert_iterator : public output_iterator {
protected:
    Container& container;
    Container::iterator iter;
public:
    insert_iterator(Container& x, Container::iterator i) 
	: container(x), iter(i) {}
    insert_iterator<Container>&
    operator=(const Container::value_type& value) { 
	iter = container.insert(iter, value);
	++iter;
	return *this;
    }
    insert_iterator<Container>& operator*() { return *this; }
    insert_iterator<Container>& operator++() { return *this; }
    insert_iterator<Container>& operator++(int) { return *this; }
};
template <class Container, class Iterator>
insert_iterator<Container> inserter(Container& x, Iterator i) {
    return insert_iterator<Container>(x, Container::iterator(i));
}
template <class BidirectionalIterator, class T, class Reference, 
          class Distance> 
class reverse_bidirectional_iterator 
    : public bidirectional_iterator<T, Distance> {
    typedef reverse_bidirectional_iterator<BidirectionalIterator, T, Reference,
                                           Distance> self;
    friend  int  operator==(const self& x, const self& y);
protected:
    BidirectionalIterator current;
public:
    reverse_bidirectional_iterator() {}
    reverse_bidirectional_iterator(BidirectionalIterator x) : current(x) {}
    BidirectionalIterator base() { return current; }
    Reference operator*() const {
	BidirectionalIterator tmp = current;
	return *--tmp;
    }
    self& operator++() {
	--current;
	return *this;
    }
    self operator++(int) {
	self tmp = *this;
	--current;
	return tmp;
    }
    self& operator--() {
	++current;
	return *this;
    }
    self operator--(int) {
	self tmp = *this;
	++current;
	return tmp;
    }
};
template <class BidirectionalIterator, class T, class Reference,
          class Distance>
inline  int  operator==(
    const reverse_bidirectional_iterator<BidirectionalIterator, T, Reference,
		                         Distance>& x, 
    const reverse_bidirectional_iterator<BidirectionalIterator, T, Reference,
		                         Distance>& y) {
    return x.current == y.current;
}
template <class RandomAccessIterator, class T, class Reference,
          class Distance> 
class reverse_iterator : public random_access_iterator<T, Distance> {
    typedef reverse_iterator<RandomAccessIterator, T, Reference, Distance>
	self;
    friend  int  operator==(const self& x, const self& y);
    friend  int  operator<(const self& x, const self& y);
    friend Distance operator-(const self& x, const self& y);
    friend self operator+(Distance n, const self& x);
protected:
    RandomAccessIterator current;
public:
    reverse_iterator() {}
    reverse_iterator(RandomAccessIterator x) : current(x) {}
    RandomAccessIterator base() { return current; }
    Reference operator*() const { return *(current - 1); }
    self& operator++() {
	--current;
	return *this;
    }
    self operator++(int) {
	self tmp = *this;
	--current;
	return tmp;
    }
    self& operator--() {
	++current;
	return *this;
    }
    self operator--(int) {
	self tmp = *this;
	++current;
	return tmp;
    }
    self operator+(Distance n) const {
	return self(current - n);
    }
    self& operator+=(Distance n) {
	current -= n;
	return *this;
    }
    self operator-(Distance n) const {
	return self(current + n);
    }
    self& operator-=(Distance n) {
	current += n;
	return *this;
    }
    Reference operator[](Distance n) { return *(*this + n); }
};
template <class RandomAccessIterator, class T, class Reference, class Distance>
inline  int  operator==(const reverse_iterator<RandomAccessIterator, T,
		                              Reference, Distance>& x, 
		       const reverse_iterator<RandomAccessIterator, T,
		                              Reference, Distance>& y) {
    return x.current == y.current;
}
template <class RandomAccessIterator, class T, class Reference, class Distance>
inline  int  operator<(const reverse_iterator<RandomAccessIterator, T,
		                             Reference, Distance>& x, 
		      const reverse_iterator<RandomAccessIterator, T,
		                             Reference, Distance>& y) {
    return y.current < x.current;
}
template <class RandomAccessIterator, class T, class Reference, class Distance>
inline Distance operator-(const reverse_iterator<RandomAccessIterator, T,
			                         Reference, Distance>& x, 
			  const reverse_iterator<RandomAccessIterator, T,
			                         Reference, Distance>& y) {
    return y.current - x.current;
}
template <class RandomAccessIterator, class T, class Reference, class Distance>
inline reverse_iterator<RandomAccessIterator, T, Reference, Distance> 
operator+(Distance n,
	  const reverse_iterator<RandomAccessIterator, T, Reference,
	                         Distance>& x) {
    return reverse_iterator<RandomAccessIterator, T, Reference, Distance>
	(x.current - n);
}
template <class OutputIterator, class T>
class raw_storage_iterator : public output_iterator {
protected:
    OutputIterator iter;
public:
    raw_storage_iterator(OutputIterator x) : iter(x) {}
    raw_storage_iterator<OutputIterator, T>& operator*() { return *this; }
    raw_storage_iterator<OutputIterator, T>& operator=(const T& element) {
	construct(iter, element);
	return *this;
    }        
    raw_storage_iterator<OutputIterator, T>& operator++() {
	++iter;
	return *this;
    }
    raw_storage_iterator<OutputIterator, T> operator++(int) {
	raw_storage_iterator<OutputIterator, T> tmp = *this;
	++iter;
	return tmp;
    }
};
template <class T, class Distance>  
class istream_iterator : public input_iterator<T, Distance> {
friend  int  operator==(const istream_iterator<T, Distance>& x,
		       const istream_iterator<T, Distance>& y);
protected:
    istream* stream;
    T value;
     int  end_marker;
    void read() {
	end_marker = (*stream) ?  1  :  0 ;
	if (end_marker) *stream >> value;
	end_marker = (*stream) ?  1  :  0 ;
    }
public:
    istream_iterator() : stream(&cin), end_marker( 0 ) {}
    istream_iterator(istream& s) : stream(&s) { read(); }
    const T& operator*() const { return value; }
    istream_iterator<T, Distance>& operator++() { 
	read(); 
	return *this;
    }
    istream_iterator<T, Distance> operator++(int)  {
	istream_iterator<T, Distance> tmp = *this;
	read();
	return tmp;
    }
};
template <class T, class Distance>
 int  operator==(const istream_iterator<T, Distance>& x,
		const istream_iterator<T, Distance>& y) {
    return x.stream == y.stream && x.end_marker == y.end_marker ||
	x.end_marker ==  0  && y.end_marker ==  0 ;
}
template <class T>
class ostream_iterator : public output_iterator {
protected:
    ostream* stream;
    char* string;
public:
    ostream_iterator(ostream& s) : stream(&s), string(0) {}
    ostream_iterator(ostream& s, char* c) : stream(&s), string(c)  {}
    ostream_iterator<T>& operator=(const T& value) { 
	*stream << value;
	if (string) *stream << string;
	return *this;
    }
    ostream_iterator<T>& operator*() { return *this; }
    ostream_iterator<T>& operator++() { return *this; } 
    ostream_iterator<T>& operator++(int) { return *this; } 
};
template <class ForwardIterator1, class ForwardIterator2, class T>
inline void __iter_swap(ForwardIterator1 a, ForwardIterator2 b, T*) {
    T tmp = *a;
    *a = *b;
    *b = tmp;
}
template <class ForwardIterator1, class ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
    __iter_swap(a, b, value_type(a));
}
template <class T>
inline void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}
template <class T>
inline const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}
template <class T, class Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}
template <class T>
inline const T& max(const T& a, const T& b) {
    return  a < b ? b : a;
}
template <class T, class Compare>
inline const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}
template <class InputIterator, class Distance>
void __distance(InputIterator first, InputIterator last, Distance& n, 
		input_iterator_tag) {
    while (first != last) { ++first; ++n; }
}
template <class ForwardIterator, class Distance>
void __distance(ForwardIterator first, ForwardIterator last, Distance& n, 
		forward_iterator_tag) {
    while (first != last) { ++first; ++n; }
}
template <class BidirectionalIterator, class Distance>
void __distance(BidirectionalIterator first, BidirectionalIterator last, 
		Distance& n, bidirectional_iterator_tag) {
    while (first != last) { ++first; ++n; }
}
template <class RandomAccessIterator, class Distance>
inline void __distance(RandomAccessIterator first, RandomAccessIterator last, 
		       Distance& n, random_access_iterator_tag) {
    n += last - first;
}
template <class InputIterator, class Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
    __distance(first, last, n, iterator_category(first));
}
template <class InputIterator, class Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}
template <class ForwardIterator, class Distance>
void __advance(ForwardIterator& i, Distance n, forward_iterator_tag) {
    while (n--) ++i;
}
template <class BidirectionalIterator, class Distance>
void __advance(BidirectionalIterator& i, Distance n, 
	       bidirectional_iterator_tag) {
    if (n >= 0)
	while (n--) ++i;
    else
	while (n++) --i;
}
template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, 
		      random_access_iterator_tag) {
    i += n;
}
template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) {
    __advance(i, n, iterator_category(i));
}
template <class ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last) {
    while (first != last) {
	destroy(&*first);
	++first;
    }
}
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
				   ForwardIterator result) {
    while (first != last) construct(&*result++, *first++);
    return result;
}
template <class ForwardIterator, class T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, 
			const T& x) {
    while (first != last) construct(&*first++, x);
}
template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
				     const T& x) {
    while (n--) construct(&*first++, x);
    return first;
}
template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last,
		    OutputIterator result) {
    while (first != last) *result++ = *first++;
    return result;
}
template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, 
				     BidirectionalIterator1 last, 
				     BidirectionalIterator2 result) {
    while (first != last) *--result = *--last;
    return result;
}
template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& value) {
    while (first != last) *first++ = value;
}
template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
    while (n-- > 0) *first++ = value;
    return first;
}
template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
					      InputIterator1 last1,
					      InputIterator2 first2) {
    while (first1 != last1 && *first1 == *first2) {
	++first1;
	++first2;
    }
    return pair<InputIterator1, InputIterator2>(first1, first2);
}
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
					      InputIterator1 last1,
					      InputIterator2 first2,
					      BinaryPredicate binary_pred) {
    while (first1 != last1 && binary_pred(*first1, *first2)) {
	++first1;
	++first2;
    }
    return pair<InputIterator1, InputIterator2>(first1, first2);
}
template <class InputIterator1, class InputIterator2>
inline  int  equal(InputIterator1 first1, InputIterator1 last1,
		  InputIterator2 first2) {
    return mismatch(first1, last1, first2).first == last1;
}
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline  int  equal(InputIterator1 first1, InputIterator1 last1,
		  InputIterator2 first2, BinaryPredicate binary_pred) {
    return mismatch(first1, last1, first2, binary_pred).first == last1;
}
template <class InputIterator1, class InputIterator2>
 int  lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
			     InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && first2 != last2) {
	if (*first1 < *first2) return  1 ;
	if (*first2++ < *first1++) return  0 ;
    }
    return first1 == last1 && first2 != last2;
}
template <class InputIterator1, class InputIterator2, class Compare>
 int  lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
			     InputIterator2 first2, InputIterator2 last2,
			     Compare comp) {
    while (first1 != last1 && first2 != last2) {
	if (comp(*first1, *first2)) return  1 ;
	if (comp(*first2++, *first1++)) return  0 ;
    }
    return first1 == last1 && first2 != last2;
}
typedef void (*PFV)( void );
typedef int (*PFU)( unsigned );
typedef int (*_PNH)( unsigned );
extern void *operator new( size_t );
extern void *operator new( size_t, void * );
extern void *operator new []( size_t );
extern void *operator new []( size_t, void * );
extern void operator delete( void * );
extern void operator delete []( void * );
   extern PFV set_new_handler( PFV );
   extern PFU _set_new_handler( PFU );
inline void* operator new(size_t, void* p) {return p;}
template <class T>
inline T* allocate(int size, T*) {
    set_new_handler(0);
    T* tmp = (T*)(::operator new((unsigned int)(size * sizeof(T))));
    if (tmp == 0) {
	cerr << "out of memory" << endl; 
	exit(1);
    }
    return tmp;
}
template <class T>
inline T* allocate(long size, T*) {
    set_new_handler(0);
    T* tmp = (T*)(::operator new((unsigned long)(size * sizeof(T))));
    if (tmp == 0) {
	cerr << "out of memory" << endl; 
	exit(1);
    }
    return tmp;
}
template <class T>
inline void deallocate(T* buffer) {
    ::operator delete(buffer);
}
template <class T>
inline void destroy(T* pointer) {
    pointer->~T();
}
inline void destroy(char*) {}
inline void destroy(unsigned char*) {}
inline void destroy(short*) {}
inline void destroy(unsigned short*) {}
inline void destroy(int*) {}
inline void destroy(unsigned int*) {}
inline void destroy(long*) {}
inline void destroy(unsigned long*) {}
inline void destroy(float*) {}
inline void destroy(double*) {}
inline void destroy(char**) {}
inline void destroy(unsigned char**) {}
inline void destroy(short**) {}
inline void destroy(unsigned short**) {}
inline void destroy(int**) {}
inline void destroy(unsigned int**) {}
inline void destroy(long**) {}
inline void destroy(unsigned long**) {}
inline void destroy(float**) {}
inline void destroy(double**) {}
inline void destroy(char*, char*) {}
inline void destroy(unsigned char*, unsigned char*) {}
inline void destroy(short*, short*) {}
inline void destroy(unsigned short*, unsigned short*) {}
inline void destroy(int*, int*) {}
inline void destroy(unsigned int*, unsigned int*) {}
inline void destroy(long*, long*) {}
inline void destroy(unsigned long*, unsigned long*) {}
inline void destroy(float*, float*) {}
inline void destroy(double*, double*) {}
inline void destroy(char**, char**) {}
inline void destroy(unsigned char**, unsigned char**) {}
inline void destroy(short**, short**) {}
inline void destroy(unsigned short**, unsigned short**) {}
inline void destroy(int**, int**) {}
inline void destroy(unsigned int**, unsigned int**) {}
inline void destroy(long**, long**) {}
inline void destroy(unsigned long**, unsigned long**) {}
inline void destroy(float**, float**) {}
inline void destroy(double**, double**) {}
template <class T1, class T2>
inline void construct(T1* p, const T2& value) {
    new (p) T1(value);
}
template <class T>
class allocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    pointer allocate(size_type n) { 
	return ::allocate((difference_type)n, (pointer)0);
    }
    void deallocate(pointer p) { ::deallocate(p); }
    pointer address(reference x) { return (pointer)&x; }
    const_pointer const_address(const_reference x) { 
	return (const_pointer)&x; 
    }
    size_type init_page_size() { 
	return max(size_type(1), size_type(4096/sizeof(T))); 
    }
    size_type max_size() const { 
	return max(size_type(1), size_type( 4294967295U /sizeof(T))); 
    }
};
class allocator<void> {
public:
    typedef void* pointer;
};
template <class T>
class  vector  {
public:
    typedef  allocator <T> vector_allocator;
    typedef T value_type;
    typedef vector_allocator::pointer pointer;
    typedef vector_allocator::pointer iterator;
    typedef vector_allocator::const_pointer const_iterator;
    typedef vector_allocator::reference reference;
    typedef vector_allocator::const_reference const_reference;
    typedef vector_allocator::size_type size_type;
    typedef vector_allocator::difference_type difference_type;
    typedef reverse_iterator<const_iterator, value_type, const_reference, 
                             difference_type>  const_reverse_iterator;
    typedef reverse_iterator<iterator, value_type, reference, difference_type>
        reverse_iterator;
protected:
    static  allocator <T> static_allocator;
    iterator start;
    iterator finish;
    iterator end_of_storage;
    void insert_aux(iterator position, const T& x);
public:
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const { return finish; }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin()); 
    }
    size_type size() const { return size_type(end() - begin()); }
    size_type max_size() const { return static_allocator.max_size(); }
    size_type capacity() const { return size_type(end_of_storage - begin()); }
     int  empty() const { return begin() == end(); }
    reference operator[](size_type n) { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }
     vector () : start(0), finish(0), end_of_storage(0) {}
     vector (size_type n, const T& value = T()) {
	start = static_allocator.allocate(n);
	uninitialized_fill_n(start, n, value);
	finish = start + n;
	end_of_storage = finish;
    }
     vector (const  vector <T>& x) {
	start = static_allocator.allocate(x.end() - x.begin());
	finish = uninitialized_copy(x.begin(), x.end(), start);
	end_of_storage = finish;
    }
     vector (const_iterator first, const_iterator last) {
	size_type n = 0;
	distance(first, last, n);
	start = static_allocator.allocate(n);
	finish = uninitialized_copy(first, last, start);
	end_of_storage = finish;
    }
    ~ vector () { 
	destroy(start, finish);
	static_allocator.deallocate(start);
    }
     vector <T>& operator=(const  vector <T>& x);
    void reserve(size_type n) {
	if (capacity() < n) {
	    iterator tmp = static_allocator.allocate(n);
	    uninitialized_copy(begin(), end(), tmp);
	    destroy(start, finish);
	    static_allocator.deallocate(start);
	    finish = tmp + size();
	    start = tmp;
	    end_of_storage = begin() + n;
	}
    }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }
    void push_back(const T& x) {
	if (finish != end_of_storage) {
	    construct(finish, x);
	    finish++;
	} else
	    insert_aux(end(), x);
    }
    void swap( vector <T>& x) {
	::swap(start, x.start);
	::swap(finish, x.finish);
	::swap(end_of_storage, x.end_of_storage);
    }
    iterator insert(iterator position, const T& x) {
	size_type n = position - begin();
	if (finish != end_of_storage && position == end()) {
	    construct(finish, x);
	    finish++;
	} else
	    insert_aux(position, x);
	return begin() + n;
    }
    void insert (iterator position, const_iterator first, 
		 const_iterator last);
    void insert (iterator position, size_type n, const T& x);
    void pop_back() {
        --finish;
        destroy(finish);
    }
    void erase(iterator position) {
	if (position + 1 != end())
	    copy(position + 1, end(), position);
	--finish;
	destroy(finish);
    }
    void erase(iterator first, iterator last) {
	 vector <T>::iterator i = copy(last, end(), first);
	destroy(i, finish);
	finish = finish - (last - first); 
    }
};
template <class T>
inline  int  operator==(const  vector <T>& x, const  vector <T>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}
template <class T>
inline  int  operator<(const  vector <T>& x, const  vector <T>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}
template <class T>
 vector <T>::vector_allocator  vector <T>::static_allocator;
template <class T>
 vector <T>&  vector <T>::operator=(const  vector <T>& x) {
    if (&x == this) return *this;
    if (x.size() > capacity()) {
	destroy(start, finish);
	static_allocator.deallocate(start);
	start = static_allocator.allocate(x.end() - x.begin());
	end_of_storage = uninitialized_copy(x.begin(), x.end(), start);
    } else if (size() >= x.size()) {
	 vector <T>::iterator i = copy(x.begin(), x.end(), begin());
	destroy(i, finish);
    } else {
	copy(x.begin(), x.begin() + size(), begin());
	uninitialized_copy(x.begin() + size(), x.end(), begin() + size());
    }
    finish = begin() + x.size();
    return *this;
}
template <class T>
void  vector <T>::insert_aux(iterator position, const T& x) {
    if (finish != end_of_storage) {
	construct(finish, *(finish - 1));
	copy_backward(position, finish - 1, finish);
	*position = x;
	++finish;
    } else {
	size_type len = size() ? 2 * size() 
	    : static_allocator.init_page_size();
	iterator tmp = static_allocator.allocate(len);
	uninitialized_copy(begin(), position, tmp);
	construct(tmp + (position - begin()), x);
	uninitialized_copy(position, end(), tmp + (position - begin()) + 1); 
	destroy(begin(), end());
	static_allocator.deallocate(begin());
	end_of_storage = tmp + len;
	finish = tmp + size() + 1;
	start = tmp;
    }
}
template <class T>
void  vector <T>::insert(iterator position, size_type n, const T& x) {
    if (n == 0) return;
    if (end_of_storage - finish >= n) {
	if (end() - position > n) {
	    uninitialized_copy(end() - n, end(), end());
	    copy_backward(position, end() - n, end());
	    fill(position, position + n, x);
	} else {
	    uninitialized_copy(position, end(), position + n);
	    fill(position, end(), x);
	    uninitialized_fill_n(end(), n - (end() - position), x);
	}
	finish += n;
    } else {
	size_type len = size() + max(size(), n);
	iterator tmp = static_allocator.allocate(len);
	uninitialized_copy(begin(), position, tmp);
	uninitialized_fill_n(tmp + (position - begin()), n, x);
	uninitialized_copy(position, end(), tmp + (position - begin() + n));
	destroy(begin(), end());
	static_allocator.deallocate(begin());
	end_of_storage = tmp + len;
	finish = tmp + size() + n;
	start = tmp;
    }
}
template <class T>
void  vector <T>::insert(iterator position, 
		       const_iterator first, 
		       const_iterator last) {
    if (first == last) return;
    size_type n = 0;
    distance(first, last, n);
    if (end_of_storage - finish >= n) {
	if (end() - position > n) {
	    uninitialized_copy(end() - n, end(), end());
	    copy_backward(position, end() - n, end());
	    copy(first, last, position);
	} else {
	    uninitialized_copy(position, end(), position + n);
	    copy(first, first + (end() - position), position);
	    uninitialized_copy(first + (end() - position), last, end());
	}
	finish += n;
    } else {
	size_type len = size() + max(size(), n);
	iterator tmp = static_allocator.allocate(len);
	uninitialized_copy(begin(), position, tmp);
	uninitialized_copy(first, last, tmp + (position - begin()));
	uninitialized_copy(position, end(), tmp + (position - begin() + n));
	destroy(begin(), end());
	static_allocator.deallocate(begin());
	end_of_storage = tmp + len;
	finish = tmp + size() + n;
	start = tmp;
    }
}
const size_t NPOS  = (size_t)(-1);
enum capacity { default_size, reserve };
template<class charT>
struct  string_char_baggage {
    typedef charT char_type;
    static void
    assign (char_type& c1, const char_type& c2)   
    {
        c1 = c2;
    }
    static  int 
    eq (const char_type& c1, const char_type& c2)   
    {
        return (c1 == c2);
    }
    static  int 
    ne (const char_type& c1, const char_type& c2)   
    {
        return !(c1 == c2);
    }
    static  int 
    lt (const char_type& c1, const char_type& c2)   
    {
        return (c1 < c2);
    }
    static char_type
    eos ()   
    {
        return char_type();      
    }
    static istream&
    char_in (istream& is, char_type& c)   
    {
        return is >> c;         
    }
    static ostream&
    char_out (ostream& os, char_type c)   
    {
        return os << c;         
    }
    static  int 
    is_del (char_type c)   
    {
        return  (_IsTable[(unsigned char)((c)+1)] & 0x02) ;
    }
    static int
    compare (const char_type* s1, const char_type* s2, size_t n)   
    {
        for (size_t i = 0; i < n; ++i, ++s1, ++s2)
            if (ne(*s1, *s2))
            {
                return lt(*s1, *s2) ? -1 : 1;
            }
        return 0;
    }
    static size_t
    length (const char_type* s)   
    {
        size_t l = 0;
        while (ne(*s++, eos()))
            ++l;
        return l;
    }
    static char_type*
    copy (char_type* s1, const char_type* s2, size_t n)   
    {
        char_type* s = s1;
        for (size_t i = 0; i < n; ++i)
            assign(*++s1, *++s2);
        return s;
    }
};
struct string_char_baggage<char> {
    typedef char char_type;
    static void
    assign (char_type& c1, const char_type& c2)   
    {
        c1 = c2;
    }
    static  int 
    eq (const char_type& c1, const char_type& c2)   
    {
        return (c1 == c2);
    }
    static  int 
    ne (const char_type& c1, const char_type& c2)   
    {
        return (c1 != c2);
    }
    static  int 
    lt (const char_type& c1, const char_type& c2)   
    {
        return (c1 < c2);
    }
    static char_type
    eos ()   
    {
        return 0;      
    }
    static istream&
    char_in (istream& is, char_type& c)   
    {
       is.get(c);
       return is;
    }
    static ostream&
    char_out (ostream& os, char_type c)   
    {
        return os << c;         
    }
    static  int 
    is_del (char_type c)   
    {
        return  (_IsTable[(unsigned char)((c)+1)] & 0x02) ;
    }
    static int
    compare (const char_type* s1, const char_type* s2, size_t n)   
    {
        return memcmp(s1, s2, n);
    }
    static size_t
    length (const char_type* s)   
    {
        return strlen(s);
    }
    static char_type*
    copy (char_type* s1, const char_type* s2, size_t n)   
    {
        return (char_type*)memcpy(s1, s2, n);
    }
};
template <class charT>
class basic_string;
template <class charT>
class basic_string_ref {
friend class basic_string<charT>;
typedef  string_char_baggage<charT>  baggage_type;
    charT*   ptr;
    size_t   len;
    size_t   res;
    size_t                       count;    
    basic_string_ref ()    ;
    basic_string_ref (size_t size, capacity cap)    ;
    basic_string_ref (const basic_string<charT>& str, size_t pos , size_t rlen)
                         ;
    basic_string_ref (const charT* s, size_t rlen, size_t rres)    ;
    basic_string_ref (const charT* s, size_t n)    ;
    basic_string_ref (const charT* s)    ;
    basic_string_ref (charT c, size_t rep)    ;
    basic_string_ref (const vector<charT>& vec)    ;
    ~basic_string_ref ()    ;
    inline void
    delete_ptr ()    ;
    inline static
    charT
    eos ()    ;
    inline static
    void
    throwlength ()   ;
    inline static
    void
    throwrange ()   ;
};
template<class charT>
class basic_string {
private:
    typedef basic_string_ref<charT>   reference_class;
    typedef basic_string_ref<charT>*  reference_pointer;
    charT*             c_str_ptr;
    reference_pointer  reference;
    inline charT*
    point ()    ;
    inline size_t&
    len ()    ;
    inline size_t
    ref_count () const    ;
    inline static
    charT
    eos ()    ;
    void
    assign_str (const charT* s, size_t slen)    ;
    void
    append_str (const charT* s, size_t slen)    ;
    void
    insert_str (size_t pos, const charT* s, size_t slen) 
                   ;
    void
    replace_str (size_t xlen, size_t pos, const charT* s, size_t slen) 
                    ;
    int
    compare_str (size_t pos, const charT* str, size_t slen, size_t strlen) 
                 const    ;
    size_t
    find_str (const charT* s, size_t pos, size_t len) const    ;
    size_t
    rfind_str (const charT* s, size_t pos, size_t len) const    ;
    size_t
    find_first_of_str (const charT* s, size_t pos, size_t len) const 
                         ;
    size_t
    find_last_of_str (const charT* s, size_t pos, size_t len) const 
                        ;
    size_t
    find_first_not_of_str (const charT* s, size_t pos, size_t len) const 
                             ;
    size_t
    find_last_not_of_str (const charT* s, size_t pos, size_t len) const 
                            ;
protected:
    basic_string (const charT* s, size_t rlen, size_t xlen)   ;
    inline void
    delete_ref ()    ;
public:
    typedef  charT                       char_type;
    typedef  string_char_baggage<charT>  baggage_type;
    basic_string ()    ;
    basic_string (size_t size, capacity cap)    ;
    basic_string (const basic_string<charT>& str, size_t pos = 0, size_t n = NPOS)
                     ;
    basic_string (const charT* s, size_t n)    ;
    basic_string (const charT* s)    ;
    basic_string (charT c, size_t rep = 1)    ;
    basic_string (const vector<charT>& vec)    ;
    ~basic_string ()    ;
    basic_string<charT>&
    operator= (const basic_string<charT>& str)    ;
    basic_string<charT>&
    operator= (const charT* s)    ;
    basic_string<charT>&
    operator= (charT c)    ;
    basic_string<charT>&
    operator+= (const basic_string<charT>& rhs)    ;
    basic_string<charT>&
    operator+= (const charT* s)    ;
    basic_string<charT>&
    operator+= (charT c)    ;
    operator vector<charT> () const    { 
        return vector<charT> (data(), data()+length());
	}
    basic_string<charT>&
    append (const basic_string<charT>& str, size_t pos = 0, size_t n = NPOS)
               ;
    basic_string<charT>&
    append (const charT* s, size_t n)    ;
    basic_string<charT>&
    append (const charT* s)    ;
    basic_string<charT>&
    append (charT c, size_t rep = 1)    ;
    basic_string<charT>&
    assign (const basic_string<charT>& str, size_t pos = 0, size_t n = NPOS)
               ;
    basic_string<charT>&
    assign (const charT* s, size_t n)    ;
    basic_string<charT>&
    assign (const charT* s)    ;
    basic_string<charT>&
    assign (charT c, size_t rep = 1)    ;
    basic_string<charT>&
    insert (size_t pos1, const basic_string<charT>& str, size_t pos2 = 0,
            size_t n = NPOS)    ;
    basic_string<charT>&
    insert (size_t pos, const charT* s, size_t n)    ;
    basic_string<charT>&
    insert (size_t pos, const charT* s)    ;
    basic_string<charT>&
    insert (size_t pos, charT c, size_t rep = 1)    ;
    basic_string<charT>&
    remove (size_t pos = 0, size_t n = NPOS)    ;
    basic_string<charT>&
    replace (size_t pos1, size_t n1, const basic_string<charT>& str, size_t pos2 = 0,
             size_t n2 = NPOS)    ;
    basic_string<charT>&
    replace (size_t pos, size_t n1, const charT* s, size_t n2)
                ;
    basic_string<charT>&
    replace (size_t pos, size_t n1, const charT* s)
                ;
    basic_string<charT>&
    replace (size_t pos, size_t n, charT c, size_t rep = 1)
                ;
    inline charT
    get_at (size_t pos) const    ;
    void
    put_at (size_t pos, charT c)    ;
    inline charT
    operator[] (size_t pos) const    ;
    charT&
    operator[] (size_t pos)    ;
    const charT*
    c_str () const    ;
    inline const charT*
    data () const    ;
    inline size_t
    length () const    ;
    void
    resize (size_t n, charT c)    ;
    void
    resize (size_t n)    ;
    inline size_t
    reserve () const    ;
    void
    reserve (size_t res_arg)    ;
    size_t
    copy (charT* s, size_t n, size_t pos = 0) const    ;
    size_t
    find (const basic_string<charT>& str, size_t pos = 0) const    ;
    size_t
    find (const charT* s, size_t pos, size_t n) const    ;
    size_t
    find (const charT* s, size_t pos = 0) const    ;
    size_t
    find (charT c, size_t pos = 0) const    ;
    size_t
    rfind (const basic_string<charT>& str, size_t pos = NPOS) const    ;
    size_t
    rfind (const charT* s, size_t pos, size_t n) const    ;
    size_t
    rfind (const charT* s, size_t pos = NPOS) const    ;
    size_t
    rfind (charT c, size_t pos = NPOS) const    ;
    size_t
    find_first_of (const basic_string<charT>& str, size_t pos = 0) const    ;
    size_t
    find_first_of (const charT* s, size_t pos, size_t n) const    ;
    size_t
    find_first_of (const charT* s, size_t pos = 0) const    ;
    size_t
    find_first_of (charT c, size_t pos = 0) const    ;
    size_t
    find_last_of (const basic_string<charT>& str, size_t pos = NPOS) const
                     ;
    size_t
    find_last_of (const charT* s, size_t pos, size_t n) const    ;
    size_t
    find_last_of (const charT* s, size_t pos = NPOS) const    ;
    size_t
    find_last_of (charT c, size_t pos = NPOS) const    ;
    size_t
    find_first_not_of (const basic_string<charT>& str, size_t pos = 0) const
                          ;
    size_t
    find_first_not_of (const charT* s, size_t pos, size_t n) const    ;
    size_t
    find_first_not_of (const charT* s, size_t pos = 0) const    ;
    size_t
    find_first_not_of (charT c, size_t pos = 0) const    ;
    size_t
    find_last_not_of (const basic_string<charT>& str, size_t pos = NPOS) const
                         ;
    size_t
    find_last_not_of (const charT* s, size_t pos, size_t n) const    ;
    size_t
    find_last_not_of (const charT* s, size_t pos = NPOS) const    ;
    size_t
    find_last_not_of (charT c, size_t pos = NPOS) const    ;
    basic_string<charT>
    substr (size_t pos = 0,  size_t n = NPOS) const    ;
    int
    compare (const basic_string<charT>& str, size_t pos = 0, size_t n = NPOS) const
                ;
    int
    compare (const charT* s, size_t pos, size_t n) const
                ;
    int
    compare (const charT* s, size_t pos = 0) const    ;
    int
    compare (charT c, size_t pos = 0, size_t rep = 1) const 
                ;
    friend
    ostream&
    operator<< (ostream& o, const basic_string<charT>& s)    ;
    friend
    istream&
    operator>> (istream& i, basic_string<charT>& s)    ;
    friend
    basic_string<charT>
    operator+ (const basic_string<charT>& lhs, const basic_string<charT>& rhs)
       ;
    friend
    basic_string<charT>
    operator+ (const charT* lhs, const basic_string<charT>& rhs)
       ;
    friend
    basic_string<charT>
    operator+ (charT lhs, const basic_string<charT>& rhs)    ;
    friend
    basic_string<charT>
    operator+ (const basic_string<charT>& lhs, const charT* rhs)
       ;
    friend
    basic_string<charT>
    operator+ (const basic_string<charT>& lhs, charT rhs)    ;
};
template <class charT>
inline void
basic_string_ref<charT>::delete_ptr ()   
{
    if (res)
    {
        delete[] ptr;
        res = 0;
        ptr = 0;
    }
}
template <class charT>
inline void
basic_string_ref<charT>::throwlength ()    
{
   cout << "Length exception occurred" << endl;
   exit(1);
}
template <class charT>
inline void
basic_string_ref<charT>::throwrange ()   
{
   cout << "Out of range exception occurred" << endl;
   exit(1);
}
template <class charT>
inline void
basic_string<charT>::delete_ref ()   
{
    --(reference->count);
    if (!(reference->count))
       delete reference;
}
template <class charT>
inline size_t
basic_string<charT>::ref_count () const   
{
    return reference->count;
}
template <class charT>
inline const charT*
basic_string<charT>::data () const   
{
    if (length())
        return reference->ptr;
    else
        return 0;
}
template <class charT>
inline charT*
basic_string<charT>::point ()   
{
    return reference->ptr;
}
template <class charT>
inline size_t&
basic_string<charT>::len ()   
{
    return reference->len;
}
template <class charT>
inline size_t
basic_string<charT>::length () const   
{
    return reference->len;
}
template <class charT>
inline size_t
basic_string<charT>::reserve () const   
{
    return reference->res;
}
template <class charT>
inline charT
basic_string<charT>::get_at (size_t pos) const   
{
    if (pos >= length())
    {
        reference_class::throwrange();
    }
    return *(data()+pos);
}
template <class charT>
inline charT
basic_string<charT>::operator[] (size_t pos) const   
{
    if (pos < length())
        return *(data()+pos);
    else
        return 0;
}
template <class charT>
inline  int 
operator== (const basic_string<charT>& lhs, const basic_string<charT>& rhs)
{
    return !(lhs.compare(rhs));
}
template <class charT>
inline  int 
operator== (const charT* lhs, const basic_string<charT>& rhs)   
{
    return !(rhs.compare(lhs));
}
template <class charT>
inline  int 
operator== (charT lhs, const basic_string<charT>& rhs)   
{
    return !(rhs.compare(lhs));
}
template <class charT>
inline  int 
operator== (const basic_string<charT>& lhs, const charT* rhs)   
{
    return !(lhs.compare(rhs));
}
template <class charT>
inline  int 
operator== (const basic_string<charT>& lhs, charT rhs)   
{
    return !(lhs.compare(rhs));
}
template <class charT>
inline  int 
operator!= (const charT* lhs, const basic_string<charT>& rhs)   
{
    return rhs.compare(lhs);
}
template <class charT>
inline  int 
operator!= (charT lhs, const basic_string<charT>& rhs)   
{
    return rhs.compare(lhs);
}
template <class charT>
inline  int 
operator!= (const basic_string<charT>& lhs, const charT* rhs)   
{
    return lhs.compare(rhs);
}
template <class charT>
inline  int 
operator!= (const basic_string<charT>& lhs, charT rhs)   
{
    return lhs.compare(rhs);
}
template <class charT>
inline  int 
operator< (const basic_string<charT>& lhs, const basic_string<charT>& rhs)
{
    if (lhs.compare(rhs) < 0)
        return  1 ;
    else
        return  0 ;
}
template <class charT>
inline  int 
operator< (const charT* lhs, const basic_string<charT>& rhs)   
{
    if (rhs.compare(lhs) > 0)
        return  1 ;
    else
        return  0 ;
}
template <class charT>
inline  int 
operator< (charT lhs, const basic_string<charT>& rhs)   
{
    if (rhs.compare(lhs) > 0)
        return  1 ;
    else
        return  0 ;
}
template <class charT>
inline  int 
operator< (const basic_string<charT>& lhs, const charT* rhs)   
{
    if (lhs.compare(rhs) < 0)
        return  1 ;
    else
        return  0 ;
}
template <class charT>
inline  int 
operator< (const basic_string<charT>& lhs, charT rhs)   
{
    if (lhs.compare(rhs) < 0)
        return  1 ;
    else
        return  0 ;
}
template <class charT>
inline  int 
operator> (const charT* lhs, const basic_string<charT>& rhs)   
{
    return (rhs < lhs);
}
template <class charT>
inline  int 
operator> (charT lhs, const basic_string<charT>& rhs)   
{
    return (rhs < lhs);
}
template <class charT>
inline  int 
operator> (const basic_string<charT>& lhs, const charT* rhs)   
{
    return (rhs < lhs);
}
template <class charT>
inline  int 
operator> (const basic_string<charT>& lhs, charT rhs)   
{
    return (rhs < lhs);
}
template <class charT>
inline  int 
operator>= (const charT* lhs, const basic_string<charT>& rhs)   
{
    return !(lhs < rhs);
}
template <class charT>
inline  int 
operator>= (charT lhs, const basic_string<charT>& rhs)   
{
    return !(lhs < rhs);
}
template <class charT>
inline  int 
operator>= (const basic_string<charT>& lhs, const charT* rhs)   
{
    return !(lhs < rhs);
}
template <class charT>
inline  int 
operator>= (const basic_string<charT>& lhs, charT rhs)   
{
    return !(lhs < rhs);
}
template <class charT>
inline  int 
operator<= (const charT* lhs, const basic_string<charT>& rhs)   
{
    return !(rhs < lhs);
}
template <class charT>
inline  int 
operator<= (charT lhs, const basic_string<charT>& rhs)   
{
    return !(rhs < lhs);
}
template <class charT>
inline  int 
operator<= (const basic_string<charT>& lhs, const charT* rhs)   
{
    return !(rhs < lhs);
}
template <class charT>
inline  int 
operator<= (const basic_string<charT>& lhs, charT rhs)   
{
    return !(rhs < lhs);
}
template <class charT>
charT
basic_string_ref<charT>::eos ()   
{
    return baggage_type::eos();
}
template <class charT>
basic_string_ref<charT>::basic_string_ref ()   
{
     res = len = 0;
     ptr = 0;
     count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (size_t size, capacity cap)
{
    if (cap == ::reserve)
    {
       len = 0;
       res = size;
       ptr = new charT [res];
    }
    else if ((cap == ::default_size) && (size != NPOS))
    {
       res = len = size;
       if (res)
       {
          ptr = new charT [res];
          for (size_t position = 0; position < len; ++position)
              baggage_type::assign (*(ptr+position), eos());
       }
       else
          ptr = 0;
    }
    else
    {
       throwlength();
    }
    count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (const basic_string<charT>& str,
                                           size_t pos, size_t rlen)   
{
    res = len = rlen;
    if (res)
    {
       ptr = new charT [res];
       baggage_type::copy (ptr, str.data()+pos, len);
    }
    else
       ptr = 0;
    count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (const charT* s, size_t rlen,
                                           size_t rres)   
{
    res = rres;
    len = rlen;
    if (res)
    {
       ptr = new charT [res];
       if (len)
          baggage_type::copy (ptr, s, len);
    }
    else
       ptr = 0;
    count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (const charT* s, size_t n)
{
    if (n == NPOS)
    {
       throwlength();
    }
    res = len = n;
    if (res)
    {
       ptr = new charT [res];
       baggage_type::copy (ptr, s, len);
    }
    else
       ptr = 0;
    count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (const charT* s)   
{
    res = len = baggage_type::length(s);
    if (res)
    {
       ptr = new charT [res];
       baggage_type::copy (ptr, s, len);
    }
    else
       ptr = 0;
    count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (charT c, size_t rep)
{
    if (rep == NPOS)
    {
       throwlength();
    }
    res = len = rep;
    if (res)
    {
       ptr = new charT [res];
       for (size_t  position = 0; position < len; ++position)
            baggage_type::assign (*(ptr+position), c);
    }
    else
       ptr = 0;
    count = 1;
}
template <class charT>
basic_string_ref<charT>::basic_string_ref (const vector<charT>& vec)
{
    size_t  n = vec.size();
    if (n == NPOS)
    {
        throwlength();
    }
    res = len = n;
    if (res)
    {
       ptr = new charT [res];
       baggage_type::copy (ptr, vec.begin(), len);
    }
    else
       ptr = 0;
    count = 1;
}
template <class charT>
basic_string_ref<charT>::~basic_string_ref ()   
{
    delete_ptr();
}
template <class charT>
charT
basic_string<charT>::eos ()   
{
    return baggage_type::eos();
}
template <class charT>
void
basic_string<charT>::assign_str (const charT* s, size_t slen)
{
    if (slen == NPOS)
    {
        reference_class::throwlength();
    }
    if ((ref_count() > 1) || (slen && (reserve() < slen)))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (s, slen);
        delete_ref();
        reference = tmp;
    }
    else if (slen)
    {
        baggage_type::copy (point(), s, slen);
    }
    reference->len = slen;
}
template <class charT>
void
basic_string<charT>::append_str (const charT* s, size_t slen)
{
    if (length() >= (NPOS-slen))
    {
        reference_class::throwlength();
    }
    if ((ref_count() > 1) || (slen > (reserve()-length())))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), length(), length()+slen);
        delete_ref();
        reference = tmp;
    }
    if (slen)
        baggage_type::copy (point()+length(), s, slen);
    reference->len += slen;
}
template <class charT>
void
basic_string<charT>::insert_str (size_t pos, const charT* s, size_t slen)
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    if (length() >= (NPOS-slen))
    {
        reference_class::throwlength();
    }
    if ((ref_count() > 1) || (slen > (reserve()-length())))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), pos, length()+slen);
        baggage_type::copy (tmp->ptr+pos+slen, data()+pos, length()-pos);
        tmp->len = length();
        delete_ref();
        reference = tmp;
    }
    else
    {
        for (size_t count = length()-pos; count > 0; --count)
             baggage_type::assign (*(point()+pos+slen+count-1),
                                   *(data()+pos+count-1));
    }
    if (slen)
        baggage_type::copy (point()+pos, s, slen);
    reference->len += slen;
}
template <class charT>
void
basic_string<charT>::replace_str (size_t xlen, size_t pos, const charT* s,
                     size_t slen)   
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    if ((length()-xlen) >= (NPOS-slen))
    {
        reference_class::throwlength();
    }
    if ((ref_count() > 1) || (reserve() < (length()+slen-xlen)))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), pos, length()+slen-xlen);
        baggage_type::copy (tmp->ptr+pos+slen, data()+pos+xlen,
                            length()-pos-xlen);
        tmp->len = length();
        delete_ref();
        reference = tmp;
    }
    else 
    {
        if (slen < xlen)
            baggage_type::copy (point()+pos+slen, data()+pos+xlen,
                                length()-pos-xlen);
        else
        {
            for (size_t count = length()-pos-xlen; count > 0; --count)
                baggage_type::assign (*(point()+pos+slen+count-1),
                                      *(data()+pos+xlen+count-1));
        }
    }
    if (slen)
        baggage_type::copy (point()+pos, s, slen);
    reference->len += (slen-xlen);
}
template <class charT>
int
basic_string<charT>::compare_str (size_t pos, const charT* str, size_t slen,
                     size_t strlen) const   
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    size_t rlen = (slen > strlen ) ?  strlen  : slen;
    int result;
    if (!length())
        return str ? (eos()- *str) : eos();
    result =  baggage_type::compare (data()+pos, str, rlen);
    return result ? result : (length()-pos-strlen);
}
template <class charT>
size_t
basic_string<charT>::find_str (const charT* s, size_t pos, size_t len) 
                               const   
{
    size_t  count = pos;
    size_t  shift;
    size_t  place;
    if ((length() == 0) || (len == 0))
        return NPOS;
    while (len <= (length()-count))
    {
        for (place = 0; place < len; ++place)
        {
            if (baggage_type::ne(*(s+len-1-place), *(data()+count+(len-1-place))))
                break;
        }
        if (place == len)
            return count;
        shift = find(*(s+len-1-place), count+(len-place));
        if (shift == NPOS)
            return NPOS;
        count = shift-(len-place-1);
    }
    return NPOS;
}
template <class charT>
size_t
basic_string<charT>::rfind_str (const charT* s, size_t pos, size_t len) 
                                const   
{
    size_t  count = (pos < (length()-len)) ? (pos+1) : (length()-len);
    size_t  shift;
    size_t  place;    
    if ((length() < len) || (len == 0))
        return NPOS;
    while (count > 0)
    {
        for (place = 0; place < len; ++place)
        {
            if (baggage_type::ne(*(s+len-1-place), *(data()+count+(len-place)-2)))
                break;
        }
        if (place == len)
            return count-1;
        shift = rfind(*(s+len-1-place), count+(len-place)-3);
        if (shift == NPOS)
            return NPOS;
        count = shift+place-len+2;
    }
    return NPOS;
}
template <class charT>
size_t
basic_string<charT>::find_first_of_str (const charT* s, size_t pos, size_t len) 
                                        const   
{
    size_t temp;
    size_t count  = pos;
    size_t result = NPOS;
    while (count < length())
    {
        temp = 0;
        while ((temp < len) && baggage_type::ne(*(data()+count), *(s+temp)))
            ++temp;
        if (temp != len)
            break;
        ++count;
    }
    temp = (count >= length()) ? NPOS : count;
    return ((result > temp) ? temp : result);
}
template <class charT>
size_t
basic_string<charT>::find_last_of_str (const charT* s, size_t pos, size_t len) 
                                       const   
{
    size_t temp = 0;
    size_t count = (pos < length()) ? (pos+1) : length();
    if (length())
    {
       while (count > 0)
       {
           temp = 0;
           --count;
           while ((temp != len) && baggage_type::ne(*(data()+count), *(s+temp)))
               ++temp;
           if (temp != len)
               break;
       }
    }
    return ((temp != len) && length()) ? count : NPOS;
}
template <class charT>
size_t
basic_string<charT>::find_first_not_of_str (const charT* s, size_t pos,
                     size_t len) const   
{
    size_t count = pos;
    while (count < length())
    {
        size_t temp  = 0;
        while (temp < len)
        {
            if (baggage_type::eq(*(data()+count), *(s+temp)))
                break;
            ++temp;
        }
        if (temp == len)
            break;
        ++count;
    }
    return  ((count >= length()) ? NPOS : count);
}
template <class charT>
size_t
basic_string<charT>::find_last_not_of_str (const charT* s, size_t pos,
                     size_t len) const   
{
    size_t temp = 0;
    size_t count = (pos < length()) ? (pos+1) : length();
    if (length())
    {
        while (count > 0)
        {
           temp = 0;
           while (temp != len)
           {
               if (baggage_type::eq(*(data()+count-1), *(s+temp)))
                   break;
               ++temp;
           }
           if (temp == len)
               break;
           --count;
       }
    }
    return ((temp == len) && length()) ? count-1 : NPOS;
}
template <class charT>
basic_string<charT>::basic_string ()   
{
    reference = new basic_string_ref<charT> ();
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (size_t size, capacity cap)
{
    reference = new basic_string_ref<charT> (size, cap);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (const basic_string<charT>& str,
                                   size_t pos, size_t n)   
{
    if (pos > str.length())
    {
       reference_class::throwrange();
    }
    size_t rlen =  (n > (str.length() - pos)) ? str.length() - pos : n;
    if ((rlen == str.length()) && (str.ref_count() != NPOS))
       (reference = str.reference)->count++;
    else
        reference = new basic_string_ref<charT> (str, pos, rlen);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (const charT* s, size_t rlen, size_t xlen)
{
    if (rlen >= (NPOS - xlen))
    {
        reference_class::throwlength();
    }
    reference = new basic_string_ref<charT> (s, rlen, rlen+xlen);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (const charT* s, size_t n)   
{
    reference = new basic_string_ref<charT> (s, n);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (const charT* s)   
{
    reference = new basic_string_ref<charT> (s);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (charT c, size_t rep)   
{
    reference = new basic_string_ref<charT> (c, rep);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::basic_string (const vector<charT>& vec)   
{
    reference = new basic_string_ref<charT> (vec);
    c_str_ptr = 0;
}
template <class charT>
basic_string<charT>::~basic_string ()   
{
    delete_ref();
    if (c_str_ptr)
        delete[] c_str_ptr;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::operator= (const basic_string<charT>& str)   
{
    if (this != &str)
    {
        delete_ref();
        if (str.ref_count() != NPOS)
           (reference = str.reference)->count++;
        else
            reference = new basic_string_ref<charT> (str, 0, str.length());
    }
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::operator= (const charT* s)   
{
    assign_str (s, baggage_type::length(s));
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::operator= (charT c)   
{
    if ((ref_count() == 1) && (reserve() >= 1))
    {
        baggage_type::assign (*(point()), c);
        reference->len = 1;
    }
    else
    {
        delete_ref();
        reference = new basic_string_ref<charT> (c, 1);
    }
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::operator+= (const basic_string<charT>& rhs)   
{
    append_str (rhs.data(), rhs.length());
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::operator+= (const charT* s)   
{
    append_str (s, baggage_type::length(s));
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::operator+= (charT c)   
{
    if (length() >= (NPOS-1))
    {
        reference_class::throwlength();
    }
    if (!((ref_count() == 1) && (reserve() > length())))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), length(), length()+1);
        delete_ref();
        reference = tmp;
    }
    baggage_type::assign (*(point()+length()), c);
    reference->len++;
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::append (const basic_string<charT>& str, size_t pos, size_t n)
{
    if (pos > str.length())
    {
        reference_class::throwrange();
    }
    append_str (str.data() + pos, (n>(str.length()-pos))?(str.length()-pos):n);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::append (const charT* s, size_t n)   
{
    append_str (s, n);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::append (const charT* s)   
{
    append_str (s, baggage_type::length(s));
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::append (charT c, size_t rep)   
{
    if (length() >= (NPOS-rep))
    {
        reference_class::throwlength();
    }
    if (rep)
    {
       if ((ref_count() > 1) || (reserve() < (length() + rep)))
       {
          reference_pointer tmp;
          tmp = new basic_string_ref<charT> (data(), length(), length()+rep);
          delete_ref();
          reference = tmp;
       }
       for (size_t count = 0; count < rep; ++count)
            baggage_type::assign (*(point()+length()+count), c);
       reference->len += rep;
    }
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::assign (const basic_string<charT>& str, size_t pos, size_t n)
{
    if (pos > str.length())
    {
        reference_class::throwrange();
    }
    size_t rlen = (n > (str.length() - pos)) ? str.length() - pos : n;
    if ((rlen == str.length()) && (str.ref_count() != NPOS))
    {
       delete_ref();
       (reference = str.reference)->count++;
    }
    else
       assign_str (str.data()+pos, rlen);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::assign (const charT* s, size_t n)   
{
    assign_str (s, n);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::assign (const charT* s)   
{
    assign_str (s, baggage_type::length(s));
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::assign (charT c, size_t rep)   
{
    if (rep == NPOS)
    {
        reference_class::throwlength();
    }
    if ((ref_count() > 1) || (rep && (reserve() < rep)))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (c, rep);
        delete_ref();
        reference = tmp;
    }
    else
    {
        for (size_t count = 0; count < rep; ++count)
            baggage_type::assign (*(point()+count), c);
        reference->len = rep;
    }
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::insert (size_t pos1, const basic_string<charT>& str,
                             size_t pos2, size_t n)   
{
    if (pos2 > str.length())
    {
        reference_class::throwrange();
    }
    size_t rlen = (n > (str.length() - pos2)) ? str.length() - pos2 : n;
    insert_str (pos1, str.data()+pos2, rlen);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::insert (size_t pos, const charT* s, size_t n)
{
    insert_str(pos, s, n);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::insert (size_t pos, const charT* s)
{
    insert_str(pos, s, baggage_type::length(s));
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::insert (size_t pos, charT c, size_t rep)
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    if ((rep == NPOS) || (length() >= (NPOS - rep)))
    {
        reference_class::throwlength();
    }
    if (rep)
    {
        size_t count;
        if ((ref_count() > 1) || (reserve() < (length()+rep)))
        {
           reference_pointer tmp;
           tmp = new basic_string_ref<charT> (data(), pos, length()+rep);
           if (length())
               for (count = length()-pos; count > 0; --count)
                    baggage_type::assign (*(tmp->ptr+pos+rep+count-1),
                                          *(data()+pos+count-1));
           tmp->len = length();
           delete_ref();
           reference = tmp;
        }
        else
        {
           for (count = length()-pos; count > 0; --count)
                baggage_type::assign (*(point()+pos+rep+count-1),
                                      *(data()+pos+count-1));
        }
        for (count = 0; count < rep; ++count)
            baggage_type::assign (*(point()+pos+count), c);
        reference->len += rep;
    }
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::remove (size_t pos, size_t n)   
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    size_t xlen = (n > (length()-pos)) ? (length()-pos) : n;
    if (ref_count() > 1)
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), pos, length());
        baggage_type::copy (tmp->ptr+pos, data()+pos+xlen, length()-xlen-pos);
        tmp->len = length()-xlen;
        delete_ref();
        reference = tmp;
    }
    else if (xlen == length())
        reference->len = 0;
    else if (xlen)
    {
        baggage_type::copy (point()+pos, data()+pos+xlen, length()-xlen-pos);
        reference->len -= xlen;
    }
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::replace (size_t pos1, size_t n1, const basic_string<charT>& str,
                     size_t pos2, size_t n2)   
{
    if (pos2 > str.length())
    {
        reference_class::throwrange();
    }
    size_t xlen = (n1 > (length()-pos1)) ? (length()-pos1) : n1;
    size_t rlen = (n2 > (str.length()-pos2)) ? (str.length()-pos2) : n2;
    replace_str (xlen, pos1, str.data()+pos2, rlen);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::replace (size_t pos, size_t n1, const charT* s, size_t n2)
{
    size_t xlen = (n1 > (length()-pos)) ? (length()-pos) : n1;
    replace_str (xlen, pos, s, n2);
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::replace (size_t pos, size_t n1, const charT* s)
{
    size_t xlen = (n1 > (length()-pos)) ? (length()-pos) : n1;
    replace_str (xlen, pos, s, baggage_type::length(s));
    return *this;
}
template <class charT>
basic_string<charT>&
basic_string<charT>::replace (size_t pos, size_t n, charT c, size_t rep)
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    size_t xlen = (n > (length()-pos)) ? (length()-pos) : n;
    if ((length()-xlen) >= (NPOS-rep))
    {
        reference_class::throwlength();
    }
    if (!rep)
        return remove (pos, n);
    else
    {
        size_t count;
        if ((ref_count() > 1) || (reserve() < (length()-xlen+rep)))
        {
            reference_pointer tmp;
            tmp = new basic_string_ref<charT> (data(), pos,
                  length()+((xlen > rep) ? (xlen-rep) : 0));
            if (rep < xlen)
                baggage_type::copy (tmp->ptr+pos+rep, data()+pos+xlen,
                                    length()-pos-xlen);
            else
            {
                for (count = length()-xlen-pos; count > 0; --count)
                    baggage_type::assign (*(tmp->ptr+pos+rep+count-1),
                                          *(data()+pos+xlen+count-1));
            }
            tmp->len = length();
            delete_ref();
            reference = tmp;
        }
        else
        {
            if (rep < xlen)
                baggage_type::copy (point()+pos+rep, data()+pos+xlen,
                                    length()-pos-xlen);
            else
            {
                for (count = length()-xlen-pos; count > 0; --count)
                    baggage_type::assign (*(point()+pos+rep+count-1),
                                          *(data()+pos+xlen+count-1));
            }
        }
        for (count = 0; count < rep; ++count)
            baggage_type::assign (*(point()+pos+count), c);
        reference->len += (rep-xlen);
    }
    return *this;
}
template <class charT>
void
basic_string<charT>::put_at (size_t pos, charT c)   
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    if ((ref_count() > 1) || (pos == reserve()))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), length(),
              length()+((pos==length())?1:0));
        delete_ref();
        reference = tmp;
    }
    if (pos == length())
        ++reference->len;
    baggage_type::assign (*(point()+pos), c);
}
template <class charT>
charT&
basic_string<charT>::operator[] (size_t pos)   
{
    if (pos >= length())
    {
        reference_class::throwrange();
    }
    if (ref_count() > 1)
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(), length(), length());
        delete_ref();
        reference = tmp;
    }
    return *(point()+pos);
}
template <class charT>
const charT*
basic_string<charT>::c_str () const   
{
    if (c_str_ptr)
        delete[] ((basic_string<charT>*)this)->c_str_ptr;
    ((basic_string<charT>*)this)->c_str_ptr = new charT [length()+1];
    if (length())
        baggage_type::copy (((basic_string<charT>*)this)->c_str_ptr, data(),
                              length());
    baggage_type::assign (*(((basic_string<charT>*)this)->c_str_ptr+length()),
                          eos());
    return c_str_ptr;
}
template <class charT>
void
basic_string<charT>::resize (size_t n, charT c)   
{
    if (n == NPOS)
    {
        reference_class::throwlength();
    }
    if ((ref_count() > 1) || (n > reserve()))
    {
        reference_pointer tmp;
        tmp = new basic_string_ref<charT> (data(),
              ((n > length()) ? length() : n), n);
        delete_ref();
        reference = tmp;
    }
    while (reference->len < n)
    {
        baggage_type::assign (*(reference->ptr+length()), c);
        ++reference->len;
    }
    reference->len = n;
}
template <class charT>
void
basic_string<charT>::resize (size_t n)   
{
    resize (n, eos());
}
template <class charT>
void
basic_string<charT>::reserve (size_t res_arg)   
{
    if (res_arg == NPOS)
    {
        reference_class::throwlength();
    }
    if (res_arg > reserve())
    {
        reference_pointer tmp;
        tmp = new  basic_string_ref<charT> (data(), length(), res_arg);
        delete_ref();
        reference = tmp;
    }
}
template <class charT>
size_t
basic_string<charT>::copy (charT* s, size_t n, size_t pos) const   
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    size_t  rlen = (n > (length()-pos)) ? (length()-pos) : n;
    if (length())
        baggage_type::copy (s, data()+pos, rlen);
    return rlen;
}
template <class charT>
size_t
basic_string<charT>::find (const basic_string<charT>& str, size_t pos) const
{
    return find_str (str.data(), pos, str.length());
}
template <class charT>
size_t
basic_string<charT>::find (const charT* s, size_t pos, size_t n) const
{
    return find_str (s, pos, n);
}
template <class charT>
size_t
basic_string<charT>::find (const charT* s, size_t pos) const   
{
    return find_str (s, pos, baggage_type::length(s));
}
template <class charT>
size_t
basic_string<charT>::find (charT c, size_t pos) const   
{
    while ((pos < length()) && (baggage_type::ne(*(data()+pos), c)))
        ++pos;
    return ((pos < length()) ? pos : NPOS);
}
template <class charT>
size_t
basic_string<charT>::rfind (const basic_string<charT>& str, size_t pos) const
{
    return rfind_str (str.data(), pos, str.length());
}
template <class charT>
size_t
basic_string<charT>::rfind (const charT* s, size_t pos, size_t n) const
{
    return rfind_str (s, pos, n);
}
template <class charT>
size_t
basic_string<charT>::rfind (const charT* s, size_t pos) const   
{
    return rfind_str (s, pos, baggage_type::length(s));
}
template <class charT>
size_t
basic_string<charT>::rfind (charT c, size_t pos) const   
{
    size_t count = ((pos < length()) ? pos+1 : length());
    if (length() == 0)
        return NPOS;
    while ((baggage_type::ne(*(data()+count-1), c)) && (count > 1))
        --count;
    if ((count == 1) && (baggage_type::ne(*(data()), c)))
        return NPOS;
    else
        return count-1;
}
template <class charT>
size_t
basic_string<charT>::find_first_of (const basic_string<charT>& str, size_t pos) const
{
    return find_first_of_str (str.data(), pos, str.length());
}
template <class charT>
size_t
basic_string<charT>::find_first_of (const charT* s, size_t pos, size_t n) const
{
    return find_first_of_str (s, pos, n);
}
template <class charT>
size_t
basic_string<charT>::find_first_of (const charT* s, size_t pos) const
{
    return find_first_of_str (s, pos, baggage_type::length(s));
}
template <class charT>
size_t
basic_string<charT>::find_first_of (charT c, size_t pos) const   
{
    return find (c, pos);
}
template <class charT>
size_t
basic_string<charT>::find_last_of (const basic_string<charT>& str, size_t pos) const
{
    return find_last_of_str (str.data(), pos, str.length());
}
template <class charT>
size_t
basic_string<charT>::find_last_of (const charT* s, size_t pos, size_t n) const
{
    return find_last_of_str (s, pos, n);
}
template <class charT>
size_t
basic_string<charT>::find_last_of (const charT* s, size_t pos) const   
{
    return find_last_of_str (s, pos, baggage_type::length(s));
}
template <class charT>
size_t
basic_string<charT>::find_last_of (charT c, size_t pos) const   
{
    return rfind (c, pos);
}
template <class charT>
size_t
basic_string<charT>::find_first_not_of (const basic_string<charT>& str, size_t pos)
                                        const   
{
    return find_first_not_of_str (str.data(), pos, str.length());
}
template <class charT>
size_t
basic_string<charT>::find_first_not_of (const charT* s, size_t pos, size_t n)
                                        const   
{
    return find_first_not_of_str (s, pos, n);
}
template <class charT>
size_t
basic_string<charT>::find_first_not_of (const charT* s, size_t pos) const
{
    return find_first_not_of_str (s, pos, baggage_type::length(s));
}
template <class charT>
size_t
basic_string<charT>::find_first_not_of (charT c, size_t pos) const   
{
    while ((pos < length()) && (baggage_type::eq(*(data()+pos), c)))
        ++pos;
    return ((pos < length()) ? pos : NPOS);
}
template <class charT>
size_t
basic_string<charT>::find_last_not_of (const basic_string<charT>& str, size_t pos)
                                       const   
{
    return find_last_not_of_str (str.data(), pos, str.length());
}
template <class charT>
size_t
basic_string<charT>::find_last_not_of (const charT* s, size_t pos, size_t n)
                                       const   
{
    return find_last_not_of_str (s, pos, n);
}
template <class charT>
size_t
basic_string<charT>::find_last_not_of (const charT* s, size_t pos) const
{
    return find_last_not_of_str (s, pos, baggage_type::length(s));
}
template <class charT>
size_t
basic_string<charT>::find_last_not_of (charT c, size_t pos) const   
{
    size_t count = ((pos < length()) ? pos+1 : length());
    if (length() == 0)
        return NPOS;
    while ((baggage_type::eq(*(data()+count-1), c)) && (count > 1))
        --count;
    if ((count == 1) && (baggage_type::eq(*(data()), c)))
        return NPOS;
    else
        return count-1;
}
template <class charT>
basic_string<charT>
basic_string<charT>::substr (size_t pos,  size_t n) const   
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    if (length())
        return basic_string<charT> (data()+pos,
        (n > (length()-pos)) ? (length()-pos) : n);
    else
        return basic_string<charT>();
}
template <class charT>
int
basic_string<charT>::compare (const basic_string<charT>& str, size_t pos,
                              size_t n) const   
{
    size_t slen   = (n > (length()-pos)) ? (length()-pos) : n;
    return compare_str (pos, str.data(), slen, str.length());
}
template <class charT>
int
basic_string<charT>::compare (const charT* s, size_t pos, size_t n) const
{
    if (n == NPOS)
    {
        reference_class::throwlength();
    }
    return compare_str (pos, s, length()-pos, n);
}
template <class charT>
int
basic_string<charT>::compare (const charT* s, size_t pos) const   
{
    return compare_str (pos, s, length()-pos, baggage_type::length(s));
}
template <class charT>
int
basic_string<charT>::compare (charT c, size_t pos, size_t rep) const
{
    if (pos > length())
    {
        reference_class::throwrange();
    }
    if (rep == NPOS)
    {
        reference_class::throwlength();
    }
    if (rep)
    {
        size_t count = 0;
        while ((count < rep) && (count < (length()-pos)) &&
                baggage_type::eq (*(data()+pos+count), c))
            ++count;
        if ((count == rep) || (count == (length()-pos)))
            return (length()-pos-count);
        else
            return (*(data()+pos+count)-c);
    }
    else
    {
        return (length()-pos);
    }
}
template <class charT>
basic_string<charT>
operator+ (const basic_string<charT>& lhs, const basic_string<charT>& rhs)
{
    typedef  basic_string<charT>::baggage_type  baggage_type;
    basic_string<charT> tmp(lhs.data(), lhs.length(), rhs.length());
    if (rhs.length())
        baggage_type::copy (tmp.point()+lhs.length(), rhs.data(), rhs.length());
    tmp.len() += rhs.length();
    return tmp;
}
template <class charT>
basic_string<charT>
operator+ (const charT* lhs, const basic_string<charT>& rhs)   
{
    typedef  basic_string<charT>::baggage_type  baggage_type; 
    size_t  slen = baggage_type::length(lhs);
    basic_string<charT> tmp(lhs, slen, rhs.length());
    if (rhs.length())
        baggage_type::copy (tmp.point()+slen, rhs.data(), rhs.length());
    tmp.len() += rhs.length();
    return tmp;
}
template <class charT>
basic_string<charT>
operator+ (charT lhs, const basic_string<charT>& rhs)   
{
    typedef  basic_string<charT>::baggage_type  baggage_type; 
    basic_string<charT> tmp(&lhs, 1, rhs.length());
    if (rhs.length())
        baggage_type::copy (tmp.point()+1, rhs.data(), rhs.length());
    tmp.len() += rhs.length();
    return tmp;
}
template <class charT>
basic_string<charT>
operator+ (const basic_string<charT>& lhs, const charT* rhs)   
{
    typedef  basic_string<charT>::baggage_type  baggage_type; 
    size_t  slen = baggage_type::length(rhs);
    basic_string<charT> tmp(lhs.data(), lhs.length(), slen);
    if (slen)
        baggage_type::copy (tmp.point()+lhs.length(), rhs, slen);
    tmp.len() += slen;
    return tmp;
}
template <class charT>
basic_string<charT>
operator+ (const basic_string<charT>& lhs, charT rhs)   
{
    typedef  basic_string<charT>::baggage_type  baggage_type; 
    basic_string<charT> tmp(lhs.data(), lhs.length(), 1);
    baggage_type::assign (*(tmp.point()+lhs.length()), rhs);
    ++tmp.len();
    return tmp;
}
template <class charT>
ostream&
operator<< (ostream& o, const basic_string<charT>& s)   
{
    typedef  basic_string<charT>::baggage_type  baggage_type;
    for (size_t count = 0; count < s.length(); ++count)
        baggage_type::char_out (o, *(s.data()+count));
    return o;
}
template <class charT>
istream&
operator>> (istream& i, basic_string<charT>& s)   
{
    typedef  basic_string<charT>::baggage_type  baggage_type; 
    s.remove();
    while ( 1 )
    {
        charT  value;
        baggage_type::char_in (i, value);
        if (!i.operator void*())
            break;
        if (!baggage_type::is_del (value))
        {
            s.append(value);
            while ( 1 )
            {
                baggage_type::char_in (i, value);
                if (!i.operator void*())
                    break;
                if (!baggage_type::is_del (value)) 
                {
                    s.append(value);
                }
                else
                    break;
            }
            break;
        }
    }
    return i;
}
typedef  basic_string<char>     cstring;
typedef  basic_string<char>     string;
inline void destroy(string* pointer) {
    pointer->~string();
}
void main()
{
  string s;
  if (s != "");
}
