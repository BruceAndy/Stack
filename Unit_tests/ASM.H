#ifndef _ASSEMBLER_H_INCLUDED_
#define _ASSEMBLER_H_INCLUDED_

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

enum CPU_CMD_ARGS_NUMBER
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
		printf("Cannot find memory to read file.\n");
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
		ARGCASE(':', 1) //recieving register number
		ARGCASE('$', 2) //recieving constant argument
		ARGCASE('*', 3) //recieving... uuhh... ptr? (METKA)
		}
#undef ARGCASE
	}
	return 0;	
}

void prog_cmd_identifier(char* buf, FILE* asmfile) //Reads the command name and writes its number in file
{
#define STR_COMPARING
#define CPU_CMD(cmdname)								   	\
if (!strcmp(#cmdname, buf))								   	\
{											   	\
	fprintf(asmfile, "%d", CPU_##cmdname);						   	\
	fprintf(asmfile, "%d", CPU_##cmdname##_ARGS_NUMBER);				   	\
	if (CPU_##cmdname##_ARGS_NUMBER)							\
	{											\
		parse_arguments(buf + word_len(buf) + 1, CPU_##cmdname##_ARGS_NUMBER, asmfile); \
	}										        \
}
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef STR_COMPARING	
}

FILE* prog_cmd_parser(const char* filename) 	//Finds command and puts '\0' at the end of it
{
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
	while(i < length)
	{
		if ( (buf[i] >= 'A' && buf[i] <= 'Z') || (buf[i] >= 'a' && buf[i] <= 'z') )
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

#endif
