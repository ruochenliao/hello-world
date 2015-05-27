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

/*
 *  FILE: open.c
 *  AUTH: mcc | jal
 *  DESC:./weenix -n -d gdb -w 10
 *  DATE: Mon Apr  6 19:27:49 1998
 */

#include "globals.h"
#include "errno.h"
#include "fs/fcntl.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/vfs.h"
#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/stat.h"
#include "util/debug.h"

/* find empty index in p->p_files[] */
int
get_empty_fd(proc_t *p)
{
        int fd;

        for (fd = 0; fd < NFILES; fd++) {
                if (!p->p_files[fd])
                        return fd;
        }

        dbg(DBG_ERROR | DBG_VFS, "ERROR: get_empty_fd: out of file descriptors "
            "for pid %d\n", curproc->p_pid);
        return -EMFILE;
}

/*
 * There a number of steps to opening a file:
 *      1. Get the next empty file descriptor.
 *      2. Call fget to get a fresh file_t.
 *      3. Save the file_t in curproc's file descriptor table.
 *      4. Set file_t->f_mode to OR of FMODE_(READ|WRITE|APPEND) based on
 *         oflags, which can be O_RDONLY, O_WRONLY or O_RDWR, possibly OR'd with
 *         O_APPEND.
 *      5. Use open_namev() to get the vnode for the file_t.
 *      6. Fill in the fields of the file_t.
 *      7. Return new fd.
 *
 * If anything goes wrong at any point (specifically if the call to open_namev
 * fails), be sure to remove the fd from curproc, fput the file_t and return an
 * error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        oflags is not valid.
 *      o EMFILE
 *        The process already has the maximum number of files open.
 *      o ENOMEM
 *        Insufficient kernel memory was available.
 *      o ENAMETOOLONG
 *        A component of filename was too long.
 *      o ENOENT
 *        O_CREAT is not set and the named file does not exist.  Or, a
 *        directory component in pathname does not exist.
 *      o EISDIR
 *        pathname refers to a directory and the access requested involved
 *        writing (that is, O_WRONLY or O_RDWR is set).
 *      o ENXIO
 *        pathname refers to a device special file and no corresponding device
 *        exists.
 */

int
do_open(const char *filename, int oflags)
{
	/*1. Get the next empty file descriptor*/
	int fd,error_num;
         vnode_t *res_vnode;
	int r_or_w = 0;
	int status = 0;
	file_t *f;

	fd = get_empty_fd( curproc ); 	
	if( fd == -EMFILE ){		
		dbg(DBG_PRINT,"(GRADING2C 1.k) The process already has the maximum number of files open.\n ");
		return -EMFILE;
		}

	/*2. Call fget to get a fresh file_t.*/

	f = fget(-1);

	/*3. Save the file_t in curproc's file descriptor table.*/
	curproc -> p_files[fd] = f;

	/*4. Set file_t->f_mode to OR of FMODE_(READ|WRITE|APPEND) based on
 *         oflags, which can be O_RDONLY, O_WRONLY or O_RDWR, possibly OR'd with
 *         O_APPEND.*/
	
	r_or_w = oflags & 0x003;
	status = oflags & 0x700;
	
       	error_num = open_namev( filename, status ,&res_vnode , NULL);

	if(error_num<0)
	  {
	    dbg(DBG_PRINT," (GRADING2B) There is error number returning from open_namev() return with %d\n",error_num);
	    fput(curproc->p_files[fd]);
	    curproc->p_files[fd]=NULL;
	    return error_num;
	  }
	if ( r_or_w == O_RDONLY){
		f -> f_mode = FMODE_READ;
		dbg(DBG_PRINT,"(GRADING2B) The file open with read only");

	}
	else if ( r_or_w == O_WRONLY ){
		f -> f_mode = FMODE_WRITE;
		dbg(DBG_PRINT,"(GRADING2B) The file open with write only");
		if(S_ISDIR( (res_vnode) -> vn_mode) ){
			fput(curproc->p_files[fd]);
			vput(res_vnode);
			curproc->p_files[fd]=NULL;
			dbg(DBG_PRINT," (GRADING2B) pathname refers to a directory and the access requested involved writing retern with -EISDIR\n");
			return -EISDIR;
		}		
	}
	else if ( r_or_w == O_RDWR ){
		f -> f_mode = FMODE_READ|FMODE_WRITE;
		dbg(DBG_PRINT,"(GRADING2B) The file open with read and write");
		if( S_ISDIR( (res_vnode) -> vn_mode ) ){
			fput(curproc->p_files[fd]);
			vput(res_vnode);
			curproc->p_files[fd]=NULL;
			dbg(DBG_PRINT," (GRADING2B) pathname refers to a directory and the access requested involved writing retern with -EISDIR\n");
			return -EISDIR;
		}
	}
	else{
		dbg(DBG_PRINT,"(GRADING2B) There is invalid value of oflags, return with -EINVAL");
		fput(curproc -> p_files[fd]);
		vput(res_vnode);
		curproc->p_files[fd] = NULL;		
		return -EINVAL;		
	}  

	/*5. Use open_namev() to get the vnode for the file_t.*/
	
	/*check the file name length ' ENAMETOOLONG '*/
	/*	int len = strlen(filename);
	if( len > MAXPATHLEN){
		dbg(DBG_PRINT,"(GRADING2B) len > MAXPATHLEN, filename is too long return with -ENAMETOOLONG \n");
		curproc->p_files[fd]=NULL;
		fput( curproc->p_files[fd] );		
		return -ENAMETOOLONG;
	}
*/
	if( status == O_APPEND ){
			dbg(DBG_PRINT,"(GRADING2B) the oflags has  O_APPEND status, set the curproc->p_files[fd]->f_mode =  FMODE_APPEND\n");
			curproc->p_files[fd]->f_mode = curproc->p_files[fd]->f_mode | FMODE_APPEND;
	}

	/*switch( -error_num ){
		case EINVAL: dbg(DBG_PRINT," error 1"); fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL; return -1; break;
		case EMFILE: dbg(DBG_PRINT," error 2"); return -1; fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL; break;
		case ENOMEM: dbg(DBG_PRINT," error 3"); return -1; fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL;break;
		case ENAMETOOLONG: dbg(DBG_PRINT," error 4"); return -1; fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL; break;
		case ENOENT: dbg(DBG_PRINT," error 5"); return -1; fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL;break;
		case EISDIR: dbg(DBG_PRINT," error 6"); return -1; fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL;break;
		case ENXIO: dbg(DBG_PRINT," error 7"); return -1; fput(curproc->p_files[fd]); curproc->p_files[fd]=NULL; break;
		default : ; break;
		}*/
	/*6. Fill in the fields of the file_t.*/
	curproc -> p_files[fd] -> f_vnode = res_vnode;
/*	curproc -> p_files[fd] -> f_refcount++;*/
	dbg(DBG_PRINT,"(GRADING2B) if there is no error when creating files, return with fd up to do_open()\n");
	return (fd);
}
