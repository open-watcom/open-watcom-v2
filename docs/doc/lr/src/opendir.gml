.func opendir _wopendir
.synop begin
#include <&dirhdr>
DIR *opendir( const char *dirname );
.ixfunc2 '&Direct' &funcb
.if &'length(&wfunc.) ne 0 .do begin
WDIR *_wopendir( const wchar_t *dirname );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function is used in conjunction with the functions
.reffunc readdir
and
.reffunc closedir
to obtain the list of file names contained in the directory specified by
.arg dirname
.period
The path indicated by
.arg dirname
can be either relative to the current working directory or it can be
an absolute path name.
.if '&machsys' ne 'QNX' .do begin
As an extension to POSIX, the last part of
.arg dirname
can contain the characters '?' and '*' for matching multiple files
within a directory.
.do end
.im dirent
.np
More than one directory can be read at the same time using the
.reffunc opendir
.ct ,
.reffunc readdir
.ct ,
.if '&machsys' eq 'QNX' .do begin
.reffunc rewinddir
.do end
and
.reffunc closedir
functions.
.if '&machsys' eq 'QNX' .do begin
.np
The result of using a directory stream after one of the
.reffunc exec&grpsfx
or
.reffunc spawn&grpsfx
family of functions is undefined.
After a call to the
.reffunc fork
function, either the parent or the child (but not both) may continue
processing the directory stream using
.reffunc readdir
or
.reffunc rewinddir
or both.
If both the parent and child processes use these functions, the result
is undefined.
Either or both processes may use
.reffunc closedir
.period
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it accepts a
wide character string argument and returns a pointer to a
.kw _wdirent
structure that can be used with the
.reffunc _wreaddir
.if '&machsys' eq 'QNX' .do begin
.ct ,
.reffunc _wrewinddir
.do end
and
.reffunc _wclosedir
functions.
.im _wdirent
.do end
.desc end
.return begin
The
.id &funcb.
function, if successful, returns a pointer to a structure
required for subsequent calls to
.reffunc readdir
to retrieve the file names
.if '&machsys' ne 'QNX' .do begin
matching the pattern
.do end
specified by
.arg dirname
.period
The
.id &funcb.
function returns
.mono NULL
if
.arg dirname
.if '&machsys' eq 'QNX' .do begin
is not a valid pathname.
.do end
.el .do begin
is not a valid pathname, or if there are no files matching
.arg dirname
.period
.do end
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Search permission is denied for a component of
.arg dirname
or read permission is denied for
.arg dirname
.period
.if '&machsys' eq 'QNX' .do begin
.term ENAMETOOLONG
The length of the argument
.arg dirname
exceeds {PATH_MAX}, or a pathname component is longer than
{NAME_MAX}.
.do end
.term ENOENT
The named directory does not exist.
.if '&machsys' eq 'QNX' .do begin
.term ENOTDIR
A component of
.arg dirname
is not a directory.
.do end
.endterm
.error end
.see begin
.im seeiodir
.see end
.exmp begin
.blktext begin
To get a list of files contained in the directory
.if '&machsys' eq 'QNX' .do begin
.filename /home/fred
of your node:
.do end
.el .do begin
.filename \watcom\h
on your default disk:
.do end
.blktext end
.blkcode begin
#include <stdio.h>
#include <&dirhdr>
.if '&machsys' ne 'QNX' .do begin

typedef struct {
    unsigned short  twosecs : 5;    /* seconds / 2 */
    unsigned short  minutes : 6;
    unsigned short  hours   : 5;
} ftime_t;

typedef struct {
    unsigned short  day     : 5;
    unsigned short  month   : 4;
    unsigned short  year    : 7;
} fdate_t;
.do end

void main()
  {
    DIR *dirp;
    struct dirent *direntp;
.if '&machsys' ne 'QNX' .do begin
    ftime_t *f_time;
    fdate_t *f_date;
.do end
.exmp break
.if '&machsys' eq 'QNX' .do begin
    dirp = opendir( "/home/fred" );
.do end
.el .do begin
    dirp = opendir( "\\watcom\\h" );
.do end
    if( dirp != NULL ) {
      for(;;) {
        direntp = readdir( dirp );
        if( direntp == NULL ) break;
.if '&machsys' eq 'QNX' .do begin
        printf( "%s\n", direntp->d_name );
.do end
.el .do begin
        f_time = (ftime_t *)&direntp->d_time;
        f_date = (fdate_t *)&direntp->d_date;
        printf( "%-12s %d/%2.2d/%2.2d "
                "%2.2d:%2.2d:%2.2d \n",
            direntp->d_name,
            f_date->year + 1980,
            f_date->month,
            f_date->day,
            f_time->hours,
            f_time->minutes,
            f_time->twosecs * 2 );
.do end
      }
      closedir( dirp );
    }
  }
.blkcode end
.im dblslash
.exmp end
.class POSIX 1003.1
.system
