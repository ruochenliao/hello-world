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

#include "types.h"
#include "globals.h"
#include "errno.h"

#include "util/debug.h"
#include "util/string.h"

#include "proc/proc.h"
#include "proc/kthread.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/pframe.h"
#include "mm/mmobj.h"
#include "mm/pagetable.h"
#include "mm/tlb.h"

#include "fs/file.h"
#include "fs/vnode.h"

#include "vm/shadow.h"
#include "vm/vmmap.h"

#include "api/exec.h"

#include "main/interrupt.h"

/* Pushes the appropriate things onto the kernel stack of a newly forked thread
 * so that it can begin execution in userland_entry.
 * regs: registers the new thread should have on execution
 * kstack: location of the new thread's kernel stack
 * Returns the new stack pointer on success. */
static uint32_t
fork_setup_stack(const regs_t *regs, void *kstack)
{
        /* Pointer argument and dummy return address, and userland dummy return
         * address */
        uint32_t esp = ((uint32_t) kstack) + DEFAULT_STACK_SIZE - (sizeof(regs_t) + 12);
        *(void **)(esp + 4) = (void *)(esp + 8); /* Set the argument to point to location of struct on stack */
        memcpy((void *)(esp + 8), regs, sizeof(regs_t)); /* Copy over struct */
        return esp;
}

/*
 * The implementation of fork(2). Once this works,
 * you're practically home free. This is what the
 * entirety of Weenix has been leading up to.
 * Go forth and conquer.
 */

int
do_fork(struct regs *regs)
{
  
  /*let newproc be a pointer to the new process and
    let newthr be a pointer to the new thread in newproc*/

    KASSERT(regs != NULL);
    dbg(DBG_PRINT,"GRADING 3A 7.a (precondition) KASSERT(regs != NULL); \n");
    KASSERT(curproc != NULL);
    dbg(DBG_PRINT,"GRADING 3A 7.a (precondition) KASSERT(curproc != NULL); \n");
    KASSERT(curproc->p_state == PROC_RUNNING);
    dbg(DBG_PRINT,"GRADING 3A 7.a (precondition) KASSERT(curproc->p_state == PROC_RUNNING); \n");

    proc_t * child_proc = proc_create( "child_proc" );
    proc_t * parent_proc = curproc;
    if( child_proc == NULL ){
    	dbg(DBG_PRINT,"failed to create the new process in do_fork\n");
    	return -ENOMEM;
    }
    pagedir_t *pd;
    vmarea_t * child_vmarea, *parent_vmarea;
    list_link_t vma_plink;
    uint32_t vhigh,vlow;
    list_link_t *l;

    /*CHECKING SOME CONDITIONS*/

    child_proc->p_vmmap = vmmap_clone( curproc -> p_vmmap );
    int i;
    /* for(i = 0; i < NFILES; i++){
    	fref( curproc -> p_files[i] );
	}*/
    l=parent_proc->p_vmmap->vmm_list.l_next;

       

    list_iterate_begin(&child_proc->p_vmmap->vmm_list, child_vmarea, vmarea_t, vma_plink){
    	/* shared */
      /*Alekhya*/
        parent_vmarea=list_item(l,vmarea_t,vma_plink);
   /*Alekhya*/

      if(parent_vmarea->vma_start == child_vmarea->vma_start)
	{
   if (child_vmarea -> vma_flags & MAP_SHARED ){
	  
	  /*Alekhya: I think this is how to get vm_area*/
	 
	    child_vmarea->vma_obj=parent_vmarea->vma_obj;
	  /*Alekhya*/
	    /*child_vmarea -> vma_obj =  parent_proc -> p_vmmap -> vmm_list -> l_next ->vma_obj;*/
    	    child_vmarea->vma_obj->mmo_ops->ref(child_vmarea->vma_obj);

	    list_insert_tail(&(child_vmarea-> vma_obj->mmo_un.mmo_vmas),&child_vmarea->vma_olink);
    	    /*list_insert_tail(&( parent_proc -> p_vmmap -> vmm_list->l_next-> vma_obj->mmo_un.mmo_vmas),&child_vmarea->vma_olink);*/
    	}
    	else if ( child_vmarea -> vma_flags & MAP_PRIVATE ){
    		/*Private*/
    		/*mmobj_t * old_shadow = parent_proc -> p_vmmap -> vmm_list -> l_next -> vma_obj;*/
	  /*Alekhya*/
	  mmobj_t * old_shadow = parent_vmarea -> vma_obj;
	  /*Alekhya*/
    		/*child process*/
    		mmobj_t * shadow_child = shadow_create();
    		shadow_child -> mmo_shadowed = old_shadow;
		shadow_child->mmo_un.mmo_bottom_obj=old_shadow->mmo_un.mmo_bottom_obj;
		
    		/*parent process*/
    		mmobj_t * shadow_parent = shadow_create();
		shadow_parent -> mmo_shadowed = old_shadow;
		shadow_parent->mmo_un.mmo_bottom_obj=old_shadow->mmo_un.mmo_bottom_obj;
		
		parent_vmarea->vma_obj=shadow_parent;
		/*parent_vmarea->vma_obj->mmo_ops->ref(parent_vmarea->vma_obj);*/
		
		/*parent_proc -> p_vmmap -> vmm_list -> l_next -> vma_obj = shadow_parent;*/
    		
		child_vmarea->vma_obj=shadow_child;
		   list_insert_tail(mmobj_bottom_vmas(shadow_child),&child_vmarea->vma_olink);
		   
		   while(old_shadow!=NULL)
		     {
		       old_shadow->mmo_ops->ref(old_shadow);
		       old_shadow=old_shadow->mmo_shadowed;
		     }
		   
		     
  	}
    	else {
    		dbg(DBG_PRINT,"error in fork\n");
    	}
	}
      l=l->l_next;
    }list_iterate_end();

    pt_unmap_range(curproc->p_pagedir,USER_MEM_LOW,USER_MEM_HIGH);
  	tlb_flush_all();



	
	context_t c1;
		  
       kthread_t *child_thread = kthread_clone( curthr );
       child_thread->kt_proc=child_proc;

       struct regs temp;
       regs->r_eax=0;
       c1.c_eip=(uint32_t)userland_entry;
       c1.c_esp=fork_setup_stack(regs,child_thread->kt_kstack);
       c1.c_pdptr=child_proc->p_pagedir;
       c1.c_kstacksz=DEFAULT_STACK_SIZE;
       c1.c_kstack=(uint32_t)child_thread->kt_kstack;
       child_thread->kt_ctx=c1;
       
       list_insert_tail(&child_proc->p_threads,&child_thread->kt_plink);
	
       /*Copy the file table of t parent to the child*/  
       /*Set any other fields necessary here*/
       
	
       for(i=0;i<NFILES;i++)
	 {
	   child_proc->p_files[i]=parent_proc->p_files[i];
	   if(parent_proc->p_files[i])
	       fref(parent_proc->p_files[i]);
	 }

              
       child_proc->p_cwd=parent_proc->p_cwd;
       vref(curproc->p_cwd);
       /*vget(child_proc->p_cwd->vn_fs,child_proc->p_cwd->vn_vno);*/
       

       child_proc->p_brk=parent_proc->p_brk;
       child_proc->p_start_brk=parent_proc->p_start_brk;
      

       return child_proc->p_pid;
}



