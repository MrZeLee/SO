#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> //isdigit

#include "ma.h"

#define FIX_SIZE 10
#define MAX 1024

// function to read last n lines from the file
// at any point without reading the entire file
int tail(int in, int pipe)
{
    // unsigned long long pos (stores upto 2^64 – 1
    // chars) assuming that long long int takes 8
    // bytes
    unsigned long long pos, pointer, empty;
    char c;
    char code[FIX_SIZE + 1];
    int r = -1;

    // Go to End of file
    if ((empty = lseek(in, 0, SEEK_END)) < 0)
        perror("lseek() failed");
    else
    {
      if (empty) {
        // pos will contain no. of chars in
        // input file.
        pos = FIX_SIZE;

        // search for '\n' characters
        while (pos)
        {
            // Move 'pos' away from end of file.
            if ((pointer = lseek(in, -pos, SEEK_END)))
            {
              if (pointer < FIX_SIZE) {
                lseek(in,0,SEEK_SET);
                break;
              }
              pos++;
              if((read(in,&c,1) > 0) && c == '\n' ) {
                // stop reading when n newlines
                // is found
                break;
              }
            }
            else {
              perror("lseek() failed");
            }
        }
        read(in,code,FIX_SIZE);
        code[FIX_SIZE] = '\0';
        r = atoi(code);
      }
        return r;
    }
}

int numDigits(int n){
    int count = 0;

    while(n != 0)
    {
        // n = n/10
        n /= 10;
        ++count;
    }
    return count;
}

int readItem(char *buffer, Item *new) {
  if (buffer[0] != ' ') return -1;
  char rem[] = " ";
  size_t i = 1;
  while (buffer[i] == ' ') i++;
  char *name;

  buffer += i * sizeof(char);

  name = strtok(buffer, rem);
  if(name == NULL) return -1;
  if((buffer = strtok(NULL, "")) == NULL) return -1;


  char *price;
  i = 0;
  while (buffer[i] == ' ') i++;

  price = strtok(buffer, rem);
  buffer = strtok(NULL, "");
  if (buffer != NULL) {
    return -1;
  }

  new->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(new->name, name);
  new->price = (double) atof(price);

  return 0;
}


//usar o fork quando estiver a escrever nos dois ficheiros
int insert(char *buffer) {
  Item new;
  if(readItem(buffer, &new) < 0) return -1;

  int fp = open("artigos", O_RDONLY | O_CREAT);
  if(fp < 0) return -1;
  int previous = tail(fp,1);
  close(fp);

  fp = open("artigos", O_WRONLY | O_APPEND);
  if(fp < 0) return -1;
  int fp1 =  open("strings", O_WRONLY | O_APPEND | O_CREAT);
  if(fp1 < 0) return -1;

  previous += 1;

  if(fork()) {
    int digits = numDigits(previous);
    char code[MAX];
    int zeros = FIX_SIZE - digits;
    for (size_t i = 0; i < zeros; i++) {
      code[i] = '0';
    }
    sprintf(code + zeros,"%d", previous);
    write(fp,code,FIX_SIZE);
    write(fp,":",1);

    //write into user
    write(1,"code: ",6);
    write(1,code,10);
    write(1,"\n",1);

    sprintf(code,"%lf", new.price);
    write(fp,code,strlen(code));
    write(fp,"\n",1);
    close(fp);
    wait(NULL);
  } else {
    write(fp1,new.name,strlen(new.name));
    write(fp1,"\n",1);
    close(fp1);write(fp1,new.name,strlen(new.name));
    write(fp1,"\n",1);
    close(fp1);
    exit(1);
  }

  return 0;
}

int getCode (char *buffer) {
  if(buffer[0] != ' ') return -1;
  size_t i = 1;
  char rem[] = " ";
  char *code = malloc(sizeof(char) * (FIX_SIZE + 1));
  while(buffer[i] == ' ') i++;
  buffer += i * sizeof(char);

  code = strtok(buffer, rem);
  if(strlen(code) > 10) return -1;
  i = 0;
  while(isdigit(code[i])) i++;
  if(code[i] != '\0' || i == 0) return -1;
  printf("%s\n", code);
  return 0;
}

int newName (char *buffer) {
  getCode(buffer);
  /*int max;
  int fp = open("artigos", O_RDONLY | O_CREAT);
  max = tail(fp,1);
  int temp = open(".strings_tmp",O_WRONLY | O_CREAT);
  */
  return 0;
}





int main(int argc, char const *argv[]) {
  char c = 0;
  char buffer[MAX];
  size_t i;
  int stat;
  int check = 0;

  while(check != 2) {
    if(check) write(1,"Write valide option.\n",21);
    check = 1;
    i = 0;
    buffer[0] = '\0';

    write(1,"> ",2);
    c = -1;
    while ((read(1,&c,1) > 0) && c != '\n') {
      buffer[i++] = c;
    }
    buffer[i] = '\0';

    if(!fork()) {
      switch (buffer[0]) {
        case 'i': if (!insert(buffer + sizeof(char))) check = 0;
                  break;
        case 'n': if (!newName(buffer + sizeof(char))) check = 0;
                  break;
        case 'p': break;
        default:
                  if(c == -1) {
                    write(1,"\n",1);
                    check = 2;
                  }
      }
      exit(check);
    }
    wait(&stat);
    if (WIFEXITED(stat))
        check = WEXITSTATUS(stat);
  }

  return 0;
}
