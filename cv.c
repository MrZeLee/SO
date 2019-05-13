#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "ma.h"

int isZero;

int main()
{
   pid_t pid = getpid();
   printf("%d\n", pid);
   int clients = open("/tmp/servers", O_WRONLY);
   if(clients < 0) {perror("open"); return -1;}

   int client_to_server;
   char myfifo[50];
   sprintf(myfifo,"/tmp/client_to_server_fifo%d",pid);

   int server_to_client;
   char myfifo2[50];
   sprintf(myfifo2,"/tmp/server_to_client_fifo%d",pid);

   char str[MAX];
   int length = 1, i = 1;
   char c;

   isZero = write(clients,&pid,sizeof(pid_t));
   if(isZero < 0) perror("write");

   isZero = close(clients);
   if(isZero < 0) perror("close");
   sleep(5);
   server_to_client = open(myfifo2, O_RDONLY);
   client_to_server = open(myfifo, O_WRONLY);

   while(1) {
     i = 0;
     c = '\0';
     while((length = read(0,&c,1)) > 0 && c != '\n') {
       str[i++] = c;
     }
     if(length < 0) perror("read");
     if(length) {
       if(i) {
         isZero = write(client_to_server, &i, sizeof(int));
         if(isZero < 0) perror("write");
         isZero = write(client_to_server, str, i);
         if(isZero < 0) perror("write");

         printf("before receiving\n");
         while ((length = read(server_to_client,&i,sizeof(int))) == 0);
         printf("after receiving -> %d\n", length);
         if(length < 0) perror("read");
         printf("before receiving1\n");
         isZero = read(server_to_client,str,i);
         str[i] = '\0';
         printf("after receiving1 -> %d\n", isZero);
         if(isZero < 0) perror("read");
         if(!strcmp("exit",str)) break;

         printf("%s<-\n", str);
         isZero = write(1,str,i);
         if(isZero < 0) perror("write");

       }
     } else {
       printf("%d\n", length);
       break;
     }
   }

   close(client_to_server);
   close(server_to_client);

   /* remove the FIFO */

   return 0;
}
