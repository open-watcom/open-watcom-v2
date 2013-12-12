.func _dos_getdiskfree
.synop begin
#include <&doshdr>
unsigned _dos_getdiskfree( unsigned drive,
                        struct diskfree_t *diskspace );
struct diskfree_t {
        unsigned short total_clusters;
        unsigned short avail_clusters;
        unsigned short sectors_per_cluster;
        unsigned short bytes_per_sector;
};
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The &func function uses system call 0x36 to obtain useful information
on the disk drive specified by
.arg drive
.ct .li .
Specify 0 for the default drive, 1 for drive A, 2 for drive B, etc.
The information about the drive is returned in the structure
.kw diskfree_t
pointed to by
.arg diskspace
.ct .li .
.desc end
.return begin
The &func function returns zero if successful.
Otherwise, it returns a non-zero value and sets
.kw errno
to
.kw EINVAL
indicating an invalid drive was specified.
.return end
.see begin
.seelist &function. _dos_getdiskfree _dos_getdrive _dos_setdrive _getdiskfree _getdrive
.see end
.exmp begin
#include <stdio.h>
#include <&doshdr>
.exmp break
void main()
  {
    struct diskfree_t disk_data;
.exmp break
    /* get information about drive 3 (the C drive) */
    if( _dos_getdiskfree( 3, &disk_data ) == 0 ) {
      printf( "total clusters: %u\n",
                        disk_data.total_clusters );
      printf( "available clusters: %u\n",
                        disk_data.avail_clusters );
      printf( "sectors/cluster: %u\n",
                        disk_data.sectors_per_cluster );
      printf( "bytes per sector: %u\n",
                        disk_data.bytes_per_sector );
    } else {
      printf( "Invalid drive specified\n" );
    }
  }
.exmp output
total clusters: 16335
available clusters: 510
sectors/cluster: 4
bytes per sector: 512
.exmp end
.class DOS
.system
