extern int     _fint86x( int, union REGS __far *, union REGS __far *, struct SREGS __far * );
extern int     _fintdos( union REGS __far *, union REGS __far * );
extern int     _fintdosx( union REGS __far *, union REGS __far *, struct SREGS __far * );
extern int     _fint86( int, union REGS __far *, union REGS __far * );
extern int     _fint86x( int, union REGS __far *, union REGS __far *, struct SREGS __far * );
extern void    _fintr( int, union REGPACK __far * );
