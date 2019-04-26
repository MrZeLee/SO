#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "ma.h"

int main()
{
  int client_to_server;
  int server_to_client;
  int clients;
  int rets;

  char *my = "/tmp/servers";
  mkfifo(my, 0666);
  clients = open(my,O_RDONLY | O_CREAT, 0666);

  char *ret = "/tmp/clients";
  mkfifo(ret, 0666);
  rets = open(ret, O_WRONLY | O_CREAT, 0666);

  int i = 0, check;
  char buf[50];
  char buf1[50];
  int length;
  char c;

  while(1) {
    check = 0;
    check = read(clients, &i, sizeof(int));
    printf("%d\n", check);

    if(check) {
      printf("%d\n", i);

      if(!fork()) {
        sprintf(buf,"/tmp/client_to_server_fifo%d",i);
        mkfifo(buf, 0666);
        sprintf(buf1,"/tmp/server_to_client_fifo%d",i);
        mkfifo(buf1, 0666);

        client_to_server = open(buf, O_RDONLY | O_CREAT, 0666);
        server_to_client = open(buf1, O_WRONLY | O_CREAT, 0666);

        write(rets,&i,sizeof(int));



        while (1)
        {
          i = 0;
          while((length = read(client_to_server, &c, 1)) && c != '\n') {
            buf[i++] = c;
          }
          printf("%d\n", length);
          if(length) {
            printf("%s\n", buf);
            write(server_to_client,buf,i);
          } else {
            break;
          }
           /* clean buf from any data */
           memset(buf, 0, sizeof(buf));
        }

        close(client_to_server);
        close(server_to_client);

        unlink(buf);
        unlink(buf1);

        exit(1);
      }
      wait(NULL);
    }
  }

  close(rets);
  close(clients);
  unlink(my);
  unlink(ret);

   return 0;
}
