/* E1208: Invalid binary constants */

static int b_bad =   0b01234;
static int b_empty = 0b;


int get_bad(void)
{
    return b_bad + b_empty ;
}


int main(void)
{

    if( get_bad() ) {
       return 1;
    }

    return 0;
}
