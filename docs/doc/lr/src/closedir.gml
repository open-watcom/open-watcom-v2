.func closedir _wclosedir _uclosedir
#include <&dirhdr>
.if '&machsys' eq 'QNX' .do begin
int closedir( DIR *dirp );
.do end
.el .do begin
int closedir( struct dirent *dirp );
.do end
.ixfunc2 '&Direct' &func
.if &'length(&wfunc.) ne 0 .do begin
int _wclosedir( struct _wdirent *dirp );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.if &'length(&ufunc.) ne 0 .do begin
int _uclosedir( UDIR *dirp );
.ixfunc2 '&Direct' &ufunc
.do end
.funcend
.desc begin
The &func function closes the directory specified by
.arg dirp
and frees the memory allocated by
.kw opendir
.ct .li .
.if '&machsys' eq 'QNX' .do begin
.np
The result of using a directory stream after one of the
.kw exec
or
.kw spawn
family of functions is undefined.
After a call to the
.kw fork
function, either the parent or the child (but not both) may continue
processing the directory stream using
.kw readdir
or
.kw rewinddir
or both.
If both the parent and child processes use these functions, the result
is undefined.
Either or both processes may use the &func function.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The &wfunc function is identical to &func except that it closes a
directory of wide-character filenames opened by
.kw _wopendir
.ct .li .
.do end
.if &'length(&ufunc.) ne 0 .do begin
.np
The &ufunc Unicode function is identical to &func except that it
closes a directory of Unicode filenames opened by
.kw _uopendir
.ct .li .
.do end
.desc end
.return begin
.if '&machsys' eq 'QNX' .do begin
If successful, the &func function returns zero.
Otherwise &minus.1 is returned and
.kw errno
is set to indicate the error.
.do end
.el .do begin
The &func function returns zero if successful, non-zero otherwise.
.do end
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EBADF
The argument
.arg dirp
does not refer to an open directory stream.
.if '&machsys' eq 'QNX' .do begin
.term EINTR
The &func function was interrupted by a signal.
.do end
.endterm
.error end
.see begin
.im seeiodir closedir
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
