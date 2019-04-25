#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
  char *code = malloc(FIX_SIZE + 1);
  int zeros = FIX_SIZE - digits;
  for (size_t i = 0; i < zeros; i++) {
    code[i] = '0';
  }
  sprintf(code + zeros,"%d", previous);
  return code;
}

int main() {
	int fp = open("test1.txt", O_WRONLY | O_CREAT | O_APPEND, 00700);
	char buffer[1024];
	for(int i = 0; i < 1000000; i++){
		char *code = int2code(i);
		sprintf(buffer,"n %s beatriz%d\n", code, i);
		write(fp,buffer,strlen(buffer));
		sprintf(buffer,"p %s %d\n", code, 1000000-i);
		write(fp,buffer,strlen(buffer));
		free(code);
	}
	return 0;
}
