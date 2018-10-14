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

#include <system.h> //UNSURE

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
int ret_from_fork() {
    return 0;
}
int sys_fork()
{
    struct list_head *entr;
    struct task_struct *t_s;
    //union task_union *t_u;
    // a) get free task struct
    if (list_empty(&freequeue))
        return -EAGAIN;
    entr = list_first(&freequeue);
    
    
    // b)
    t_s = list_head_to_task_struct(entr);
    copy_data(current(), t_s, 4*KERNEL_STACK_SIZE);
    
    // Have to init signals table, etc
    // c)
    allocate_DIR(t_s);
    
    // d)
    int pag; 
    int best=-1, bestcount=-1; // bescount counts 1 as 0...
    int curr=-1;
    int nframes[TOTAL_PAGES]; //TODO ask: too much mem?
    
    page_table_entry * process_PT =  get_PT(current());
    page_table_entry * process_PT_new =  get_PT(t_s);
    for (pag = 0; pag < TOTAL_PAGES; pag++){
        if (process_PT[pag].bits.present){
            if (process_PT[pag].bits.rw && process_PT[pag].bits.user){ //USER DATA/STACK
                nframes[pag] = alloc_frame();
                if (nframes[pag] != -1){
                    process_PT_new[pag].bits.present = 1;
                    process_PT_new[pag].bits.rw = 1;
                    process_PT_new[pag].bits.user = 1;
                    process_PT_new[pag].bits.pbase_addr = nframes[pag];
                }else{
                    while (pag != -1){
                        if (nframes[pag] != -1)
                            free_frame((unsigned int) nframes[pag]);
                        pag--;
                    }
                    // No need to de-allocate dir?? TODO
                    return -ENOMEM;
                }
            }else{
                nframes[pag] = -1;
                process_PT_new[pag].bits.present = 1;
                process_PT_new[pag].bits.rw = process_PT[pag].bits.rw;
                process_PT_new[pag].bits.user = process_PT[pag].bits.user;
                process_PT_new[pag].bits.pbase_addr = process_PT[pag].bits.pbase_addr;
            }
            curr = -1;
        }else{
            if(curr == -1)
		        curr = pag;
	        if(pag-curr > bestcount) {
		        best = curr;
		        bestcount = pag-curr;
	        }
        }
    }
    
    //Changing logical space + copying pages
    curr = 0;
    for (pag = 0; pag < TOTAL_PAGES; pag++){
        if (nframes[pag] != -1) {
            if(curr == bestcount + 1) {
                for(curr = 0; curr <= bestcount; curr++) {
                    del_ss_pag(process_PT, curr+best);
                }
                set_cr3(process_PT);
                curr = 0;
            }
            set_ss_pag(process_PT, curr+best, nframes[pag]);
            copy_data((void*)process_PT[pag].bits.pbase_addr, (void*)process_PT[curr+best].bits.pbase_addr, 4*KERNEL_STACK_SIZE); //!!! TODO check
            curr++;
        }
    }
    for(; curr >= 0; curr--) {
        del_ss_pag(process_PT, curr+best);
    }
    set_cr3(process_PT);
    
    // removing the new t_s entry from freequeue here, since no more errors.
    list_del(entr);
    
    //Assign PID
    int possiblePID = current()->PID + 142; //not really random, but different from task index.
    int i; int valid = 0;
    while(!valid) {
        possiblePID ++;
        valid = 1;
        for(i=0; i<NR_TASKS; i++) {
            if(possiblePID == task[i].task.PID)
               valid = 0;
        }
    }
    t_s->PID = possiblePID;
    //Change task_struct and stack
     // quantum? or what? Stats?
     
    //Prepare child stack with content that emulates result of call to task_switch. (create and use ret_from_fork). (like idle)
    //IDEA change KERNEL_EBP like in idle to change the return directions.
    *(t_s->KERNEL_EBP) = ret_from_fork;
    t_s->KERNEL_EBP = (t_s->KERNEL_EBP)-1; // conversion is good?
    //insert in ready_queue
    list_add_tail(entr, &readyqueue);
    //return pid of child
    return possiblePID;
    
    /*
    t_u = (union task_union*)t_s;
    tss.esp0 = (int) &(t_u->stack[KERNEL_STACK_SIZE]);
    writeMSR(0x175, (void*) tss.esp0);
    set_cr3(t_s->dir_pages_baseAddr);
    */
    // creates the child process
    
}

void sys_exit()
{  
}


//write syscall
int sys_write(int fd, char* buffer, int size) {
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

//gettime syscall
int sys_gettime() {
	// check user params
	return zeos_ticks;
}
