.func _hdopen
#include <&iohdr>
int _hdopen( int os_handle, int mode );
.funcend
.desc begin
The &func function takes a previously opened operating system file
handle specified by
.arg os_handle
and opened with access and sharing specified by
.arg mode
.ct , and creates a POSIX-style file handle.
.desc end
.return begin
The &func function returns the new POSIX-style file handle if
successful.
Otherwise, it returns -1.
.return end
.see begin
.seelist &function. close _dos_open fdopen fopen freopen _fsopen
.seelist &function. _grow_handles _hdopen open _open_osfhandle
.seelist &function. _os_handle _popen sopen
.see end
.exmp begin
#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <&iohdr>
#include <windows.h>

void main()
  {
    HANDLE os_handle;
    DWORD desired_access, share_mode;
    int handle;

    os_handle = CreateFileA( "file", GENERIC_WRITE,
                          0, NULL, CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL, NULL );

    if( os_handle == INVALID_HANDLE_VALUE ) {
        printf( "Unable to open file\n" );
    } else {
        handle = _hdopen( os_handle, O_RDONLY );
        if( handle != -1 ) {
            write( handle, "hello\n", 6 );
            close( handle );
        } else {
            CloseHandle( os_handle );
        }
    }
  }
.exmp end
.class WATCOM
.system
