The access permissions for the file or directory are specified as a
combination of bits (defined in the
.hdrfile sys&pc.stat.h
header file).
.pp
The following bits define permissions for the owner.
.begterm 12 $compact
.termhd1 Permission
.termhd2 Meaning
.term S_IRWXU
Read, write, execute/search
.term S_IRUSR
Read permission
.term S_IWUSR
Write permission
.term S_IXUSR
Execute/search permission
.endterm
.pp
The following bits define permissions for the group.
.if '&machsys' eq 'PP' .do begin
PenPoint does not support "group" permissions.
These bits are ignored.
.do end
.begterm 12 $compact
.termhd1 Permission
.termhd2 Meaning
.term S_IRWXG
Read, write, execute/search
.term S_IRGRP
Read permission
.term S_IWGRP
Write permission
.term S_IXGRP
Execute/search permission
.endterm
.pp
The following bits define permissions for others.
.if '&machsys' eq 'PP' .do begin
PenPoint does not support "others" permissions.
These bits are ignored.
.do end
.begterm 12 $compact
.termhd1 Permission
.termhd2 Meaning
.term S_IRWXO
Read, write, execute/search
.term S_IROTH
Read permission
.term S_IWOTH
Write permission
.term S_IXOTH
Execute/search permission
.endterm
.if '&machsys' ne 'PP' .do begin
.pp
The following bits define miscellaneous permissions used by other
implementations.
.begterm 12 $compact
.termhd1 Permission
.termhd2 Meaning
.term S_IREAD
is equivalent to S_IRUSR (read permission)
.term S_IWRITE
is equivalent to S_IWUSR (write permission)
.term S_IEXEC
is equivalent to S_IXUSR (execute/search permission)
.endterm
.do end
