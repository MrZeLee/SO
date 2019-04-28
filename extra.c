#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> //isdigit



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
