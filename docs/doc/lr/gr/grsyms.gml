.if '&which' eq 'C' .do begin
:SET SYMBOL='lang'          VALUE='C'.
:SET SYMBOL='langsuff'      VALUE='c'.
:SET SYMBOL='langsuffup'    VALUE='C'.
:SET SYMBOL='langabrv'      VALUE='C'.
:SET SYMBOL='short'         VALUE='short'.
:SET SYMBOL='double'        VALUE='double'.
:SET SYMBOL='long'          VALUE='long'.
:SET SYMBOL='hext'          VALUE='h'.
:SET SYMBOL='routine'       VALUE='function'.
:SET SYMBOL='routines'      VALUE='functions'.
:SET SYMBOL='rroutines'     VALUE='Functions'.
:SET SYMBOL='strings'       VALUE='strings'.
.if '&machsys' eq 'QNX' .do begin
:SET SYMBOL='isbn'          VALUE='1-55094-053-8'.
.do end
.el .if '&machsys' eq 'NEC' .do begin
:SET SYMBOL='isbn'          VALUE='x-xxxxx-xxx-x'.
.do end
.el .do begin
:CMT. :SET SYMBOL='isbn'          VALUE='1-55094-040-6'.
.do end
.do end
.el .do begin
:SET SYMBOL='lang'          VALUE='FORTRAN 77'.
:SET SYMBOL='langsuff'      VALUE='for'.
:SET SYMBOL='langsuffup'    VALUE='FOR'.
:SET SYMBOL='langabrv'      VALUE='FORTRAN'.
:SET SYMBOL='short'         VALUE='integer*2'.
:SET SYMBOL='double'        VALUE='double precision'.
:SET SYMBOL='long'          VALUE='integer*4'.
:SET SYMBOL='hext'          VALUE='fi'.
:SET SYMBOL='routine'       VALUE='routine'.
:SET SYMBOL='routines'      VALUE='routines'.
:SET SYMBOL='rroutines'     VALUE='Routines'.
:SET SYMBOL='strings'       VALUE='addresses of strings'.
:SET SYMBOL='isbn'          VALUE=''.
.do end
