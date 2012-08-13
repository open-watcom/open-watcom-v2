/*
 * MEM2.C - memory allocation/usage routines
 *
 * By:  Craig Eisler
 *      June 4-5,11-12,17 1990
 *      July 16,31 1990
 *      August 1 1990
 *      September 11 1990
 *      October 31 1990
 *      November 4,7,19,22 1990
 *      December 28 1990
 *      January 1 1991
 *	March 30 1991
 *      
 * Routines:    
 *              ResAddLLItemAtEnd
 *              ResInsertLLItemAfter
 *              ResInsertLLItemBefore
 *              ResDeleteLLItem
 *              ResReplaceLLItem
 */

#include <stdlib.h>
#include "mem2.h"

typedef struct ss {
struct ss *next,*prev;
} ss;

/*
 * ResAddLLItemAtEnd - create a new item at the tail of a linked list
 */
void ResAddLLItemAtEnd( ss **head, ss **tail, ss *item )
{

	if( *head == NULL ) {
	    *head = *tail = item;
	    item->next = NULL;
	    item->prev = NULL;
	} else {
	    item->prev = *tail;
	    item->next = NULL;
	    (*tail)->next = item;
	    *tail = item;
	} /* if */

} /* ResAddLLItemAtEnd */

/*
 * ResInsertLLItemAfter - insert an item into a linked list after an item
 */
#pragma off (unreferenced)
void ResInsertLLItemAfter( ss **head, ss **tail, ss *who, ss *item )
#pragma on (unreferenced)
{
ss *after_who;

	after_who = who->next;
	who->next = item;
	item->prev = who;
	item->next = after_who;
	
	if( after_who == NULL ) {
	    *tail = item;
	} else {
	    after_who->prev = item;
	} /* if */

} /* ResInsertLLItemAfter */
	
/*
 * ResInsertLLItemBefore - insert an item into a linked list after an item
 */
#pragma off (unreferenced)
void ResInsertLLItemBefore( ss **head, ss **tail, ss *who, ss *item )
#pragma on (unreferenced)
{
ss *before_who;

	before_who = who->prev;
	who->prev = item;
	item->next = who;
	item->prev = before_who;

	if( before_who == NULL ) {
	    *head = item;
	} else {
	    before_who->next = item;
	} /* if */

} /* ResInsertLLItemBefore */

/*
 * ResDeleteLLItem - delete an item from a linked list
 */
void *ResDeleteLLItem( ss **head, ss **tail, ss *item )
{

#if 0
	if( item == *head && item == *tail ) {
	    *head = *tail = NULL;
	} else if( item == *head ) {
	    *head = item->next;
	    (*head)->prev = NULL;
	} else if( item == *tail ) {
	    *tail = item->prev;
	    (*tail)->next = NULL;
	} else {
	    item->prev->next = item->next;
	    item->next->prev = item->prev;
	} /* if */
#else
	if( item == *head ) *head = item->next;
	if( item == *tail ) *tail = item->prev;
	if( item->prev != NULL ) item->prev->next = item->next;
	if( item->next != NULL ) item->next->prev = item->prev;
#endif
	return( item );

} /* ResDeleteLLItem */

/*
 * ResReplaceLLItem - drop a replacement item into a linked list
 */
void ResReplaceLLItem( ss **head, ss **tail, ss *item, ss *new )
{
	if( item == *head ) *head = new;
	if( item == *tail ) *tail = new;
	
	new->prev = item->prev;
	new->next = item->next;
	
	if( item->prev != NULL ) item->prev->next = new;
	if( item->next != NULL ) item->next->prev = new;
 
} /* ResReplaceLLItem */
