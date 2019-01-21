/*
 *  Common filetype macros
 */
#define S_ISUID     004000      /* set user id on execution         */
#define S_ISGID     002000      /* set group id on execution        */
#define S_ISVTX     001000      /* sticky bit (does nothing yet)    */

#define S_ENFMT     002000      /* enforcement mode locking         */

/*
 *  Owner permissions
 */
#define S_IRWXU     000700      /* Read, write, execute/search      */
#define S_IRUSR     000400      /* Read permission                  */
#define S_IWUSR     000200      /* Write permission                 */
#define S_IXUSR     000100      /* Execute/search permission        */

#define S_IREAD     S_IRUSR     /* Read permission                  */
#define S_IWRITE    S_IWUSR     /* Write permission                 */
#define S_IEXEC     S_IXUSR     /* Execute/search permission        */

:include ext.sp
#define _S_IREAD    S_IREAD
#define _S_IWRITE   S_IWRITE
#define _S_IEXEC    S_IEXEC
:include extepi.sp

/*
 *  Group permissions
 */
#define S_IRWXG     000070      /* Read, write, execute/search      */
#define S_IRGRP     000040      /* Read permission                  */
#define S_IWGRP     000020      /* Write permission                 */
#define S_IXGRP     000010      /* Execute/search permission        */

/*
 *  Other permissions
 */
#define S_IRWXO     000007      /* Read, write, execute/search      */
#define S_IROTH     000004      /* Read permission                  */
#define S_IWOTH     000002      /* Write permission                 */
#define S_IXOTH     000001      /* Execute/search permission        */

/*
 *  Encoding of the file mode
 */
:segment QNX
#define _S_IFMT     0xF000          /* Type of file mask    */
#define _S_IFIFO    0x1000          /* FIFO (pipe)          */
#define _S_IFCHR    0x2000          /* Character special    */
#define _S_IFDIR    0x4000          /* Directory            */
#define _S_IFNAM    0x5000          /* Special named file   */
#define _S_IFBLK    0x6000          /* Block special        */
#define _S_IFREG    0x8000          /* Regular              */
#define _S_IFLNK    0xA000          /* Symbolic link        */
#define _S_IFSOCK   0xC000          /* Socket               */

:include ext.sp
#define S_IFMT      _S_IFMT
#define S_IFIFO     _S_IFIFO
#define S_IFCHR     _S_IFCHR
#define S_IFDIR     _S_IFDIR
#define S_IFNAM     _S_IFNAM
#define S_IFBLK     _S_IFBLK
#define S_IFREG     _S_IFREG
#define S_IFLNK     _S_IFLNK
#define S_IFSOCK    _S_IFSOCK
:include extepi.sp
:elsesegment
#define S_IFMT      0xF000          /* Type of file mask    */
#define S_IFIFO     0x1000          /* FIFO (pipe)          */
#define S_IFCHR     0x2000          /* Character special    */
#define S_IFDIR     0x4000          /* Directory            */
#define S_IFNAM     0x5000          /* Special named file   */
:segment LINUX
#define S_IFBLK     0x6000          /* Block special        */
:elsesegment DOS | RDOS
#define S_IFBLK     0               /* Block special        */
:endsegment
#define S_IFREG     0x8000          /* Regular              */
:segment LINUX
#define S_IFLNK     0xA000          /* Symbolic link        */
#define S_IFSOCK    0xC000          /* Socket               */
:elsesegment DOS | RDOS
#define S_IFLNK     0               /* Symbolic link        */
#define S_IFSOCK    0               /* Socket               */
:endsegment

:include ext.sp
#define _S_IFMT     S_IFMT
#define _S_IFIFO    S_IFIFO
#define _S_IFCHR    S_IFCHR
#define _S_IFDIR    S_IFDIR
#define _S_IFNAM    S_IFNAM
#define _S_IFBLK    S_IFBLK
#define _S_IFREG    S_IFREG
#define _S_IFLNK    S_IFLNK
#define _S_IFSOCK   S_IFSOCK
:include extepi.sp
:endsegment
