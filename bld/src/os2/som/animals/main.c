#include "animals.h"

#define BIG_DOG_MAX 4

int main( int argc, char **argv )
{
    Environment *ev = somGetGlobalEnvironment();
    string      dogNames[] = {
        "Lassie", "Guffy", "Waldo",
        "Pluto", "Elvis", "Spike"
    };
    string      dogBreeds[] = {
        "Collie", "Retriever", "German Shepherd",
        "Animated", "Hounddog", "Bulldog"
    };
    string      dogColors[] = {
        "tan", "brown", "black"
    };
    sequence(Animals_Animal)    allCreatures;
    int i;

    /*
     * Create classes.
     */
    Animals_AnimalNewClass( Animals_Animal_MajorVersion, Animals_Animal_MinorVersion );
    Animals_DogNewClass( Animals_Dog_MajorVersion, Animals_Dog_MinorVersion );
    Animals_LittleDogNewClass( Animals_LittleDog_MajorVersion, Animals_LittleDog_MinorVersion );
    Animals_BigDogNewClass( Animals_BigDog_MajorVersion, Animals_BigDog_MinorVersion );

    /*
     * Optionally turn on debug tracing.
     */
    SOM_TraceLevel = (argc > 1 ? 1 : 0);

    /*
     * Create objects using constructors
     */
    Animals_M_Animal_newAnimal( _Animals_Animal, ev, "Scary", "Roar!!!" );
    Animals_Kennel_newDog( _Animals_Dog, ev, "Rover", "Grrr", "Retriever", "yellow" );
    Animals_Kennel_newDog( _Animals_LittleDog, ev, "Pierre", "yap yap", "French Poodle", "black" );
    for( i = 0; i < (sizeof( dogNames ) / sizeof( string )); i++)
        Animals_Kennel_newDog( _Animals_BigDog, ev, dogNames[i], (string)NULL, dogBreeds[i], dogColors[i%3] );

    /*
     * Display objects.
     */
    allCreatures = Animals_M_Animal__get_instances( _Animals_Animal, ev );

    for( i = 0; i < allCreatures._length; i++ ) {
        Animals_Animal_display( allCreatures._buffer[i], ev );
    }


    somPrintf( "Total animal population is currently %d.\n", allCreatures._length );

    /*
     * Free objects.
     */
    for( i = 0; i < allCreatures._length; i++ )
        SOMObject_somFree( allCreatures._buffer[i] );

    somPrintf( "\n" );

    return( 0 );
}
