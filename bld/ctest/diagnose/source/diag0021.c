/* Must report an error instead of crashing. The __FILE__ identifier
 * can cause problems when macro expansion does not end.
 */
#define VERIFY( exp )   if( 1 ) { F = __FILE__; }

const char *F;

void TestDisk( unsigned short *sector )
{
    VERIFY( sector[255] == 0xAA55;  /* <-- missing closing parenthesis */
}
