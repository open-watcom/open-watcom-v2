From: roskos@dockmaster.arpa (Eric Roskos)
Newsgroups: comp.binaries.ibm.pc
Subject: Public Domain TAR w/source
Date: 11 Jun 88 04:10:16 GMT
Summary: pdtar.arc, public domain TAR w/source code
Approved: dick@slvblc.UUCP

[This port supports both MS-DOS and Minix.  DF]

tar provides a way to store many files into a single archive, which can
be kept in another Unix file, stored on an I/O device such as tape,
floppy, cartridge, or disk, or piped to another program.  It is useful
for making backup copies, or for packaging up a set of files to move
them to another system.

tar has existed since Version 7 Unix with very little change.  It has
been proposed as the standard format for interchange of files among
systems that conform to the P1003 "Portable Operating System" standard.

This version of tar supports the extensions which were proposed in
the P1003 draft standards, including owner and group names, and support
for named pipes, fifos, and block and character devices.  Under DOS
and Minix, it also supports the P1003 Trial Use Standard format for
multivolume archives on floppy disk.

When run under DOS, this version of tar performs various conversions
intended to aid in transferring files between DOS and Unix.  Filenames
are converted by deleting all but the last "." from the filename when
reading an archive under DOS; and they are converted to lowercase when
writing an archive under DOS.  (If you use the -v option, it will show
you when dot conversions like this take place.)  If a conversion is
dubious, it will prompt you to enter a proper DOS filename.

Also when running under DOS, conversion between newlines and CR/LFs
is performed for ASCII files (the archive is always written in the
Unix-compatible form, i.e., with only newlines present).

When reading an archive, this version of tar continues after finding
an error.  Previous versions required the "i" option to ignore checksum
errors.

This port is by Eric Roskos (csed-1!roskos, roskos@CSED-1.IDA.ORG,
or Roskos@DOCKMASTER.ARPA), and has unfortunately diverged too far to
make re-merging with future releases of the original PD Tar very
feasible.
