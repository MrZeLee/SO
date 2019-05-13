#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
  char *code = malloc(10 + 2);
  int zeros = 10 - digits;
  for (size_t i = 0; i < zeros; i++) {
    code[i] = '0';
  }
  sprintf(code + zeros,"%d", previous);
  return code;
}

int main() {
	int fp = open("test2.txt", O_WRONLY | O_CREAT | O_APPEND, 00700);
	char buffer[1024];
	char* code;
	code = int2code(0);
	for(int i = 0; i < 1000000; i++){
		sprintf(buffer,"%s\n", code);
		write(fp,buffer,strlen(buffer));
	}
	close(fp);
	fp = open("test1.txt", O_WRONLY | O_CREAT | O_APPEND, 00700);
	for(int i = 0; i < 1000000; i++){
		sprintf(buffer,"%s %d\n", code ,-1);
		write(fp,buffer,strlen(buffer));
	}

	close(fp);
	return 0;
}
