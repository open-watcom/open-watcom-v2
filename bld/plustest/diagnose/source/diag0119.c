// DIAG119 -- test #define, #undef of predefined macros

#undef __LINE__
#undef __FILE__
#undef __DATE__
#undef __TIME__
#undef __STDC__
#undef __cplusplus
#undef defined

#define __LINE__        1
#define __FILE__        2
#define __DATE__        3
#define __TIME__        4
#define __STDC__        1
#define __cplusplus     6
#define defined         7
