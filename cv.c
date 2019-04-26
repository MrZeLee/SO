#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "ma.h"

int main()
{
   pid_t pid = getpid();
   printf("%d\n", pid);
   int clients = open("/tmp/servers", O_WRONLY);

   int rets = open("/tmp/clients", O_RDONLY);
   read(rets,&pid,sizeof(int));
   printf("%d\n", pid);

   int client_to_server;
   char myfifo[50];
   sprintf(myfifo,"/tmp/client_to_server_fifo%d",pid);

   int server_to_client;
   char myfifo2[50];
   sprintf(myfifo2,"/tmp/server_to_client_fifo%d",pid);

   char str[MAX];
   int length = 1, i = 1;
   char c;

   write(clients,&pid,sizeof(pid_t));

   close(clients);

   client_to_server = open(myfifo, O_WRONLY);
   printf("%d\n", client_to_server);
   server_to_client = open(myfifo2, O_RDONLY);


   while(1) {
     i = 0;
     c = '\0';
     while((length = read(0,&c,1)) && c != '\n') {
       str[i++] = c;
     }
     printf("%d\n", length);
     printf("%d\n", i);
     str[i] = '\n';
     if(length) {
       if(i) {
         write(client_to_server, str, i+1);
         printf("test\n" );
         read(server_to_client,str,i);
       }
     } else {
       break;
     }
   }

   close(client_to_server);
   close(server_to_client);

   /* remove the FIFO */

   return 0;
}
