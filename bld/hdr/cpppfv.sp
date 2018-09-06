#ifndef _PFV_DEFINED
#define _PFV_DEFINED
 #define _PFV_DEFINED_
 typedef void (*PFV)( void );
#endif
#ifndef _PFU_DEFINED
#define _PFU_DEFINED
 #define _PFU_DEFINED_
 typedef int (*PFU)( unsigned );
#endif
#ifndef _PNH_DEFINED
#define _PNH_DEFINED
 #define _PNH_DEFINED_
 typedef int (*_PNH)( unsigned );
#endif
:segment DOS | LINUX | RDOS
#if defined(__SW_BR) || defined(_RTDLL)
 #ifndef _PPV_DEFINED
 #define _PPV_DEFINED
  #define _PPV_DEFINED_
  typedef void (*_PVV)( void *);
 #endif
 #ifndef _PUP_DEFINED
 #define _PUP_DEFINED
  #define _PUP_DEFINED_
  typedef void* (*_PUP)( std::size_t );
 #endif
#endif
:endsegment
