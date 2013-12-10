.np
The file
.hdrfile &dirhdr
contains definitions for the structure
.if '&machsys' eq 'QNX' .do begin
.kw dirent
and the
.kw DIR
type.
.np
In QNX the
.kw dirent
structure contains a
.kw stat
structure in the
.kw d_stat
member.
To speed up applications which often want both the name and the stat
data, a resource manager may return the
.kw stat
information at the same time the
.kw readdir
function is called.
.np
However, since the support of this feature is left to the discretion
of various resource managers, every program must use the following
test to determine if the
.kw d_stat
member contains valid data:
.millust begin
d_stat.st_status & _FILE_USED
.millust end
.pc
This test must be performed after every
.kw readdir
call.
.np
If the
.kw d_stat
member doesn't contain valid data and the data is needed then the
application should construct the file's pathname and call
.kw stat
or
.kw lstat
as appropriate.
.do end
.*
.el .do begin
.kw dirent
.ct .li .
.blkcode begin
#if defined(__OS2__) || defined(__NT__)
#define NAME_MAX 255    /* maximum for HPFS or NTFS */
#else
#define NAME_MAX  12    /* 8 chars + '.' +  3 chars */
#endif

typedef struct dirent {
    char    d_dta[ 21 ];        /* disk transfer area */
    char    d_attr;             /* file's attribute */
    unsigned short int d_time;  /* file's time */
    unsigned short int d_date;  /* file's date */
    long    d_size;             /* file's size */
    char    d_name[ NAME_MAX + 1 ]; /* file's name */
    unsigned short d_ino;       /* serial number */
    char    d_first;            /* flag for 1st time */
} DIR;
.blkcode end
.np
The file attribute field
.kw d_attr
field is a set of bits representing the following attributes.
.blkcode begin
_A_RDONLY       /* Read-only file */
_A_HIDDEN       /* Hidden file */
_A_SYSTEM       /* System file */
_A_VOLID        /* Volume-ID entry (only MSFT knows) */
_A_SUBDIR       /* Subdirectory */
_A_ARCH         /* Archive file */
.blkcode end
.np
If the
.mono _A_RDONLY
bit is off, then the file is read/write.
.np
The format of the
.kw d_time
field is described by the following structure
(this structure is not defined in any &company header file).
.blkcode begin
typedef struct {
    unsigned short  twosecs : 5;    /* seconds / 2 */
    unsigned short  minutes : 6;    /* minutes (0,59) */
    unsigned short  hours   : 5;    /* hours (0,23) */
} ftime_t;
.blkcode end
.np
The format of the
.kw d_date
field is described by the following structure
(this structure is not defined in any &company header file).
.blkcode begin
typedef struct {
    unsigned short  day     : 5;    /* day (1,31) */
    unsigned short  month   : 4;    /* month (1,12) */
    unsigned short  year    : 7;    /* 0 is 1980 */
} fdate_t;
.blkcode end
.np
See the sample program below for an example of the use of these
structures.
.do end
