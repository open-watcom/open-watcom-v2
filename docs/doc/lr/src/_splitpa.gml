.func _splitpath _wsplitpath _u_splitpath
#include <stdlib.h>
void _splitpath( const char *path,
.if '&machsys' eq 'PP' .do begin
                       char *node,
.do end
.el .if '&machsys' eq 'QNX' .do begin
                       char *node,
.do end
.el .do begin
                       char *drive,
.do end
                       char *dir,
                       char *fname,
                       char *ext );
.ixfunc2 '&Parsing' '&func'
.if &'length(&wfunc.) ne 0 .do begin
void _wsplitpath( const wchar_t *path,
.if '&machsys' eq 'PP' .do begin
                        wchar_t *node,
.do end
.el .if '&machsys' eq 'QNX' .do begin
                        wchar_t *node,
.do end
.el .do begin
                        wchar_t *drive,
.do end
                        wchar_t *dir,
                        wchar_t *fname,
                        wchar_t *ext );
.ixfunc2 '&Parsing' '&wfunc'
.ixfunc2 '&Wide' '&wfunc'
.do end
.if &'length(&ufunc.) ne 0 .do begin
void _u_splitpath( const wchar_t *path,
.if '&machsys' eq 'PP' .do begin
                         wchar_t *node,
.do end
.el .if '&machsys' eq 'QNX' .do begin
                         wchar_t *node,
.do end
.el .do begin
                         wchar_t *drive,
.do end
                         wchar_t *dir,
                         wchar_t *fname,
                         wchar_t *ext );
.ixfunc2 '&Parsing' '&ufunc'
.do end
.funcend
.desc begin
The &func function splits up a full pathname into four components
consisting of a
.if '&machsys' eq 'PP' .do begin
node specification (e.g., \\foo),
directory path (e.g., \usr\include),
file name (e.g., myfile)
and
file name extension or suffix (e.g., .dat).
.do end
.el .if '&machsys' eq 'QNX' .do begin
node specification (e.g., //2),
directory path (e.g., /home/fred),
file name (e.g., myfile)
and
file name extension or suffix (e.g., .dat).
.do end
.el .do begin
drive letter,
directory path,
file name
and
file name extension.
.do end
The argument
.arg path
points to a buffer containing the full pathname to be split up.
.im widefunc
.im unifunc
.np
The maximum size required for each buffer is specified by the manifest
constants
.kw _MAX_PATH
.ct,
.if '&machsys' eq 'PP' .do begin
.kw _MAX_NODE
.ct,
.do end
.el .if '&machsys' eq 'QNX' .do begin
.kw _MAX_NODE
.ct,
.do end
.el .do begin
.kw _MAX_DRIVE
(or
.kw _MAX_VOLUME
for Netware applications),
.do end
.kw _MAX_DIR
.ct,
.kw _MAX_FNAME
.ct,
and
.kw _MAX_EXT
which are defined in
.mono <stdlib.h>.
.begnote $setptnt 8
.if '&machsys' eq 'PP' .do begin
.note node
The
.arg node
argument points to a buffer that will be filled in with the node
specification (e.g., \\foo, \\fred, etc.) if a node is specified in
the full pathname.
.do end
.el .if '&machsys' eq 'QNX' .do begin
.note node
The
.arg node
argument points to a buffer that will be filled in with the node
specification (e.g., //0, //1, etc.) if a node is specified in the
full pathname.
.do end
.el .do begin
.note drive
The
.arg drive
argument points to a buffer that will be filled in with
the drive letter (e.g., A, B, C, etc.) followed by a colon if a drive
is specified in the full pathname (filled in by &func.).
.np
For Netware applications, the
.arg drive
argument points to a buffer that will be filled in with
the volume identifier (e.g., \\NAME_SPACE) if a volume
is specified in the full pathname (filled in by &func.).
.do end
.note dir
The
.arg dir
argument points to a buffer that will be filled in with the pathname
including the trailing slash.
.if '&machsys' ne 'QNX' .do begin
Either forward slashes (/) or backslashes (\) may be used.
.do end
.note fname
The
.arg fname
argument points to a buffer that will be filled in with
the base name of the file without any extension (suffix) if a file
name is specified in the full pathname (filled in by &func.).
.note ext
The
.arg ext
argument points to a buffer that will be filled in with
the filename extension (suffix) including the leading period if an
extension is specified in the full pathname (filled in by &func.).
.if '&machsys' eq 'QNX' .do begin
If more than one period appears in the filename, the suffix consists
of the final period and characters following it.
If
.arg ext
is a NULL pointer then the extension or suffix is included with the
file name.
.do end
.endnote
.np
The arguments
.if '&machsys' eq 'PP' .do begin
.arg node
.ct ,
.do end
.el .if '&machsys' eq 'QNX' .do begin
.arg node
.ct ,
.do end
.el .do begin
.arg drive
.ct ,
.do end
.arg dir
.ct ,
.arg fname
and
.arg ext
will not be filled in if they are NULL pointers.
.np
For each component of the full pathname that is not present, its
corresponding buffer will be set to an empty string.
.desc end
.return begin
The &func function returns no value.
.return end
.see begin
.seelist _splitpath _fullpath _makepath _splitpath2
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char full_path[ _MAX_PATH ];
.if '&machsys' eq 'PP' .do begin
    char node[ _MAX_NODE ];
.do end
.el .if '&machsys' eq 'QNX' .do begin
    char node[ _MAX_NODE ];
.do end
.el .do begin
    char drive[ _MAX_DRIVE ];
.do end
    char dir[ _MAX_DIR ];
    char fname[ _MAX_FNAME ];
    char ext[ _MAX_EXT ];
.exmp break
.if '&machsys' eq 'PP' .do begin
    _makepath(full_path,"\\\\foo","\\fred\\h","stdio","h");
.do end
.el .if '&machsys' eq 'QNX' .do begin
    _makepath(full_path,"//0","/home/fred/h","stdio","h");
.do end
.el .do begin
    _makepath(full_path,"c","watcomc\\h\\","stdio","h");
.do end
    printf( "Full path is: %s\n\n", full_path );
.if '&machsys' eq 'PP' .do begin
    _splitpath( full_path, node, dir, fname, ext );
.do end
.el .if '&machsys' eq 'QNX' .do begin
    _splitpath( full_path, node, dir, fname, ext );
.do end
.el .do begin
    _splitpath( full_path, drive, dir, fname, ext );
.do end
    printf( "Components after _splitpath\n" );
.if '&machsys' eq 'PP' .do begin
    printf( "node:  %s\n", node );
.do end
.el .if '&machsys' eq 'QNX' .do begin
    printf( "node:  %s\n", node );
.do end
.el .do begin
    printf( "drive: %s\n", drive );
.do end
    printf( "dir:   %s\n", dir );
    printf( "fname: %s\n", fname );
    printf( "ext:   %s\n", ext );
  }
.exmp output
.if '&machsys' eq 'PP' .do begin
Full path is: \\foo\fred\h\stdio.h
.do end
.el .if '&machsys' eq 'QNX' .do begin
Full path is: //0/home/fred/h/stdio.h
.do end
.el .do begin
Full path is: c:watcomc\h\stdio.h
.do end

Components after _splitpath
.if '&machsys' eq 'PP' .do begin
node:  \\foo
dir:   \fred\h\
.do end
.el .if '&machsys' eq 'QNX' .do begin
node:  //0
dir:   /home/fred/h/
.do end
.el .do begin
drive: c:
dir:   watcomc\h\
.do end
fname: stdio
ext:   .h
.im dblslash
.exmp end
.class WATCOM
.system
