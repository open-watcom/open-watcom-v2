int uu;

static char *f();
char *f() {		// internal
    uu=__LINE__;
    return 0;
}
char *g();
static char *g() {	// error
    uu=__LINE__;
    return 0;
}
void h();
inline void h() {	// external
    uu=__LINE__;
}
inline void l();
void l() {		// internal
    uu=__LINE__;
}
inline void m();
extern void m() {	// internal
    uu=__LINE__;
}
static void n();
inline void n() {	// internal
    uu=__LINE__;
}
static int a;
int a;			// two defn's
static int b;
extern int b;		// static
int c;
static int c;		// error
extern int d;
static int d;		// error
