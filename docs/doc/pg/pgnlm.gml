.chap Creating NetWare 386 NLM Applications
.*
.np
.ix 'NLM support' 'version 4.0'
.ix 'NLM support' 'version 4.1'
&product supports version 4.0 of the Netware 386 API.
We include the following components:
.begnote $break
.note header files
.ix 'NLM' 'header files'
Header files for the Netware 4.0 API are located in the
.fi &pathnamup.\NOVH
directory.
.note import libraries
.ix 'NLM' 'import libraries'
Import libraries for the Netware 4.0 API are located in the
.fi &pathnamup.\NOVI
directory.
.note libraries
.ix 'NLM' 'libraries'
The &lang libraries for Netware 4.0 is located in the
.fi &pathnamup.\LIB386
and
.fi &pathnamup.\LIB386\NETWARE
directories.
.note debug servers
.ix 'NLM' 'debugging'
Servers for remote debugging of Netware 4.0 NLMs are located in the
.fi &pathnamup.\NLM
directory.
.ix 'NLM' 'sampler'
The same directory also contains the &smpname for NLMs.
.endnote
.np
Applications built for version 4.0 will run on 4.1.
We do not include support for any API specific to version 4.1.
Netware developers must use the support included with &product version
10.0 or greater since the version supplied by Novell only works
with &product version 9.5.
Netware 4.1 support requires modification to the header files supplied
by Novell.
Contact Novell for more information.
.np
The following special notes apply to developing applications for NetWare.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.autonote
.note
You must compile your source files with the "&sw.bt=NETWARE" option.
This will cause the compiler to:
.begbull $compact
.bull
use the small memory model instead of the flat memory model,
.bull
use stack-based calling conventions,
.bull
search the
.ev NETWARE_INCLUDE
environment variable before searching the
.ev INCLUDE
environment variable, and
.bull
reference a special startup symbol,
.kw __WATCOM_Prelude
.ct , in the libraries.
.endbull
.note
You must compile your source files with the small memory model option
("ms").
This is accomplished by specifying the "&sw.bt=NETWARE" option.
.note
You must compile your source files with one of the stack-based calling
convention options ("3s", "4s" or "5s").
This is accomplished by specifying the "&sw.bt=NETWARE" option.
.note
You must set the
.ev NETWARE_INCLUDE
environment variable to point to the
.fi &pathnamup.\NOVH
directory.
This environment variable will be searched first when you compile with
the "&sw.bt=NETWARE" option.
Alternatively, you can set the
.ev INCLUDE
environment variable to include
.fi &pathnamup.\NOVH
before other include directories.
.note
If you are using the compile and link utility &wclvarup32.,
you must use the following options: "&sw.l=NETWARE &sw.bt=NETWARE".
.note
You must specify
.millust begin
system NETWARE
.millust end
when linking an NLM.
This is automatic if you are using &wclvarup32. and the "&sw.l=NETWARE"
option.
.note
If you are using other Netware APIs such as NWSNUT, then you
must include
.kw module
and
.kw import
statements as input to the &lnkname..
.exam begin
module nwsnut
import @%WATCOM%\novi\nwsnut.imp
.exam end
.pc
This is done automatically for the C Library (CLIB.IMP).
The following import lists have been provided for Netware API
libraries.
.millust begin
AIO.IMP
APPLETLK.IMP
BSD.IMP
CLIB.IMP
DSAPI.IMP
MATHLIB.IMP
NWSNUT.IMP
SOCKLIB.IMP
STREAMS.IMP
TLI.IMP
.millust end
.endnote
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.autonote
.note
You must compile your source files with the small memory model option
("ms").
.note
You must compile your source files with the stack-based calling
convention option ("sc").
.note
You must specify
.millust begin
system NETWARE
.millust end
when linking an NLM.
This is automatic if you are using &wclvarup32. and the "&sw.l=NETWARE"
option.
.endnote
.do end
