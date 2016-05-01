.func rewinddir _wrewinddir
.synop begin
#include <sys/types.h>
#include <&dirhdr>
void rewinddir( DIR *dirp );
.ixfunc2 '&Direct' &funcb
.if &'length(&wfunc.) ne 0 .do begin
void _wrewinddir( WDIR *dirp );
.ixfunc2 '&Direct' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function resets the position of the directory stream to
which
.arg dirp
refers to the beginning of the directory.
It also causes the directory stream to refer to the current state
of the corresponding directory, as a call to
.kw opendir
would have done.
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
Either or both processes may use
.kw closedir
.ct .li .
.do end
.if &'length(&wfunc.) ne 0 .do begin
.np
The
.id &wfunc.
function is identical to
.id &funcb.
except that it rewinds a
directory of wide-character filenames opened by
.kw _wopendir
.ct .li .
.do end
.desc end
.return begin
The
.id &funcb.
function does not return a value.
.return end
.see begin
.im seeiodir
.see end
.exmp begin
.blktext begin
The following example lists all the files in a directory,
creates a new file, and then relists the directory.
.blktext end
.blkcode begin
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <&dirhdr>
.exmp break
void main()
{
    DIR *dirp;
    struct dirent *direntp;
    int &fd;
.exmp break
.if '&machsys' eq 'QNX' .do begin
    dirp = opendir( "/home/fred" );
.do end
.el .do begin
    dirp = opendir( "\\watcom\\h\\*.*" );
.do end
    if( dirp != NULL ) {
        printf( "Old directory listing\n" );
        for(;;) {
            direntp = readdir( dirp );
            if( direntp == NULL )
                break;
            printf( "%s\n", direntp->d_name );
        }
.exmp break
.if '&machsys' eq 'QNX' .do begin
        &fd = creat( "/home/fred/file.new",
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
.do end
.el .do begin
        &fd = creat( "\\watcom\\h\\file.new",
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
.do end
        close( &fd );
.exmp break
        rewinddir( dirp );
        printf( "New directory listing\n" );
        for(;;) {
            direntp = readdir( dirp );
            if( direntp == NULL )
                break;
            printf( "%s\n", direntp->d_name );
        }
        closedir( dirp );
    }
}
.blkcode end
.im dblslash
.exmp end
.class POSIX 1003.1
.system
