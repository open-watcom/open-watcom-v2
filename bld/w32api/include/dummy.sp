/* The following macros make it possible to disable nameless unions in certain structures
 * by defining NONAMELESSUNION.  They are shared with several other header files.
 */
#ifndef DUMMYUNIONNAME
    #ifdef NONAMELESSUNION
        #define DUMMYUNIONNAME  u
        #define DUMMYUNIONNAME2 u2
        #define DUMMYUNIONNAME3 u3
        #define DUMMYUNIONNAME4 u4
        #define DUMMYUNIONNAME5 u5
    #else
        #define DUMMYUNIONNAME
        #define DUMMYUNIONNAME2
        #define DUMMYUNIONNAME3
        #define DUMMYUNIONNAME4
        #define DUMMYUNIONNAME5
    #endif
#endif
