.np
The file
.hdrfile sys&pc.stat.h
contains definitions for the structure
.kw stat
.ct .li .
.if '&machsys' ne 'QNX' .do begin
.begterm 10
.termhd1 Field
.termhd2 Type/Meaning
.term st_dev
(dev_t)
the disk drive the file resides on
.term st_ino
(ino_t)
this inode's number (not used for DOS)
.term st_mode
(unsigned short)
file mode
.term st_nlink
(short)
number of hard links
.term st_uid
(unsigned long)
user-id (always 'root' for DOS)
.term st_gid
(short)
group-id (always 'root' for DOS)
.term st_rdev
(dev_t)
this should be the device type but it is the same as st_dev for the
time being
.term st_size
(off_t)
total file size
.term st_atime
(time_t)
this should be the file "last accessed" time if the file system
supports it
.term st_mtime
(time_t)
the file "last modified" time
.term st_ctime
(time_t)
this should be the file "last status change" time if the file system
supports it
.np
.us The following fields are Netware only:
.term st_btime
(time_t)
the file "last archived" time
.term st_attr
(unsigned long)
the file's attributes
.term st_archivedID
(unsigned long)
the user/object ID that last archived file
.term st_updatedID
(unsigned long)
the user/object ID that last updated file
.term st_inheritedRightsMask
(unsigned short)
the inherited rights mask
.term st_originatingNameSpace
(unsigned char)
the originating name space
.endterm
.np
The structure
.kw _stati64
differs from
.kw stat
in the following way:
.begterm
.term st_size
(__int64)
total file size (as a 64-bit value)
.endterm
.np
.do end
.np
At least the following macros are defined in the
.hdrfile sys&pc.stat.h
header file.
.begterm 12
.termhd1 Macro
.termhd2 Meaning
.term S_ISFIFO(m)
Test for FIFO.
.term S_ISCHR(m)
Test for character special file.
.term S_ISDIR(m)
Test for directory file.
.term S_ISBLK(m)
Test for block special file.
.term S_ISREG(m)
Test for regular file.
.if '&machsys' eq 'QNX' .do begin
.term S_ISLNK(m)
Test for symbolic link.
:cmt. .term S_ISNAM(m)
:cmt. Test for special named file
:cmt. .term S_ISSOCK(m)
:cmt. Test for socket.
.do end
.endterm
.np
The value
.arg m
supplied to the macros is the value of the
.kw st_mode
field of a
.kw stat
structure.
The macro evaluates to a non-zero value if the test is true and zero
if the test is false.
.*
.np
The following bits are encoded within the
.kw st_mode
field of a
.kw stat
structure.
.begterm 12 $compact
.termhd1 Mask
.termhd2 Owner Permissions
.term S_IRWXU
Read, write, search (if a directory), or execute (otherwise)
.term S_IRUSR
Read permission bit
.term S_IWUSR
Write permission bit
.term S_IXUSR
Search/execute permission bit
.term S_IREAD
==
.kw S_IRUSR
(for Microsoft compatibility)
.term S_IWRITE
==
.kw S_IWUSR
(for Microsoft compatibility)
.term S_IEXEC
==
.kw S_IXUSR
(for Microsoft compatibility)
.endterm
.np
.kw S_IRWXU
is the bitwise inclusive OR of
.kw S_IRUSR
.ct ,
.kw S_IWUSR
.ct , and
.kw S_IXUSR
.ct .li .
.begterm 12 $compact
.termhd1 Mask
.if '&machsys' eq 'QNX' .do begin
.termhd2 Group Permissions
.do end
.el .do begin
.termhd2 Group Permissions (same as owner's on DOS, OS/2 or Windows)
.do end
.term S_IRWXG
Read, write, search (if a directory), or execute (otherwise)
.term S_IRGRP
Read permission bit
.term S_IWGRP
Write permission bit
.term S_IXGRP
Search/execute permission bit
.endterm
.np
.kw S_IRWXG
is the bitwise inclusive OR of
.kw S_IRGRP
.ct ,
.kw S_IWGRP
.ct , and
.kw S_IXGRP
.ct .li .
.begterm 12 $compact
.termhd1 Mask
.if '&machsys' eq 'QNX' .do begin
.termhd2 Other Permissions
.do end
.el .do begin
.termhd2 Other Permissions (same as owner's on DOS, OS/2 or Windows)
.do end
.term S_IRWXO
Read, write, search (if a directory), or execute (otherwise)
.term S_IROTH
Read permission bit
.term S_IWOTH
Write permission bit
.term S_IXOTH
Search/execute permission bit
.endterm
.np
.kw S_IRWXO
is the bitwise inclusive OR of
.kw S_IROTH
.ct ,
.kw S_IWOTH
.ct , and
.kw S_IXOTH
.ct .li .
.begterm 12 $compact
.termhd1 Mask
.termhd2 Meaning
.term S_ISUID
.if '&machsys' ne 'QNX' .do begin
(Not supported by DOS, OS/2 or Windows)
.do end
Set user ID on execution.
The process's effective user ID shall be set to that of the owner of
the file when the file is run as a program. On a regular file, this
bit should be cleared on any write.
.term S_ISGID
.if '&machsys' ne 'QNX' .do begin
(Not supported by DOS, OS/2 or Windows)
.do end
Set group ID on execution.
Set effective group ID on the process to the file's group when the
file is run as a program. On a regular file, this bit should be
cleared on any write.
.endterm
