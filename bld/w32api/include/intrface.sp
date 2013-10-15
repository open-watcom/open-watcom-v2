/* Macros to declare interfaces - these macros can be used for both C and C++. Define
 * CINTERFACE to have these macros expand in C++ code as if they were in C code.
 * Define CONST_VTABLE to have constant vtables in C.
 */
#if defined( __cplusplus ) && !defined( CINTERFACE )
    #define __STRUCT__                              struct
    #define interface                               __STRUCT__
    #define STDMETHOD( f )                          virtual HRESULT STDMETHODCALLTYPE f
    #define STDMETHOD_( x, f )                      virtual x STDMETHODCALLTYPE f
    #define STDMETHODV( f )                         virtual HRESULT STDMETHODVCALLTYPE f
    #define STDMETHODV_( x, f )                     virtual x STDMETHODVCALLTYPE f
    #define PURE                                    = 0
    #define THIS_
    #define THIS                                    void
    #define DECLARE_INTERFACE( x )                  interface x
    #define DECLARE_INTERFACE_( x, p )              interface x : public p
#else
    #define interface                               struct
    #define STDMETHOD( f )                          HRESULT (STDMETHODCALLTYPE *f)
    #define STDMETHOD_( x, f )                      x (STDMETHODCALLTYPE *f)
    #define STDMETHODV( f )                         HRESULT (STDMETHODVCALLTYPE *f)
    #define STDMETHODV_( x, f )                     x (STDMETHODVCALLTYPE *f)
    #define PURE
    #define THIS_                                   INTERFACE FAR *This,
    #define THIS                                    INTERFACE FAR *This
    #ifdef CONST_VTABLE
        #define DECLARE_INTERFACE( x ) \
            typedef interface x { \
                const struct x##Vtbl *lpVtbl; \
            } x; \
            typedef const struct x##Vtbl x##Vtbl; \
            const struct x##Vtbl
    #else
        #define DECLARE_INTERFACE( x ) \
            typedef interface x { \
                struct x##Vtbl *lpVtbl; \
            } x; \
            typedef struct x##Vtbl x##Vtbl; \
            struct x##Vtbl
    #endif
    #define DECLARE_INTERFACE_( x, p )              DECLARE_INTERFACE( x )
#endif
#define IFACEMETHOD( f )                            STDMETHOD( f )
#define IFACEMETHOD_( x, f )                        STDMETHOD_( x, f )
#define IFACEMETHODV( f )                           STDMETHODV( f )
#define IFACEMETHODV_( x, f )                       STDMETHODV_( x, f )
