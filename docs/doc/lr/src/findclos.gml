.func _findclose
#include <&iohdr>
int _findclose( long handle );
.ixfunc2 '&DosFunc' _findclose
.synop end
.desc begin
The &func function closes the directory of filenames established by a
call to the
.kw _findfirst
function.
The
.arg handle
argument was returned by the
.kw _findfirst
function.
.desc end
.return begin
If successful, &func returns 0; otherwise, &func and returns &minus.1
and sets
.kw errno
to one of the following values:
.begterm
.termhd1 Constant
.termhd2 Meaning
.term ENOENT
No matching files
.endterm
.return end
.see begin
.seelist &function. _dos_find...
.seelist &function. _findclose _findfirst _findnext closedir opendir readdir
.see end
.exmp begin
#include <stdio.h>
#include <&iohdr>

void main()
  {
    struct _finddata_t  fileinfo;
    long                handle;
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
.class DOS
.system
