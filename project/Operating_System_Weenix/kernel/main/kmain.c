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

#include "util/gdb.h"
#include "util/init.h"
#include "util/debug.h"
#include "util/string.h"
#include "util/printf.h"

#include "mm/mm.h"
#include "mm/page.h"
#include "mm/pagetable.h"
#include "mm/pframe.h"

#include "vm/vmmap.h"
#include "vm/shadowd.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "main/acpi.h"
#include "main/apic.h"
#include "main/interrupt.h"
#include "main/gdt.h"

#include "proc/sched.h"
#include "proc/proc.h"
#include "proc/kthread.h"

#include "drivers/dev.h"
#include "drivers/blockdev.h"
#include "drivers/disk/ata.h"
#include "drivers/tty/virtterm.h"
#include "drivers/pci.h"

#include "api/exec.h"
#include "api/syscall.h"

#include "fs/vfs.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/fcntl.h"
#include "fs/stat.h"

#include "test/kshell/kshell.h"
#include "errno.h"

GDB_DEFINE_HOOK(boot)
GDB_DEFINE_HOOK(initialized)
GDB_DEFINE_HOOK(shutdown)

static void       hard_shutdown(void);
static void      *bootstrap(int arg1, void *arg2);
static void      *idleproc_run(int arg1, void *arg2);
static kthread_t *initproc_create(void);
static void      *initproc_run(int arg1, void *arg2);

static context_t bootstrap_context;
static int gdb_wait = GDBWAIT;

/**
 * This is the first real C function ever called. It performs a lot of
 * hardware-specific initialization, then creates a pseudo-context to
 * execute the bootstrap function in.
 */
void *selftest_proc_run(int arg1, void *arg2){
	/*self test here*/
	

	dbg(DBG_PRINT,"(GRADING2C)(kmain.c)(selftest_proc_run) selftests begin\n");

	return NULL;
}
int selftest(kshell_t *kshell, int argc, char **argv)
{

         proc_t* selftest_proc = proc_create("self_test");
         kthread_t *thread = kthread_create(selftest_proc,selftest_proc_run, 0, NULL);
         sched_make_runnable(thread);
         KASSERT(kshell != NULL);
         dbg(DBG_PRINT, "(GRADING2C)(kmain.c)(self_test_proc) self_test_proc() is invoked");
         return 0;
}

int vfs_selftest(kshell_t *kshell, int argc, char **argv)
{
  int fd1,fd2;
  char *y="/ab/fil";
  char x[2];
  int err;
  do_mkdir("/ab");
  do_mknod("/ab/new", S_IFCHR,MKDEVID(1,1));
  fd1=do_open("/ab/new",2);
  fd2=do_dup2(fd1,NFILES+1);
  if(fd2<0)
    {
      dbg(DBG_PRINT,"File not created\n");
    }
  do_mknod("/ab/notmade",4096,MKDEVID(1,1));
  do_mknod("/ab/new/not",S_IFCHR,MKDEVID(1,1));
  do_mknod("/ab/new", S_IFCHR,MKDEVID(1,1));
  do_mknod("", S_IFCHR,MKDEVID(1,1));

  /*do_close(fd1);*/
    for(fd2=1;fd2<35;fd2++)
    {
      sprintf(x,"%d",fd2);
      strcat(y,x);
      do_mknod(y,S_IFCHR,MKDEVID(1,0));
      err=do_open(y,2);
      if(err<0)
	{
	  break;
	}
      if(fd2<10)
	{
	  y[strlen(y)-1]='\0';
	}
      else
	{
	   y[strlen(y)-2]='\0';
	}
      
    }
do_mknod("/ab/new1", S_IFCHR,MKDEVID(1,1));
 err=do_dup(fd1);
  do_unlink("/ab/new/ab");
 do_unlink("/ab/new");
 do_close(fd1);
 for(fd2=NFILES-1;fd2>0;fd2--)
   {
     err=do_close(fd2);
     sprintf(x,"%d",fd2);
      strcat(y,x);
      do_unlink(y);  
      if(err<0)
	{
	  break;
	}
      if(fd2<10)
	{
	  y[strlen(y)-1]='\0';
	}
      else
	{
	   y[strlen(y)-2]='\0';
	}
   }
 do_link("/a","/dev");
 do_link("/dev","/a");
 do_link("/dev","/a");
 do_rmdir("/a");
 /* mkdir("/k");
    do_link("/ab","/k");*/
  do_rmdir("/ab");
  /*do_rmdir("/k");*/

 /*GS: SELF TESTS*/
    dbg(DBG_PRINT,"\n*************************************************************\n");
    dbg(DBG_PRINT,"\n\n\n\n(GRADING2C)(kmain.c)(selftest_proc_run) selftests begin\n");

    int retVal = 0;
    int i = 0;

    /* 1. dbg(DBG_PRINT, "(GRADING2C) (vfs_syscall.c) (do_stat)  strlen too long, return -ENAMETOOLONG\n");*/
    char longPath[1024 + 1] = {0};
    for(i = 0; i < 1025; i++)
        longPath[i] = 'a';
    struct stat buf;
    retVal = do_stat(longPath, &buf);
    retVal=do_chdir(longPath);

    /*2. dbg(DBG_PRINT, "(GRADING2B) ENOTDIR or ENOENT\n");*/
    retVal = do_stat("", &buf);

    /*3. dbg(DBG_PRINT, "(GRADING2C) (vfs_syscall.c) (do_getdent) Invalid file descriptor fd, return -EBADF\n");*/
    struct dirent dirp;
    retVal = do_getdent(-1, &dirp);

    /*4. dbg(DBG_PRINT, "(GRADING2C) (vfs_syscall.c) (do_getdent) Invalid file descriptor fd, return -EBADF\n");*/
    retVal = do_getdent(1, &dirp);

    /*5. dbg(DBG_PRINT, "(GRADING2C) (vfs_syscall.c) (do_getdent) File descriptor does not refer to a directory, return -ENOTDIR\n");*/
    do_mknod("/./file", S_IFCHR,MKDEVID(1,1));
    fd1 = do_open("/./file",2);
    retVal = do_getdent(fd1, &dirp);

    do_unlink("/./file");
    do_close(fd1);
    /*6. dbg(DBG_PRINT, "(GRADING2C) (vfs_syscall.c) (do_rename) Both are valid names \n");*/
    /* and */
    /*7. dbg(DBG_PRINT, "(GRADING2C) (vfs_syscall.c) (do_rename) error do_link, return error\n"); \n");*/
    retVal = do_rename("/./aaa", "/./bbb");


    dbg(DBG_PRINT,"\n\nretVal=%d",retVal);
    dbg(DBG_PRINT,"\n*************************************************************\n");
  return 0;
}
extern void *testproc(int argc,void *argv);
int fabertest(kshell_t *kshell, int argc, char **argv)
{

         proc_t* new = proc_create("kshell2");
         kthread_t *thread = kthread_create(new,testproc, NULL, NULL);
         sched_make_runnable(thread);
	 KASSERT(kshell != NULL);
	 dbg(DBG_PRINT, "(GRADING1C)(kmain.c)(fabertest) faber testproc() is invoked");
	 return 0;
}


extern void *vfstest_main(int argc,void *argv);
int vfstest(kshell_t *kshell, int argc, char **argv)
{

	 proc_t* new = proc_create("kshell4");
	 new->p_cwd=vfs_root_vn;
	 vref(vfs_root_vn);
	 kthread_t *thread = kthread_create(new,vfstest_main, 1, NULL);
	 sched_make_runnable(thread);
	 KASSERT(kshell != NULL);
	 dbg(DBG_PRINT, "(GRADING2B)(kmain.c)(vfstest) vfstest_main() is invoked");
	 return 0;
}

extern void *sunghan_test(int arg1, void *arg2);

int sTest()
{
        proc_t* new = proc_create("kshell3");
        kthread_t *thread = kthread_create(new,sunghan_test, NULL, NULL);
        sched_make_runnable(thread);
	return 0;
}

extern void *sunghan_deadlock_test(int arg1,void *arg2); 

int sTest2()
{
  /* proc_t* new = proc_create("kshell2");
        kthread_t *thread = kthread_create(new,sunghan_deadlock_test, NULL, NULL);
        sched_make_runnable(thread);*/
  sunghan_deadlock_test(NULL,NULL);
	return 0;
}

void
kmain()
{
        GDB_CALL_HOOK(boot);

        dbg_init();
        dbgq(DBG_CORE, "Kernel binary:\n");
        dbgq(DBG_CORE, "  text: 0x%p-0x%p\n", &kernel_start_text, &kernel_end_text);
        dbgq(DBG_CORE, "  data: 0x%p-0x%p\n", &kernel_start_data, &kernel_end_data);
        dbgq(DBG_CORE, "  bss:  0x%p-0x%p\n", &kernel_start_bss, &kernel_end_bss);

        page_init();

        pt_init();
        slab_init();
        pframe_init();

        acpi_init();
        apic_init();
          pci_init();
        intr_init();

        gdt_init();

        /* initialize slab allocators */
#ifdef __VM__
        anon_init();
        shadow_init();
#endif
        vmmap_init();
        proc_init();
        kthread_init();

#ifdef __DRIVERS__
        bytedev_init();
        blockdev_init();
#endif

        void *bstack = page_alloc();
        pagedir_t *bpdir = pt_get();
        KASSERT(NULL != bstack && "Ran out of memory while booting.");
        /* This little loop gives gdb a place to synch up with weenix.  In the
         * past the weenix command started qemu was started with -S which
         * allowed gdb to connect and start before the boot loader ran, but
         * since then a bug has appeared where breakpoints fail if gdb connects
         * before the boot loader runs.  See
         *
         * https://bugs.launchpad.net/qemu/+bug/526653
         *
         * This loop (along with an additional command in init.gdb setting
         * gdb_wait to 0) sticks weenix at a known place so gdb can join a
         * running weenix, set gdb_wait to zero  and catch the breakpoint in
         * bootstrap below.  See Config.mk for how to set GDBWAIT correctly.
         *
         * DANGER: if GDBWAIT != 0, and gdb is not running, this loop will never
         * exit and weenix will not run.  Make SURE the GDBWAIT is set the way
         * you expect.
         */
        while (gdb_wait) ;
        context_setup(&bootstrap_context, bootstrap, 0, NULL, bstack, PAGE_SIZE, bpdir);
        context_make_active(&bootstrap_context);

        panic("\nReturned to kmain()!!!\n");
}

/**
 * Clears all interrupts and halts, meaning that we will never run
 * again.
 */
static void
hard_shutdown()
{
#ifdef __DRIVERS__
        vt_print_shutdown();
#endif
        __asm__ volatile("cli; hlt");
}

/**
 * This function is called from kmain, however it is not running in a
 * thread context yet. It should create the idle process which will
 * start executing idleproc_run() in a real thread context.  To start
 * executing in the new process's context call context_make_active(),
 * passing in the appropriate context. This function should _NOT_
 * return.
 *
 * Note: Don't forget to set curproc and curthr appropriately.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
bootstrap(int arg1, void *arg2)
{
    /*Andy*/
        proc_t *idle_process=NULL;
        kthread_t *thr=NULL;

        /* necessary to finalize page table information */
        pt_template_init();

        /*NOT_YET_IMPLEMENTED("PROCS: bootstrap");*/
        /* create a new process*/
        idle_process=proc_create("idle_process");
        curproc=idle_process;
        /*the process should not be null*/
        KASSERT(NULL!=curproc);
	    dbg(DBG_PRINT,"GRADING1A 1.a KASSERT(NULL != curproc);\n");
	/*	KASSERT(idle_process !=NULL);
		dbg(DBG_PRINT,"KASSERT(idle_process !=NULL) make sure that what has been created is the "idle" process \n");*/
        KASSERT( PID_IDLE==curproc->p_pid);
        dbg(DBG_PRINT,"GRADING1A 1.a KASSERT(PID_IDLE == curproc->p_pid);\n");

        /*create a new thread*/
        thr=kthread_create(idle_process,idleproc_run,0,NULL);
        curthr=thr;
        KASSERT(curthr !=NULL);
        dbg(DBG_PRINT,"GRADING1A 1.a KASSERT(NULL != curthr);\n");
        /*Make the thread run*/
        sched_make_runnable(thr);
        /*Switch to the next thread in kt_runq*/
        context_make_active(&curthr->kt_ctx);
        panic("weenix returned to bootstrap()!!! BAD!!!\n");
        return NULL;
    /*Andy*/
}

/**
 * Once we're inside of idleproc_run(), we are executing in the context of the
 * first process-- a real context, so we can finally begin running
 * meaningful code.
 *
 * This is the body of process 0. It should initialize all that we didn't
 * already initialize in kmain(), launch the init process (initproc_run),
 * wait for the init process to exit, then halt the machine.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
idleproc_run(int arg1, void *arg2)
{
        int status;
        pid_t child;
        /*Andy*/
        /* create init proc */
        kthread_t *initthr = initproc_create();
        init_call_all();
        GDB_CALL_HOOK(initialized);

        /* Create other kernel threads (in order) */

#ifdef __VFS__
        /* Once you have VFS remember to set the current working directory
         * of the idle and init processes */

        initthr->kt_proc->p_cwd=vfs_root_vn;
        vref(vfs_root_vn);
        dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) set the cwd of init process to vfs_root_vn\n");
        curproc->p_cwd=vfs_root_vn;
    	vref(vfs_root_vn);
    	dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) make directory set the cwd of idle process to vfs_root_vn \n");

        /* Here you need to make the null, zero, and tty devices using mknod */
        /* You can't do this until you have VFS, check the include/drivers/dev.h
         * file for macros with the device ID's you will need to pass to mknod */
        /*NOT_YET_IMPLEMENTED("VFS: idleproc_run");*/
    	/*Andy*/
        do_mkdir("/dev");
        dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) make directory /dev\n");
		do_mknod("/dev/null", S_IFCHR,MKDEVID(1,0));
		dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) make directory /dev/null\n");
		do_mknod("/dev/zero", S_IFCHR,MKDEVID(1,1));
		dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) make directory /dev/zero\n");
		do_mknod("/dev/tty0", S_IFCHR,MKDEVID(2,0));
		dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) make directory /dev/tty0\n");
		do_mknod("/dev/tty1", S_IFCHR,MKDEVID(2,1));
		dbg(DBG_PRINT,"(GRADING2B)(kmain.c)(idleproc_run) make directory /dev/tty1\n");

#endif

        /* Finally, enable interrupts (we want to make sure interrupts
         * are enabled AFTER all drivers are initialized) */
        intr_enable();

        /* Run initproc */
        sched_make_runnable(initthr);
        /* Now wait for it */
        child = do_waitpid(-1, 0, &status);
        KASSERT(PID_INIT == child);

#ifdef __MTP__
        kthread_reapd_shutdown();
#endif


#ifdef __SHADOWD__
        /* wait for shadowd to shutdown */
        shadowd_shutdown();
#endif

#ifdef __VFS__
        /* Shutdown the vfs: */
        dbg_print("weenix: vfs shutdown...\n");
        vput(curproc->p_cwd);
        if (vfs_shutdown())
                panic("vfs shutdown FAILED!!\n");

#endif

        /* Shutdown the pframe system */
#ifdef __S5FS__
        pframe_shutdown();
#endif

            dbg_print("\nweenix: halted cleanly!\n");
        GDB_CALL_HOOK(shutdown);
        hard_shutdown();
        return NULL;
}

/**
 * This function, called by the idle process (within 'idleproc_run'), creates the
 * process commonly refered to as the "init" process, which should have PID 1.
 *
 * The init process should contain a thread which begins execution in
 * initproc_run().
 *
 * @return a pointer to a newly created thread which will execute
 * initproc_run when it begins executing
 */
static kthread_t *
initproc_create(void)
{
    /*Andy*/
        /*proc_initproc is a static pointer to initial process in \proc.c*/
        proc_t *init_process=NULL;
        kthread_t *init_thr=NULL;
        /* create a new process*/
        init_process=proc_create("init_process");
        /*the process should not be null*/
        KASSERT(init_process !=NULL);
        dbg(DBG_PRINT,"GRADING1A 1.b KASSERT(NULL!=init_process)\n");
        KASSERT(PID_INIT==init_process->p_pid);
        dbg(DBG_PRINT,"GRADING1A 1.b KASSERT(PID_INIT==init_process)\n");
        /*NOT_YET_IMPLEMENTED("PROCS: initproc_create");*/
        /*create a new thread*/
        init_thr=kthread_create(init_process,initproc_run,NULL,NULL);
        KASSERT(init_thr !=NULL);
        dbg(DBG_PRINT,"GRADING1A 1.b KASSERT(init_thr !=NULL)\n");
        /*Make the thread run*/
/*        sched_make_runnable(init_thr);
        sched_switch();*/
        return init_thr;
    /*Andy*/
}

/**
 * The init thread's function changes depending on how far along your Weenix is
 * developed. Before VM/FI, you'll probably just want to have this run whatever
 * tests you've written (possibly in a new process). After VM/FI, you'll just
 * exec "/bin/init".
 *
 * Both arguments are unused.
 *
 * @param arg1 the first argument (unused)
 * @param arg2 the second argument (unused)
 */
static void *
initproc_run(int arg1, void *arg2)
{
  /*Andy*/

       kshell_add_command("fabertest", fabertest, "PROCS: invoke faber testproc()");
 kshell_add_command("vfs_selftest1", vfs_selftest, "My vfs test");
       dbg(DBG_PRINT,"(GRADING1E)(kmain.c)(initproc_run) add command fabertest\n");
       kshell_add_command("stest", sTest, "PROCS: sunghan_test()");
       dbg(DBG_PRINT,"(GRADING1E)(kmain.c)(initproc_run) add command stest\n");
       kshell_add_command("deadlock", sTest2, "PROCS: sunghandeadlock__test()");
       dbg(DBG_PRINT,"(GRADING1E)(kmain.c)(initproc_run) add command deadlock\n");
              kshell_add_command("vfstest", vfstest, "VFS TEST");
       dbg(DBG_PRINT,"(GRADING1E)(kmain.c)(initproc_run) add command vfstest\n");
       /*       kshell_add_command("selftest", selftest, "SELF_TEST");
       */
    /*       kernel_execve("/sbin/init",NULL,NULL);*/
       
       char *argv[] = { NULL };
       char *envp[] = { NULL };
            kernel_execve("/sbin/init", argv, envp);


        kshell_t *kshell = kshell_create(0);
        if (NULL == kshell) panic("init: Couldn't create kernel shell\n");

	while (kshell_execute_next(kshell));
	kshell_destroy(kshell);

	
  return NULL;

}
