#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include "prog_check.h"
#include "files_strings.h"

using namespace std;

int parse_arguments(char* ptr, int num, FILE* asmfile) //divides arguments and writes them in array
{
	for(int i = 0; i < num; i++)
	{
		switch(ptr[0])
		{	
#define ARGCASE(argsym, argtypenum)				\
case argsym :							\
	fprintf(asmfile, "%c", argtypenum);			\
	ptr++;							\
	if ((ptr[0] >= '0' && ptr[0] <= '9') || (ptr[0] == '.'))\
	{							\
		int arg_length      = arg_len(ptr);		\
		char sym            = *(ptr + arg_length);	\
		*(ptr + arg_length) = '\0';			\
		double arg = 0;					\
		sscanf(ptr, "%lg", &arg);			\
		fprintf(asmfile, "%lg", arg);			\
		*(ptr + arg_length) = sym;			\
		ptr                += arg_length + 1;		\
	}							\
	break;
		ARGCASE('%', 1) //recieving register number
		ARGCASE('$', 2) //recieving constant argument
		ARGCASE('*', 3) //recieving... uuhh... ptr? (METKA)

		}
#undef ARGCASE
	}
	return 0;	
}

void prog_cmd_identifier(char* buf, FILE* asmfile) //Reads the command name and writes its number in file
{
#define CPU_CMDS
#define CPU_CMD(cmdname, num)								   	\
if (!strcmp(#cmdname, buf))								   	\
{											   	\
	fprintf(asmfile, "%d", CPU_##cmdname);						   	\
	if (CPU_##cmdname##_ARGS_NUMBER)							\
		parse_arguments(buf + word_len(buf) + 1, CPU_##cmdname##_ARGS_NUMBER, asmfile); \
}
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef CPU_CMDS	
}

FILE* prog_cmd_parser(const char* filename) 	//Finds command and puts '\0' at the end of it
{
	int ok = prog_check_func(filename); //searches for mistackes in assembler code
	(ok)?printf("check programm said it's allright\n"):printf("PIZDA");
	if(filename == NULL)
	{
		printf("Program file name requested.\n");
		exit(1);
	}
	char* buf     = read_prog(filename);
	size_t length = file_len (filename);
	FILE* asmfile = fopen("ASM.r", "wb");
	if(!asmfile)
	{
		printf("Invalid name of program file.\n");
		exit(1);
	}
	int i = 0;
	bool arg_read = false;
	while(i < length)
	{
		if ( isalpha(buf[i]) /*(buf[i] >= 'A' && buf[i] <= 'Z') || (buf[i] >= 'a' && buf[i] <= 'z') */)
		{
			int w_len      = word_len(buf + i);
			char sym       = buf[i + w_len];
			buf[i + w_len] = '\0';
			prog_cmd_identifier(buf + i, asmfile);
			buf[i + w_len] = sym;
			i += w_len;
		}	
		i++;
	} 
	return asmfile;
}

int main(int argc, char* argv[])
{

	prog_cmd_parser(argv[1]);
	printf("Check up 'ASM.r' file\n");	

	return 0;
}
