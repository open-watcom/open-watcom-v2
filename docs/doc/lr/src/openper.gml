.np
When the file is to be created (
.ct .kw O_CREAT
is specified), an additional argument must be passed which contains
the file permissions to be used for the new file.
.im openperv
.if '&machsys' ne 'PP' .do begin
.if '&machsys' ne 'QNX' .do begin
.np
All files are readable with DOS; however, it is a good idea to set
.kw S_IREAD
when read permission is intended for the file.
.do end
.do end
.np
The &func function applies the current file permission mask
to the specified permissions (see
.kw umask
.ct ).
