// check diagnostic for macro definition conflict

#define MAC1 1

#define MAC2(a) (a)

#define MAC1 3

#define MAC2(a) (a/2)

void main( void )
{
}
