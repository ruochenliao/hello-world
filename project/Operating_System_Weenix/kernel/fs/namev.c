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

#include "kernel.h"
#include "globals.h"
#include "types.h"
#include "errno.h"

#include "util/string.h"
#include "util/printf.h"
#include "util/debug.h"

#include "fs/dirent.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/vfs.h"
#include "fs/vnode.h"

/* This takes a base 'dir', a 'name', its 'len', and a result vnode.
 * Most of the work should be done by the vnode's implementation
 * specific lookup() function, but you may want to special case
 * "." and/or ".." here depnding on your implementation.
 *
 * If dir has no lookup(), return -ENOTDIR.
 *
 * Note: returns with the vnode refcount on *result incremented.
 */
int
lookup(vnode_t *dir, const char *name, size_t len, vnode_t **result)
{
	/*Andy*/
	int res;
	KASSERT(NULL != dir);
	dbg(DBG_PRINT, "(GRADING2A 2.a) (precondition) KASSERT(NULL != dir);\n");
	KASSERT(NULL != name);
	dbg(DBG_PRINT, "(GRADING2A 2.a) (precondition) KASSERT(NULL != name);\n");
	KASSERT(NULL != result);
	dbg(DBG_PRINT, "(GRADING2A 2.a) (precondition) KASSERT(NULL != result);\n");

	/*special case: name="." */
/*	if ((len == 1)&& (strcmp(name,".")==0)) {
		*result=curproc->p_cwd;
		return 0;
	}*/
	/*special case: name=".." */
/*	if ((len == 1)&& (strcmp(name,"..")==0)) {
		dbg(DBG_PRINT,"lookup:name =..\n");
		return -ENOTEMPTY;
	}*/
	/*A component used as a directory in path is not, in fact, a directory.*/
	if (dir->vn_ops->lookup == NULL) {
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(lookup) not a directory\n");
		return -ENOTDIR;
	}

	if(len>NAME_LEN){
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) name too long\n");
		return -ENAMETOOLONG;
	}
	/* This sovles the test case a/b/ */
	if (len == 0) {
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) lenth of name is 0\n");
		*result = dir;
		vget(dir->vn_fs, dir->vn_vno);
		return 0;
	}
	res=dir->vn_ops->lookup(dir,name,len,result);
	return res;
	

    /*NOT_YET_IMPLEMENTED("VFS: lookup");*/
}


/* When successful this function returns data in the following "out"-arguments:
 *  o res_vnode: the vnode of the parent directory of "name"
 *  o name: the `basename' (the element of the pathname)
 *  o namelen: the length of the basename
 *
 * For example: dir_namev("/s5fs/bin/ls", &namelen, &name, NULL,
 * &res_vnode) would put 2 in namelen, "ls" in name, and a pointer to the
 * vnode corresponding to "/s5fs/bin" in res_vnode.
 *
 * The "base" argument defines where we start resolving the path from:
 * A base value of NULL means to use the process's current working directory,
 * curproc->p_cwd.  If pathname[0] == '/', ignore base and start with
 * vfs_root_vn.  dir_namev() should call lookup() to take care of resolving each
 * piece of the pathname.
 *
 * Note: A successful call to this causes vnode refcount on *res_vnode to
 * be incremented.
 */
int
dir_namev(const char *pathname, size_t *namelen, const char **name,
          vnode_t *base, vnode_t **res_vnode)
{
	int len=strlen(pathname);
	char buffer[MAXPATHLEN+1];
	char *start, *slash;
	vnode_t *dir;
	int errno;

	/* NOT_YET_IMPLEMENTED("VFS: dir_namev");*/
	KASSERT(NULL != pathname);
	dbg(DBG_PRINT, "(GRADING2A 2.b) (precondition) KASSERT(NULL != pathname)\n");
	KASSERT(NULL != namelen);
	dbg(DBG_PRINT, "(GRADING2A 2.b) (precondition) KASSERT(NULL != namelen)");
	KASSERT(NULL != name);
	dbg(DBG_PRINT, "(GRADING2A 2.b) (precondition) KASSERT(NULL != name");
	KASSERT(NULL != res_vnode);
	dbg(DBG_PRINT, "(GRADING2A 2.b) (precondition) KASSERT(NULL != res_vnode)");
		/*return errno*/
		if (len == 0) {
			dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) pathname length is zero\n");
			return -EINVAL;
		}
		/*set up the directory*/
		if (pathname[0] == '/') {
			dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) set dir=root\n");
			dir = vfs_root_vn;
			++pathname;
			len=strlen(pathname);
			*res_vnode = dir;
		}else{
			if (base==NULL) {
				dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) set dir=p_cwd\n");
				dir = curproc->p_cwd;
			}else{
				dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) set dir=base\n");
				dir = base;
			}
		}
		KASSERT(NULL != dir);
		dbg(DBG_PRINT, "(GRADING2A 2.b) (middle) KASSERT(NULL != dir)\n");
		vget(dir->vn_fs, dir->vn_vno);
		/*revolve each piece of the pathname*/
		strncpy(buffer, pathname, len);
		buffer[len] = '\0';
		start = buffer;
		while ((slash = strchr(start, '/')) != NULL) {
			*slash = '\0';
		/*if (strlen(start) > NAME_LEN) {
			  vput(dir);
			  dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) pathname too long\n");
			  return -ENAMETOOLONG;
			}*/
			
		if((errno = lookup(dir, start, strlen(start), res_vnode))<0){
				vput(dir);
				dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) error in lookup\n");
				return errno;
			}
			vput(dir);
			/*vget() is called inside the lookup function, so call vput()*/
	
			dir = *res_vnode;
			start = slash + 1;
		}

		/*This was giving an error*/
		/*strncpy(((char *)(*name)),start,*namelen);*/
		/*lookup(dir, start, strlen(start), res_vnode);*/
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(dir_namev) set up res_vnode\n");
		*namelen = strlen(start);
		*name=pathname+(start-buffer);
		*res_vnode = dir;
		return 0;
}

/* This returns in res_vnode the vnode requested by the other parameters.
 * It makes use of dir_namev and lookup to find the specified vnode (if it
 * exists).  flag is right out of the parameters to open(2); see
 * <weenix/fnctl.h>.  If the O_CREAT flag is specified, and the file does
 * not exist call create() in the parent directory vnode.
 *
 * Note: Increments vnode refcount on *res_vnode.
 */
int
open_namev(const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base)
{
	const char *name;
	vnode_t *dir;
	size_t namelen;
	int errno;
	int no_files;
	/*The process already has the maximum number of files open.*/
	/*	if(get_empty_fd(curproc)<0){
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(open_namev) get_empty_fd(curproc)<0,maximum num of file open,return -EMFILE\n");
		return -EMFILE;
		}*/
   /*NOT_YET_IMPLEMENTED("VFS: open_namev");*/

	errno=dir_namev(pathname, &namelen, &name, base, &dir);
	if(errno<0)
	  {
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(open_namev) errno=dir_namev(pathname, &namelen, &name, base, &dir)<0,error when dir_namev(),return errno\n");
	    return errno;
	  }
	if ((errno = lookup(dir, name, namelen, res_vnode))==-ENOENT){
		dbg(DBG_PRINT,"(GRADING2B)(namev.c)(open_namev) (errno = lookup(dir, name, namelen, res_vnode))==-ENOENT,file not exist\n");
		/*If the O_CREAT flag is specified, and the file does
		 * not exist call create() in the parent directory vnode.*/
		if((flag & O_CREAT)==O_CREAT){
			KASSERT(NULL != (dir)->vn_ops->create);
			dbg(DBG_PRINT, "(GRADING2A 2.c) (middle) if((errno = lookup(dir, name, namelen, res_vnode))==-ENOENT && (flag & O_CREAT)==O_CREAT),KASSERT(NULL != dir->vn_ops->create);\n");
			/*create the node*/
			/*errno = (*res_vnode)->vn_ops->create(dir, name, namelen, res_vnode);*/
			errno = dir->vn_ops->create(dir, name, namelen, res_vnode);
			/*Not sure, pathname refers to a device special file and no corresponding deviceexists.
			if((*res_vnode)->vn_mode==S_IFCHR||(*res_vnode)->vn_devid==NULL){
				return -ENXIO;
			}*/
		}
	}
	vput(dir);
	return errno;

}

#ifdef __GETCWD__
/* Finds the name of 'entry' in the directory 'dir'. The name is writen
 * to the given buffer. On success 0 is returned. If 'dir' does not
 * contain 'entry' then -ENOENT is returned. If the given buffer cannot
 * hold the result then it is filled with as many characters as possible
 * and a null terminator, -ERANGE is returned.
 *
 * Files can be uniquely identified within a file system by their
 * inode numbers. */
int
lookup_name(vnode_t *dir, vnode_t *entry, char *buf, size_t size)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_name");
        return -ENOENT;
}


/* Used to find the absolute path of the directory 'dir'. Since
 * directories cannot have more than one link there is always
 * a unique solution. The path is writen to the given buffer.
 * On success 0 is returned. On error this function returns a
 * negative error code. See the man page for getcwd(3) for
 * possible errors. Even if an error code is returned the buffer
 * will be filled with a valid string which has some partial
 * information about the wanted path. */
ssize_t
lookup_dirpath(vnode_t *dir, char *buf, size_t osize)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_dirpath");

        return -ENOENT;
}
#endif /* __GETCWD__ */
