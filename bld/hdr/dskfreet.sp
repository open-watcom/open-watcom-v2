#ifndef _DISKFREE_T_DEFINED
#define _DISKFREE_T_DEFINED
 #define _DISKFREE_T_DEFINED_
 struct _diskfree_t {
     unsigned    total_clusters;
     unsigned    avail_clusters;
     unsigned    sectors_per_cluster;
     unsigned    bytes_per_sector;
 };
 #define diskfree_t _diskfree_t
#endif
