.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="stdin"  value="unit 5".
:set symbol="stdout"  value="unit 6".
.do end
.if '&lang' eq 'C' .do begin
:set symbol="stdin"  value="stdin".
:set symbol="stdout"  value="stdout".
.do end
.if '&lang' eq 'C/C++' .do begin
:set symbol="stdin"  value="stdin (C++ cin)".
:set symbol="stdout"  value="stdout (C++ cout)".
.do end
.*
.section Console Device in a Windowed Environment
.*
.np
In a &lang application that runs under DOS,
.us &stdin
and
.us &stdout
are connected to the standard input and standard output devices
respectively.
It is not a recommended practice to read directly from the standard
input device or write to the standard output device when running in a
windowed environment.
.ix 'windowed applications' 'default windowing environment'
For this reason, a default windowing environment is created for &lang
applications that read from
.us &stdin
or write to
.us &stdout..
When your application is started, a window is created in which output
to
.us &stdout
is displayed and input from
.us &stdin
is requested.
.np
In addition to the standard I/O device, it is also possible to perform
I/O to the console by explicitly opening a file whose name is "CON".
When this occurs, another window is created and displayed.
This window is different from the one created for standard input and
standard output.
In fact, every time you open the console device a different window is
created.
This provides a simple multi-windowing system for multiple streams
of data to and from the console device.
