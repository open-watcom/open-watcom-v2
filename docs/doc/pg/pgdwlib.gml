.section Default Windowing Library Functions
.*
.np
.ix 'default windowing library functions'
.ix 'library functions' 'default windowing'
A few library functions have been written to enable some simple
customization of the default windowing system's behaviour.
The following functions are supplied:
.begnote $setptnt 5
.if '&lang' eq 'C/C++' .do begin
.note _dwDeleteOnClose
.ix '_dwDeleteOnClose'
.millust begin
int _dwDeleteOnClose( int handle );
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note dwfDeleteOnClose
.ix 'dwfDeleteOnClose'
.millust begin
integer function dwfDeleteOnClose( unit )
integer unit
.millust end
.do end
.np
This function tells the console window that it should close itself
when the file is closed. You must pass to it the
.if '&lang' eq 'C/C++' .do begin
handle
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
unit number
.do end
associated with the opened console.
.if '&lang' eq 'C/C++' .do begin
.note _dwSetAboutDlg
.ix '_dwSetAboutDlg'
.millust begin
int _dwSetAboutDlg( const char *title, const char *text );
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note dwfSetAboutDlg
.ix 'dwfSetAboutDlg'
.millust begin
integer function dwfSetAboutDlg( title, text )
character*(*) title
character*(*) text
.millust end
.do end
.np
.if '&lang' eq 'C/C++' .do begin
.sr whatisnull='NULL'
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.sr whatisnull='CHAR(0)'
.do end
This function sets the about dialog box of the default windowing
system. The "title" points to the string that will replace the current
title. If title is &whatisnull. then the title will not be replaced.
The "text" points to a string which will be placed in the about box.
To get multiple lines, embed a new line after each logical line in the
string. If "text" is &whatisnull., then the current text in the about
box will not be replaced.
.if '&lang' eq 'C/C++' .do begin
.note _dwSetAppTitle
.ix '_dwSetAppTitle'
.millust begin
int _dwSetAppTitle( const char *title );
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note dwfSetAppTitle
.ix 'dwfSetAppTitle'
.millust begin
integer function dwfSetAppTitle( title )
character*(*) title
.millust end
.do end
.np
This function sets the main window's title.
.if '&lang' eq 'C/C++' .do begin
.note _dwSetConTitle
.ix '_dwSetConTitle'
.millust begin
int _dwSetConTitle( int handle, const char *title );
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note dwfSetConTitle
.ix 'dwfSetConTitle'
.millust begin
integer function dwfSetConTitle( unit, title )
integer unit
character*(*) title
.millust end
.do end
.np
This function sets the console window's title which corresponds to the
.if '&lang' eq 'C/C++' .do begin
handle
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
unit number
.do end
passed to it.
.if '&lang' eq 'C/C++' .do begin
.note _dwShutDown
.ix '_dwShutDown'
.millust begin
int _dwShutDown( void );
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note dwfShutDown
.ix 'dwfShutDown'
.millust begin
integer function dwfShutDown()
.millust end
.do end
.np
This function shuts down the default windowing I/O system.
The application will continue to execute but no windows will be
available for output.
.if '&lang' eq 'C/C++' .do begin
.note _dwYield
.ix '_dwYield'
.millust begin
int _dwYield( void );
.millust end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.note dwfYield
.ix 'dwfYield'
.millust begin
integer function dwfYield()
.millust end
.do end
.np
This function yields control back to the operating system, thereby
giving other processes a chance to run.
.endnote
.np
.if '&lang' eq 'C/C++' .do begin
These functions are described more fully in the
.book WATCOM C Library Reference.
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
These functions are described more fully in the
.book &product User's Guide.
.do end
