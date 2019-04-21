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
#define READ 0
#define USER_FAIL 2
#define SYS_FAIL 3
#define CLOSE 4

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

char* int2code(int previous) {
  int digits = numDigits(previous);
  char *code = malloc(sizeof(char) * (FIX_SIZE + 1));
  int zeros = FIX_SIZE - digits;
  for (size_t i = 0; i < zeros; i++) {
    code[i] = '0';
  }
  sprintf(code + zeros,"%d", previous);
  return code;
}

int spacecount (char string[]){
	int i, count = 0;

	for (i = 0; string[i] != '\0'; i++)
		if (string[i] == ' ') count++;

	return count;
}

// function to read last n lines from the file
// at any point without reading the entire file
int maxCode()
{
    int in = open("artigos", O_RDONLY | O_CREAT);
    if(in < 0) {
      write(2,"open(\"artigos\") failure\n",24);
      return SYS_FAIL;
    }
    // unsigned long long pos (stores upto 2^64 – 1
    // chars) assuming that long long int takes 8
    // bytes
    unsigned long long pos, pointer, empty;
    char c;
    char code[FIX_SIZE + 1];
    int r = -1;

    // Go to End of file
    if ((empty = lseek(in, 0, SEEK_END)) < 0) {
      write(2,"lseek() failed\n",15);
      r = SYS_FAIL;
    }
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
              write(2,"lseek() failed\n",15);
              r = SYS_FAIL;
            }
        }
        read(in,code,FIX_SIZE);
        code[FIX_SIZE] = '\0';
        r = atoi(code);
      }
        return r;
    }
}

int isFloat(char *price) {
  int i = 0;
  char p;
  int point = -2;
  while((p = price[i++]) != '\0' && point < 0 ) {
    if(isdigit(p) || p == '.') {
      if(p == '.') point++;
    } else point = 0;
  }
  if(!point) return USER_FAIL;
  return 0;
}

char** retArg (char *buffer) {
  int i = 0;
  char *temp = strtok(buffer," ");
  char **arg = malloc(sizeof(int) * (spacecount(buffer)+2));
  while (temp){
					arg[i] = malloc(sizeof(strlen(temp) + 1));
          strcpy(arg[i++],temp);
					temp = strtok(NULL," ");
			}
  arg[i] = NULL;
  return arg;
}

void cRetArg (char **args) {
  for (size_t i = 0; args[i]; i++) {
    free(args[i]);
  }
  free(args);
}

//usar o fork quando estiver a escrever nos dois ficheiros
int insert(char *buffer) {
  Item new;
  int j;
  char **args = retArg(buffer);
  new.name = args[0];
  if((j = isFloat(args[1]))) return j;
  new.price = atof(args[1]);

  int previous = maxCode();
  if(previous == -2) return SYS_FAIL;

  int fp = open("artigos", O_WRONLY | O_APPEND);
  if(fp < 0) {
    write(2,"open(\"artigos\") failure\n",24);
    return SYS_FAIL;
  }
  int fp1 =  open("strings", O_WRONLY | O_APPEND | O_CREAT);
  if(fp1 < 0) {
    write(2,"open(\"strings\") failure\n",24);
    return SYS_FAIL;
  }

  previous += 1;

  if(fork()) {
    char *code = int2code(previous);
    write(fp,code,FIX_SIZE);
    write(fp,":",1);

    //write into user
    write(1,"code: ",6);
    write(1,code,10);
    free(code);
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
  cRetArg(args);

  return 0;
}

int getCode (char *buffer) {
  if(buffer[0] != ' ') return -USER_FAIL;
  size_t i = 1;
  char rem[] = " ";
  char *code = malloc(sizeof(char) * (FIX_SIZE + 1));
  if (!code) write(2,"malloc() failed\n",16);
  while(buffer[i] == ' ') i++;
  buffer += i * sizeof(char);

  code = strtok(buffer, rem);
  if(strlen(code) > 10) {
    free(code);
    return -USER_FAIL;
  }
  i = 0;
  while(isdigit(code[i])) i++;
  if(code[i] != '\0' || i == 0) {
    free(code);
    return -USER_FAIL;
  }
  return atoi(code);
}

int wrFileLine (char *file, char *test, int code) {
  int temp = open(".tmp",O_WRONLY | O_CREAT);
  if(temp < 0) {
    write(2,"open(\".tmp\") failure\n",21);
    return SYS_FAIL;
  }
  int fp = open(file, O_RDONLY | O_CREAT);
  if(fp < 0) {
    close(temp);
    write(2,"open() failure\n",24);
    return SYS_FAIL;
  }
  int count = 0;
  char c;

  while(read(fp,&c,1) > 0) {
    if(count == code) {
      while(read(fp,&c,1) > 0 && c != '\n');
      count++;
      write(temp,test,strlen(test));
      write(temp,"\n",1);
    }
    else {
      if(c == '\n') count++;
      write(temp,&c,1);
    }
  }
  close(temp);
  close(fp);
  int p[2];
  pipe(p);
  if(!fork()) {
    close(p[0]);
    dup2(p[1],1);
    close(p[1]);
    execlp("mv","mv",".tmp",file,NULL);
    exit(1);
  }
  else {
    close(p[1]);
    wait(NULL);
    count = 0;
    while(read(p[0],&c,1) > 0) {
      count++;
    }
    close(p[0]);
  }
  if(count) {
    write(2,"execlp(mv) failure\n",19);
    return SYS_FAIL;
  }
  return 0;
}

int newName (char *buffer) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;

  int max = maxCode();
  if(max == -2) return SYS_FAIL;
  int i = 0;
  char t;
  while((t = args[0][i]) != '\0') {
    if (!isdigit(t)) return USER_FAIL;
    i++;
  }
  int code = atoi(args[0]);

  if (code > max) {
    write(2,"Code not found\n",15);
    return USER_FAIL;
  }

  i = wrFileLine("strings",args[1],code);

  cRetArg(args);
  return i;
}

int newPrice (char *buffer) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;

  int max = maxCode();
  if(max == -2) return SYS_FAIL;

  int i = 0;
  char t;
  while((t = args[0][i]) != '\0') {
    if (!isdigit(t)) return USER_FAIL;
    i++;
  }

  int cod = atoi(args[0]);
  if (cod > max) {
    write(2,"Code not found\n",15);
    return USER_FAIL;
  }

  char *code = int2code(cod);
  args[0] = realloc(args[0],(sizeof(char) * (FIX_SIZE + strlen(args[1]) + 1)));
  code[10] = ':';

  if((i = isFloat(args[1]))) return i;
  float price = atof(args[1]);

  sprintf((code + 11),"%lf", price);

  i = wrFileLine("artigos",code,cod);

  cRetArg(args);
  free(code);
  return i;
}

int main(int argc, char const *argv[]) {
  char c = 0;
  char buffer[MAX];
  size_t i;
  int stat;
  int check = 0;




    write(1,"> ",2);
    c = -1;
    i = 0;
    buffer[0] = '\0';
    while ((read(1,&c,1) > 0)) {
      if(c == '\n') {
        check = USER_FAIL;
        buffer[i] = '\0';

        if(i != 1) {
          if(!fork()) {
            switch (buffer[0]) {
              case 'i': check = insert(buffer + sizeof(char));
                        break;
              case 'n': check = newName(buffer + sizeof(char));
                        break;
              case 'p': check = newPrice(buffer + sizeof(char));
                        break;
              default: break;
            }
            exit(check);
          } else {
            wait(&stat);
            if (WIFEXITED(stat))
                check = WEXITSTATUS(stat);
          }
        }
        if(check == USER_FAIL) write(1,"Write valide option.\n",21);
        if(check == SYS_FAIL) return 1;
        write(1,"> ",2);
        i = 0;
      }
      else buffer[i++] = c;
    }

    write(1,"\n",1);

  return 0;
}
