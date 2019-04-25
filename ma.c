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
    if (!n) return 1;

    int count = 0;

    while(n != 0)
    {
        n /= 10;
        ++count;
    }
    return count;
}

char* int2code(int previous) {
  int digits = numDigits(previous);
  char *code = malloc(FIX_SIZE + 2);
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

// function to read last line from the file
// at any point without reading the entire file

unsigned int maxCode() {
    int in = open("artigos", O_RDONLY | O_CREAT, 00700);
    if(in < 0) {
      write(2,"open(\"artigos\") failure\n",24);
      return 4294967295;
    }

    unsigned long long empty;
    unsigned int max = 0;

    // Go to End of file
    if ((empty = lseek(in, -(sizeof(unsigned int) + sizeof(float)), SEEK_END)) == -1) {
      return max;
    }
    //printf("%lld\n", empty);
    read(in,&max,sizeof(unsigned int));
    close(in);
    return (max+1);
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
  int space = spacecount(buffer);
  char *temp = strtok(buffer," ");
  char **arg = malloc(sizeof(int) * space);
  while (temp){
					arg[i] = malloc(strlen(temp) + 1);
          strcpy(arg[i++],temp);
					temp = strtok(NULL," ");
			}
  arg[i] = NULL;
  return arg;
}

void cRetArg (char **args) {
  if (args != NULL) {
    for (size_t i = 0; args[i] != NULL; i++) {
      free(args[i]);
    }
    free(args);
  }
}

//usar o fork quando estiver a escrever nos dois ficheiros
int insert(char *buffer, unsigned int *current, int fp, int fp1) {

  int j;
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;
  if((j = isFloat(args[1]))) return j;

    char *code = int2code(*current);
    //output terminal
    strcat(code,"\n");
    write(1,code,FIX_SIZE+1);

    //artigos code and price
    float price = atof(args[1]);
    write(fp,current,sizeof(unsigned int));
    write(fp,&price,sizeof(float));

    //write in strings
    int length = strlen(args[0]) + 2;
    char name[length];
    strcpy(name,args[0]);
    strcat(name,"\n");
    write(fp1,name,length-1);

  *current = *current + 1;
  cRetArg(args);
  free(code);

  return 0;
}

/*int getCode (char *buffer) {
  if(buffer[0] != ' ') return -USER_FAIL;
  size_t i = 1;
  char rem[] = " ";
  char *code = malloc(FIX_SIZE + 1);
  if (!code) write(2,"malloc() failed\n",16);
  while(buffer[i] == ' ') i++;
  buffer += i;

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
  int ret = atoi(code);
  free(code);
  return ret;
}*/

/*int wrFileLine (char *file, char *test, int code) {
  int temp = open(".tmp",O_WRONLY | O_CREAT, 00700);
  if(temp < 0) {
    write(2,"open(\".tmp\") failure\n",21);
    return SYS_FAIL;
  }
  int fp = open(file, O_RDONLY | O_CREAT, 00700);
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
  int done;

  if(!fork()) {
    done = execlp("mv","mv",".tmp",file,NULL);
    exit(done);
  }
  else {
    if (WIFEXITED(done)) done = WEXITSTATUS(done);
    if(done) {
      write(2,"execlp(mv) failure\n",19);
      return SYS_FAIL;
    }
  }

  return 0;
}*/

int refreshFile (char* file, char* temp_file, char* tmp) {

  int fp = open(file, O_RDONLY);
  int temp = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 00700);
  int fp2 = open(temp_file, O_RDONLY);
  if(fp2 < 0) return 1;


  int check = 1,check1 = 1, i, count = 0;
  char c, buffer[MAX];
  int code = -1;
  char **args = NULL;

  while(check || check1) {

    if(check) {
      i = 0;
      while((check = read(fp2,&c,1)) > 0 && c != '\n') {
        buffer[i++] = c;
      }
      buffer[i] = '\0';

      if(check) {
        args = retArg(buffer);
        if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;
        code = atoi(args[0]);
      } else code = -1;
    }

    while((check1 = read(fp,&c,1)) > 0) {
      if(count == code) {
        while((check1 = read(fp,&c,1)) > 0 && c != '\n');
        count++;
        write(temp,args[1],strlen(args[1]));
        write(temp,"\n",1);
        break;
      }
      else {
        if(c == '\n') count++;
        write(temp,&c,1);
      }
    }
    if(check) {
      cRetArg(args);
    }
  }

  close(fp);
  close(temp);
  close(fp2);
  int done;

  if(!fork()) {
    done = execlp("mv","mv",tmp,file,NULL);
    exit(done);
  }
  else {
    wait(&done);
    if (WIFEXITED(done)) done = WEXITSTATUS(done);
    if(done) {
      write(2,"execlp(mv) failure\n",19);
      return SYS_FAIL;
    }
  }
  return 0;
}

/*int newName (char *buffer, int max) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;

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
}*/

char *str2Code (char* arg, int* max) {
  int i = 0;
  char t;
  while((t = arg[i]) != '\0') {
    if (!isdigit(t)) return NULL;
    i++;
  }

  int cod = atoi(arg);
  if (cod > *max) {
    write(2,"Code not found\n",15);
    return NULL;
  }
  *max = cod;
  char *code = int2code(cod);

  return code;
}

int newPrice (char *buffer, int fp, int max) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;

  for (size_t i = 0; i < FIX_SIZE; i++) {
    if(!isdigit(args[0][i])) {
      write(2,"Invalid Code\n",13);
      return USER_FAIL;
    }
  }
  if(args[0][FIX_SIZE] != '\0') return USER_FAIL;

  int code = atoi(args[0]);

  int i;
  if((i = isFloat(args[1]))) return i;
  float price = atof(args[1]);

  i = lseek(fp, (code * (sizeof(unsigned int) + sizeof(float))) + sizeof(unsigned int), SEEK_SET);
  write(fp,&price,sizeof(float));
  i = lseek(fp, 0, SEEK_END);

  cRetArg(args);
  return 0;
}

int saveName (char *buffer, int max, char *file_temp) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) {
    cRetArg(args);
    return USER_FAIL;
  }

  for (size_t i = 0; i < FIX_SIZE; i++) {
    if(!isdigit(args[0][i])) {
      write(2,"Invalid Code\n",13);
      return USER_FAIL;
    }
  }
  if(args[0][FIX_SIZE] != '\0') return USER_FAIL;

  int fp = open(file_temp, O_WRONLY | O_APPEND | O_CREAT, 00700);

  int length = FIX_SIZE + strlen(args[1]) + 2;
  char code[length + 1];

  strcpy(code,args[0]);
  strcat(code," ");
  strcat(code,args[1]);
  strcat(code,"\n");

  write(fp,code,length);

  cRetArg(args);
  close(fp);
  return 0;
}

int finishFiles(char *file, char *file_temp, char *tmp) {
  char c;
  int r;
  int fp, done;
  if((fp = open(file_temp, O_WRONLY)) > -1) {
    close(fp);

    if(!fork()) {
      done = execlp("sort","sort","-n","-o",file_temp,file_temp,NULL);
      exit(done);
    } else{
          wait(&done);

          if (WIFEXITED(done)) done = WEXITSTATUS(done);
          if(done) {
            write(2,"execlp(mv) failure\n",19);
            return SYS_FAIL;
          }
    }
    if((r = refreshFile(file,file_temp,tmp))) return r;
    if(!fork()) {
      done = execlp("rm","rm",file_temp,NULL);
      exit(done);
    } else{
      wait(&done);
      if (WIFEXITED(done)) done = WEXITSTATUS(done);
      if(done) {
        write(2,"execlp(mv) failure\n",19);
        return SYS_FAIL;
      }
    }
  }
  return 0;
}

int main(int argc, char const *argv[]) {
  unsigned int max = maxCode();
  if(max == -2) return SYS_FAIL;

  int fp = open("artigos", O_WRONLY);
  if(fp < 0) {
    write(2,"open(\"artigos\") failure\n",25);
    return SYS_FAIL;
  }

  int fp1 = open("strings", O_WRONLY | O_APPEND | O_CREAT, 00700);
  if(fp1 < 0) {
    write(2,"open(\"strings\") failure\n",24);
    return SYS_FAIL;
  }

  char c = 0;
  char buffer[MAX];
  size_t i;
  int check = 0;

  c = -1;
  i = 0;
  buffer[0] = '\0';
  while ((read(0,&c,1) > 0)) {
    if(c == '\n') {
      check = USER_FAIL;
      buffer[i] = '\0';

      if(i != 1) {
          switch (buffer[0]) {
            case 'i': check = insert(buffer + 1,&max,fp,fp1);
                      break;
//            case 'n': check = newName(buffer + sizeof(char),max);
            case 'n': check = saveName(buffer + 1,max,".saveName");
                      break;
//            case 'p': check = newPrice(buffer + sizeof(char),max);
            case 'p': check = newPrice(buffer + 1,fp, max);
                      break;
            default: break;
          }
        }
      if(check == USER_FAIL) write(1,"Write valide option.\n",21);
      if(check == SYS_FAIL) return 1;
      i = 0;
    }
    else buffer[i++] = c;
  }

  close(fp);
  close(fp1);

  finishFiles("strings",".saveName",".tmp");

  return 0;
}
