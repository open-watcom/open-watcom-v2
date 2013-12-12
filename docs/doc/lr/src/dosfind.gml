.func begin _dos_find... Functions
.func2 _dos_findclose
.func2 _dos_findfirst
.func2 _dos_findnext
.func2 _wdos_findclose
.func2 _wdos_findfirst
.func2 _wdos_findnext
.funcw _wdos_find...
.func gen
#include <&doshdr>
unsigned _dos_findfirst( const char *path,
                         unsigned attributes,
                         struct find_t *buffer );
unsigned _dos_findnext(  struct find_t *buffer );
unsigned _dos_findclose( struct find_t *buffer );
.ixfunc2 '&DosFunc' _dos_findclose
.ixfunc2 '&DosFunc' _dos_findfirst
.ixfunc2 '&DosFunc' _dos_findnext

struct find_t {
    char reserved[21];      /* reserved for use by DOS   */
    char attrib;            /* attribute byte for file   */
    unsigned short wr_time; /* time of last write to file*/
    unsigned short wr_date; /* date of last write to file*/
    unsigned long  size;    /* length of file in bytes   */
#if defined(__OS2__) || defined(__NT__)
    char name[256];         /* null-terminated filename  */
#else
    char name[13];          /* null-terminated filename  */
#endif
};
.if &'length(&wfunc.) ne 0 .do begin

unsigned _wdos_findfirst( const wchar_t *path,
                          unsigned attributes,
                          struct _wfind_t *buffer );
unsigned _wdos_findnext( struct _wfind_t *buffer );
unsigned _wdos_findclose( struct _wfind_t *buffer );
.ixfunc2 '&DosFunc' _wdos_findclose
.ixfunc2 '&DosFunc' _wdos_findfirst
.ixfunc2 '&DosFunc' _wdos_findnext
.ixfunc2 '&Wide' _wdos_findclose
.ixfunc2 '&Wide' _wdos_findfirst
.ixfunc2 '&Wide' _wdos_findnext

struct _wfind_t {
    char reserved[21];      /* reserved for use by DOS    */
    char attrib;            /* attribute byte for file    */
    unsigned short wr_time; /* time of last write to file */
    unsigned short wr_date; /* date of last write to file */
    unsigned long  size;    /* length of file in bytes    */
#if defined(__OS2__) || defined(__NT__)
    wchar_t name[256];      /* null-terminated filename   */
#else
    wchar_t name[13];       /* null-terminated filename   */
#endif
};
.do end
.synop end
.desc begin
The
.kw _dos_findfirst
function uses system call 0x4E to return information on the first file
whose name and attributes match the
.arg path
and
.arg attributes
arguments.
The information is returned in a
.kw find_t
structure pointed to by
.arg buffer
.ct .li .
The
.arg path
argument may contain wildcard characters ('?' and '*').
The
.arg attributes
argument may be any combination of the following constants:
.im dosattr
.np
The
.arg attributes
argument is interpreted by DOS as follows:
.autopoint
.point
If
.kw _A_NORMAL
is specified, then normal files are included in the search.
.point
If any of
.kw _A_HIDDEN
.ct ,
.kw _A_SYSTEM
.ct ,
.kw _A_SUBDIR
are specified, then normal files and the specified type of files are
included in the search.
.point
If
.kw _A_VOLID
is specified, then volume-ID's are also included in the search. Note:
The
.kw _A_VOLID
attribute is not supported on systems other than DOS (e.g. Win32, OS/2).
.point
.kw _A_RDONLY
and
.kw _A_ARCH
are ignored by this function.
.endpoint
.np
The format of the
.kw wr_time
field is described by the following structure
(this structure is not defined in any &company header file).
.blkcode begin
typedef struct {
    unsigned short  twosecs : 5;    /* seconds / 2 */
    unsigned short  minutes : 6;    /* minutes (0,59) */
    unsigned short  hours   : 5;    /* hours (0,23) */
} ftime_t;
.blkcode end
.np
The format of the
.kw wr_date
field is described by the following structure
(this structure is not defined in any &company header file).
.blkcode begin
typedef struct {
    unsigned short  day     : 5;    /* day (1,31) */
    unsigned short  month   : 4;    /* month (1,12) */
    unsigned short  year    : 7;    /* 0 is 1980 */
} fdate_t;
.blkcode end
.np
The
.kw _dos_findnext
function uses system call 0x4F to return information on the next file
whose name and attributes match the pattern supplied to the
.kw _dos_findfirst
function.
.np
On some systems (e.g. Win32, OS/2), you must call
.kw _dos_findclose
to indicate that you are done matching files.
This function deallocates any resources that were allocated by the
.kw _dos_findfirst
function.
.if &'length(&wfunc.) ne 0 .do begin
The wide-character
.kw _wdos_findclose
.ct ,
.kw _wdos_findfirst
and
.kw _wdos_findnext
functions are similar to their counterparts but operate on
wide-character strings.
.do end
.desc end
.return begin
The
.kw _dos_find...
functions return zero if successful.
Otherwise, the
.kw _dos_findfirst
and
.kw _dos_findnext
functions return an OS error code and set
.kw errno
accordingly.
.return end
.see begin
.seelist _dos_findfirst opendir readdir closedir
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
.exmp break
void main()
{
    struct find_t   fileinfo;
    unsigned        rc;         /* return code */
.exmp break
    /* Display name and size of "*.c" files */
    rc = _dos_findfirst( "*.c", _A_NORMAL, &fileinfo );
    while( rc == 0 ) {
        printf( "%14s %10ld\n", fileinfo.name,
                                fileinfo.size );
        rc = _dos_findnext( &fileinfo );
    }
    #if defined(__OS2__)
    _dos_findclose( &fileinfo );
    #endif
}
.exmp end
.* next two lines remove function group names from classification
.funcw
.func_
.class DOS
.system
