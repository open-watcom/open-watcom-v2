struct stat {
        unsigned long  st_dev;
        unsigned long  st_ino;
        unsigned short st_mode;
        unsigned short st_nlink;
        unsigned short st_uid;
        unsigned short st_gid;
        unsigned long  st_rdev;
        unsigned long  st_size;
        unsigned long  st_blksize;
        unsigned long  st_blocks;
        time_t         st_atime;
        unsigned long  st_atime_nsec;
        time_t         st_mtime;
        unsigned long  st_mtime_nsec;
        time_t         st_ctime;
        unsigned long  st_ctime_nsec;
        unsigned long  __unused4;
        unsigned long  __unused5;
};

/* This matches struct stat64 in glibc2.1, hence the absolutely
 * insane amounts of padding around dev_t's.
 */
struct stat64 {
        unsigned long long      st_dev;
        unsigned char           __pad0[4];
#define STAT64_HAS_BROKEN_ST_INO        1
        unsigned long           __st_ino;
        unsigned int            st_mode;
        unsigned int            st_nlink;
        unsigned long           st_uid;
        unsigned long           st_gid;
        unsigned long long      st_rdev;
        unsigned char           __pad3[4];
        long long               st_size;
        unsigned long           st_blksize;
        unsigned long           st_blocks;  /* Number 512-byte blocks allocated. */
        unsigned long           __pad4;     /* future possible st_blocks high bits */
        time_t                  st_atime;
        unsigned long           st_atime_nsec;
        time_t                  st_mtime;
        unsigned int            st_mtime_nsec;
        time_t                  st_ctime;
        unsigned long           st_ctime_nsec;
        unsigned long long      st_ino;
};

#define STAT_HAVE_NSEC 1
