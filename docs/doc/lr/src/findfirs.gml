.func _findfirst _findfirsti64 _wfindfirst _wfindfirsti64
.synop begin
#include <&iohdr>
intptr_t _findfirst( const char *filespec,
                 struct _finddata_t *fileinfo );
.ixfunc2 '&DosFunc' _findfirst
intptr_t _findfirsti64( const char *filespec,
                    struct _finddatai64_t *fileinfo );
.ixfunc2 '&DosFunc' _findfirsti64
.if &'length(&wfunc.) ne 0 .do begin
intptr_t _wfindfirst( const wchar_t *filespec,
                  struct _wfinddata_t *fileinfo );
.ixfunc2 '&DosFunc' _wfindfirst
.ixfunc2 '&Wide' _wfindfirst
intptr_t _wfindfirsti64( const wchar_t *filespec,
                  struct _wfinddatai64_t *fileinfo );
.ixfunc2 '&DosFunc' _wfindfirsti64
.ixfunc2 '&Wide' _wfindfirsti64
.do end
.synop end
.desc begin
The
.id &funcb.
function returns information on the first file whose name
matches the
.arg filespec
argument.
The
.arg filespec
argument may contain wildcard characters ('?' and '*').
The information is returned in a
.kw _finddata_t
structure pointed to by
.arg fileinfo
.period
.millust begin
struct _finddata_t {
    unsigned    attrib;
    time_t      time_create;     /* -1 for FAT file systems */
    time_t      time_access;     /* -1 for FAT file systems */
    time_t      time_write;
    _fsize_t    size;
    char        name[_MAX_PATH];
};
.millust end
.np
The &func64 function returns information on the first file whose
name matches the
.arg filespec
argument.
It differs from the
.id &_func.
function in that it returns a 64-bit file
size.
The
.arg filespec
argument may contain wildcard characters ('?' and '*').
The information is returned in a
.kw _finddatai64_t
structure pointed to by
.arg fileinfo
.period
.millust begin
struct _finddatai64_t {
    unsigned    attrib;
    time_t      time_create;     /* -1 for FAT file systems */
    time_t      time_access;     /* -1 for FAT file systems */
    time_t      time_write;
    __int64     size;            /* 64-bit size info        */
    char        name[_MAX_PATH];
};
.millust end
.widefunc &wfunc. &funcb.
.if &'length(&wfunc.) ne 0 .do begin
.millust begin
struct _wfinddata_t {
    unsigned    attrib;
    time_t      time_create;     /* -1 for FAT file systems */
    time_t      time_access;     /* -1 for FAT file systems */
    time_t      time_write;
    _fsize_t    size;
    wchar_t     name[_MAX_PATH];
};
.millust end
.np
The wide character &wfunc64 function is similar to the &func64
function but operates on wide character strings.
It differs from the
.id &wfunc.
function in that it returns a 64-bit file
size.
.millust begin
struct _wfinddatai64_t {
    unsigned    attrib;
    time_t      time_create;     /* -1 for FAT file systems */
    time_t      time_access;     /* -1 for FAT file systems */
    time_t      time_write;
    __int64     size;            /* 64-bit size info        */
    wchar_t     name[_MAX_PATH];
};
.millust end
.do end
.desc end
.return begin
If successful,
.id &_func.
returns a unique search handle identifying the
file or group of files matching the
.arg filespec
specification, which can be used in a subsequent call to
.reffunc _findnext
or to
.reffunc _findclose
.period
Otherwise,
.id &_func.
returns &minus.1 and sets
.kw errno
to one of the following values:
.begterm
.termhd1 Constant
.termhd2 Meaning
.term ENOENT
No matching files
.term EINVAL
Invalid filename specification
.endterm
.return end
.see begin
.seelist _dos_find&grpsfx
.seelist _findclose _findfirst _findnext closedir opendir readdir
.see end
.exmp begin
#include <stdio.h>
#include <&iohdr>

void main()
  {
    struct _finddata_t  fileinfo;
    intptr_t            handle;
    int                 rc;
.exmp break
    /* Display name and size of "*.c" files */
    handle = _findfirst( "*.c", &fileinfo );
    rc = handle;
    while( rc != -1 ) {
      printf( "%14s %10ld\n", fileinfo.name,
                              fileinfo.size );
      rc = _findnext( handle, &fileinfo );
    }
    _findclose( handle );
  }
.exmp end
.listnew Classification:
DOS
.listend
.system
