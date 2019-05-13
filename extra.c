#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> //isdigit

#define FIX_SIZE 10

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

  for (size_t i = 0; i < zeros; i++)
  {
    code[i] = '0';
  }
  sprintf(code + zeros,"%d", previous);
  return code;
}

int spacecount (char string[]){
	int i, count = 1;

	for (i = 0; string[i] != '\0'; i++)
		if (string[i] == ' ') count++;

	return count;
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
