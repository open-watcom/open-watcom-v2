/* Macros to specify shell functions */
#ifndef WINSHELLAPI
    #define WINSHELLAPI     DECLSPEC_IMPORT
#endif
#ifndef SHSTDAPI
    #define SHSTDAPI        EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
    #define SHSTDAPI_( x )  EXTERN_C DECLSPEC_IMPORT x STDAPICALLTYPE
#endif
