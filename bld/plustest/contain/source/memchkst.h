MEMCHK_STATUS( MC_OK,			"no error" )
MEMCHK_STATUS( MC_CHECKSUM_ERROR,	"memory tracker block corrupted" )
MEMCHK_STATUS( MC_LO_BOUND_ERROR,	"lower boundary of block corrupted" )
MEMCHK_STATUS( MC_HI_BOUND_ERROR,	"upper boundary of block corrupted" )
MEMCHK_STATUS( MC_FREE_UNALLOC_ERROR,	"free of unallocated memory (freed twice?)" )
MEMCHK_STATUS( MC_SIMPLE_ARRAY_ERROR,	"new() memory freed by delete[]()" )
MEMCHK_STATUS( MC_ARRAY_SIMPLE_ERROR,	"new[]() memory freed by delete()" )
MEMCHK_STATUS( MC_SIMPLE_NOT_FREED_ERROR,"new() memory not deleted" )
MEMCHK_STATUS( MC_ARRAY_NOT_FREED_ERROR, "new[]() memory not deleted" )

#undef MEMCHK_STATUS
