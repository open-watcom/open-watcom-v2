.np
The file
.hdrfile &dirhdr
contains definitions for the structure
.if '&machsys' eq 'QNX' .do begin
.kw udirent
and the
.kw UDIR
type.
.do end
.el .do begin
.kw udirent
.ct .li .
.do end
.blkcode begin
.if '&machsys' eq 'QNX' .do begin
/*
 * Please note:
 *
 *     The d_stat data is valid if
 *           d_stat.st_status & _FILE_USED
 *     If it is not flagged as used, you must call stat()
 *     to get the data.
 */
struct udirent {
  struct stat   d_stat;
  char          d_name[1 + NAME_MAX];
};

typedef struct _dir {
  int           dd_fd,            /* file descriptor    */
                dd_loc,           /* index into array   */
                dd_size;          /* num array elements */
  struct udirent dd_buf[_DIRBUF];  /* _dir_entry array   */
  char          dd_prefixes[200]; /* qnx_prefix_query   */
} UDIR;
.do end
.el .do begin
typedef struct udirent {
    char    d_dta[ 21 ];       /* disk transfer area */
    char    d_attr;            /* file's attribute */
    unsigned short int d_time; /* file's time */
    unsigned short int d_date; /* file's date */
    long    d_size;            /* file's size */
    char    d_name[ 13 ];      /* file's name */
    ino_t   d_ino;             /* serial number */
    char    d_first;           /* flag for 1st time */
} UDIR;
.do end
.blkcode end
