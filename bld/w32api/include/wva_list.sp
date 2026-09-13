:: The _VA_LIST macro provides a unified name for the variadic argument list
:: type in both C and C++. This macro allows writing portable code that works
:: in both languages without worrying about where va_list is declared.
/* Unify va_list type for C and C++ portability */
#ifdef __cplusplus
    #define _VA_LIST    std::va_list
#else
    #define _VA_LIST    va_list
#endif
