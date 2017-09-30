// check diagnostic for enum symbol conflict

enum test {
   ONE,
   TWO,
   TREE,
   FOR
};

int TREE = 0;

int TWO;

void FOR( void );

void main( void )
{
enum test {
   ONE,
   TWO,
   TREE,
   FOR
};

int TREE = 0;

int TWO;

    TWO = TREE / 2;
    FOR();

}

