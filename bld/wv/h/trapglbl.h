extern void InitTrap( const char *parms );
//extern bool ReInitTrap( const char *parms );
extern void FiniTrap( void );
extern void InitSuppServices( void );
extern void FiniSuppServices( void );

extern bool InitCapabilities( void );
extern bool InitFileInfoSupp( void );
extern bool InitEnvSupp( void );
extern bool InitOvlSupp( void );
extern bool InitThreadSupp( void );
extern bool InitRunThreadSupp( void );
extern bool InitAsyncSupp( void );

extern trap_shandle GetSuppId( char *name );

extern void RemoteSuspend( void );
extern void RemoteResume( void );

extern bool SetCapabilitiesExactBreakpointSupport( bool status, bool set_switch );
extern bool IsExactBreakpointsSupported( void );
extern bool Is8ByteBreakpointsSupported( void );

