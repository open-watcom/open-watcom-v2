.np
The access mode is established by a combination of the bits defined in
the
.hdrfile fcntl.h
header file.
The following bits may be set:
.begterm 15
.termhd1 Mode
.termhd2 Meaning
.term O_RDONLY
permit the file to be only read.
.term O_WRONLY
permit the file to be only written.
.term O_RDWR
permit the file to be both read and written.
.term O_APPEND
causes each record that is written to be written at the end
of the file.
.term O_CREAT
has no effect when the file indicated by
.arg filename
already exists;
otherwise, the file is created;
.term O_TRUNC
causes the file to be truncated to contain no data when the file exists;
has no effect when the file does not exist.
.if '&machsys' ne 'QNX' .do begin
.term O_BINARY
causes the file to be opened in binary mode which means that data
will be transmitted to and from the file unchanged.
.term O_TEXT
causes the file to be opened in text mode which means that
carriage-return characters are written before any linefeed character
that is written and causes
carriage-return characters to be removed when encountered during
reads.
.term O_NOINHERIT
indicates that this file is not to be inherited by a child process.
.do end
.if '&machsys' eq 'QNX' .do begin
.term O_TEMP
indicates that this file is to be treated as "temporary". It is a request
to keep the data in cache, if possible, for fast access to temporary files.
.do end
.term O_EXCL
indicates that this file is to be opened for exclusive access.
If the file exists and
.kw O_CREAT
was also specified then the open will fail (i.e., use
.kw O_EXCL
to ensure that the file does not already exist).
.endterm
.if '&machsys' ne 'QNX' .do begin
.np
When neither
.kw O_TEXT
nor
.kw O_BINARY
are specified, the default value in the global variable
.kw _fmode
is used to set the file translation mode.
When the program begins execution, this variable has a value of
.kw O_TEXT
.ct .li .
.do end
.np
.kw O_CREAT
must be specified when the file does not exist and it is to be written.
