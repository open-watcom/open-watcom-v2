void BubbleSort( int list[], int n )
/**********************************/
{
    int index1 = 0;

    if( n < 2 )return;
    do {
        int index2;

        index2 = index1 + 1;
        do {
            int temp;

            if( list[ index1 ] > list[ index2 ] ) {
                temp = list[ index1 ];
                list[ index1 ] = list[ index2 ];
                list[ index2 ] = temp;
            }
        } while( ++index2 < n );
    } while( ++index1 < n-1 );
}
