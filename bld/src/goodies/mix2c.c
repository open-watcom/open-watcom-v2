/* MIX2C.C - This C function accepts 3 integer arguments
 *           and returns their maximum.
 *
 * Compile: wcc /ml mix2c
 *          wcc386  mix2c
 */

long int tmax3( long int arga,
                long int argb,
                long int argc )
{
    long int   result;
    result = arga;
    if( argb > result ) result = argb;
    if( argc > result ) result = argc;
    return( result );
}
