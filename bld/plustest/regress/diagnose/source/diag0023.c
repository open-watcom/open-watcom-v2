char *pc;
int *pi;
unsigned *pu;
signed *ps;
long *pl;
float *pf;
double *pd;

typedef int I;
typedef short S;

void foo( void )
{
    pi->int::~char();
    pc->char::~int();
    pu->unsigned::~void();
    ps->__segment::~signed();
    pl->long::~char();
    pf->float::~int();
    pd->double::~long();

    pi->I::~S();
}
