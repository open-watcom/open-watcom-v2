/* These macro is used by other macros to invoke PostMessage().  It is shared with
 * several other header files.
 */
#ifndef PSTMSG
    #ifdef __cplusplus
        #define PSTMSG  ::PostMessage
    #else
        #define PSTMSG  PostMessage
    #endif
#endif
