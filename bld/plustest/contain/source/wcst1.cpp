#include <wcskip.h>
#include <stdio.h>
#include <stdlib.h>

const int sampleSize = 1000;
int keys[sampleSize];

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
};

void test1() {
    WCValSkipList<int> skip_list;
    register int i,k;
    int temp;

    printf( "No output is good...\n" );
    for(k=0;k<sampleSize;k++) {
        keys[k]=rand();
        skip_list.insert(keys[k]);
    };

    for(i=0;i<4;i++) {
        for(k=0;k<sampleSize;k++) {
	    if (!skip_list.find(keys[k], temp)) printf("error in search #%d,#%d\n",i,k);
	    if (temp != keys[k]) printf("search returned wrong value\n");
	};
        for(k=0;k<sampleSize;k++) {
	    if (! skip_list.remove(keys[k])) printf("error in delete\n");
            keys[k] = rand();
	    skip_list.insert(keys[k]);
	};
    };
};


void test2() {
    WCPtrSkipList<int> skip_list;
    register int i,k;
    int *temp;

    printf( "No output is good...\n" );
    for(k=0;k<sampleSize;k++) {
        keys[k]=rand();
        skip_list.insert(&keys[k]);
    };

    for(i=0;i<1;i++) {
        for(k=0;k<sampleSize;k++) {
	    if ( ( temp = skip_list.find(&keys[k] ) ) == 0 ) {
		printf("error in search #%d,#%d\n",i,k);
	    } else if (*temp != keys[k]) {
		printf("search returned wrong value\n");
	    }
	};
        for(k=0;k<sampleSize;k++) {
	    if (!skip_list.remove(&keys[k])) printf("error in delete\n");
//            keys[k] = rand();
	    skip_list.insert(&keys[k]);
	};
    };
};


void test3() {
    WCValSkipListSet<int> skip_list;
    register int i,k;
    int temp;

    printf( "No output is good...\n" );
    for(k=0;k<sampleSize;k++) {
        do{
	    keys[k]=rand();
        } while( !skip_list.insert(keys[k]) );
    };

    for(i=0;i<4;i++) {
        for(k=0;k<sampleSize;k++) {
	    if (!skip_list.find(keys[k], temp)) printf("error in search #%d,#%d\n",i,k);
	    if (temp != keys[k]) printf("search returned wrong value\n");
	};
        for(k=0;k<sampleSize;k++) {
	    if (! skip_list.remove(keys[k])) printf("error in delete\n");
	    do{
		keys[k]=rand();
	    } while( !skip_list.insert(keys[k]) ) ;
	};
    };
};


void test4() {
    WCPtrSkipListSet<int> skip_list;
    register int i,k;
    int *temp;

    printf( "No output is good...\n" );
    for(k=0;k<sampleSize;k++) {
        do{
	    keys[k]=rand();
        } while( !skip_list.insert(&keys[k]) );
    };

    for(i=0;i<4;i++) {
        for(k=0;k<sampleSize;k++) {
	    if ( ( temp = skip_list.find(&keys[k] ) ) == 0 ) {
		printf("error in search #%d,#%d\n",i,k);
	    } else if (*temp != keys[k]) {
		printf("search returned wrong value\n");
	    }
	};
        for(k=0;k<sampleSize;k++) {
	    if ( skip_list.remove(&keys[k]) != &keys[k]) printf("error in delete\n");
	    do{
		keys[k]=rand();
	    } while( !skip_list.insert(&keys[k]) );
	};
    };
};


void test5() {
    WCValSkipListDict<int,int> skip_list;
    register int i,k;
    int temp;

    printf( "No output is good...\n" );
    for(k=0;k<sampleSize;k++) {
        do{
	    keys[k]=rand();
        } while( !skip_list.insert(keys[k],keys[k]) );
    };

    for(i=0;i<4;i++) {
        for(k=0;k<sampleSize;k++) {
	    if (!skip_list.find(keys[k], temp)) printf("error in search #%d,#%d\n",i,k);
	    if (temp != keys[k]) printf("search returned wrong value\n");
	};
        for(k=0;k<sampleSize;k++) {
	    if (! skip_list.remove(keys[k])) printf("error in delete\n");
	    do{
		keys[k]=rand();
	    } while( !skip_list.insert(keys[k],keys[k]) );
	};
    };
};


void test6() {
    WCPtrSkipListDict<int,int> skip_list;
    register int i,k;
    int *temp;

    printf( "No output is good...\n" );
    for(k=0;k<sampleSize;k++) {
        do{
	    keys[k]=rand();
        } while( !skip_list.insert(&keys[k],&keys[k]) );
    };

    for(i=0;i<4;i++) {
        for(k=0;k<sampleSize;k++) {
	    if ( ( temp = skip_list.find(&keys[k] ) ) == 0 ) {
		printf("error in search #%d,#%d\n",i,k);
	    } else if (*temp != keys[k]) {
		printf("search returned wrong value\n");
	    }
	};
        for(k=0;k<sampleSize;k++) {
	    if (skip_list.remove(&keys[k]) != &keys[k]) printf("error in delete\n");
	    do{
		keys[k]=rand();
	    } while( !skip_list.insert(&keys[k],&keys[k]) );
	};
    };
};
