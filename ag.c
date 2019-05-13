#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int isZero;

int main()
{
   int clients = open("/tmp/servers", O_WRONLY);
   if(clients < 0) {perror("open"); return -1;}

   pid_t send = 0;
   isZero = write(clients,&send,sizeof(pid_t));
   if(isZero < 0) perror("write");

   isZero = close(clients);
   if(isZero < 0) perror("close");

   return 0;
}
