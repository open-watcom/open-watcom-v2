.if '&machine' eq '80386' .do begin
:set symbol="exeformat"  value="phar"
.do end
.el .do begin
:set symbol="exeformat"  value="dos"
.do end
.*
.im wlmemlay
.np
In addition to these special segments, the following conventions are
used by &cmpname..
.autonote
.note
.ix 'CODE class'
.ix 'class' 'CODE'
The "CODE" class contains the executable code for your application.
.ix '_TEXT segment'
.ix 'segment' '_TEXT'
In a small code model, this consists of the segment "_TEXT".
.if '&lang' eq 'FORTRAN 77' .do begin
In a big code model, this consists of the segments "<subprogram>_TEXT"
where <subprogram> is the name of a subprogram.
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
In a big code model, this consists of the segments "<module>_TEXT"
where <module> is the file name of the source file.
.do end
.note
.ix 'FAR_DATA class'
.ix 'class' 'FAR_DATA'
The "FAR_DATA" class consists of the following:
.begnote
.if '&lang' eq 'FORTRAN 77' .do begin
.note (a)
arrays whose size exceeds the data threshold in large data memory
models (the data threshold is 256 bytes unless changed using the "dt"
compiler option)
.note (b)
equivalenced variables in large data memory models
.do end
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.note (a)
data objects whose size exceeds the data threshold in large data
memory models (the data threshold is 32K unless changed using the "zt"
compiler option)
.note (b)
data objects defined using the "FAR" or "HUGE" keyword,
.note (c)
literals whose size exceeds the data threshold in large data memory
models (the data threshold is 32K unless changed using the "zt"
compiler option)
.note (d)
literals defined using the "FAR" or "HUGE" keyword.
.do end
.endnote
.endnote
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
You can override the default naming convention used by &cmpname to
name segments.
.autopoint
.point
.ix '&ccmdup16 options' 'nm'
.ix '&pcmdup16 options' 'nm'
.ix '&ccmdup32 options' 'nm'
.ix '&pcmdup32 options' 'nm'
The &cmpname "nm" option can be used to change the name of the module.
This, in turn, changes the name of the code segment when compiling for
a big code model.
.point
.ix '&ccmdup16 options' 'nt'
.ix '&pcmdup16 options' 'nt'
.ix '&ccmdup32 options' 'nt'
.ix '&pcmdup32 options' 'nt'
The &cmpname "nt" option can be used to specify the name of the code
segment regardless of the code model used.
.endpoint
.do end
