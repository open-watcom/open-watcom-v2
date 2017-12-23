s/#include \"histsplt.h\"/extern int __historical_splitparms;/
s/#include \"exitwmsg.h\"/#ifdef _M_I86/
/#ifdef _M_I86/a\
    extern _WCRTLINK _WCNORETURN void __fatal_runtime_error( char __far *, unsigned );\
#else\
    extern _WCRTLINK _WCNORETURN void __fatal_runtime_error( char *, unsigned );\
#endif
