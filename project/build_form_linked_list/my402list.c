#include <stdio.h>
#include <string.h>
#include "my402list.h"
#include "cs402.h"
#include <stdlib.h>
/*
int My402ListInit(My402List*);
int  My402ListLength(My402List*)
//int  My402ListLength(My402List*);
int  My402ListAppend(My402List*, void*);
int  My402ListPrepend(My402List*, void*);
int  My402ListInsertBefore(My402List*, void*, My402ListElem*);
int  My402ListInsertAfter(My402List*, void*, My402ListElem*);

void My402ListUnlink(My402List*, My402ListElem*);
int  My402ListLength(My402List*);
int  My402ListEmpty(My402List*);

My402ListElem *My402ListFirst(My402List*);
My402ListElem *My402ListLast(My402List*);
My402ListElem *My402ListNext(My402List*, My402ListElem*);
My402ListElem *My402ListPrev(My402List*, My402ListElem*);

void My402ListUnlinkAll(My402List*);
My402ListElem *My402ListFind(My402List*, void*);
*/

//////////////// list initiation///////////////////////
int My402ListInit(My402List* anchorptr){
	(anchorptr -> anchor).next = &(anchorptr -> anchor);
	(anchorptr -> anchor).prev = &(anchorptr -> anchor);
	anchorptr -> num_members = 0;
	return TRUE;
}
/////////////// add a node after the last element////////////////
int  My402ListAppend(My402List* anchorptr, void*obj){
	My402ListElem *last;
	last = (My402ListElem*) malloc(sizeof(My402ListElem));
	last -> obj = obj;
	last -> prev = (anchorptr -> anchor).prev;
	last -> next = &(anchorptr -> anchor);
	anchorptr -> anchor.prev -> next = last;
	anchorptr -> anchor.prev = last;
	++(anchorptr -> num_members); 
//	printf("Append is done\n");
	return TRUE;
}
////////////////add a node before the first element /////////////
int  My402ListPrepend(My402List* anchorptr, void*obj){
	My402ListElem * first;
	first = (My402ListElem*) malloc(sizeof(My402ListElem));
	first -> obj = obj;
	first -> prev = &(anchorptr -> anchor);
	first -> next = (anchorptr-> anchor).next;
	(anchorptr -> anchor).next -> prev = first;
	(anchorptr -> anchor).next = first;
	++(anchorptr -> num_members);
	return TRUE;
}

//////////////////insert a node after /////////////////////
int  My402ListInsertAfter(My402List*anchorptr, void*obj, My402ListElem*to){
	My402ListElem * from;
	from  = (My402ListElem*)malloc(sizeof(My402ListElem));	
	from -> obj =obj;
	from ->next = to -> next;
	from ->prev = to;
	to -> next ->prev = from;
	to -> next =from;
	++(anchorptr -> num_members);
	return TRUE;
}

//////////////// insert a node before/////////////////////
int  My402ListInsertBefore(My402List*anchorptr, void*obj, My402ListElem*to){
	My402ListElem * from;
	from = (My402ListElem*)malloc(sizeof(My402ListElem));
	from -> obj =obj;
	from -> next = to;
	from -> prev = to -> prev;
	to -> prev -> next = from;
	to -> prev = from;
	++(anchorptr -> num_members);
	return TRUE;
}


/////////////////return the lenghth /////////
int  My402ListLength(My402List* anchorptr){
	return anchorptr -> num_members;
}
////////////////////judge if the list is empty/////////////////////////////
int  My402ListEmpty(My402List* anchorptr){
	if(anchorptr -> num_members == 0){
		return TRUE;
	}
	else return FALSE;	
}
////////////////////return the first node////////////////////////////////
My402ListElem *My402ListFirst(My402List*anchorptr){
	int num = My402ListEmpty(anchorptr);
	if ( num == 1){
		return NULL;
	}
	else 
	return (anchorptr -> anchor).next; 
}

////////////////////return the last node////////////////////////////////////
My402ListElem *My402ListLast(My402List*anchorptr){
	int num = My402ListEmpty(anchorptr);
	if ( num == 1){
		return NULL;
	}
	else 	
	return (anchorptr -> anchor).prev;
}

////////////////////return the next node////////////////////
My402ListElem *My402ListNext(My402List*anchorptr, My402ListElem*next){
	if ( next -> next == &(anchorptr -> anchor) )
		return ((My402ListElem *)NULL);
	else 
		return (next -> next);
}

///////////////////return the previous node//////////////////////////////
My402ListElem *My402ListPrev(My402List*anchorptr, My402ListElem*prev){
	if ( prev -> prev == &(anchorptr -> anchor) )
		return ((My402ListElem *)NULL);
	else 
		return (prev -> prev);
}

/////////////////  delete an node of the link /////////////
void My402ListUnlink(My402List*anchorptr, My402ListElem*middle){
	middle -> next ->prev = middle -> prev;// -> next -> prev ->prev;
	middle -> prev -> next = middle -> next;
	//middle -> next ->prev ->next = middle -> next;
	free(middle);
	--(anchorptr -> num_members);
}

////////////////////delete all the link////////////////
void My402ListUnlinkAll(My402List*anchorptr){
//	My402List *p = anchorptr;

	while(( (anchorptr->anchor).next != &(anchorptr->anchor) )||((anchorptr ->anchor).prev != &(anchorptr->anchor) )){
		My402ListElem *p = (anchorptr ->anchor).next;
		(anchorptr -> anchor).next -> next -> prev =&(anchorptr->anchor);
		(anchorptr ->anchor).next = (anchorptr ->anchor).next -> next;
		free(p);
	}
//	anchorptr -> anchor.next = (My402ListElem*)NULL;
//	anchorptr -> anchor.prev = (My402ListElem*)NULL;
//	free(&(anchorptr -> anchor));
	anchorptr -> num_members = 0;
}

///////////////////// find an element //////////////////////////////
My402ListElem *My402ListFind(My402List*anchorptr, void*obj){
	My402ListElem*p = &(anchorptr -> anchor);
	
	while( (anchorptr -> anchor).next != &(anchorptr-> anchor) ){
//		My402List*p = anchorptr;
		if (p -> obj == obj)
			return p;
		else
		p = p -> next;
	}
	return (My402ListElem *)NULL;
}




