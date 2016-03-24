#if defined( __SW_BR ) || defined( __MAKE_DLL_MATHLIB ) || defined( __MAKE_DLL_WRTLIB )
_WMRTLINK extern void (*__get__cnvd2ld( void ))( dbl_arg src, ld_arg dst );
_WMRTLINK extern void (*__get__cnvs2d( void ))( char *, double * );
_WMRTLINK extern FAR_STRING (*__get_EFG_Format( void ))( char *, my_va_list *, SPECS __SLIB * );
#endif
_WMRTLINK extern FAR_STRING _EFG_Format( char *buffer, my_va_list *args, SPECS __SLIB *specs );
_WMRTLINK void __cnvs2d( char *buf, double *value );
