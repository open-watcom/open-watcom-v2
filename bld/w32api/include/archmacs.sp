/* Define the appropriate macro for the current processor. */
#if defined(_M_I386) && !defined(_X86_)
    #define _X86_
#elif defined(_M_ALPHA) && !defined(_ALPHA_)
    #define _ALPHA_
#elif defined(_M_PPC) && !defined(_PPC_)
    #define _PPC_
#elif defined(_M_MRX000) && !defined(_MIPS_)
    #define _MIPS_
#endif
