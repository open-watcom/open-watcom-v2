/* These macro is used by other macros to invoke SendMessage().  It is shared with
 * several other header files.
 */
#ifndef SNDMSG
    #ifdef __cplusplus
        #define SNDMSG  ::SendMessage
    #else
        #define SNDMSG  SendMessage
    #endif
#endif
