#if __WATCOMC__ > 1060
#if defined(__I86__)
#define X_extra __cdecl
#elif defined(__386__)
#define X_extra __stdcall
#else
#define X_extra
#endif
#else
#define X_extra
#endif
struct X_extra X {
    X();
    X( int x ) : x(x) {
    }
    ~X();
    X( X const &s );
    int x;
};

extern int c( void );
extern void throw_X( void );
