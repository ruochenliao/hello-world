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
#include "kernel.h"
#include "errno.h"

#include "util/debug.h"

#include "proc/proc.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/pagetable.h"

#include "vm/pagefault.h"
#include "vm/vmmap.h"

/*
 * This gets called by _pt_fault_handler in mm/pagetable.c The
 * calling function has already done a lot of error checking for
 * us. In particular it has checked that we are not page faulting
 * while in kernel mode. Make sure you understand why an
 * unexpected page fault in kernel mode is bad in Weenix. You
 * should probably read the _pt_fault_handler function to get a
 * sense of what it is doing.
 *
 * Before you can do anything you need to find the vmarea that
 * contains the address that was faulted on. Make sure to check
 * the permissions on the area to see if the process has
 * permission to do [cause]. If either of these checks does not
 * pass kill the offending process, setting its exit status to
 * EFAULT (normally we would send the SIGSEGV signal, however
 * Weenix does not support signals).
 *
 * Now it is time to find the correct page (don't forget
 * about shadow objects, especially copy-on-write magic!). Make
 * sure that if the user writes to the page it will be handled
 * correctly.
 *
 * Finally call pt_map to have the new mapping placed into the
 * appropriate page table.
 *
 * @param vaddr the address that was accessed to cause the fault
 *
 * @param cause this is the type of operation on the memory
 *              address which caused the fault, possible values
 *              can be found in pagefault.h
 */


void
handle_pagefault(uintptr_t vaddr, uint32_t cause)
{
   vmarea_t *v;
  uint32_t vfn;
  uint32_t pagenum;
  pframe_t *pfault;
  pagedir_t *pd;
  uint32_t pdflag,ptflag;
  vfn=ADDR_TO_PN(vaddr);
  v=vmmap_lookup(curproc->p_vmmap,vfn);
  int err;
  
  if(vfn==NULL ||  v==NULL)
    do_exit(EFAULT);
  
  if(cause & FAULT_WRITE)
    {
      if(!((v->vma_prot)& PROT_WRITE))
	  do_exit(EFAULT);
    }
  if(cause & FAULT_EXEC)
    {
      if(!((v->vma_prot)& PROT_EXEC))
	  do_exit(EFAULT);
    }
  /*if(cause & FAULT_RESERVED)*/
   if(!((v->vma_prot)& PROT_READ))
	  do_exit(EFAULT);      

  pdflag=PD_PRESENT | PD_USER;
  ptflag=PT_PRESENT | PT_USER;
  
  pagenum=vfn-(v->vma_start)+(v->vma_off);
  if((cause&FAULT_WRITE) == FAULT_WRITE)
    {
      err=pframe_lookup(v->vma_obj,pagenum,1,&pfault);
      if(err<0)
	do_exit(EFAULT);
      pdflag=pdflag | PD_WRITE;
      ptflag=ptflag | PT_WRITE;
      pframe_dirty(pfault); 
    }
  else
    {
        err=pframe_lookup(v->vma_obj,pagenum,0,&pfault);
	    if(err<0)
            do_exit(EFAULT);
    }
  pd=pt_get();
  if(pd==NULL)
    {
      do_exit(EFAULT);
    }
  
  uintptr_t paddr;

  paddr=pt_virt_to_phys((uintptr_t)pfault->pf_addr);
  
  pt_map(pd,(uintptr_t)PAGE_ALIGN_DOWN(vaddr),(uintptr_t)PAGE_ALIGN_DOWN(paddr),pdflag,ptflag);

  /*        NOT_YET_IMPLEMENTED("VM: handle_pagefault");*/
}
