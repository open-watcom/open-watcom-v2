#include <windows.h>
#include <stdio.h>
#include <assert.h>

void log_it( char *m ) {
    FILE *fp = fopen( "exec.out", "a" );
    assert( fp != 0 );
    fputs( m, fp );
    fclose( fp );
}

void de() {
    unsigned e = GetLastError();
    assert( e == 0 );
    log_it( "no errors\n" );
}

void main() {
    log_it( "tomexe: >start\n" );
    HINSTANCE dll = LoadLibrary( "tomdll.dll" );
    de();
    if( !dll ) {
	log_it( "tomexe: >loadlib failed\n" );
	exit(1);
    }
    log_it( "tomexe: LoadLibrary\n" );
    void (*entry)() = (void(*)()) GetProcAddress( dll, "dll_entry_" );
    de();
    if( ! entry ) {
	log_it( "tomexe: >getprocaddr failed\n" );
	exit(1);
    }
    log_it( "tomexe: GetProcAddress\n" );
    entry();
    log_it( "tomexe: <done\n" );
}
