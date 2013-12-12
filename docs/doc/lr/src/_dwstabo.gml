.func _dwSetAboutDlg
.synop begin
#include <wdefwin.h>
int _dwSetAboutDlg( const char *title, const char *text );
.synop end
.desc begin
The &func function sets the "About" dialog box of the default
windowing system.
The argument
.arg title
points to the string that will replace the current title.
If
.arg title
is NULL then the title will not be replaced.
The argument
.arg text
points to a string which will be placed in the "About" box.
To get multiple lines, embed a new line after each logical line in the
string.
If
.arg text
is NULL, then the current text in the "About" box will not be
replaced.
.np
The &func function is one of the support functions that can be called
from an application using &company's default windowing support.
.desc end
.return begin
The &func function returns 1 if it was successful and 0 if not.
.return end
.see begin
.seelist &function. _dwDeleteOnClose _dwSetAboutDlg _dwSetAppTitle _dwSetConTitle _dwShutDown _dwYield
.see end
.exmp begin
#include <wdefwin.h>
#include <stdio.h>

void main()
  {
    FILE *sec;
.exmp break
    _dwSetAboutDlg( "Hello World About Dialog",
                    "About Hello World\n"
                    "Copyright 1994 by WATCOM\n" );
    _dwSetAppTitle( "Hello World Application Title" );
    _dwSetConTitle( 0, "Hello World Console Title" );
    printf( "Hello World\n" );
    sec = fopen( "CON", "r+" );
    _dwSetConTitle( fileno( sec ),
                    "Hello World Second Console Title" );
    _dwDeleteOnClose( fileno( sec ) );
    fprintf( sec, "Hello to second console\n" );
    fprintf( sec, "Press Enter to close this console\n" );
    fflush( sec );
    fgetc( sec );
    fclose( sec );
  }
.exmp end
.class WATCOM
.system
