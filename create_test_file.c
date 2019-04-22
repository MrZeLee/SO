#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <string.h>
#include <stdio.h>

int main() {
	int fp = open("test1.txt", O_WRONLY | O_CREAT | O_TRUNC);
	char buffer[1024];
	for(int i = 0; i < 11001; i++){
		sprintf(buffer,"n %d test%d\n", i, i);
		write(fp,buffer,strlen(buffer));
	}
	return 0;
}
