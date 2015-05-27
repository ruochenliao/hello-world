Documentation for Kernel Assignment 2
=====================================

+-------------+
| BUILD & RUN |
+-------------+

Comments: Please use the standard kernel 2 commands:
          make clean
          make
          ./weenix -n
          
          "vfstest" kshell command
          	    This will invoke vfstest_main() 
          	    (with argc=1 and argv=NULL) in "kernel/test/vfstest/vfstest.c

+------+
| SKIP |
+------+

No.

+---------+
| GRADING |
+---------+

(A.1) In fs/vnode.c:
    (a) In special_file_read(): 6 pts
    (b) In special_file_write(): 6 pts

(A.2) In fs/namev.c:
    (a) In lookup(): 6 pts
    (b) In dir_namev(): 10 pts
    (c) In open_namev(): 2 pts

(3) In fs/vfs_syscall.c:
    (a) In do_write(): 6 pts
    (b) In do_mknod(): 2 pts
    (c) In do_mkdir(): 2 pts
    (d) In do_rmdir(): 2 pts
    (e) In do_unlink():2 pts
    (f) In do_stat():  2 pts

(B) vfstest: 40 pts
    Comments: Works twice.

(C) Self-checks :
    Comments:
	The code paths which are not covered in (A) and (B) are checked using our self test check.
    All the selftests can be invokded inside kshell using command vfs_selftest1(in kmain.c the function is vfs_selftest())
    (1.a) do_dup2  This test checks for invalid nfd
    (1.b) do_mknod This test checks whether mode is wrong
    (1.c) do_mknod This test checks when we are trying to make a node inside something which is not a directory
    (1.d) do_mknod This test checks whether node already exists
    (1.e) do_mknod This test returns error when the path is too short
    (1.f) do_unlink This test returns error when we are trying to unlink a node from something which is not a directory
    (1.g) do_dup   This test checks whether maximum number of files already opened. Returns error as max has been reached.
    (1.h) do_link  This test checks whether link was succesffully created, returns success
    (1.i) do_link  This test returns error because the tonode  already exists
    (1.j) do_link  This test checks if the fromnode is an invalid directory
    (1.k) do_open  This test checks if do open reaches NFILES
    (1.l) do_stat This test gives an error as the name is too long.
    (1.m) do_rename This test gives an error as do_link is not successful
    (1.n)do_chdir This test returns error as the pathname is too long.

    Missing required section(s) in README file (procs-README.txt): No
Submitted binary file : No
Submitted extra (unmodified) file : No
Wrong file location in submission : No
Use dbg_print(...) instead of dbg(DBG_PRINT, ...) : No
Not properly indentify which dbg() printout is for which item in the grading guidelines : No
Cannot compile : Compiles
Compiler warnings : None
"make clean" : Works
Useless KASSERT : Where reqired and needed.
Insufficient/Confusing dbg : No
Kernel panic : No
Cannot halt kernel cleanly : Halts Cleanly

+------+
| BUGS |
+------+

Comments: No bugs.

+---------------------------+
| CONTRIBUTION FROM MEMBERS |
+---------------------------+

Equal.

+-------+
| OTHER |
+-------+

Special DBG setting in Config.mk for certain tests: No
Comments on deviation from spec (you will still lose points, but it's better to let the grader know): No
General comments on design decisions: 
We lost marks for DBG statement format last time, and for self checks. We have corrected those this time, for kernel1 as well


