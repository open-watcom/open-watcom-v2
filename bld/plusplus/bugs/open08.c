static int i = 2;

void foo( void )
{
    extern int i = 3;

    i = 5;
}

static int j = 2;       /* should this be an error in C++? */
extern int j;
