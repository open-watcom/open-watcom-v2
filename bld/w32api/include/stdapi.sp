/* Standard method and API calling conventions */
#define STDMETHODCALLTYPE   __stdcall
#define STDMETHODVCALLTYPE  __cdecl
#define STDAPICALLTYPE      __stdcall
#define STDAPIVCALLTYPE     __cdecl

/* Macro used to specify extern "C" */
#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif

/* Standard method implementation and API declaration macros */
#define STDAPI                      EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_( x )                EXTERN_C x STDAPICALLTYPE
#define STDAPIV                     EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_( x )               EXTERN_C x STDAPIVCALLTYPE
#define STDMETHODIMP                HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_( x )          x STDMETHODCALLTYPE
#define STDMETHODIMPV               HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_( x )         x STDMETHODVCALLTYPE
