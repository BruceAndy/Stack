#ifndef _PROGRAM_READER_INCLUDED_
#define _PROGRAM_READER_INCLUDED_

#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include "files_strings.h"
#include "prog_check.h"

using namespace std;

/*void skip_space(char** ptr)
{
	while(**ptr == ' ' || **ptr == '\t')
	{
		(*ptr)++;
	}
}*/

int parse_arguments(char* ptr, int num, FILE* asmfile) //divides arguments and writes them in array
{
	skip_space(&ptr);
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


int read_cmd_word(char* buf, int* args_number, int* cmdnum) //Reads the command name and number of arguments
{
#define CPU_CMDS
#define CPU_CMD(cmdname, argnum)			       \
if (!strcmp(#cmdname, buf))			   	       \
{							       \
	*args_number = CPU_##cmdname##_ARGS_NUMBER;	       \
	*cmdnum = CPU_##cmdname;			       \
	return 1;					       \
} 
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef CPU_CMDS	
	return 0; 
}

int isword(char** wptr, int* cmdnum, FILE* asmfile) // Writes it in file and returns number of arguments
{
	char* word = *wptr;
	if(isalpha(*word))
	{
		int args_num;
		int wlen = word_len(word);
		char sym = *(word + wlen);
		*(word + wlen) = '\0';
		read_cmd_word(word, &args_num, cmdnum);
		fprintf(asmfile, "%i", *cmdnum);
		*(word + wlen) = sym;
		*wptr += wlen;
		return args_num;
	} else
		return -1;
}

int parse_line(char* line, FILE* asmfile)
{
	char* beginning = line;
	int length = line_len(line);
	line[length] = '\0';
	int cmdnum;
	int args_num = isword(&line, &cmdnum, asmfile);	
	if(args_num > 0)
		parse_arguments(line, args_num, asmfile);
	*(beginning + length) = '\n';
	return 1;
}
	
int prog_parsing(const char* filename)
{
	if(filename == NULL)
	{
		printf("Error! Program file name requested.\n");
		exit(1);
	}
	char* buf;
	size_t length = file_len (filename);
	buf = prog_check_func(filename);
	//printf("File to read from is: \n%s\n", buf);
	int strings   = prog_lines_count(buf);
	FILE* asmfile = fopen("Assembled_file.b", "wb");
	if(!asmfile) 
	{
		printf("Error! Cannot create binary file.\n");
		exit(1);
	}
	int line = 1;
	while(line <= strings) {
		parse_line(buf, asmfile);
		if(line < strings)	
		{
			buf += line_len(buf) + 1; 
		}
		else 
			break;
		line++;
	}
	return 1;
}


int main(int argc, char* argv[])
{
	if(prog_parsing(argv[1]))
		printf("Check up Assembled_file.b\n");
	
}

#endif
