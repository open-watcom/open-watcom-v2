.*
.*
.mnote obj_file
is a file specification for the name of an object file.
.if '&target' eq 'QNX' .do begin
If no file extension is specified, a file extension of "o" is assumed.
.do end
.el .do begin
If no file extension is specified, a file extension of "obj" is
assumed if you are running a DOS, OS/2 or Windows-hosted version of
the &lnkname..
Also, if you are running a DOS, OS/2 or Windows-hosted version of
the &lnkname, the object file specification can contain wild cards
(*, ?).
A file extension of "o" is assumed if you are running a UNIX-hosted
version of the &lnkname..
.do end
