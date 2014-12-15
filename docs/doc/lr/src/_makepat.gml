.func _makepath _wmakepath
.synop begin
#include <stdlib.h>
void _makepath( char *path,
.if '&machsys' eq 'QNX' .do begin
                const char *node,
.do end
.el .do begin
                const char *drive,
.do end
                const char *dir,
                const char *fname,
                const char *ext );
.ixfunc2 '&Parsing' &funcb
.if &'length(&wfunc.) ne 0 .do begin
void _wmakepath( wchar_t *path,
.if '&machsys' eq 'QNX' .do begin
                  const wchar_t *node,
.do end
.el .do begin
                  const wchar_t *drive,
.do end
                  const wchar_t *dir,
                  const wchar_t *fname,
                  const wchar_t *ext );
.ixfunc2 '&Parsing' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The
.id &funcb.
function constructs a full pathname from the components
consisting of a
.if '&machsys' eq 'QNX' .do begin
node specification (e.g., //2),
directory path (e.g., /home/fred),
file name (e.g., myfile)
and
file name extension or suffix (e.g., dat).
.do end
.el .do begin
drive letter,
directory path,
file name
and
file name extension.
.do end
The full pathname
.if '&machsys' eq 'QNX' .do begin
(e.g., //2/home/fred/myfile.dat)
.do end
is placed in the buffer pointed to by the argument
.arg path
.ct .li .
.im widefunc
.np
The maximum size required for each buffer is specified by the manifest
constants
.kw _MAX_PATH
.ct,
.if '&machsys' eq 'QNX' .do begin
.kw _MAX_NODE
.ct,
.do end
.el .do begin
.kw _MAX_DRIVE
.ct,
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
.if '&machsys' eq 'QNX' .do begin
.note node
The
.arg node
argument points to a buffer containing the node specification
(e.g., //0, //1, etc.) followed by an optional "/".
The
.id &funcb.
function will automatically insert a "/" following the node
number in the full pathname if it is missing.
If
.arg node
is a NULL pointer or points to an empty string, no node specification
will be placed in the full pathname.
.do end
.el .do begin
.note drive
The
.arg drive
argument points to a buffer containing the drive letter (A, B, C,
etc.) followed by an optional colon.
The
.id &funcb.
function will automatically insert a colon in the full
pathname if it is missing.
If
.arg drive
is a NULL pointer or points to an empty string, no drive letter or
colon will be placed in the full pathname.
.do end
.note dir
The
.arg dir
argument points to a buffer containing just the pathname.
.if '&machsys' ne 'QNX' .do begin
Either forward slashes (/) or backslashes (\) may be used.
.do end
The trailing slash is optional.
The
.id &funcb.
function will automatically insert a trailing slash in the
full pathname if it is missing.
If
.arg dir
is a NULL pointer or points to an empty string, no slash will be
placed in the full pathname.
.note fname
The
.arg fname
argument points to a buffer containing the base name of the file
without any extension (suffix).
.note ext
The
.arg ext
argument points to a buffer containing the filename extension or
suffix.
A leading period (.) is optional.
The
.id &funcb.
routine will automatically insert a period in the full
pathname if it is missing.
If
.arg ext
is a NULL pointer or points to an empty string, no period will be
placed in the full pathname.
.endnote
.desc end
.return begin
The
.id &funcb.
function returns no value.
.return end
.see begin
.seelist _fullpath _splitpath
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char full_path[ _MAX_PATH ];
.if '&machsys' eq 'QNX' .do begin
    char node[ _MAX_NODE ];
.do end
.el .do begin
    char drive[ _MAX_DRIVE ];
.do end
    char dir[ _MAX_DIR ];
    char fname[ _MAX_FNAME ];
    char ext[ _MAX_EXT ];
.exmp break
.if '&machsys' eq 'QNX' .do begin
    _makepath(full_path,"//0","/home/fred/h","stdio","h");
.do end
.el .do begin
    _makepath(full_path,"c","watcomc\\h\\","stdio","h");
.do end
    printf( "Full path is: %s\n\n", full_path );
.if '&machsys' eq 'QNX' .do begin
    _splitpath( full_path, node, dir, fname, ext );
.do end
.el .do begin
    _splitpath( full_path, drive, dir, fname, ext );
.do end
    printf( "Components after _splitpath\n" );
.if '&machsys' eq 'QNX' .do begin
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
.if '&machsys' eq 'QNX' .do begin
Full path is: //0/home/fred/h/stdio.h
.do end
.el .do begin
Full path is: c:watcomc\h\stdio.h
.do end

Components after _splitpath
.if '&machsys' eq 'QNX' .do begin
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
