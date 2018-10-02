/*
 * libc.c 
 */

#include <libc.h>

#include <errno.h>

#include <types.h>

int errno;

//char *strerror[]={"fksfsdl", "fakfa
void itoa(int a, char *b);

void perror(const char * s) {
//write first s, then : ' '
    int count = 0;
    int colon = 0;
    if(s != NULL && s[0] != '\0') {
        while(s[count++] != '\0');
        write(1, s, count-1);
        colon = 1;
    }
    char error [10];
    itoa(errno, error);
    write(1, error, 1);
    
    /*
    switch(errno) {
    case ENOSYS:
      count = 25;
      if(colon) write(1, ": Function not implemented\n", count+2);
      else write(1, "Function not implemented\n", count);
      break;
    case EACCES:
      count = 18;
      if(colon) write(1, ": Permission denied\n", count+2);
      else write(1, "Permission denied\n", count);
      break;
    case EBADF:
      count = 16;
      if(colon) write(1, ": Bad file number\n", count+2);
      else write(1, "Bad file number\n", count);
      break;
    case EFAULT:
      count = 12;
      if(colon) write(1, ": Bad address\n", count+2);
      else write(1, "Bad address\n", count);
      break;
    case EINVAL:
      count = 17;
      if(colon) write(1, ": Invalid argument\n", count+2);
      else write(1, "Invalid argument\n", count);
    }*/
}

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

