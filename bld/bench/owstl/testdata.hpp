#include <cstdlib>
#if defined(_MSC_VER) && _MSC_VER <= 1200
namespace std{
    using ::rand;
};
#endif

class TestData{
    int* mData;
    int mSize;
public:
    TestData( int size ) : mSize(size), mData(new int [size]) {}
    ~TestData() { delete[] mData; }
    void fill_linear() { for( int i = 0; i < mSize; i++ ) mData[i] = i; }
    void fill_rand() {
        mData[0] = 0x55555555;
        for( int i = 0; i < mSize; i++ ){
            mData[i] *= std::rand();
            mData[i] += std::rand();
            for( int j = 0; j < i ; j++ ){ //check for repetition
                if( mData[j] == mData[i] ){
                    i--;
                    break;
                }
            }
        }
    }
    int operator[]( int index ) const { return mData[index]; }
};
