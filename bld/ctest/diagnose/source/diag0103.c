/* valid in c23 mode, available in older standards as extension */
/* tested here with the OW default standard and extensions disabled */

static int b_zero = 0b0;
static int b_one  = 0b1;
static int b_two  = 0b10;
static int b_42   = 0b101010;


/* Keep the optimizer away from our test code */
volatile int extra = 0;


int get_0(void)
{
    return b_zero + extra;
}

int get_1(void)
{
    return b_one + extra;
}

int get_2(void)
{
    return b_two * b_one + extra;
}

int get_42(void)
{
    return b_42 + extra;
}


int main(int argc, char * argv[])
{
    /* Keep the optimizer away from our test code */
    if(( argc == 42 ) && ( argv[0] == argv[1] )) {
        extra++;
    }

    if(( get_0() != 0 ) || ( get_1() != 1 ) || ( get_2() != 2 ) ||
       ( get_42() != 42 )) {

       return 1;
    }

    return 0;

}
