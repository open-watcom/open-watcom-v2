/* ==================================================================
 * BlockAlloc
 * an faster allocator for the performance tests so the algorithm can 
 * be tested and the results not obsured by poor allocation time
 * TO DO:
 *      * it doesn't free blocks back to new/malloc when destroyed
 */
#include <memory>
#include <stdlib.h>
#include <new>
template< class Type >
class BlockAlloc : public std::allocator< Type > {
public:
    template< class Type2 >
    struct rebind { typedef BlockAlloc< Type2 > other; };

    BlockAlloc( ) : mChunkSize(0), mFreeList(0) { }
    BlockAlloc( const BlockAlloc & ) : mChunkSize(0), mFreeList(0) { }
    template< class Type2 >
    BlockAlloc( const BlockAlloc< Type2 > & ) : mChunkSize(0), mFreeList(0) { }
    ~BlockAlloc( ) { }

    pointer allocate( size_type n, std::allocator< void >::const_pointer = 0 )
    {
        if( n != 1 ) throw std::bad_alloc();    //can't handle more than one
        pointer result;
        if( mFreeList != 0 ){
            result = reinterpret_cast< pointer >( mFreeList );
            mFreeList = mFreeList->next;
        }else if( mChunkSize != 0 ){
            --mChunkSize;
            result = mFreeChunk;
            ++mFreeChunk;
        }else{
            //perhaps it would be better start allocating only 1 and 
            //double up each time upto some limit so as not to reduce waste?
            const int BLOCKSPERCHUNK = 100;
            //std::cout<<"allocing "<<n<<" of size "<<sizeof(Type)<<"\n" ;
            mFreeChunk = reinterpret_cast< pointer >(
                operator new ( sizeof( List ) * BLOCKSPERCHUNK ) );
            //mFreePtr = reinterpret_cast< Type* >( 
            //    malloc( n * sizeof( Type ) * 100 ) );
            if( mFreeChunk == 0 ) throw std::bad_alloc();
            mChunkSize = BLOCKSPERCHUNK-1;
            result = mFreeChunk;
            ++mFreeChunk;
        }
        //std::cout<<(int)result<<", "<<(int)mFreePtr<<"\n";
        return( result );
    }

    void deallocate( pointer p, size_type n)
    {
        n = 1;
        List* old_head = mFreeList;
        mFreeList = reinterpret_cast< List* >( p );
        mFreeList->next = old_head;
    }

    void construct( pointer p, const Type &value )
        { new ( ( void * )p ) Type( value ); }

    void destroy( pointer p )
        { ( ( Type * )p )->~Type( ); }
    
private:
    pointer mFreeChunk;
    size_t mChunkSize;
    //the free list is held in the memory chunks themselves
    //a list is allocated rather than a type just in case
    //type is smaller than a pointer
    union List{
        List* next;
        char place_holder[sizeof( Type )];
    };
    List* mFreeList;
};
