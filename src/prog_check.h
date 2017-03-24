#ifndef _PROGRAM_CHECK_INCLUDED_
#define _PROGRAM_CHECK_INCLUDED_

/*This program is to find syntax errors in users program file*/

#include <cctype>
#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include "files_strings.h"

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

/*void skip_space(char** ptr)
{
	while(**ptr == ' ' || **ptr == '\t')
		(*ptr)++;
}*/

int is_argument(char* ptr, int num, int line)
{
	ptr++;
	for(int i = 0; i < num; i++)
	{
		switch(*ptr)
		{	
#define ARGCASE(argsym)							 		    	      \
case argsym :								 		    	      \
	ptr++;								 		    	      \
	if(*ptr < '0' && *ptr > '9')					  		    	      \
	{								 		    	      \
		printf("At line %i : Error! Argument value requiered after '%c' was taken.\n", line, *(ptr - 1));\
		exit(0);						 		    	      \
	}								 		    	      \
	break;
		ARGCASE('%') 
		ARGCASE('$') 
		ARGCASE('*') 
		default:
			printf("At line %i: Error! Unknown argument specifier '%c'.\n", line, *ptr);
			exit(1);
#undef ARGCASE
		}
	}
	return 1;	
}

int check_absence(char* line, int l, char* cmdname)
{
	line += word_len(line);
	skip_space(&line);
	if(*line != '\n' && *line != '\0')
	{
		printf("At line %i: Error! Too many arguments for command %s.\n", l, cmdname);
		exit(1);
	}
	return 1;
}

int check_cmd_word(char* buf, int* args_number, int line, char** cmd) //Reads the command name
{
#define CPU_CMDS
#define CPU_CMD(cmdname, argnum)			      \
if (!strcmp(#cmdname, buf))			   	      \
{							      \
	*args_number = CPU_##cmdname##_ARGS_NUMBER;	      \
	*cmd = (char*) calloc(word_len(buf) + 1, sizeof(char));\
	sscanf(buf, "%s", *cmd);			      \
	(*cmd)[word_len(buf) + 1] = '\0';		      \
	return 1;					      \
} else
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef CPU_CMDS	
	{
		printf("At line %i: Error! Unknown command '%s'.\n", line, buf);
		exit(1);
	}
	return 1; 
}

int isword(char** wptr, int line, char** cmdname) // If word is ok returns number of arguments
{
	char* word = *wptr;
	if(isalpha(*word))
	{
		int args_num;
		int wlen = word_len(word);
		char sym = *(word + wlen);
		*(word + wlen) = '\0';
		check_cmd_word(word, &args_num, line, cmdname);
		*(word + wlen) = sym;
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
	if((*line == ':' || *line == '$' || *line == '*'))
	{
		printf("At line %i : Error! Command name requiered before argument type '%c' was token.\n", l, *line);
		exit(0);
		
	}
	if(isdigit(*line))
	{
		printf("At line %i : Error! Command name and argument type expected before '%c' was token.\a", l, *line);
		exit(0);
	}
	char* cmdname;
	int args_num = isword(&line, l, &cmdname);	
	switch(args_num){
		case 0 :
			check_absence(line, l, cmdname);
			break;
		case -1 :
		{
			printf("At line %i: Error! Command was not parsed.\n", l);
			exit(0);
		}
		default:
		{
			while(!isprint(*line))
				line++;
			is_argument(line, args_num, l);
			break;
		}
	}
	*(beginning + length) = '\n';
	return 1;
}
		
char* prog_check_func(const char* filename)
{
	if(filename == NULL)
	{
		printf("Error! Program file name requested.\n");
		exit(1);
	}
	char* buf_prev     = read_prog(filename);
	char* buf  	   = read_prog(filename);
	size_t length = file_len (filename);
	int strings = prog_lines_count(buf);
	FILE* asmfile = fopen("ASM.r", "wb");
	if(!asmfile)
	{
		printf("OHUENNUY Error! Invalid name of program file.\n");
		exit(1);
	}
	int line = 1;
	while(line <= strings) {
		check_line(buf, line);
		if(line < strings)	
		{
			buf += line_len(buf) + 1; 
		}
		else 
			break;
		line++;
	}
	//*str = buf_prev;
	//printf("progcheck has finished; Read file %s", *str);
	return buf_prev;
}

#endif
