.func getpwnam
.synop begin
#include <pwd.h>
struct passwd *getpwnam( const char *name );
.synop end
.desc begin
The
.id &funcb.
function locates and returns a parsed password database entry
for the user with the specified username.  If a matching entry is
not found or an error occurs, a null pointer will be returned.
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
A matching parsed password database entry, or NULL if no match is found.
.return end
.see begin
.seelist setpwent endpwent getpwuid getpwent
.see end
.exmp begin
.blktext begin
The following program will locate and print out the user id corresponding
to a user name 'root.'
.blktext end
.blkcode begin
#include <stdio.h>
#include <pwd.h>

void main()
  {
    struct passwd *pw;
    
    pw = getpwnam( "root" );
    if(pw)
        printf("The user id for root is %d\n", (int)pw->pw_uid);
    else
        printf("User 'root' was not found.\n");
  }
.blkcode end
.exmp end
.class POSIX
.system
