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
  char *code = malloc(FIX_SIZE + 1);
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
int maxCode()
{
    int in = open("artigos", O_RDONLY | O_CREAT, 00700);
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
    }
    close(in);
    return r;
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
int insert(char *buffer, int *previous, int fp, int fp1) {
  Item new;
  int j;
  char **args = retArg(buffer);
  new.name = args[0];
  if((j = isFloat(args[1]))) return j;
  new.price = atof(args[1]);



  *previous = *previous + 1;
  int length;

  while(wait(NULL) > 0);

  if(!fork()) {
    char *code = int2code(*previous);
    char *price = malloc(100);
    sprintf(price,"%lf", new.price);


    //write into user
    length = 6 + FIX_SIZE + 2;
    char *output = malloc(length);
    strcpy(output,"code: ");
    strcat(output,code);
    strcat(output,"\n");
    write(1,output,length-1);
    free(output);

    length = FIX_SIZE + 1 + strlen(price) + 2;
    char *total = malloc(length);
    strcpy(total,code);
    strcat(total," ");
    strcat(total,price);
    free(price);
    strcat(total,"\n");

    write(fp,total,length-1);
    free(code);

    length = strlen(new.name) + 2;
    char *name = malloc(length);
    strcpy(name,new.name);
    strcat(name,"\n");

    write(fp1,name,length-1);
    free(name);
    exit(1);
  }
  cRetArg(args);

  return 0;
}

int getCode (char *buffer) {
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
}

int wrFileLine (char *file, char *test, int code) {
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
}

int refreshFile (char* file, char* temp_file, int addCode, char* tmp) {

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
        code = atoi(args[0]);
      } else code = -1;
    }

    while((check1 = read(fp,&c,1)) > 0) {
      if(count == code) {
        while((check1 = read(fp,&c,1)) > 0 && c != '\n');
        count++;
        if(addCode) {
          write(temp,args[0],strlen(args[0]));
          write(temp," ",1);
          write(temp,args[1],strlen(args[1]));
        } else {
          write(temp,args[1],strlen(args[1]));
        }
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

int newName (char *buffer, int max) {
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
}

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

int newPrice (char *buffer, int max) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) return USER_FAIL;
  char *code = str2Code(args[0], &max);
  if (!code) return USER_FAIL;

  code = realloc(code, FIX_SIZE + strlen(args[1]) + 1);
  code[10] = ':';

  int i;
  if((i = isFloat(args[1]))) return i;
  float price = atof(args[1]);

  sprintf((code + 11),"%lf", price);

  i = wrFileLine("artigos",code,max);

  cRetArg(args);
  free(code);
  return i;
}

int saveName (char *buffer, int max, char *file_temp, int addCode) {
  char **args = retArg(buffer);
  if(args[0] == NULL || args[1] == NULL || args[2] != NULL) {
    cRetArg(args);
    return USER_FAIL;
  }

  int fp = open(file_temp, O_WRONLY | O_APPEND | O_CREAT, 00700);

  char *code = str2Code(args[0], &max);
  if (!code) return USER_FAIL;

  if(addCode) {
    int j;
    if((j = isFloat(args[1]))) return j;
    float price = atof(args[1]);
    char strPrice[MAX];
    sprintf(strPrice,"%lf",price);
    args[1] = realloc(args[1], strlen(strPrice) + 1);
    strcpy(args[1],strPrice);
  }

  code = realloc(code,FIX_SIZE + strlen(args[1]) + 1);
  strcat(code," ");

  write(fp,code,strlen(code));
  write(fp,args[1],strlen(args[1]));
  write(fp,"\n",1);
  cRetArg(args);
  close(fp);
  free(code);
  return 0;
}

int finishFiles(char *file, char *file_temp, int addCode, char *tmp) {
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
    if((r = refreshFile(file,file_temp,addCode,tmp))) return r;
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
  int max = maxCode();
  if(max == -2) return SYS_FAIL;

  int fp = open("artigos", O_WRONLY | O_APPEND);
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
            case 'n': check = saveName(buffer + 1,max,".saveName",0);
                      break;
//            case 'p': check = newPrice(buffer + sizeof(char),max);
            case 'p': check = saveName(buffer + 1,max,".savePrice",1);
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

  if(!fork()) {
    finishFiles("strings",".saveName",0,".tmp");
    exit(1);
  } else{
    if(!fork()) {
      finishFiles("artigos",".savePrice",1,".tmp1");
    } else {
      while(wait(NULL) > 0);
    }
  }

  return 0;
}
