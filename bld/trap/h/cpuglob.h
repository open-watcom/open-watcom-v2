
#if defined( _M_IX86 ) || defined( _M_X64 )

#define TRACE_BIT       0x100
#define BRKPOINT        0xCC

extern unsigned GetDS( void );
extern unsigned GetCS( void );
extern unsigned GetSS( void );
#if defined( _M_I86 )
#pragma aux GetDS = "mov ax,ds" value[ax];
#pragma aux GetCS = "mov ax,cs" value[ax];
#pragma aux GetSS = "mov ax,ss" value[ax];
#else
#pragma aux GetDS = "mov eax,ds" value[eax];
#pragma aux GetCS = "mov eax,cs" value[eax];
#pragma aux GetSS = "mov eax,ss" value[eax];
#endif

extern void BreakPoint( void );
#if defined( _M_IX86 )
#pragma aux BreakPoint = BRKPOINT;
#endif

extern void BreakPointParm( unsigned long );
#if defined( _M_I86 )
#pragma aux BreakPointParm = BRKPOINT parm [dx ax] aborts;
#else
#pragma aux BreakPointParm = BRKPOINT parm [eax] aborts;
#endif


typedef unsigned char   opcode_type;

#elif defined( __PPC__ )

#define TRACE_BIT       (1U << MSR_L_se)
#define BRKPOINT        0x7FE00008

#define GetDS()         0
#define GetCS()         0

typedef unsigned        opcode_type;

#elif defined( __MIPS__ )

//#define TRACE_BIT      0
#define BRKPOINT        0x0000000D

#define GetDS()         0
#define GetCS()         0

typedef unsigned        opcode_type;

#elif defined( __AXP__ )

//#define TRACE_BIT      0
#define BRKPOINT        0x00000080

#define GetDS()         0
#define GetCS()         0

typedef unsigned        opcode_type;

#else

    #error globcpu.h not configured for machine

#endif
