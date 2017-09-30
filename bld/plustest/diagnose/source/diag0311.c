typedef int Array[2];

void f1( int (*ai)[2] )
{ }

void f2( const int (*ai)[2] )
{ }

void f3( Array *ai )
{ }

void f4( const Array *ai )
{ }


void g( int (*ai)[2] )
{ }

void g( const int (*ai)[2] )
{ }

void g( Array *ai )
{ }

void g( const Array *ai )
{ }


int main() {
    int (*ai1)[2] = 0;
    const int (*ai2)[2] = 0;
    Array *ai3 = 0;
    const Array *ai4 = 0;


    ai1 = ai2;
    f1( ai2 );
    ai1 = ai4;
    f1( ai4 );

    ai3 = ai2;
    f3( ai2 );
    ai3 = ai4;
    f3( ai4 );
}
