/* @(#)__field_alloc.h	1.3   Release Date: 2/19/93
//	 Author:  Kent G. Budge, 
//		  Computational Physics Research and Development (1431)
//		  Sandia National Laboratories 
*/

#ifndef __field_allocH
#define __field_allocH

#include <stdlib.h>

void* __field_alloc_Allocate(size_t);
void __field_alloc_Deallocate(void*);
void __field_alloc_collect_garbage(void);

#endif // ifndef __field_allocH
