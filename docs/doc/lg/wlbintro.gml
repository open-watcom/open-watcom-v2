.*
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
The &libname can be used to create and update object library files.
It takes as input an object file or a library file and creates or updates
a library file.
..if '&cmpcmd' ne 'wcexp' ..do begin
For OS/2, Win16 and Win32 applications, it can also create import
libraries from Dynamic Link Libraries.
..do end
.ix 'library manager'
.np
An object library is essentially a collection of object files.
These object files generally contain utility routines that can be used
as input to the &lnkname to create an application.
The following are some of the advantages of using library files.
.autonote
.note
Only those modules that are referenced will be included in the
executable file.
This eliminates the need to know which object files should be
included and which ones should be left out when linking an application.
.note
Libraries are a good way of organizing object files.
When linking an application, you need only list one library file
instead of several object files.
.endnote
..if '&cmpcmd' ne 'wcexp' ..do begin
.np
The &libname currently runs under the following operating systems.
.begbull
.bull
DOS
.bull
OS/2
.bull
QNX
.bull
Windows
.endbull
