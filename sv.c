#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "extra.h"

int isZero;

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
  printf("%d\n", sizeof(double));
  int artigos;
  int stock = open("stock",O_RDWR | O_CREAT, 0666);
  if(stock < 0){ perror("open"); return -1;}

  int vendas = open("vendas", O_RDONLY | O_CREAT, 0666);
  if(vendas < 0){ perror("open"); return -1;}
  isZero = close(vendas);
  if(isZero < 0){ perror("close"); return -1;}

  int client_to_server;
  int server_to_client;
  int clients;
  int exits, in;

  mkfifo("/tmp/exits", 0666);

  char *my = "/tmp/servers";
  mkfifo(my, 0666);
  clients = open(my,O_RDONLY | O_CREAT, 0666);
  if(clients < 0){ perror("open"); return -1;}

  int i = 0, check;
  char buf[MAX], buf1[MAX], buf2[50];
  int pid;
  int pidClients[2];
  isZero = pipe(pidClients);
  if(isZero < 0) perror("pipe");
  int once = 1;

  while(1) {

    if(once){
      if(!fork()) {
        isZero = close(pidClients[1]);
        if(isZero < 0) perror("close");
        while(1){
          artigos = open("artigos",O_RDONLY);
          if(artigos < 0){ perror("open"); return -1;}

          long int artigos_size = lseek(artigos,0,SEEK_END);
          artigos_size = artigos_size>>3;

          long int stock_size = lseek(stock,0,SEEK_END);
          stock_size = stock_size>>2;
          int left = artigos_size - stock_size;
          int zero = 0;
          while(left) {
            isZero = write(stock,&zero,sizeof(int));
            if(isZero < 0) perror("write");
            left--;
          }
          isZero = close(artigos);
          if(isZero < 0) perror("close");
          printf("brokehere\n");

          check = 0;
          i = 0;
              i = read(pidClients[0],&check,sizeof(int));
              if(i < 0) perror("read");
              if(check && i) {
                sprintf(buf,"/tmp/in%d",check);
                in = open(buf, O_WRONLY);
                check = (int) artigos_size;
                printf("inall -> %d\n", in);
                i = write(in,&check,sizeof(int));
                if(i < 0) perror("write");
                isZero = close(in);
                if(isZero < 0) perror("close");
                printf("i -> %d\n", i);

                exits = open("/tmp/exits", O_RDONLY);
                if(exits < 0) perror("open");
                while((i = read(exits,&check,sizeof(int))) == 0);
                isZero = close(exits);
                if(isZero < 0) perror("close");
                printf("i1 -> %d\n", i);
                //if(check == 999)
                printf("codigoall -> %d\n", check);
            }
        }
        exit(1);
      }
      once = 0;
      isZero = close(pidClients[0]);
      if(isZero < 0) perror("close");
      if(!fork()) {
        check = 0;
        while(1) {
          isZero = write(pidClients[1],&check,sizeof(int));
          if(isZero < 0) perror("write");
          sleep(10);
        }
        exit(1);
      }

    }

    check = 0;
    pid = 1;
    check = read(clients, &pid, sizeof(pid_t));
    if(check < 0) perror("read");

    if(pid == 0){
      check = 0;
      if(!fork()) {
        char newFile[50];
        time_t current_time;
        time(&current_time);
        struct tm *time1 = localtime(&current_time);
        sprintf(newFile,"%d-%d-%dT%d:%d:%d",time1->tm_year + 1900, time1->tm_mon + 1, time1->tm_mday, time1->tm_hour, time1->tm_min, time1->tm_sec);

        mkfifo("/tmp/in1", 0666);
        pid = 1;
        check = write(pidClients[1],&pid,sizeof(int));
        if(check < 0) perror("write");
        printf("pid -> %d\n", pid);
        printf("check -> %d\n", check);
        check = 1;
        sprintf(buf1,"/tmp/in%d",pid);
        printf("%s\n", buf1);
        printf("before opening\n");
        in = open(buf1, O_RDONLY);
        printf("after opening -> %d\n", in);
        if(in < 0) perror("open");
        printf("in -> %d\n", in);
        i = 0;
        isZero = read(in,&i,sizeof(int));
        if(isZero < 0) perror("read");
        printf("readed -> %d\n", i);
        isZero = close(in);
        if(isZero < 0) perror("close");

        vendas = open("vendas",O_RDONLY);
        stock = open("stock",O_RDONLY);
        int quantidade = open("quantidade", O_RDWR | O_CREAT, 0666);

        long int stock_size = lseek(stock,0,SEEK_END);
        stock_size = stock_size>>2;

        long int quantidade_size = lseek(quantidade,0,SEEK_END);
        quantidade_size = quantidade_size>>2;
        int left = stock_size - quantidade_size;
        int zero = 0;
        printf("Left -> %d\n", left);
        while(left) {
          isZero = write(quantidade,&zero,sizeof(int));
          if(isZero < 0) perror("write");
          left--;
        }

        int precos = open("precos", O_RDWR | O_CREAT, 0666);
        if(precos < 0) perror("open");

        long int precos_size = lseek(precos,0,SEEK_END);
        precos_size = precos_size/(sizeof(double));
        left = stock_size - precos_size;
        printf("Left -> %d\n", left);
        double zero1 = 0;
        while(left) {
          isZero = write(precos,&zero1,sizeof(double));
          if(isZero < 0) perror("write");
          left--;
        }

        int codigo;
        int quantidade1, quantidade2;
        double preco1, preco2;
        lseek(vendas,0,SEEK_SET);
        while(1) {
          printf("before read\n");
          if(!(read(vendas,&codigo,sizeof(int)))) break;
          printf("after read\n" );
          printf("codigo -> %d\n", codigo);
          lseek(quantidade,((sizeof(int))*codigo),SEEK_SET);
          lseek(precos,((sizeof(double))*codigo),SEEK_SET);

          read(vendas,&quantidade2,sizeof(int));
          read(quantidade,&quantidade1,sizeof(int));
          printf("QUANTIDADE -> %d\n", quantidade1);
          quantidade1 += quantidade2;
          lseek(quantidade,((sizeof(int))*(-1)),SEEK_CUR);
          write(quantidade,&quantidade1,sizeof(int));

          read(vendas,&preco2,sizeof(double));
          read(precos,&preco1,sizeof(double));
          preco1 += preco2;
          printf("Preco -> %f\n", preco1);
          lseek(precos,((sizeof(double))*(-1)),SEEK_CUR);
          write(precos,&preco1,sizeof(double));
        }
        printf("exited?\n");

        close(vendas);
        close(precos);
        close(quantidade);

        int agregar = open(newFile, O_WRONLY | O_CREAT, 0666);
        quantidade = open("quantidade", O_RDWR);
        precos = open("precos", O_RDWR);

        codigo = 0;
        quantidade2 = 0;
        preco2 = 0;
        while(1) {
          if(!(read(quantidade,&quantidade1,sizeof(int)))) break;
          if(quantidade1) {
            lseek(quantidade,(-sizeof(int)),SEEK_CUR);
            write(quantidade,&quantidade2,sizeof(int));

            lseek(precos,((sizeof(double))*codigo),SEEK_SET);
            read(precos,&preco1,sizeof(double));

            lseek(precos,(-sizeof(double)),SEEK_CUR);
            write(precos,&preco2,sizeof(double));

            char *strcode = int2code(codigo);
            write(agregar,strcode,FIX_SIZE);
            sprintf(newFile," %d %lf\n",quantidade1, preco1);
            write(agregar,newFile,strlen(newFile));
            free(strcode);
          }
          codigo++;
        }

        vendas = open("vendas", O_TRUNC);

        close(vendas);
        close(precos);
        close(agregar);
        close(quantidade);

        check = 200;
        exits = open("/tmp/exits", O_WRONLY);
        isZero = write(exits,&check,4);
        printf("exits write -> %d\n", isZero);
        if(isZero < 0) perror("write");
        isZero = close(exits);
        if(isZero < 0) perror("close");

        unlink("/tmp/in1");
        exit(1);
      }
    }

    if(check) {
      sprintf(buf,"/tmp/server_to_client_fifo%d",pid);
      mkfifo(buf, 0666);
      sprintf(buf1,"/tmp/client_to_server_fifo%d",pid);
      mkfifo(buf1, 0666);
      sprintf(buf2,"/tmp/in%d", pid);
      mkfifo(buf2, 0666);
      check = 0;

      if(!fork()) {
        int max;
        printf("enter\n");
        server_to_client = open(buf, O_WRONLY);
        if(server_to_client < 0) perror("open");
        printf("enter %d\n", server_to_client);
        //printf("exits -> %d\n", exits);
        printf("pid -> %d\n", pid);
        client_to_server = open(buf1, O_RDONLY);
        if(client_to_server < 0) perror("open");

        int length, j;
        char **args;
        double price;
        while (1)
        {
          check = 1;
          length = 0;
          i = 1;
          printf("enter\n");
          isZero = read(client_to_server,&length,sizeof(int));
          if(isZero < 0) perror("read");
          isZero = read(client_to_server, buf, length);
          if(isZero < 0) perror("read");
          buf[length] = '\0';
          printf("readed -> %s\n", buf);

          check = write(pidClients[1],&pid,sizeof(int));
          if(check < 0) perror("write");
          printf("pid -> %d\n", pid);
          printf("check -> %d\n", check);
          check = 1;
          sprintf(buf1,"/tmp/in%d",pid);
          printf("%s\n", buf1);
          printf("before opening\n");
          in = open(buf1, O_RDONLY);
          printf("after opening -> %d\n", in);
          if(in < 0) perror("open");
          printf("in -> %d\n", in);
          i = 0;
          isZero = read(in,&max,sizeof(int));
          if(isZero < 0) perror("read");
          printf("readed -> %d\n", max);
          isZero = close(in);
          if(isZero < 0) perror("close");

          i = 0;

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
                      i = 14;
                      isZero = write(server_to_client,&i,sizeof(int));
                      if(isZero < 0) perror("write");
                      isZero = write(server_to_client,"Invalid Code1\n",i);
                      if(isZero < 0) perror("write");
                      check = 0;
                      break;
                    }
                  }
                } else {
                  i = 14;
                  isZero = write(server_to_client,&i,sizeof(int));
                  if(isZero < 0) perror("write");
                  isZero = write(server_to_client,"Invalid Code2\n",i);
                  if(isZero < 0) perror("write");
                  check = 0;
                }
              } else check = 0;

              if(check) {
                int code = atoi(args[0]);
                printf("max -> %d\n", max);
                if(args[0] != NULL && code < max-1) {
                  if(args[1] == NULL) {
                    lseek(stock,sizeof(int)*code,SEEK_SET);
                    isZero = read(stock,&i,sizeof(int));
                    if(isZero < 0) perror("read");
                    artigos = open("artigos",O_RDONLY);
                    if(artigos < 0){ perror("open"); return -1;}
                    printf("%d\n", code);
                    lseek(artigos,(sizeof(int) + sizeof(double))*code + sizeof(int),SEEK_SET);
                    price = 10;
                    isZero = read(artigos,&price,sizeof(double));
                    if(isZero < 0) perror("read");
                    isZero = close(artigos);
                    if(isZero < 0) perror("close");
                    sprintf(buf,"%d %lf\n", i, price);
                    length = strlen(buf);
                    isZero = write(server_to_client,&length,sizeof(int));
                    if(isZero < 0) perror("write");
                    isZero = write(server_to_client,buf,length);
                    if(isZero < 0) perror("write");
                  } else if(args[2] == NULL) {
                    check = 1;
                    int signal = 1;
                    if(args[1][0] == '-') {
                      signal = -1;
                      args[1] = args[1] + 1;
                    }
                    j = 0;
                    while(args[1][j] != '\0'){
                      if(!isdigit(args[1][j++])) {
                        i = 15;
                        isZero = write(server_to_client,&i,sizeof(int));
                        if(isZero < 0) perror("write");
                        isZero = write(server_to_client,"Invalid Input1\n",i);
                        if(isZero < 0) perror("write");
                        check = 0;
                        break;
                      }
                    }
                    if(check) {
                      int change = atoi(args[1]) * signal;
                      lseek(stock,sizeof(int)*code,SEEK_SET);
                      isZero = read(stock,&i,sizeof(int));
                      if(isZero < 0) perror("read");
                      printf("I -> %d\n", i);
                      i += change;
                      printf("change -> %d\nnew -> %d\n", change,i);
                      if(i < 0) {
                        change -= i;
                        i = 0;
                        printf("ERROR\n");
                      }
                      if(signal == -1 && change != 0) {
                        change = -change;
                        artigos = open("artigos",O_RDONLY);
                        if(artigos < 0){ perror("open"); return -1;}
                        lseek(artigos,(sizeof(int) + sizeof(double))*code + sizeof(int),SEEK_SET);
                        price = 10;
                        isZero = read(artigos,&price,sizeof(double));
                        if(isZero < 0) perror("read");
                        isZero = close(artigos);
                        if(isZero < 0) perror("close");

                        vendas = open("vendas", O_WRONLY | O_APPEND);
                        if(vendas < 0) perror("open");

                        isZero = write(vendas,&code,sizeof(int));
                        if(isZero < 0) perror("write1");
                        isZero = write(vendas,&change,sizeof(int));
                        if(isZero < 0) perror("write2");
                        price *= (double) (change);
                        isZero = write(vendas,&price,sizeof(double));
                        if(isZero < 0) perror("write3");

                        isZero = close(vendas);
                        if(isZero < 0) perror("close");
                      }

                      lseek(stock,-sizeof(int),SEEK_CUR);
                      isZero = write(stock,&i,sizeof(int));
                      if(isZero < 0) perror("write");
                      sprintf(buf,"%d\n", i);
                      length = strlen(buf);
                      isZero = write(server_to_client,&length,sizeof(int));
                      if(isZero < 0) perror("write");
                      isZero = write(server_to_client,buf,length);
                      if(isZero < 0) perror("write");
                    }
                    if(signal == -1) args[1] -= 1;
                  }
                } else {
                  i = 15;
                  isZero = write(server_to_client,&i,sizeof(int));
                  isZero = write(server_to_client,"Invalid Input2\n",i);
                  if(isZero < 0) perror("write");
                }
              } else check = 1;
            } else {
              if(length) {
                i = 15;
                isZero = write(server_to_client,&i,sizeof(int));
                if(isZero < 0) perror("write");
                isZero = write(server_to_client,"Invalid Input3\n",i);
                if(isZero < 0) perror("write");
              } else {
                break;
              }
            }
             /* clean buf from any data */
             memset(buf, 0, sizeof(buf));
             if(check) cRetArg(args);
             else check = 1;
          } else {
            i = 15;
            isZero = write(server_to_client,&i,sizeof(int));
            if(isZero < 0) perror("write");
            isZero = write(server_to_client,"Invalid Input4\n",i);
            if(isZero < 0) perror("write");
          }
          check = 200;
          exits = open("/tmp/exits", O_WRONLY);
          isZero = write(exits,&check,4);
          printf("exits write -> %d\n", isZero);
          if(isZero < 0) perror("write");
          isZero = close(exits);
          if(isZero < 0) perror("close");
        }
        check = 999;
        exits = open("/tmp/exits", O_WRONLY);
        isZero = write(exits,&check,4);
        if(isZero < 0) perror("write");
        isZero = close(exits);
        if(isZero < 0) perror("close");
        printf("exit\n");


        isZero = close(client_to_server);
        if(isZero < 0) perror("close");
        isZero = close(server_to_client);
        if(isZero < 0) perror("close");

        exit(1);
      }
    }

  }

  isZero = close(clients);
  if(isZero < 0) perror("close");
  unlink(my);
  isZero = close(stock);
  if(isZero < 0) perror("close");
  isZero = close(artigos);
  if(isZero < 0) perror("close");

   return 0;
}
