#pragma aux __int3 = 0xcc;
void barf( void )
{
    __int3();
}
