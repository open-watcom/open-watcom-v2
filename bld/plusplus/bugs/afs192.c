// should diagnose this
void foo( int *p )
{
    p->int::~int( 1, 2, 3 );
}
