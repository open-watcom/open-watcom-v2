.func umask _umask
.synop begin
#include <sys/types.h>
#include <sys/stat.h>
.if '&machsys' eq 'QNX' .do begin
mode_t umask( mode_t cmask );
.ixfunc2 '&OsIo' umask
.do end
.el .do begin
#include <fcntl.h>
#include <&iohdr>
mode_t umask( mode_t cmask );
.ixfunc2 '&OsIo' umask
mode_t _umask( mode_t cmask );
.ixfunc2 '&OsIo' _umask
.do end
.synop end
.desc begin
The
.id &funcb.
function sets the process's file mode creation mask to
.arg cmask
.period
The process's file mode creation mask is used during
.if '&machsys' eq 'QNX' .do begin
.reffunc creat
.ct ,
.reffunc mkdir
.ct ,
.reffunc mkfifo
.ct ,
.reffunc open
or
.reffunc _sopen
.do end
.el .do begin
.reffunc creat
.ct ,
.reffunc open
or
.reffunc _sopen
.do end
to turn off permission bits in the
.arg permission
argument supplied.
In other words, if a bit in the mask is on, then the corresponding bit
in the file's requested permission value is disallowed.
.im ansiconf
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
.period
.return end
.see begin
.seelist umask chmod creat mkdir mkfifo open _sopen
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
    mode_t old_mask;
.do end
.exmp break
    /* set mask to create read-only files */
    old_mask = umask( S_IWUSR | S_IWGRP | S_IWOTH |
                      S_IXUSR | S_IXGRP | S_IXOTH );
}
.exmp end
.class POSIX 1003.1
.system
