.func getpwent
.synop begin
#include <pwd.h>
struct passwd *getpwent( );
.synop end
.desc begin
The
.id &funcb.
function reads, parses, and returns entries from the password 
database at /etc/passwd.  The first call will return the first
entry, and subsequent calls return subsequent entries.  A null
pointer is returned if either an error is encountered or no
further entries exist.
.np
The structure returned is defined as:
.blkcode begin
struct passwd {
    char        *pw_name;       /* user's login name */
    char        *pw_passwd;     /* user's password   */
    uid_t       pw_uid;         /* user ID number    */
    gid_t       pw_gid;         /* group ID number   */
    char        *pw_gecos;      /* real name         */
    char        *pw_dir;        /* home directory    */
    char        *pw_shell;      /* initial program   */
};
.blkcode end
.np
On most modern Linux implementations, the resulting password
will simply be 'x' as passwords are not normally stored in the
password database itself.
.np
The pointer returned by 
.id &funcb.
points to a static location, and the user should free neither 
the pointer itself nor any of its consituent structure members.
.np
This function is not thread-safe.  Other calls to functions
accessing the password database may affect the return value from
this function.
.desc end
.return begin
A parsed password database entry, or NULL if no further entries
exist or an error occurred.
.return end
.see begin
.seelist setpwent endpwent getpwnam getpwuid
.see end
.exmp begin
.blktext begin
The following program will print out each user and their user id in
the system's password database
.blktext end
.blkcode begin
#include <stdio.h>
#include <pwd.h>

void main()
  {
    struct passwd *pw;
    
    setpwent( );
    
    while((pw = getpwent( )) != NULL) {
        printf("User id %d is %s\n", (int)pw->pw_uid, pw->pw_name);
    }
    
    endpwent( );

  }
.blkcode end
.exmp end
.class POSIX
.system
