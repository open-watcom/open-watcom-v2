struct stat {
        unsigned        st_dev;
        long            st_pad1[3];             /* Reserved for network id */
        ino_t           st_ino;
        mode_t          st_mode;
        nlink_t         st_nlink;
        uid_t           st_uid;
        gid_t           st_gid;
        unsigned        st_rdev;
        long            st_pad2[2];
        off_t           st_size;
        long            st_pad3;
        /*
         * Actually this should be timestruc_t st_atime, st_mtime and st_ctime
         * but we don't have it under Linux.
         */
        time_t          st_atime;
        long            st_atime_nsec;
        time_t          st_mtime;
        long            st_mtime_nsec;
        time_t          st_ctime;
        long            st_ctime_nsec;
        long            st_blksize;
        long            st_blocks;
        long            st_pad4[14];
};

/*
 * This matches struct stat64 in glibc2.1, hence the absolutely insane
 * amounts of padding around dev_t's.  The memory layout is the same as of
 * struct stat of the 64-bit kernel.
 */

struct stat64 {
        unsigned long       st_dev;
        unsigned long       st_pad0[3];     /* Reserved for st_dev expansion  */
        unsigned long long  st_ino;
        mode_t              st_mode;
        nlink_t             st_nlink;
        uid_t               st_uid;
        gid_t               st_gid;
        unsigned long       st_rdev;
        unsigned long       st_pad1[3];     /* Reserved for st_rdev expansion  */
        long long           st_size;
        /*
         * Actually this should be timestruc_t st_atime, st_mtime and st_ctime
         * but we don't have it under Linux.
         */
        time_t              st_atime;
        unsigned long       st_atime_nsec;  /* Reserved for st_atime expansion  */
        time_t              st_mtime;
        unsigned long       st_mtime_nsec;  /* Reserved for st_mtime expansion  */
        time_t              st_ctime;
        unsigned long       st_ctime_nsec;  /* Reserved for st_ctime expansion  */
        unsigned long       st_blksize;
        unsigned long       st_pad2;
        long long           st_blocks;
};
