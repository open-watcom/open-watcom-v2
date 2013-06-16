#ifndef __WATCOMC__

/* clibext.h:
   This file contains defines and prototypes of functions that are present
   in Watcom's CLIB but not in many other C libraries */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>      /* for off_t */
#if defined(__QNX__) || defined(__SVR4)
    #define _XPG4_2         /* Required on Solaris... */
    #include <strings.h>    /* for str*case* functions */
    #undef _XPG4_2          /* ...but causes trouble */
#endif
#ifdef __UNIX__
    #include <sys/wait.h>
#endif
#include "clibint.h"

#if defined( __GNUC__ ) || defined( __SUNPRO_C )
#define _INTEGRAL_MAX_BITS  64
#endif

/* An equivalent of the __unaligned keyword may be necessary on RISC
 * architectures, but on x86/x64 it's useless
 */
#define _WCUNALIGNED

#define _WCRTLINK
#define _WCI86FAR
#define _WCNEAR
#define __near
#define near
#define __based(x)

#if defined( __UNIX__ )

#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef O_TEXT
#define O_TEXT 0
#endif
#ifndef P_WAIT
#define P_WAIT 0
#endif
#define stricmp strcasecmp
#define strcmpi strcasecmp
#define strnicmp strncasecmp
#define _MAX_PATH   (PATH_MAX+1)
#define _MAX_DRIVE  3
#define _MAX_DIR    (PATH_MAX-3)
#define _MAX_FNAME  (PATH_MAX-3)
#define _MAX_EXT    (PATH_MAX-3)
#define _snprintf snprintf
#define _fsopen(x,y,z) fopen(x,y)
#define _fmemcpy memcpy
#define __int64 long long
#define __alloca alloca

#elif defined( _MSC_VER )

#define S_ISDIR(x) (((x)&_S_IFMT)==_S_IFDIR)
#define S_ISREG(x) (((x)&_S_IFMT)==_S_IFREG)
#define S_ISCHR(x) (((x)&_S_IFMT)==_S_IFCHR)
#define _A_VOLID 0
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#define FNM_NOMATCH     1
#define FNM_NOESCAPE    0x01
#define FNM_PATHNAME    0x02
#define FNM_PERIOD      0x04
#define FNM_IGNORECASE  0x08
#define FNM_LEADING_DIR 0x10
#define NAME_MAX FILENAME_MAX
#define PATH_MAX FILENAME_MAX
#define STDIN_FILENO  _fileno( stdin )
#define STDOUT_FILENO _fileno( stdout )
#define STDERR_FILENO _fileno( stderr )
#define __alloca _alloca
#define fseeko fseek
#define strcasecmp stricmp
#define _grow_handles _setmaxstdio
#define snprintf _snprintf
#define _mbislead _ismbblead
#define utoa ultoa
#define gmtime_r(a,b) gmtime_s(b,a)
#define localtime_r(a,b) localtime_s(b,a)

#endif

#define _MAX_PATH2 (_MAX_PATH + 3)

#ifndef getch
#define getch getchar
#endif
#define _vsnprintf vsnprintf
#define __va_list  va_list
#define __Strtold(s,ld,endptr) ((*(double *)(ld))=strtod(s,endptr))

#ifndef max 
#define max(x,y) (((x)>(y))?(x):(y))
#endif
#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined( __UNIX__ )

extern char   *itoa( int value, char *buf, int radix );
extern char   *utoa( unsigned int value, char *buf, int radix );
extern char   *ltoa( long int value, char *buf, int radix );
extern char   *ultoa( unsigned long int value, char *buf, int radix );
extern void   _splitpath( const char *path, char *drive, char *dir, char *fname, char *ext );
extern void   _makepath( char *path, const char *drive, const char *dir, const char *fname, const char *ext );
extern char   *_fullpath( char *buf, const char *path, size_t size );
extern char   *strlwr( char *string );
extern char   *strupr( char *string );
extern char   *strrev( char *string );
extern int    memicmp(const void *, const void *, size_t);
extern off_t  tell( int handle );
extern long   filelength(int handle);
extern int    eof( int fildes );
extern void   _searchenv( const char *name, const char *env_var, char *buf );
extern char   *strnset( char *string, int c, size_t len );
extern int    spawnlp( int mode, const char *path, const char *cmd, ... );
extern int    spawnvp( int mode, const char *cmd, const char * const *args );

#elif defined( _MSC_VER )

typedef struct find_t {
    char            reserved[21];       /* reserved for use by DOS    */
    char            attrib;             /* attribute byte for file    */
    unsigned short  wr_time;            /* time of last write to file */
    unsigned short  wr_date;            /* date of last write to file */
    unsigned long   size;               /* length of file in bytes    */
    char            name[NAME_MAX+1];   /* null-terminated filename   */
} find_t;
typedef struct dirent {
    char                d_dta[21];          /* disk transfer area */
    char                d_attr;             /* file's attribute */
    unsigned short int  d_time;             /* file's time */
    unsigned short int  d_date;             /* file's date */
    long                d_size;             /* file's size */
    char                d_name[NAME_MAX+1]; /* file's name */
    unsigned short      d_ino;              /* serial number (not used) */
    char                d_first;            /* flag for 1st time */
    char                *d_openpath;        /* path specified to opendir */
} dirent;
typedef struct dirent   DIR;

#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef long    ssize_t;
#endif
typedef int     mode_t;

extern void     _dos_getdrive( unsigned *drive );
extern void     _dos_setdrive( unsigned drivenum, unsigned *drives );
extern unsigned _dos_getfileattr( const char *path, unsigned *attribute );
extern unsigned _dos_setfileattr( const char *path, unsigned attribute );
extern int      setenv( const char *name, const char *newvalue, int overwrite );
extern void     unsetenv( const char *name );
extern DIR      *opendir( const char *dirname );
extern struct dirent *readdir( DIR *dirp );
extern int      closedir( DIR *dirp );
extern unsigned _dos_open( const char *name, unsigned mode, void **h );
extern unsigned _dos_creat( const char *name, unsigned mode, void **h );
extern unsigned _dos_close( void *h );
extern unsigned _dos_getftime( void *h, unsigned *date, unsigned *time );
extern unsigned _dos_setftime( void *h, unsigned date, unsigned time );
extern unsigned _dos_read( void *h, void *buffer, unsigned count, unsigned *bytes );
extern unsigned _dos_write( void *h, void const *buffer, unsigned count, unsigned *bytes );
extern unsigned _dos_findfirst( const char *__path, unsigned __attr, find_t *__buf );
extern unsigned _dos_findclose( struct find_t *__buf );
extern unsigned sleep( unsigned );
extern void __GetNTAccessAttr( int rwmode, unsigned long *desired_access, unsigned long *attr );
extern void __GetNTShareAttr( int mode, unsigned long *share_mode );

extern int      getopt( int __argc, char * const __argv[], const char *__optstring );
/* Globals used and set by getopt() */
extern char   *optarg;
extern int    optind;
extern int    opterr;
extern int    optopt;

extern int fnmatch( const char *__pattern, const char *__string, int __flags );

#endif

extern int    _vbprintf( char *s, size_t bufsize, const char *format, __va_list arg );
extern void   _splitpath2( const char *inp, char *outp, char **drive, char **dir, char **fn, char **ext );
extern int    _bgetcmd( char *buffer, int len );
extern char   *getcmd( char *buffer );
extern char   *_cmdname( char *name );

#ifdef __cplusplus
}
#endif

#endif
