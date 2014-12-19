.func umask _umask
.synop begin
#include <sys/types.h>
#include <sys/stat.h>
.if '&machsys' eq 'QNX' .do begin
mode_t umask( mode_t cmask );
.do end
.el .do begin
#include <fcntl.h>
#include <&iohdr>
int umask( int cmask );
.if &'length(&_func.) ne 0 .do begin
int _umask( int cmask );
.ixfunc2 '&OsIo' &_func
.do end
.do end
.ixfunc2 '&OsIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function sets the process's file mode creation mask to
.arg cmask
.ct .li .
The process's file mode creation mask is used during
.if '&machsys' eq 'QNX' .do begin
.kw creat
.ct,
.kw mkdir
.ct,
.kw mkfifo
.ct,
.kw open
or
.kw sopen
.do end
.el .do begin
.kw creat
.ct,
.kw open
or
.kw sopen
.do end
to turn off permission bits in the
.arg permission
argument supplied.
In other words, if a bit in the mask is on, then the corresponding bit
in the file's requested permission value is disallowed.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ISO C naming conventions.
.do end
.pp
The argument
.arg cmask
is a constant expression involving the constants described below.
.im openperv
.pp
For example, if
.kw S_IRUSR
is specified, then reading is not allowed (i.e., the file is write only).
If
.kw S_IWUSR
is specified, then writing is not allowed (i.e., the file is read only).
.desc end
.return begin
The
.id &funcb.
function returns the previous value of
.arg cmask
.ct .li .
.return end
.see begin
.seelist umask chmod creat mkdir mkfifo open sopen
.see end
.exmp begin
#include <sys/types.h>
#include <sys/stat.h>
.if '&machsys' ne 'QNX' .do begin
#include <fcntl.h>
#include <&iohdr>
.do end

void main( void )
{
.if '&machsys' eq 'QNX' .do begin
    mode_t old_mask;
.do end
.el .do begin
    int old_mask;
.do end
.exmp break
    /* set mask to create read-only files */
    old_mask = umask( S_IWUSR | S_IWGRP | S_IWOTH |
                      S_IXUSR | S_IXGRP | S_IXOTH );
}
.exmp end
.ansiname &_func
.class POSIX 1003.1
.system
