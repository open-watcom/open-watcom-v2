.np
The file
.hdrfile &dirhdr
contains definitions for the structure
.if '&machsys' eq 'QNX' .do begin
.kw _wdirent
and the
.kw DIR
type.
.do end
.el .do begin
.kw _wdirent
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
struct _wdirent {
  struct stat   d_stat;
  wchar_t       d_name[1 + NAME_MAX];
};

typedef struct _dir {
  int           dd_fd,            /* file descriptor    */
                dd_loc,           /* index into array   */
                dd_size;          /* num array elements */
  struct _wdirent dd_buf[_DIRBUF];/* _dir_entry array   */
  wchar_t       dd_prefixes[200]; /* qnx_prefix_query   */
} UDIR;
.do end
.el .do begin
struct _wdirent {
    char    d_dta[21];      /* disk transfer area */
    char    d_attr;         /* file's attribute */
    unsigned short int d_time;/* file's time */
    unsigned short int d_date;/* file's date */
    long    d_size;         /* file's size */
    wchar_t d_name[NAME_MAX+1];/* file's name */
    unsigned short d_ino;   /* serial number (not used) */
    char    d_first;        /* flag for 1st time */
};
.do end
.blkcode end
