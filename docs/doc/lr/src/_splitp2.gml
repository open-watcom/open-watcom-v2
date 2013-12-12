.func _splitpath2 _wsplitpath2
.synop begin
#include <stdlib.h>
.if '&machsys' eq 'QNX' .do begin
.   .se *drv=node
.do end
.el .do begin
.   .se *drv=drive
.do end
void _splitpath2( const char *inp,
                        char *outp,
                        char **&*drv.,
                        char **dir,
                        char **fname,
                        char **ext );
.ixfunc2 '&Parsing' &func
.if &'length(&wfunc.) ne 0 .do begin
void _wsplitpath2( const wchar_t *inp,
                         wchar_t *outp,
                         wchar_t **&*drv.,
                         wchar_t **dir,
                         wchar_t **fname,
                         wchar_t **ext );
.ixfunc2 '&Parsing' &wfunc
.ixfunc2 '&Wide' &wfunc
.do end
.synop end
.desc begin
The &func function splits up a full pathname into four components
consisting of a
.if '&machsys' eq 'QNX' .do begin
node specification (e.g., //2), directory path (e.g., /home/fred),
file name (e.g., myfile) and file name extension or suffix (e.g.,
dat).
.do end
.el .do begin
drive letter, directory path, file name and file name extension.
.do end
.begnote $setptnt 8
.note inp
The argument
.arg inp
points to a buffer containing the full pathname to be split up.
.note outp
The argument
.arg outp
points to a buffer that will contain all the components of the path,
each separated by a null character.
The maximum size required for this buffer is specified by the manifest
constant
.kw _MAX_PATH2
which is defined in
.mono <stdlib.h>.
.if '&machsys' eq 'QNX' .do begin
.note node
The
.arg node
argument is the location that is to contain the pointer to the node
specification (e.g., //0, //1, etc.) if a node is specified in the
full pathname (filled in by &func.).
.do end
.el .do begin
.note drive
The
.arg drive
argument is the location that is to contain the pointer to
the drive letter (e.g., A, B, C, etc.) followed by a colon if a drive
is specified in the full pathname (filled in by &func.).
.do end
.note dir
The
.arg dir
argument is the location that is to contain the pointer to the
directory path including the trailing slash if a directory path is
specified in the full pathname (filled in by &func.).
.if '&machsys' ne 'QNX' .do begin
Either forward slashes (/) or backslashes (\) may be used.
.do end
.note fname
The
.arg fname
argument is the location that is to contain the pointer to
the base name of the file without any extension (suffix) if a file
name is specified in the full pathname (filled in by &func.).
.note ext
The
.arg ext
argument is the location that is to contain the pointer to
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
.arg &*drv.
.ct ,
.arg dir
.ct ,
.arg fname
and
.arg ext
will not be filled in if they are NULL pointers.
.np
For each component of the full pathname that is not present, its
corresponding pointer will be set to point at a NULL string ('\0').
.np
This function reduces the amount of memory space required when
compared to the
.kw splitpath
function.
.im widefunc
.desc end
.return begin
The &func function returns no value.
.return end
.see begin
.seelist _splitpath2 _fullpath _makepath _splitpath
.see end
.exmp begin
#include <stdio.h>
#include <stdlib.h>

void main()
  {
    char full_path[ _MAX_PATH ];
    char tmp_path[ _MAX_PATH2 ];
    char *&*drv.;
    char *dir;
    char *fname;
    char *ext;
.exmp break
    _makepath(full_path,"c","watcomc\\h","stdio","h");
    printf( "Full path is: %s\n\n", full_path );
    _splitpath2( full_path, tmp_path,
                 &&*drv., &dir, &fname, &ext );
    printf( "Components after _splitpath2\n" );
.if '&machsys' eq 'QNX' .do begin
    printf( "&*drv.:  %s\n", &*drv. );
.do end
.el .do begin
    printf( "&*drv.: %s\n", &*drv. );
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

Components after _splitpath2
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
