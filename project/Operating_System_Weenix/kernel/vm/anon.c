/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove this comment block from this file.    */
/******************************************************************************/

#include "globals.h"
#include "errno.h"

#include "util/string.h"
#include "util/debug.h"

#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/mm.h"
#include "mm/page.h"
#include "mm/slab.h"
#include "mm/tlb.h"

int anon_count = 0; /* for debugging/verification purposes */

static slab_allocator_t *anon_allocator;

static void anon_ref(mmobj_t *o);
static void anon_put(mmobj_t *o);
static int  anon_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf);
static int  anon_fillpage(mmobj_t *o, pframe_t *pf);
static int  anon_dirtypage(mmobj_t *o, pframe_t *pf);
static int  anon_cleanpage(mmobj_t *o, pframe_t *pf);

static mmobj_ops_t anon_mmobj_ops = {
        .ref = anon_ref,
        .put = anon_put,
        .lookuppage = anon_lookuppage,
        .fillpage  = anon_fillpage,
        .dirtypage = anon_dirtypage,
        .cleanpage = anon_cleanpage
};

/*
 * This function is called at boot time to initialize the
 * anonymous page sub system. Currently it only initializes the
 * anon_allocator object.
 */
void
anon_init()
{
	/*GS: initialize the anon_allocator object.*/
	anon_allocator = slab_allocator_create("anon_allocator", sizeof(mmobj_t));


	/*(postcondition)*/
	KASSERT(anon_allocator);
	dbg(DBG_PRINT,"(GRADING3A 4.a) (postcondition) KASSERT(anon_allocator);\n");


	/*NOT_YET_IMPLEMENTED("VM: anon_init");*/
}

/*
 * You'll want to use the anon_allocator to allocate the mmobj to
 * return, then then initialize it. Take a look in mm/mmobj.h for
 * macros which can be of use here. Make sure your initial
 * reference count is correct.
 */
mmobj_t *
anon_create()
{

	dbg(DBG_PRINT,"(GRADING3E) anon_create()\n");


	/*GS: pointer to the mmobj to return*/
	mmobj_t *obj = NULL;

	/*GS: use the anon_allocator to allocate the mmobj to return*/
	obj = (mmobj_t*)slab_obj_alloc(anon_allocator);

	if(obj != NULL)
	{

		dbg(DBG_PRINT,"(GRADING3E) anon_create() if(obj != NULL)\n");

		/*GS: then then initialize it*/
		mmobj_init(obj, &anon_mmobj_ops);

		/*GS: initial reference count, set to 1*/
		obj->mmo_refcount++;

		/*GS: this variable for debugging/verification purposes */
		anon_count++;
	}

	return obj;

	/*NOT_YET_IMPLEMENTED("VM: anon_create");*/
	/*return NULL;*/
}

/* Implementation of mmobj entry points: */

/*
 * Increment the reference count on the object.
 */
static void
anon_ref(mmobj_t *o)
{

	KASSERT(o && (0 < o->mmo_refcount) && (&anon_mmobj_ops == o->mmo_ops));
	dbg(DBG_PRINT,"(GRADING3A 4.b) (precondition) KASSERT(o && (0 < o->mmo_refcount) && (&anon_mmobj_ops == o->mmo_ops));\n");

    o->mmo_refcount++;

	/*GS: this variable for debugging/verification purposes */
	anon_count++;


    /*NOT_YET_IMPLEMENTED("VM: anon_ref");*/
}

/*
 * Decrement the reference count on the object. If, however, the
 * reference count on the object reaches the number of resident
 * pages of the object, we can conclude that the object is no
 * longer in use and, since it is an anonymous object, it will
 * never be used again. You should unpin and uncache all of the
 * object's pages and then free the object itself.
 */
static void
anon_put(mmobj_t *o)
{
	KASSERT(o && (0 < o->mmo_refcount) && (&anon_mmobj_ops == o->mmo_ops));
	dbg(DBG_PRINT,"(GRADING3A 4.c) (precondition) KASSERT(o && (0 < o->mmo_refcount) && (&anon_mmobj_ops == o->mmo_ops));\n");

	/*GS: If the reference count on the object reaches the number of resident
	 * pages of the object, we can conclude that the object is no longer in use
	 */
	if( (o->mmo_refcount-1) == (o->mmo_nrespages) )
	{
		dbg(DBG_PRINT,"(GRADING3E) anon_put() if( (o->mmo_refcount-1) == (o->mmo_nrespages) )\n");


		/*GS: unpin and uncache all of the object's pages and then
		 * free the object itself */

		pframe_t *pf_current;

		dbg(DBG_PRINT,"(GRADING3E) anon_put() list_iterate_begin()\n");
		list_iterate_begin(&(o->mmo_respages),pf_current,pframe_t,pf_olink)
		{
			while(pframe_is_pinned(pf_current))
				pframe_unpin(pf_current);

			while(pframe_is_busy(pf_current))
				sched_sleep_on(&pf_current->pf_waitq);

			pframe_free(pf_current);

		}list_iterate_end();


		slab_obj_free(anon_allocator,o);
	}


	/*GS: Decrement the reference count on the object*/
	o->mmo_refcount--;

	/*GS: this variable for debugging/verification purposes */
	anon_count--;

	return;

	/*NOT_YET_IMPLEMENTED("VM: anon_put");*/
}

/* Get the corresponding page from the mmobj. No special handling is
 * required. */
static int
anon_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf)
{

	dbg(DBG_PRINT,"(GRADING3E) anon_lookuppage()\n");

	int retVal = 0;

	retVal = pframe_get(o, pagenum, pf);

	return retVal;

	/*NOT_YET_IMPLEMENTED("VM: anon_lookuppage");
	 * return -1;*/
}

/* The following three functions should not be difficult. */

static int
anon_fillpage(mmobj_t *o, pframe_t *pf)
{
	KASSERT(pframe_is_busy(pf));
	dbg(DBG_PRINT,"(GRADING3A 4.d) (precondition) KASSERT(pframe_is_busy(pf));\n");

	KASSERT(!pframe_is_pinned(pf));
	dbg(DBG_PRINT,"(GRADING3A 4.d) (precondition) KASSERT(!pframe_is_pinned(pf));\n");

	/*GS: fill the page with 0's and pin the frame*/
	memset(pf->pf_addr,0,PAGE_SIZE);

	pframe_pin(pf);

	return 0;

	/* NOT_YET_IMPLEMENTED("VM: anon_fillpage");
        return 0;*/
}

static int
anon_dirtypage(mmobj_t *o, pframe_t *pf)
{

	dbg(DBG_PRINT,"(GRADING3E) anon_dirtypage()\n");

	/*GS: pin page if not pinned*/
    if(!pframe_is_pinned(pf))
    {
    	dbg(DBG_PRINT,"(GRADING3E) pframe_pin()\n");
        pframe_pin(pf);
    }

	/*GS: macro to set page dirty*/
	pframe_set_dirty(pf);
	return 0;

	/* NOT_YET_IMPLEMENTED("VM: anon_dirtypage");
        return -1; */
}

static int
anon_cleanpage(mmobj_t *o, pframe_t *pf)
{

	dbg(DBG_PRINT,"(GRADING3E) anon_cleanpage()\n");

	/*GS: unpin if pinned before cleaning page*/
	if(pframe_is_pinned(pf))
	{
		pframe_unpin(pf);

		dbg(DBG_PRINT,"(GRADING3E) pframe_unpin()\n");
	}

	/*GS: macro to clear page dirty*/
	pframe_clear_dirty(pf);

	return 0;

	/* NOT_YET_IMPLEMENTED("VM: anon_cleanpage");
        return -1;*/
}
