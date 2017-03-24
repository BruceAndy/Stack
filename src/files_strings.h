#ifndef _FILE_WORKING_LIBRARY_INCLUDED_
#define _FILE_WORKING_LIBRARY_INCLUDED_

void skip_space(char** ptr)
{
	while(**ptr == ' ' || **ptr == '\t')
		(*ptr)++;
}

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

	char * buf  = (char*) calloc (filesize + 1, sizeof(char));
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

int line_len(char* ptr)
{
	int counter = 0;
	for(int j = 0; (ptr[j] != '\n'); j++) 
		counter++;
	return counter;
}

#endif
