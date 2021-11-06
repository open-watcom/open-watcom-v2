/* Symbolic constants for stream I/O */

#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2
:segment DOS
:include ext.sp
#if !defined(__NT__)
#define STDAUX_FILENO   3
#define STDPRN_FILENO   4
#endif
:include extepi.sp
:endsegment
