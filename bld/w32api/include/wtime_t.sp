:: The _TIME_T macro provides a unified name for the time_t
:: type in both C and C++. This macro allows writing portable code that works
:: in both languages without worrying about where time_t is declared.
/* Unify time_t type for C and C++ portability */
#ifdef __cplusplus
    #define _TIME_T     std::time_t
#else
    #define _TIME_T     time_t
#endif
