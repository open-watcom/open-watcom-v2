/* Test unterminated string behaviour for the C compiler.
 * -za switch ON
 */

#if 0
char *s1 = " ;
#endif

int main( void )
{
    return ( 0 );
}

