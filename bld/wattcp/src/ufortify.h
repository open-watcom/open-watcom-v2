/*
 * FILE:
 *   ufortify.h
 *
 * DESCRIPTION:
 *   User options for fortify. Changes to this file require fortify.c to be
 * recompiled, but nothing else.
 */

#define FORTIFY_STORAGE              /* storage for public functions   */

#define FORTIFY_ALIGNMENT        sizeof(double) /* Byte alignment of all memory blocks */

#define FORTIFY_BEFORE_SIZE      32  /* Bytes to allocate before block */
#define FORTIFY_BEFORE_VALUE   0xA3  /* Fill value before block        */
                      
#define FORTIFY_AFTER_SIZE       32  /* Bytes to allocate after block  */
#define FORTIFY_AFTER_VALUE    0xA5  /* Fill value after block         */

#define FORTIFY_FILL_ON_ALLOCATE               /* Nuke out malloc'd memory       */
#define FORTIFY_FILL_ON_ALLOCATE_VALUE   0xA7  /* Value to initialize with       */

#define FORTIFY_FILL_ON_DEALLOCATE             /* free'd memory is cleared       */
#define FORTIFY_FILL_ON_DEALLOCATE_VALUE 0xA9  /* Value to de-initialize with    */

#define FORTIFY_FILL_ON_CORRUPTION             /* Nuke out corrupted memory    */

#define FORTIFY_CHECK_ALL_MEMORY_ON_ALLOCATE     /* !! added */
#define FORTIFY_CHECK_ALL_MEMORY_ON_DEALLOCATE   /* !! added */
#define FORTIFY_PARANOID_DEALLOCATE

#define FORTIFY_WARN_ON_ZERO_MALLOC  /* !! added */ /* A debug is issued on a malloc(0) */
#define FORTIFY_FAIL_ON_ZERO_MALLOC  /* !! added */ /* A malloc(0) will fail            */

#define FORTIFY_WARN_ON_ALLOCATE_FAIL    /* A debug is issued on a failed alloc  */
#define FORTIFY_WARN_ON_FALSE_FAIL       /* See Fortify_SetAllocateFailRate      */
#define FORTIFY_WARN_ON_SIZE_T_OVERFLOW  /* Watch for breaking the 64K limit in  */
                                         /* some braindead architectures...      */

#define FORTIFY_TRACK_DEALLOCATED_MEMORY

#define FORTIFY_DEALLOCATED_MEMORY_LIMIT 1048576 /* Maximum amount of deallocated bytes to keep */
#define FORTIFY_VERBOSE_WARN_WHEN_DISCARDING_DEALLOCATED_MEMORY  /* !! added */

#define FORTIFY_STRDUP                   /* if you use non-ANSI strdup() */

#ifdef __HIGHC__
  #define FORTIFY_LOCK()   // Fortify_LockLocalData(1) /* to-do!! */
  #define FORTIFY_UNLOCK() // Fortify_LockLocalData(0)
#else
  #define FORTIFY_LOCK()
  #define FORTIFY_UNLOCK()
#endif

#define FORTIFY_DELETE_STACK_SIZE    256

#ifdef __cplusplus                /* C++ only options go here */

/*  #define FORTIFY_PROVIDE_ARRAY_NEW     */
/*  #define FORTIFY_PROVIDE_ARRAY_DELETE  */

/*  #define FORTIFY_AUTOMATIC_LOG_FILE */
    #define FORTIFY_LOG_FILENAME            "fortify.log"
    #include <iostream.h>
    #define FORTIFY_FIRST_ERROR_FUNCTION    cout << "\a\a\aFortify Hit Generated!\n"

#endif /* __cplusplus */
