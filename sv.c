#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "extra.h"

int limpaEspacos (char texto[]) {
    int r=0, i;
    for(i = 0; texto[i] != '\0'; i++)
    {
        if(texto[i] == ' ' && texto[i] == texto[i+1]);
        else
        {
            texto[r++] = texto[i];
        }
    }
    texto[r] = '\0';
    //... completar
    return r;
}


int main()
{
  int artigos = open("artigos",O_RDONLY);
  if(artigos < 0) return -1;
  long int artigos_size = lseek(artigos,0,SEEK_END);
  artigos_size = artigos_size>>3;

  int stock = open("stock",O_RDWR | O_CREAT, 0666);
  long int stock_size = lseek(stock,0,SEEK_END);
  stock_size = stock_size>>2;
  int left = artigos_size - stock_size;
  int zero = 0;
  printf("%d\n", left);
  while(left) {
    write(stock,&zero,sizeof(int));
    left--;
  }

  int client_to_server;
  int server_to_client;
  int clients;

  char *my = "/tmp/servers";
  mkfifo(my, 0666);
  clients = open(my,O_RDONLY | O_CREAT, 0666);


  int i = 0, check;
  char buf[MAX];
  char buf1[MAX];
  int length;
  char c;

  while(1) {
    check = 0;
    check = read(clients, &i, sizeof(int));

    if(check) {
      sprintf(buf,"/tmp/client_to_server_fifo%d",i);
      sprintf(buf1,"/tmp/server_to_client_fifo%d",i);
      mkfifo(buf, 0666);
      mkfifo(buf1, 0666);


      if(!fork()) {
        server_to_client = open(buf1, O_WRONLY);
        client_to_server = open(buf, O_RDONLY);

        int length, j, check = 1;
        char **args;
        while (1)
        {
          length = 0;
          i = 0;
          read(client_to_server,&length,sizeof(int));
          read(client_to_server, buf, length);
          buf[length] = '\0';
          if(buf[0] != ' ') {
            limpaEspacos(buf);
            if(spacecount(buf) > 2) check = 0;
            if(check) {
              args = retArg(buf);
            }

            if(length && check) {
              if(args[0] != NULL){
                if(strlen(args[0]) == FIX_SIZE && args[2] == NULL) {
                  for (j = 0; args[0][j] != '\0'; j++) {
                    if(!(isdigit(args[0][j]))) {
                      i = 13;
                      write(server_to_client,&i,sizeof(int));
                      write(server_to_client,"Invalid Code\n",i);
                      check = 0;
                      break;
                    }
                  }
                } else {
                  i = 13;
                  write(server_to_client,&i,sizeof(int));
                  write(server_to_client,"Invalid Code\n",i);
                  check = 0;
                }
              } else check = 0;

              if(check) {
                int code = atoi(args[0]);
                if(args[0] != NULL) {
                  if(args[1] == NULL) {
                    lseek(stock,sizeof(int)*code,SEEK_SET);
                    read(stock,&i,sizeof(int));
                    lseek(artigos,(sizeof(int) + sizeof(float))*code + sizeof(int),SEEK_SET);
                    float price;
                    read(artigos,&price,sizeof(float));
                    sprintf(buf,"stock:%d price:%lf\n", i, price);
                    length = strlen(buf);
                    write(server_to_client,&length,sizeof(int));
                    write(server_to_client,buf,length);
                  } else if(args[2] == NULL) {
                    int signal = 1;
                    if(args[1][0] == '-') {
                      signal = -1;
                      args[1] = args[1] + 1;
                    }
                    j = 0;
                    while(args[1][j] != '\0'){
                      if(!isdigit(args[1][j++])) {
                        i = 14;
                        write(server_to_client,&i,sizeof(int));
                        write(server_to_client,"Invalid Input\n",i);
                        check = 0;
                        break;
                      }
                    }
                    if(check) {
                      int change = atoi(args[1]) * signal;
                      lseek(stock,sizeof(int)*code,SEEK_SET);
                      read(stock,&i,sizeof(int));
                      if((i += change) < 0) i = 0;

                      lseek(stock,-sizeof(int),SEEK_CUR);
                      write(stock,&i,sizeof(int));
                      sprintf(buf,"stock:%d\n", i);
                      length = strlen(buf);
                      write(server_to_client,&length,sizeof(int));
                      write(server_to_client,buf,length);
                    }
                    if(signal == -1) args[1] -= 1;
                  }
                } else {
                  i = 14;
                  write(server_to_client,&i,sizeof(int));
                  write(server_to_client,"Invalid Input\n",i);
                }
              } else check = 1;
            } else {
              if(length) {
                i = 14;
                write(server_to_client,&i,sizeof(int));
                write(server_to_client,"Invalid Input\n",i);
              } else break;
            }
             /* clean buf from any data */
             memset(buf, 0, sizeof(buf));
             if(check) cRetArg(args);
             else check = 1;
          } else {
            i = 14;
            write(server_to_client,&i,sizeof(int));
            write(server_to_client,"Invalid Input\n",i);
          }
        }

        close(client_to_server);
        close(server_to_client);

        unlink(buf);
        unlink(buf1);

        exit(1);
      }
    }
  }

  close(clients);
  unlink(my);
  close(stock);
  close(artigos);

   return 0;
}
