/*
 * Macros which convert POSIX file handle to/from ISO stream handle pointer (FILE *)
 * convert POSIX invalid handle -1 to ISO NULL and back
 * to enable simple test for NULL for both handles
 */
#if defined( _WIN64 )
#define POSIX2FP(ph)    ((FILE *)(unsigned __int64)((ph) + 1))
#define FP2POSIX(fp)    (((int)(unsigned __int64)(fp)) - 1)
#else
#define POSIX2FP(ph)    ((FILE *)(unsigned long)((ph) + 1))
#define FP2POSIX(fp)    (((int)(unsigned long)(fp)) - 1)
#endif
