/* None of the following undef/redef attempts are allowed */
#define __DATE__            stuff
#define __FILE__            stuff
#define __LINE__            stuff
#define __STDC__            stuff
#define __STDC_HOSTED__     stuff
#define __STDC_LIB_EXT1__   stuff
#define __STDC_VERSION__    stuff
#define __TIME__            stuff

#undef __DATE__
#undef __FILE__
#undef __LINE__
#undef __STDC__
#undef __STDC_HOSTED__
#undef __STDC_LIB_EXT1__
#undef __STDC_VERSION__
#undef __TIME__

/* Make sure predefined macros have good values */
#if __LINE__ < 10 || __LINE__ > 128
    #error __LINE__ incorrect
#endif

#if __STDC__ != 1
    #error __STDC__ incorrect
#endif

#if __STDC_HOSTED__ != 1
    #error __STDC_HOSTED__ incorrect
#endif

#if __STDC_LIB_EXT1__ < 200500L || __STDC_LIB_EXT1__ > 210000L
    #error __STDC_LIB_EXT1__ incorrect
#endif

#if __STDC_VERSION__ < 199400L || __STDC_VERSION__ > 210000L
    #error __STDC_VERSION__ incorrect
#endif

/* The __cplusplus macro must not be predefined, ie. the following must succeed */
#define __cplusplus     nonsense

int dummy;  /* Satisfy ISO C requirement on at least one external definition */
