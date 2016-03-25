#if defined( __NT__ )
#include <windows.h>

extern int  APIENTRY LibMain( HANDLE hdll, ULONG reason, LPVOID reserved );
#endif

extern int          __fdll_initialize( void );
extern int          __fdll_terminate( void );

#if defined( _M_IX86 )
  #pragma aux __fdll_initialize "^_"
  #pragma aux __fdll_terminate "^_"
#endif

extern int          __fthrd_initialize( void );
extern int          __fthrd_terminate( void );

#if defined( _M_IX86 )
  #pragma aux __fthrd_initialize "^_"
  #pragma aux __fthrd_terminate "^_"
#endif

extern int          __dll_initialize( void );

extern int          __FInitDLL( void );
extern void         __FTermDLL( void );

extern unsigned     __FInitThrd( void );
extern void         __FTermThrd( void );
