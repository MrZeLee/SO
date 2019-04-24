#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>


int sv(int argc, char *argv[]){

	FILE *v;
	char *fifo = "tmp/fifo"
	mkfifo(fifo,0666); 
	int i,n;
	char b;
	int f;
	char buffer[1024];

	if(argc<2){
		printf("usar o programas <venda> \n");
		return 0;
	}

	for(i=1;i<argc;i++){

		v = open(VENDAS.txt,O_APPEND);

	}



}

/*
Vendas : int,int float codigo quantidade preço
Stocks :	
*/

int vendas(char){



}