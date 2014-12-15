.func _dos_open
.synop begin
#include <&doshdr>
#include <fcntl.h>
#include <share.h>
unsigned _dos_open( const char *path,
                    unsigned mode,
                    int *&fd );
.ixfunc2 '&DosFunc' &funcb
.ixfunc2 '&OsIo' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses system call 0x3D to open the file specified by
.arg path
.ct , which must be an existing file.
The
.arg mode
argument specifies the file's access, sharing and inheritance permissions.
The access mode must be one of:
.begterm 12 $compact
.termhd1 Mode
.termhd2 Meaning
.term O_RDONLY
Read only
.term O_WRONLY
Write only
.term O_RDWR
Both read and write
.endterm
.np
The sharing permissions, if specified, must be one of:
.begterm 12 $compact
.termhd1 Permission
.termhd2 Meaning
.term SH_COMPAT
Set compatibility mode.
.term SH_DENYRW
Prevent read or write access to the file.
.term SH_DENYWR
Prevent write access of the file.
.term SH_DENYRD
Prevent read access to the file.
.term SH_DENYNO
Permit both read and write access to the file.
.endterm
.np
The inheritance permission, if specified, is:
.begterm 12
.termhd1 Permission
.termhd2 Meaning
.term O_NOINHERIT
File is not inherited by a child process
.endterm
.desc end
.return begin
The
.id &funcb.
function returns zero if successful.
Otherwise, it returns an
MS-DOS error code and sets
.kw errno
to one of the following values:
.begterm 12
.termhd1 Constant
.termhd2 Meaning
.term EACCES
Access denied because
.arg path
specifies a directory or a volume ID, or opening a read-only file
for write access
.term EINVAL
A sharing mode was specified when file sharing is not installed,
or access-mode value is invalid
.term EMFILE
No more &handle.s available, (too many open files)
.term ENOENT
Path or file not found
.endterm
.return end
.see begin
.seelist _dos_close _dos_creat _dos_creatnew _dos_open
.seelist _dos_read _dos_write fdopen fopen
.seelist freopen _fsopen _grow_handles _hdopen open
.seelist _open_osfhandle _popen sopen
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
#include <fcntl.h>
#include <share.h>
.exmp break
void main()
  {
    int &fd;
.exmp break
    if( _dos_open( "file", O_RDONLY, &amp.&fd ) != 0 ) {
        printf( "Unable to open file\n" );
    } else {
        printf( "Open succeeded\n" );
        _dos_close( &fd );
    }
  }
.exmp end
.class DOS
.system
