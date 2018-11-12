#include <libc.h>
#include <stats.h>

#define WORKMODEL 2
#define SCHED 1
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

int badfib(int n){
    if (n == 0) return 0;
    if (n == 1) return 1;
    return badfib(n-1)+badfib(n-2);
}

void heavyIO() {
    int i;
    char buff[1];
    for(i=0;i<30;i++) {
        read(0,buff,25+i%50);
    }
    writeStats(getpid(), 1);
}

void heavyCPU() {
    int i;
    for(i=0;i<100;i++) {
        badfib(i%30);
    }
    writeStats(getpid(), 2);
}

void mixed() {
    int i;
    char buff[1];
    for(i=0;i<50;i++) {
        badfib(i%30);
        if(i%3==0)
            read(0,buff,25+i%50);
    }
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
  if(!SCHED) set_sched_policy(0);
  char buff = '\n';
  write(1, &buff, 1);
  x = fork();
  if(!x) {
    x = fork();
    if(!x) {
        workload3();
        writeStats(0, 0);
    } else workload2();
  } else workload1();
  
  exit();
  return 0;
}
