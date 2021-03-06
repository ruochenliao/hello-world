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

#include "config.h"
#include "globals.h"

#include "errno.h"

#include "util/init.h"
#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"

#include "proc/kthread.h"
#include "proc/proc.h"
#include "proc/sched.h"

#include "mm/slab.h"
#include "mm/page.h"

kthread_t *curthr; /* global */
static slab_allocator_t *kthread_allocator = NULL;

#ifdef __MTP__
/* Stuff for the reaper daemon, which cleans up dead detached threads */
static proc_t *reapd = NULL;
static kthread_t *reapd_thr = NULL;
static ktqueue_t reapd_waitq;
static list_t kthread_reapd_deadlist; /* Threads to be cleaned */

static void *kthread_reapd_run(int arg1, void *arg2);
#endif

void
kthread_init()
{
        kthread_allocator = slab_allocator_create("kthread", sizeof(kthread_t));
        KASSERT(NULL != kthread_allocator);
}

/**
 * Allocates a new kernel stack.
 *
 * @return a newly allocated stack, or NULL if there is not enough
 * memory available
 */
static char *
alloc_stack(void)
{
        /* extra page for "magic" data */
        char *kstack;
        int npages = 1 + (DEFAULT_STACK_SIZE >> PAGE_SHIFT);
        kstack = (char *)page_alloc_n(npages);

        return kstack;
}

/**
 * Frees a stack allocated with alloc_stack.
 *
 * @param stack the stack to free
 */
static void
free_stack(char *stack)
{
        page_free_n(stack, 1 + (DEFAULT_STACK_SIZE >> PAGE_SHIFT));
}

void
kthread_destroy(kthread_t *t)
{
        KASSERT(t && t->kt_kstack);
        free_stack(t->kt_kstack);
        if (list_link_is_linked(&t->kt_plink))
                list_remove(&t->kt_plink);

        slab_obj_free(kthread_allocator, t);
}

/*
 * Allocate a new stack with the alloc_stack function. The size of the
 * stack is DEFAULT_STACK_SIZE.
 *
 * Don't forget to initialize the thread context with the
 * context_setup function. The context should have the same pagetable
 * pointer as the process.
 */
kthread_t *
kthread_create(struct proc *p, kthread_func_t func, long arg1, void *arg2)
{
  char *kstack1;
  context_t c1;/*=kmalloc(sizeof(struct context *));*/
  
  /*Need to allocate memory for the thread */
  kthread_t *k1=slab_obj_alloc(kthread_allocator);
  /*To make sure the process exits and does not point to null pointer*/
  KASSERT(p!=NULL);
  dbg(DBG_PRINT,"(GRADING1A 3.a) (precondition) KASSERT(NULL != p); /* should have associated process */\n");
  
  /*allocates a new kernel stack to the thread ot be created */
  kstack1=alloc_stack();
  
  /*the context of the thread has to be setup, the function is defined in context.c. Here c1 is the context structure. func is the function which is being implemented by this thread. arg1, arg2 are arguments to this func. kstack1 is the stack allocated for this thread. DEFAULT stack size is the size of kstack1. p is the pointer to the */
  /*TODO figure out where the pagetable pointer has to point to*/
  context_setup(&c1,func,arg1,arg2,kstack1,DEFAULT_STACK_SIZE,p->p_pagedir);
  
  /*assign this context to the thread*/
  k1->kt_ctx=c1;
  k1->kt_wchan=NULL;
  k1->kt_retval=NULL;
  list_link_init(&k1->kt_plink);
  list_link_init(&k1->kt_qlink);
  list_insert_tail(&p->p_threads,&k1->kt_plink);

  /*the stack allocated to the kernel*/
  k1->kt_kstack=kstack1;

  /*the error no if a system call fails, so initially none, so assigned zero*/
  k1->kt_errno=0;
  
  /*the process which needs to be run on this thread.*/
  k1->kt_proc=p;
  k1->kt_wchan=NULL;
  /*initially its cancellable state is 0. which is changed while calling pthread_cancel*/
  k1->kt_cancelled=0;

  /*All threads are initially in running state*/
  k1->kt_state=KT_RUN;
    
  /*
    ktqueue_enqueue(&kt_runq,k1);*/ 
  /*Put the newly created thread in the runq (has been initialized in sched.c)*/

  return k1;
  /*TODO Check what needs to be done.*/
  
  
}

/*
 * If the thread to be cancelled is the current thread, this is
 * equivalent to calling kthread_exit. Otherwise, the thread is
 * sleeping and we need to set the cancelled and retval fields of the
 * thread.
 *
 * If the thread's sleep is cancellable, cancelling the thread should
 * wake it up from sleep.
 *
 * If the thread's sleep is not cancellable, we do nothing else here.
 */
void
kthread_cancel(kthread_t *kthr, void *retval)
{
  /*kthread_t *temp=kmalloc(sizeof(struct kthread *));
  temp=ktqueue_dequeue(kt_runq);*/
  KASSERT(kthr!=NULL);
  dbg(DBG_PRINT,"(GRADING1A 3.b) (precondition) KASSERT(NULL != kthr); /* should have thread */\n");
  
  /*Checking is this thread is the current thread.*/
  if(kthr==curthr)
    {
      dbg(DBG_PRINT,"(GRADING1E) (kthread.c) (kthread_cancel) if(kthr==curthr), kthread exit called");
      kthread_exit(retval);
    }
  else
    {
      /*the cancelled and retval has been set.*/
      dbg(DBG_PRINT,"(GRADING1E) (kthread.c) (kthread_cancel) if(kthr!=curthr),check KT_STATE is SLEEP_CANCELLABLE");
      kthr->kt_cancelled=1;
      kthr->kt_retval=retval;
      
      if(kthr->kt_state==KT_SLEEP_CANCELLABLE)
	{
	  /*As we can only cancel a thread which is currently being run, if it is sleeping, we need to put it back onto the run queue.*/
	  /*	  kthr->kt_state=KT_RUN;*/

          dbg(DBG_PRINT,"(GRADING1E) (kthread.c) (kthread_cancel)  if(kthr->kt_state==KT_SLEEP_CANCELLABLE), sched_cancel(kthr)called");
          sched_cancel(kthr);	  
	  /*Schedular will have to probably put it back in the queue.*/
	
	}
      
    }
  
}

/*
 * You need to set the thread's retval field, set its state to
 * KT_EXITED, and alert the current process that a thread is exiting
 * via proc_thread_exited.
 *
 * It may seem unneccessary to push the work of cleaning up the thread
 * over to the process. However, if you implement MTP, a thread
 * exiting does not necessarily mean that the process needs to be
 * cleaned up.
 */
void
kthread_exit(void *retval)
{
  
    KASSERT(!curthr->kt_wchan);
  dbg(DBG_PRINT,"(GRADING1A 3.c) KASSERT(!curthr->kt_wchan); /* queue should be empty */\n");
  KASSERT(!curthr->kt_qlink.l_next && !curthr->kt_qlink.l_prev); /* queue should be empty */
  dbg(DBG_PRINT,"(GRADING1A 3.c) KASSERT(!curthr->kt_qlink.l_next && !curthr->kt_qlink.l_prev); /* queue should be empty */\n");
  KASSERT(curthr->kt_proc == curproc);
  dbg(DBG_PRINT,"(GRADING1A 3.c) KASSERT(curthr->kt_proc == curproc)\n");

  /*The retval has to be set while exiting*/
  curthr->kt_retval=retval;

  /*the state of the thread has to be changed as the thread needs to know that it exited*/
  curthr->kt_state=KT_EXITED;
  
  /*this function means the process knows that it needs to be cleaned upso that new threads can run on it.*/
      proc_thread_exited(retval);
  


}

/*
 * The new thread will need its own context and stack. Think carefully
 * about which fields should be copied and which fields should be
 * freshly initialized.
 *
 * You do not need to worry about this until VM.
 */
kthread_t *
kthread_clone(kthread_t *thr)
{
    kthread_t *k1=slab_obj_alloc(kthread_allocator);
      char *kstack1;

      memcpy(k1,thr, sizeof(kthread_t));
      kstack1=alloc_stack();
      k1->kt_kstack=kstack1;
      sched_make_runnable(k1);

      /*Im not sure how to give a new context*/
      return k1;
    /*NOT_YET_IMPLEMENTED("VM: kthread_clone");
      return NULL;*/
}

/*
 * The following functions will be useful if you choose to implement
 * multiple kernel threads per process. This is strongly discouraged
 * unless your weenix is perfect.
 */
#ifdef __MTP__
int
kthread_detach(kthread_t *kthr)
{
        NOT_YET_IMPLEMENTED("MTP: kthread_detach");
        return 0;
}

int
kthread_join(kthread_t *kthr, void **retval)
{
        NOT_YET_IMPLEMENTED("MTP: kthread_join");
        return 0;
}

/* ------------------------------------------------------------------ */
/* -------------------------- REAPER DAEMON ------------------------- */
/* ------------------------------------------------------------------ */
static __attribute__((unused)) void
kthread_reapd_init()
{
        NOT_YET_IMPLEMENTED("MTP: kthread_reapd_init");
}
init_func(kthread_reapd_init);
init_depends(sched_init);

void
kthread_reapd_shutdown()
{
        NOT_YET_IMPLEMENTED("MTP: kthread_reapd_shutdown");
}

static void *
kthread_reapd_run(int arg1, void *arg2)
{
        NOT_YET_IMPLEMENTED("MTP: kthread_reapd_run");
        return (void *) 0;
}
#endif
