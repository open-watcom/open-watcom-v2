.func open _open _wopen
.synop begin
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int open( const char *path, int access, ... );
.ixfunc2 '&OsIo' &funcb
.if &'length(&_func.) ne 0 .do begin
int _open( const char *path, int access, ... );
.ixfunc2 '&OsIo' &_func
.do end
.if &'length(&wfunc.) ne 0 .do begin
int _wopen( const wchar_t *path, int access, ... );
.ixfunc2 '&OsIo' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function opens a file at the operating system level.
The name of the file to be opened is given by
.arg path
.ct .li .
The file will be accessed according to the access mode specified by
.arg access
.ct .li .
The optional argument is the file permissions to be used when the
.kw O_CREAT
flag is on in the
.arg access
mode.
.if &'length(&_func.) ne 0 .do begin
.np
The
.id &_func.
function is identical to &funcb..
Use
.id &_func.
for ANSI/ISO naming conventions.
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it
accepts a wide character string argument for
.arg path
.ct .li .
.do end
.im openacc
.im openper
.desc end
.return begin
If successful,
.id &funcb.
returns a &handle for the file.
When an error occurs while opening the file, &minus.1 is returned.
.return end
.error begin
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Access denied because
.arg path
specifies a directory or a volume ID, or attempting to open a
read-only file for writing
.term EMFILE
No more &handle.s available (too many open files)
.term ENOENT
Path or file not found
.endterm
.error end
.see begin
.im seeioos
.see end
.exmp begin
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void main()
  {
    int &fd;
.exmp break
    /* open a file for output                  */
    /* replace existing file if it exists      */

    &fd = open( "file",
                O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

    /* read a file which is assumed to exist   */

    &fd = open( "file", O_RDONLY );

    /* append to the end of an existing file   */
    /* write a new file if file does not exist */

    &fd = open( "file",
                O_WRONLY | O_CREAT | O_APPEND,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
  }
.exmp end
.class begin POSIX 1003.1
.ansiname &_func
.class end
.system
