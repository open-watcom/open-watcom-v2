typedef struct tree *PTREE;
extern void traverse( PTREE, PTREE (*)( PTREE ) );

void doit( PTREE x )
{
    x = x;
}

void foo( PTREE t )
{
    traverse( t, doit );        // doesn't diagnose "bad 2nd parm"!
}
