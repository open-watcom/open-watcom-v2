/*
 *  Common filetype macros
 */
#define S_ISUID     004000      /* set user id on execution         */
#define S_ISGID     002000      /* set group id on execution        */
#define S_ISVTX     001000      /* sticky bit (does nothing yet)    */

#define S_ENFMT     S_ISGID     /* enforcement mode locking         */

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
#define S_IFMT      0xF000          /* Type of file mask    */
#define S_IFIFO     0x1000          /* FIFO (pipe)          */
#define S_IFCHR     0x2000          /* Character special    */
#define S_IFDIR     0x4000          /* Directory            */
#define S_IFNAM     0x5000          /* Special named file   */
:segment LINUX
#define S_IFBLK     0x6000          /* Block special        */
:elsesegment DOS
#define S_IFBLK     0               /* Block special        */
:endsegment
#define S_IFREG     0x8000          /* Regular              */
:segment LINUX
#define S_IFLNK     0xA000          /* Symbolic link        */
#define S_IFSOCK    0xC000          /* Socket               */
:elsesegment DOS
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
