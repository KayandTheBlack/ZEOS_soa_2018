/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}


//write syscall
int sys_write(int fd, char* buffer, int size) { //doesn't get here
	// check user params
	int err = check_fd(fd, ESCRIPTURA);
	if(err) return err;
	if(buffer == NULL) return -EFAULT;
	if(size < 0) return -EINVAL;
	char sysbuffer[4096];
	int written = 0; 
	int tmpsize=4096;
	while(size > 0) {
		if(size < tmpsize) tmpsize = size;
		// copy data from/to user space
		copy_from_user(buffer, sysbuffer, tmpsize); //check err
		// implement requested service
		sys_write_console(sysbuffer, tmpsize);

		buffer = buffer+tmpsize*sizeof(char);
		size -= tmpsize;
		written += tmpsize;
	}

	// return result
	return written;
}
