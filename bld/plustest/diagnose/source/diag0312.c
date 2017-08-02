#pragma warning 931 9
#pragma warning 932 9
#pragma warning 933 9

struct A {
};

const A f( )
{
    return A( );
}

const int g( )
{
    return 0;
}

int main( )
{
    int *pi = 0;
    const short s1 = static_cast< const short >( *pi );
    const short s2 = *( ( short * ) pi );

    return 0;
}
