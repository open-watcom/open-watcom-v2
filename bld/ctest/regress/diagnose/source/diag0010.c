/* Test unterminated string behaviour for the C compiler.
 * -za switch OFF
 */

#if 0
char *s1 = " ;
#endif

char *s2 = " ;

int main( void )
{
    return ( 0 );
}

