#ifndef __MA_H__
#define __MA_H__


#define FIX_SIZE 10
#define MAX 1024
#define READ 0
#define USER_FAIL 2
#define SYS_FAIL 3
#define CLOSE 4



//Conta o númeor de espaços de array de caracteres
int spacecount (char string[]);

//
unsigned int maxCode();

//
int isFloat(char *price);

//
char **retArg (char *buffer);

//
void cRetArg (char **args);

//
int insert(char *buffer, unsigned int *current, int fp, int fp1);

//
int refreshFile (char* file, char* temp_file, char* tmp);

//
char *str2Code (char* arg, int* max);

//
int newPrice (char *buffer, int fp, int max);

//
int saveName (char *buffer, int max, char *file_temp);

//
int finishFiles(char *file, char *file_temp, char *tmp);

#endif
