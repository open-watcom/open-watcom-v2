
extern void     *ProfAlloc( size_t size );
extern void     ProfFree( void *ptr );
extern void     *ProfRealloc( void *p, size_t new_size );
extern void     *ProfCAlloc( size_t size );
extern void     WPMemOpen( void );
extern void     WPMemClose( void );
extern void     WPMemPrtUsage( void );
#if defined( TRMEM ) && defined( _M_IX86 )
#pragma aux ProfAlloc __frame
#pragma aux ProfFree __frame
#pragma aux ProfRealloc __frame
#pragma aux ProfCAlloc __frame
#endif
