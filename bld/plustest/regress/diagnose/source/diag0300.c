#include <assert.h>

static bool const condition = true
                , complain  = ( assert( condition ), false )
		                ;
#error one error
