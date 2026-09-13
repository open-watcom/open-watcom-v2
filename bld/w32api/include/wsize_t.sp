:: The _SIZE_T macro provides a unified name for the size_t
:: type in both C and C++. This macro allows writing portable code that works
:: in both languages without worrying about where size_t is declared.
/* Unify size_t type for C and C++ portability */
#ifdef __cplusplus
    #define _SIZE_T     std::size_t
#else
    #define _SIZE_T     size_t
#endif
