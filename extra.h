#ifndef __MA_H__
#define __EXTRA_H__

#define FIX_SIZE 10
#define MAX 1024

//Dá o número de digitos de um inteiro
int numDigits(int n);

//Transforma um inteiro num array de caracteres com 10 de tamanho
char* int2code(int previous);

//Conta o númeor de espaços de array de caracteres
int spacecount (char string[]);


char **retArg (char *buffer);

//
void cRetArg (char **args);

#endif
