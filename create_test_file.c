#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <string.h>
#include <stdio.h>

int main() {
	int fp = open("test2.txt", O_WRONLY | O_CREAT | O_APPEND, 00700);
	char buffer[1024];
	for(int i = 0; i < 1000000; i++){
		sprintf(buffer,"meio\n");
		write(fp,buffer,strlen(buffer));
	}
	return 0;
}
