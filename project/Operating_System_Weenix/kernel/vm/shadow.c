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

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/shadowd.h"

#define SHADOW_SINGLETON_THRESHOLD 5

int shadow_count = 0; /* for debugging/verification purposes */
#ifdef __SHADOWD__
/*
 * number of shadow objects with a single parent, that is another shadow
 * object in the shadow objects tree(singletons)
 */
static int shadow_singleton_count = 0;
#endif

static slab_allocator_t *shadow_allocator;

static void shadow_ref(mmobj_t *o);
static void shadow_put(mmobj_t *o);
static int  shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf);
static int  shadow_fillpage(mmobj_t *o, pframe_t *pf);
static int  shadow_dirtypage(mmobj_t *o, pframe_t *pf);
static int  shadow_cleanpage(mmobj_t *o, pframe_t *pf);

static mmobj_ops_t shadow_mmobj_ops = {
        .ref = shadow_ref,
        .put = shadow_put,
        .lookuppage = shadow_lookuppage,
        .fillpage  = shadow_fillpage,
        .dirtypage = shadow_dirtypage,
        .cleanpage = shadow_cleanpage
};

/*
 * This function is called at boot time to initialize the
 * shadow page sub system. Currently it only initializes the
 * shadow_allocator object.
 */
void
shadow_init()
{
  dbg(DBG_PRINT,"entered shadow init\n");
shadow_allocator = slab_allocator_create( "shadow_allocator", sizeof(mmobj_t) );
	KASSERT(shadow_allocator);
	dbg(DBG_PRINT,"(GRADING3A 6.a) (postcondition) KASSERT(shadow_allocator)\n ");
	/*     NOT_YET_IMPLEMENTED("VM: shadow_init");*/
}

/*
 * You'll want to use the shadow_allocator to allocate the mmobj to
 * return, then then initialize it. Take a look in mm/mmobj.h for
 * macros which can be of use here. Make sure your initial
 * reference count is correct.
 */
mmobj_t *
shadow_create()
{
  
  dbg(DBG_PRINT,"shadow_create is called in shadow.c\n");
	mmobj_t * shadow;
	shadow = (mmobj_t *) slab_obj_alloc(shadow_allocator);
	if( shadow == NULL ){
		dbg(DBG_VM,"WARNING: bad allocation for mmobj_t in shadow_create\n");
		return NULL;
	}

	mmobj_init( shadow, &shadow_mmobj_ops );
	shadow_count = 1;
    shadow -> mmo_refcount = 1;
    dbg(DBG_PRINT,"shadow_create is used successfully \n");
    return shadow;
    /*NOT_YET_IMPLEMENTED("VM: shadow_create");
      return NULL;*/
}

/* Implementation of mmobj entry points: */

/*
 * Increment the reference count on the object.
 */
static void
shadow_ref(mmobj_t *o)
{
   dbg(DBG_PRINT,"entered shadow ref\n");
     KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops));
    dbg(DBG_PRINT,"(GRADING3A 6.b) (precondition) KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops))\n");
   o->mmo_refcount++;
   
  /*NOT_YET_IMPLEMENTED("VM: shadow_ref");*/
}

/*
 * Decrement the reference count on the object. If, however, the
 * reference count on the object reaches the number of resident
 * pages of the object, we can conclude that the object is no
 * longer in use and, since it is a shadow object, it will never
 * be used again. You should unpin and uncache all of the object's
 * pages and then free the object itself.
 */


static void
shadow_put(mmobj_t *o)
{
  dbg(DBG_PRINT,"entered shadow put\n");
  KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops));
  dbg(DBG_PRINT,"(GRADING3A 6.c) (1 pts) (precondition) KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops))\n");
  pframe_t *temp;

  if(o->mmo_refcount-1==o->mmo_nrespages)
    {
      list_iterate_begin(&(o->mmo_respages),temp, pframe_t,pf_olink)
      {
	while(pframe_is_pinned(temp))
	  {
	    pframe_unpin(temp);
	  }
	while(pframe_is_busy(temp))
	  sched_sleep_on(&temp->pf_waitq);
	pframe_free(temp);
      }
      list_iterate_end();

  slab_obj_free(shadow_allocator,o);

    }

  o->mmo_refcount --;
  shadow_count--;
  return;

}


/* This function looks up the given page in this shadow object. The
 * forwrite argument is true if the page is being looked up for
 * writing, false if it is being looked up for reading. This function
 * must handle all do-not-copy-on-not-write magic (i.e. when forwrite
 * is false find the first shadow object in the chain which has the
 * given page resident). copy-on-write magic (necessary when forwrite
 * is true) is handled in shadow_fillpage, not here. It is important to
 * use iteration rather than recursion here as a recursive implementation
 * can overflow the kernel stack when looking down a long shadow chain */
static int
shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf)
{
   dbg(DBG_PRINT,"entered shadow lookup page\n");
  mmobj_t *shad;
  pframe_t *p;
  int err;
  shad=o;
     while(shad->mmo_shadowed)
      {
      list_iterate_begin(&(shad->mmo_respages),p, pframe_t, pf_olink)
	{
	  if(pagenum==p->pf_pagenum && !forwrite)
	    {
		  *pf=p;
		  dbg(DBG_PRINT,"LEaving shadow lookuppage\n");
		  return 0;
	    }
	}
      list_iterate_end();
       shad=shad->mmo_shadowed;
    }
      err= shad->mmo_ops->lookuppage(shad,pagenum,forwrite,pf);
      		  dbg(DBG_PRINT,"LEaving shadow lookuppage\n");
  	if(err<0 && forwrite)
		return err;
	else if(forwrite)
		return pframe_get(o,pagenum,pf);
	else
		return err;
}

/* As per the specification in mmobj.h, fill the page frame starting
 * at address pf->pf_addr with the contents of the page identified by
 * pf->pf_obj and pf->pf_pagenum. This function handles all
 * copy-on-write magic (i.e. if there is a shadow object which has
 * data for the pf->pf_pagenum-th page then we should take that data,
 * if no such shadow object exists we need to follow the chain of
 * shadow objects all the way to the bottom object and take the data
 * for the pf->pf_pagenum-th page from the last object in the chain).
 * It is important to use iteration rather than recursion here as a 
 * recursive implementation can overflow the kernel stack when 
 * looking down a long shadow chain */

static int
shadow_fillpage(mmobj_t *o, pframe_t *pf)
{
  dbg(DBG_PRINT,"entered shadow_fillpage\n");
   KASSERT(pframe_is_busy(pf));
   dbg(DBG_PRINT,"(GRADING3A 6.d)(precondition) KASSERT(pframe_is_busy(pf))\n");
   KASSERT(!pframe_is_pinned(pf));
   dbg(DBG_PRINT,"(GRADING3A 6.d) (precondition) KASSERT(!pframe_is_pinned(pf))\n");
  pframe_t *temp;
  mmobj_t *shad=o; 
  while(shad->mmo_shadowed)
    {
      shad=shad->mmo_shadowed;
  list_iterate_begin(&(shad->mmo_respages), temp, pframe_t, pf_olink)
    {
      if(temp->pf_pagenum == pf->pf_pagenum)
	{
	  memcpy(pf->pf_addr,temp->pf_addr,PAGE_SIZE);
	  return 0;
	}
    }
  list_iterate_end();
  
    }
  return shad->mmo_ops->fillpage(shad,pf);
  /* memcpy(pf->pf_addr,temp->pf_addr,PAGE_SIZE);*/
  /*return 0;*/
}

/* These next two functions are not difficult. */

static int
shadow_dirtypage(mmobj_t *o, pframe_t *pf)
{
  dbg(DBG_PRINT,"entered shadow_dirtypage\n");
  if(!pframe_is_pinned(pf))
    {
      pframe_pin(pf);
    }
  
  pframe_set_dirty(pf);
  return 0;
  /*NOT_YET_IMPLEMENTED("VM: shadow_dirtypage");
    return -1;*/
}

static int
shadow_cleanpage(mmobj_t *o, pframe_t *pf)
{
   dbg(DBG_PRINT,"entered shadow_cleanpage\n");
  if(pframe_is_pinned(pf))
    {
      pframe_unpin(pf);
    }
  
  pframe_clear_dirty(pf);
  
  return 0;
  /*NOT_YET_IMPLEMENTED("VM: shadow_cleanpage");
    return -1;*/
}
