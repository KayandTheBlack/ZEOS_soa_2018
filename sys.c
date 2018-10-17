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
    struct list_head auxiliar = (*entr);
    

    // b)
    t_s = list_head_to_task_struct(entr);
    copy_data(current(), t_s, 4*KERNEL_STACK_SIZE);
    
    // Have to init signals table, etc
    // c)
    allocate_DIR(t_s);
    
    // d)
    
    int pag;
    int nframes[NUM_PAG_DATA];
    
    page_table_entry * process_PT =  get_PT(current());
    page_table_entry * process_PT_new =  get_PT(t_s);
    page_table_entry * dir_current = get_DIR(current());
    
    for(pag = 0; pag < NUM_PAG_DATA; pag++){
        if(process_PT[pag].bits.present) {
            nframes[pag] = alloc_frame();
            if (nframes[pag] != -1){
                process_PT_new[pag].entry = process_PT[pag].entry;
            } else {
                while (pag != -1){
                    if (nframes[pag] != -1)
                        free_frame((unsigned int) nframes[pag]);
                    pag--;
                }
            }
        }
    }
    //Kernel copy
    for (pag = 0; pag < NUM_PAG_KERNEL; pag++){
        process_PT_new[pag].entry = process_PT[pag].entry;
    }
    //Code copy
    for (pag = PAG_LOG_INIT_CODE; pag < PAG_LOG_INIT_CODE+NUM_PAG_CODE; pag++){
        process_PT_new[pag].entry = process_PT[pag].entry;
    }
    //Data link
    for (pag = PAG_LOG_INIT_DATA; pag < NUM_PAG_DATA+PAG_LOG_INIT_DATA; pag++){
        set_ss_pag(process_PT_new,pag,nframes[pag-PAG_LOG_INIT_DATA]);
    }
    //Temporal pages acquisition
    int best=-1, curr=-1, bestcount=0;
    for(int i=0; i<TOTAL_PAGES; i++){
        if(! process_PT[pag].bits.present) {
            if(curr == -1)
                curr = pag;
            if((pag-curr+1) > bestcount) {
                best = curr;
                bestcount = pag-curr;
            }
        }
    }
    //DATA copy
    for(pag = PAG_LOG_INIT_DATA, curr=0; pag < NUM_PAG_DATA+PAG_LOG_INIT_DATA; pag++) {
        if(curr == bestcount) {
            curr = 0;
            set_cr3(dir_current);
        }
        set_ss_pag(process_PT, curr+best, nframes[pag-PAG_LOG_INIT_DATA]);
        copy_data((void*) (pag << 12), (void*)((curr+best)<< 12), 4*KERNEL_STACK_SIZE);
        del_ss_pag(process_PT, curr+best); //will need flush above
    }
    set_cr3(dir_current);
    
    // removing the new t_s entry from freequeue here, since no more errors.
    (*entr) = auxiliar;
    list_del(entr);
    
    //Assign PID
    int possiblePID = current()->PID + 1; //not really random, but different from task index.
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
    t_s->KERNEL_EBP = (unsigned long* ) (((long) getebp() & 0x00000fff) | (long)t_s);
    *(t_s->KERNEL_EBP) = (long) ret_from_fork;
    t_s->KERNEL_EBP =  (t_s->KERNEL_EBP)-1; // conversion is good?
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
    int pag;
    page_table_entry * process_PT =  get_PT(current());
    for(pag = 0; pag < NUM_PAG_DATA; pag++) {
        if(process_PT[pag+PAG_LOG_INIT_DATA].bits.present) 
            free_frame(get_frame(process_PT, pag+PAG_LOG_INIT_DATA));
    }
    update_process_state_rr(current(), &freequeue);
    sched_next_rr();
    
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
