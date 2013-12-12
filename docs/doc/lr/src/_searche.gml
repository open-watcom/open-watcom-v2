.func _searchenv _wsearchenv
.synop begin
#include <stdlib.h>
void _searchenv( const char *name,
                 const char *env_var,
                       char *pathname );
.ixfunc2 '&Search' &func
.if &'length(&wfunc.) ne 0 .do begin
void _wsearchenv( const wchar_t *name,
                  const wchar_t *env_var,
                        wchar_t *pathname );
.ixfunc2 '&Search' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function searches for the file specified by
.arg name
in the list of directories assigned to the environment variable
specified by
.arg env_var
.ct .li .
Common values for
.arg env_var
are PATH, LIB and INCLUDE.
.pp
The current directory is searched first to find the specified file.
If the file is not found in the current directory,
each of the directories
specified by the environment variable is searched.
.pp
The full pathname is placed in the buffer pointed to by the argument
.arg pathname
.ct .li .
If the specified file cannot be found, then
.arg pathname
will contain an empty string.
.im widefunc
.desc end
.return begin
The &func function returns no value.
.return end
.see begin
.seelist _searchenv getenv setenv _splitpath putenv
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void display_help( FILE *fp )
  {
    printf( "display_help T.B.I.\n" );
  }
.exmp break
void main()
  {
    FILE *help_file;
    char full_path[ _MAX_PATH ];

    _searchenv( "watcomc.hlp", "PATH", full_path );
    if( full_path[0] == '\0' ) {
      printf( "Unable to find help file\n" );
    } else {
      help_file = fopen( full_path, "r" );
      display_help( help_file );
      fclose( help_file );
    }
  }
.exmp end
.class WATCOM
.system
