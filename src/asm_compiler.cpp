
/*This program destinated to find syntax errors in users program file*/

#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

using namespace std;

enum CPU_CMD_NUMS
{
#define CPU_CMDS
#define CPU_CMD(cmdname, argamount) CPU_##cmdname,
#include  "CPU_ACTIONS.h"
#undef CPU_CMD
#undef CPU_CMDS
};

enum CPU_CMD_ARGS_NUMBERS
{
#define CPU_CMDS
#define CPU_CMD(cmdname, argnumber) CPU_##cmdname##_ARGS_NUMBER = argnumber,
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef CPU_CMDS
};

size_t file_len(const char* filename) //returns file length
{
	FILE* f = fopen(filename, "r");
	fseek(f, 0, SEEK_END);
	size_t filesize = ftell(f);
	rewind(f);	
	return filesize; 
}

size_t prog_lines_count(char* buf)    //Counts number of lines in users program
{
	char * prevstrend = buf;
	int linesnum = 0;
	while (1)		
	{	
		prevstrend = strchr(prevstrend + 1, '\n');
		if (!prevstrend)
			break;
		linesnum++;
	}	
	return linesnum;
}

char* read_prog(const char* filename)  //Reads users program and writes it in string 
{
	FILE* f = fopen(filename, "r");
	size_t filesize = file_len(filename);

	char * buf  = (char*) calloc (filesize, sizeof(char));
	if(!buf)
	{
		printf("Error! Cannot find memory to read file.\n");
		exit(1);
	}
	fread(buf, sizeof(char), filesize, f);
	return buf;
}

int word_len(char* ptr) // returns length of a command
{
	int counter = 0;
	int i = 1;
	for (int i = 0; (ptr[i] != ' ' && ptr[i] != '\n' && ptr[i] != '\t' && ptr[i] != '\0'); i++)
		counter++;
	return counter;
}

int arg_len(char* ptr) //returns argument length
{
	int counter = 0;
	int i = 0;
	while ((ptr[i] >= '0' && ptr[i] <= '9') || (ptr[i] == '.'))
	{
		i++;
		counter++;
	}
	return counter;
}

int is_argument(char* ptr, int num, int line)
{
	for(int i = 0; i < num; i++)
	{
		switch(*ptr)
		{	
#define ARGCASE(argsym)							 		    	     \
case argsym :								 		    	     \
	ptr++;								 		    	     \
	if(*ptr < '0' && *ptr > '9')					  		    	     \
	{								 		    	     \
		printf("At line %i : Error! Argument value requiered near '%c'.\n", line, *(ptr - 1));\
		exit(0);						 		    	     \
	}								 		    	     \
	break;
		ARGCASE(':') 
		ARGCASE('$') 
		ARGCASE('*') 
#undef ARGCASE
		default:
			printf("At line %i : Error! Expected type specificator before '%c'.\n", line, *ptr);
			exit(0);
		}
	}
	return 1;	
}

int check_cmd_word(char* buf, int* args_number, int line) //Reads the command name
{
#define STR_COMPARING
#define CPU_CMD(cmdname)			   \
if (!strcmp(#cmdname, buf))			   \
{						   \
	*args_number = CPU_##cmdname##_ARGS_NUMBER;\
} else
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef STR_COMPARING	
	{
		printf("At line %i: Error! Unknown command '%s'.\n", line, buf);
		exit(1);
	}
	return 1; 
}

int line_len(char* ptr)
{
	int counter = 0;
	for(int j = 0; (ptr[j] != '\n'); j++) 
		counter++;
	return counter;
}

int isword(char** wptr, int line) // If word is ok returns number of arguments
{
	char* word = *wptr;
	if( (*word>= 'A' && *word <= 'Z') || (*word >= 'a' && *word <= 'z') )
	{
		int args_num;
		int wlen = word_len(word);
		char sym = *(word + wlen);
		*(word + wlen) = '\0';
		check_cmd_word(word, &args_num, line);
		*wptr += wlen;
		return args_num;
	} else
		return -1;
}

int check_line(char* line, int l)
{
	char* beginning = line;
	int length = line_len(line);
	line[length] = '\0';

	if( (*line == ':' || *line == '$' || *line == '*'))
	{
		printf("At line %i : Error! Command name requiered before argument type '%c' was token.\n", l, *line);
		exit(0);
		
	}
	if( (*line >= '0' && *line <= '9' || *line == '.') )
	{
		printf("At line %i : Error! Command name and argument type expected before '%c' was token.\n", l, *line);
		exit(0);
	}
	int args_num = isword(&line, l);
	if(args_num > 0)
	{
		while(!isprint(*line))
			line++;
		is_argument(line, args_num, l);
	}
	*(beginning + length) = '\n';
	return 1;
}
	
int prog_check_func(const char* filename)
{
	if(filename == NULL)
	{
		printf("Error! Program file name requested.\n");
		exit(1);
	}
	char* buf     = read_prog(filename);
	size_t length = file_len (filename);
	FILE* asmfile = fopen("ASM.r", "wb");
	if(!asmfile)
	{
		printf("OHUENNUY Error! Invalid name of program file.\n");
		exit(1);
	}
	int line = 1;
	int strings = prog_lines_count(buf);
	while(line <= strings) {
		check_line(buf, line);
		line++;
		buf += line_len(buf) + 1;
	}
	return 1;
}
