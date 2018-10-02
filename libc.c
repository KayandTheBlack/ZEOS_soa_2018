/*
 * libc.c 
 */

#include <libc.h>

#include <errno.h>

#include <types.h>

int errno;

char *strerror[]={
"DEFAULT ERROR\n",
"Operation not permitted\n", //1
"No such file or directory\n",
"No such process\n",
"Interrupted system call\n",
"I/O error\n", //5
"No such device or address\n",
"Argument list too long\n",
"Exec format error\n",
"Bad file number\n",
"No child processes\n", //10
"Try again\n",
"Out of memory\n",
"Permission denied\n",
"Bad address\n",
"Block device required\n", //15
"Device or resource busy\n",
"File exists\n",
"Cross-device link\n",
"No such device\n",
"Not a directory\n", //20
"Is a directory\n",
"Invalid argument\n",
"File table overflow\n",
"Too many open files\n",
"Not a typewriter\n", //25
"Text file busy\n",
"File too large\n",
"No space left on device\n",
"Illegal seek\n",
"Read-only file system\n", //30
"Too many links\n",
"Broken pipe\n",
"Math argument out of domain of func\n",
"Math result not representable\n",
"Resource deadlock would occur\n", //35
"File name too long\n",
"No record locks available\n",
"Function not implemented\n",
"Directory not empty\n",
"Too many symbolic links encountered\n", //40
"Operation would block\n", //impossible
"No message of desired type\n" //42
};
//void itoa(int a, char *b);



void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}


void perror(const char * s) {
//write first s, then : ' '
    int count = 0;
    int colon = 0;
    char x[2] = ": ";
    if(s != NULL && s[0] != '\0') {
        while(s[count++] != '\0');
        write(1, s, count-1);
        colon = 1;
    }
    /*char error [10];
    itoa(errno, error);
    write(1, error, 1);*/
    if(colon) write(1, x, 2);
    int l = strlen(strerror[errno]);
    write(1, strerror[errno], l);
}

