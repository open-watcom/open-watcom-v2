#if defined(__cplusplus)
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void dll_entry_1( void );
EXTERNC void dll_entry_2( void );

int main( void )
{
    dll_entry_1();
    dll_entry_2();
    return( 0 );
}
