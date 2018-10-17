/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>



union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return (struct task_struct*)((unsigned int)l&0xfffff000);
}

extern struct list_head blocked;

struct list_head freequeue;

struct list_head readyqueue;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void) {
    struct list_head *entr;
    struct task_struct *t_s;
    union task_union *t_u;
    entr = list_first(&freequeue);
    list_del(entr);
    t_s = list_head_to_task_struct(entr);
    t_u = (union task_union*)t_s;
    t_s->PID=0;
    allocate_DIR(t_s);
    // Init execution context here
    t_u->stack[KERNEL_STACK_SIZE-1] = (long) cpu_idle;
    t_u->stack[KERNEL_STACK_SIZE-2] = (long) 0;
    t_s->KERNEL_EBP = &(t_u->stack[KERNEL_STACK_SIZE-2]);
    idle_task = t_s;    
}

void init_task1(void)
{
    struct list_head *entr;
    struct task_struct *t_s;
    union task_union *t_u;
    entr = list_first(&freequeue);
    list_del(entr);
    t_s = list_head_to_task_struct(entr);
    t_s->PID=1;
    
    t_s->QUANTUM = MAX_QUANTUM;
    
    quantum = MAX_QUANTUM;
    allocate_DIR(t_s);
    set_user_pages(t_s);
    t_u = (union task_union*)t_s;
    tss.esp0 = (int) &(t_u->stack[KERNEL_STACK_SIZE]);
    writeMSR(0x175, (void*) tss.esp0);
    set_cr3(t_s->dir_pages_baseAddr);
}


void init_sched(){
    int i;
    INIT_LIST_HEAD(&freequeue);
    INIT_LIST_HEAD(&readyqueue);
    for (i=0; i<NR_TASKS; i++){
        list_add_tail(&(task[i].task.list),&freequeue);
    }
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}



void inner_task_switch(union task_union*t) {
    tss.esp0 = (int)&(t->stack[KERNEL_STACK_SIZE]);
    writeMSR(0x175, (void*) tss.esp0);
    set_cr3(t->task.dir_pages_baseAddr);
    current()->KERNEL_EBP = getebp()+0x12;//TODO: mention in class, this function is kinda retarded and stores ebx edi esi.
    setesp(t->task.KERNEL_EBP);
}



void update_sched_data_rr(){
    //update quantum
    if(quantum != 0) quantum--;
}
void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
    //delete from current queue, insert in dest if not null
    if(t->list.prev !=  0) //node in some list
        list_del(&(t->list));
    if(dest != NULL) list_add_tail(&(t->list), dest);
}
int needs_sched_rr(){
    // if quantum exceeded, return 1, else 0
    /*if(quantum == 0){
        if(list_empty(&readyqueue)) return 0;
        return 1;
    }
    return 0;*/
    return ((quantum == 0) && (!(list_empty(&readyqueue))));
}
void sched_next_rr(){
    // executed AFTER update_process_state_rr, selects next process to execute. Extracts it from ready_queue and puts it to calls task_switch(?) Implements ROUND ROBIN
    struct task_struct * new;
    struct list_head *entr;
    if(list_empty(&readyqueue)) {
        new = idle_task;
        quantum = 0; //we want to exit ASAP
    } else {
        entr = list_first(&readyqueue);
        list_del(entr);
        new = list_head_to_task_struct(entr);
        quantum = new->QUANTUM;
    }
    task_switch((union task_union*) new);
}

void schedule() {
    update_sched_data_rr();
    int x = needs_sched_rr();
    if(x){ // how do we know if current is blocked to put idle? Let's assume no idle yet
        //context switch
        if(current() != idle_task) update_process_state_rr(current(), &readyqueue);
        sched_next_rr();
    }
}
int get_quantum(struct task_struct *t) {
    return t->QUANTUM;
}
void set_quantum(struct task_struct *t, int new_quantum){
    t->QUANTUM = new_quantum;
}

