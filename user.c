#include <libc.h>
#include <stats.h>

#define WORKMODEL 1
char buff[24];

int pid;

void writeStats(int pid, int wl){
    struct stats aux;
    char buff[100];
    int max=0;
    if(get_stats(pid, &aux) == -1) perror();
    else {
        itoa(wl, buff);
        max = strlen(buff);
        buff[max] = ' ';
        itoa(aux.user_ticks, &(buff[max+1]));
        max = strlen(buff);
        buff[max] = ' ';
        itoa(aux.blocked_ticks, &(buff[max+1]));
        max = strlen(buff);
        buff[max] = ' ';
        itoa(aux.ready_ticks, &(buff[max+1]));
        max = strlen(buff);
        buff[max] = ' ';
        itoa(aux.system_ticks, &(buff[max+1]));
        max = strlen(buff);
        buff[max] = '\n';
        write(1, buff, max+1);
    }
}

void heavyIO() {
    
    writeStats(getpid(), 1);
}

void heavyCPU() {
    
    writeStats(getpid(), 2);
}

void mixed() {
    
    writeStats(getpid(), 3);
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  int x;
  void(* workload1)(void);
  void(* workload2)(void);
  void(* workload3)(void);
  if(WORKMODEL == 1) {
    workload1 = heavyIO;
    workload2 = heavyIO;
    workload3 = heavyIO;
  } else if(WORKMODEL == 2) {
    workload1 = heavyCPU;
    workload2 = heavyCPU;
    workload3 = heavyCPU;
  } else if(WORKMODEL == 3) {
    workload1 = mixed;
    workload2 = mixed;
    workload3 = mixed;
  } else if(WORKMODEL == 4) {
    workload1 = heavyCPU;
    workload2 = heavyIO;
    workload3 = mixed;
  }
  x = fork();
  if(!x) x = fork();
  else workload1();
  if(!x) x = fork();
  else workload2();
  if(!x) {
	workload3();
	writeStats(0, 0);
  }
  exit();
  return 0;
}
