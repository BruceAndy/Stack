#include <stdio.h>
#include <cctype>
#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include "files_strings.h"

#define MAX_LBL_NAME 25

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

struct Label
{
	char*  name;
	int    addr;
};


class Assembler
{
public:
	Assembler (const char* filename);
	~Assembler();
	void   prog_parsing();
private:
	void   prog_check_func ();
	FILE*  binFile_;
	FILE*  asmFile_;
	char*  prog_str_buf_;
	size_t strs_num_;
	Label* labels_;
	int    cur_line_;
	size_t lbl_ptr_; // label pointer
	/* checking part */
	int    check_label     ();
	void   skip_comment    ();
	int    create_lbl_arr  ();
	int    check_line      ();
	int    isword          (char** cmdname);
	int    check_cmd_word  (int*   arg_number, char** cmd);
	int    check_absence   (char* cmdname);
	int    is_argument     (int num, char* cmdname);
	void   print_labels    ();
	void   seek_eoline     ();
	void   lbl_arr_d_tor   ();
	/* inventory part */ 
	void   skip_space      ();			
	size_t prog_lines_count();
	void   read_prog       ();
	int    word_len	       ();
	int    arg_len         ();
	int    line_len        ();	
	size_t file_len        ();
	/* Assembling part */	
	int    lbl_paster      ();
	int    parse_label     ();
	int    parse_line      ();
	int    isword          ();
	int    read_cmd_word   (int* args_number, int* cmdnum);
	int    parse_arguments (int num);	
	void   fprint_arg      ();
};

Assembler::Assembler(const char* filename):
	cur_line_ (0)
{
	asmFile_ = fopen(filename, "r");
	binFile_      = fopen("Assembled_file.b", "wb");
	read_prog();
	strs_num_	= prog_lines_count();
	create_lbl_arr();
	cur_line_ = 0;
}

Assembler::~Assembler()
{
	fclose(asmFile_);
	fclose(binFile_);
	free(prog_str_buf_);
	lbl_arr_d_tor();
}

void Assembler::lbl_arr_d_tor()
{
	for(int i = 0; i < lbl_ptr_; i++)
		free(labels_[i].name);
	free(labels_);
}

void Assembler::skip_comment() // i'm too lazy to put this function in each suitable place
{
	skip_space();
	if(*prog_str_buf_ == ';')
	{
		seek_eoline();
		prog_str_buf_++;
	}
}

int Assembler::create_lbl_arr() // creates label array (how is that possible??)
{
	size_t strs_num_ = prog_lines_count();
	labels_ = (Label*) calloc (strs_num_, sizeof(Label));
	lbl_ptr_ = 0;
	for(int i = 0; i < strs_num_; i++)
	{
		labels_[i].addr = -1;
		labels_[i].name = (char*) calloc (MAX_LBL_NAME + 1, sizeof(char));
	}
	return 1;
}

void Assembler::print_labels() //prints label array
{
	printf("\nLabels:\n");
	for(int i = 0; i < lbl_ptr_; i++)
		printf("Label %i: '%s'\n", i, labels_[i].name);
	printf("\n");
}


int Assembler::parse_label() // remembers openning label address
{
	int llen = word_len();
	char sym = prog_str_buf_[llen];
	prog_str_buf_[llen] = '\0';
	labels_[lbl_ptr_].name = (char*) calloc (llen + 1, sizeof(char));
	sscanf(prog_str_buf_, "%s", labels_[lbl_ptr_].name);
	labels_[lbl_ptr_].name[llen] = '\0';
	labels_[lbl_ptr_].addr = cur_line_;
	lbl_ptr_++;
	cur_line_--; 	    // equal to staying at current line
	strs_num_--;        // label line doesn't matter
	prog_str_buf_[llen] = sym;
	skip_comment();
	return 1;
}

int Assembler::check_label() // skips labeled argument in first read
{
	if(word_len() > MAX_LBL_NAME)
	{
		printf("At line %i: Error! Too big name for another little label.\n", cur_line_);
		exit(1);
	}
	prog_str_buf_ += word_len();
	return 1;	
}

int Assembler::is_argument(int num, char* cmdname) // checks the argument or/and its number
{
	for(int i = 0; i < num; i++)
	{
		if(*prog_str_buf_ == ',')
		{
			prog_str_buf_++;
			skip_space();
		}
		if(isalpha(*prog_str_buf_))
		{
			check_label();
			continue;
		}
		switch(*prog_str_buf_)
		{	 
#define ARGCASE(argsym)							 		    	      		\
case argsym :								 		    	      		\
	prog_str_buf_++;						 		    	      		\
	if(!isdigit(*prog_str_buf_) && !isalpha(*prog_str_buf_))	  		    	      		\
	{								 		    	      		\
		printf("At line %i : Error! Argument value requiered after '%c' token.\n", cur_line_, *(prog_str_buf_ - 1));  \
		exit(0);											\
	} else {                                                                                               \
		prog_str_buf_ += arg_len();										\
		break;												\
	}
		ARGCASE('%') 
		ARGCASE('$')
		case '\0':
			printf("At line %i: Error! Too few arguments for command %s.\n", cur_line_, cmdname);
			exit(1); 
		case '\n':
			printf("At line %i: Error! Too few arguments for command %s.\n", cur_line_, cmdname);
			exit(1); 
		default:	
			printf("At line %i: Error! Unknown argument specifier '%c'.\n", cur_line_, *prog_str_buf_);
			exit(1);
#undef ARGCASE
		}
	}
	skip_comment();
	return	1;
}

int Assembler::check_absence(char* cmdname) // checks if command with no args recieved no args
{
	prog_str_buf_ += word_len();
	skip_comment();
	if(*prog_str_buf_ != '\n' && *prog_str_buf_ != '\0')
	{
		printf("At line %i: Error! Too many arguments for command %s.\n", cur_line_, cmdname);
		exit(1);
	}
	return 0;
}

int Assembler::check_cmd_word(int* args_number, char** cmd) //Reads the command name
{
#define CPU_CMDS
#define CPU_CMD(cmdname, argnum)			      \
if (!strcmp(#cmdname, prog_str_buf_))		   	      \
{							      \
	*args_number = CPU_##cmdname##_ARGS_NUMBER;	      \
	*cmd = (char*) calloc(word_len() + 1, sizeof(char));  \
	sscanf(prog_str_buf_, "%s", *cmd);		      \
	(*cmd)[word_len() + 1] = '\0';			      \
	return 1;					      \
} else
#include "CPU_ACTIONS.h"
#undef CPU_CMD
#undef CPU_CMDS	
	{
		printf("At line %i: Error! Unknown command '%s'.\n", cur_line_, prog_str_buf_);
		exit(1);
	}
	return 1; 
}

int Assembler::isword(char** cmdname) // If word is ok returns number of arguments
{
	if(isalpha(*prog_str_buf_))
	{
		int args_num;
		int wlen = word_len();
		if(*(prog_str_buf_ + wlen) == ':')
		{
			parse_label();
			prog_str_buf_ += wlen;
			return -2;
		}
	char sym = *(prog_str_buf_ + wlen);
		*(prog_str_buf_ + wlen) = '\0';
		check_cmd_word(&args_num, cmdname);
		*(prog_str_buf_ + wlen) = sym;
		prog_str_buf_ += wlen;
		return args_num;
	} else
		return -1;
}

int Assembler::check_line() //checks the line (Captain)
{
	skip_space();
	if(*prog_str_buf_ == ';')
	{
		skip_comment();
		cur_line_--;
		return 0;
	}
	int length = line_len();
	if((*prog_str_buf_ == ':' || *prog_str_buf_ == '$' || *prog_str_buf_ == '*'))
	{
		printf("At line %i : Error! Command or label name requiered before argument type '%c' token.\n", cur_line_, *prog_str_buf_);
		exit(0);
		
	}
	if(isdigit(*prog_str_buf_))
	{
		printf("At line %i : Error! Command name and argument type expected before '%c' token.\a", cur_line_, *prog_str_buf_);
		exit(0);
	}
	char* cmdname;
	int args_num = isword(&cmdname);	
	switch(args_num)
	{
		case 0 :
			check_absence(cmdname);
			break;
		case -1 :
			printf("At line %i: Error! Command was not parsed.\n", cur_line_);
			exit(0);
		case -2 :
			break;
		default:
		{
			skip_space();
			is_argument(args_num, cmdname);
			break;
		}
	}
	return 0;
}
		
void Assembler::prog_check_func()
{
	while(cur_line_ < strs_num_)
	{
		check_line();
		if(cur_line_ < strs_num_ - 1)	
		{
			seek_eoline();
			prog_str_buf_++; 
		}
		else 
			break;
		cur_line_++;
	}
	read_prog();
	printf("IT'S CHECKED CONGRATULATIONS!\n");
}



/* inventory part */



void Assembler::skip_space()
{
	while(*prog_str_buf_ == ' ' || *prog_str_buf_ == '\t')
		prog_str_buf_++;
}

size_t Assembler::file_len() //returns file length
{
	fseek(asmFile_, 0, SEEK_END);
	size_t filesize = ftell(asmFile_);
	rewind(asmFile_);	
	return filesize; 
}

size_t Assembler::prog_lines_count()    //Counts number of lines in users program
{
	char * prevstrend = prog_str_buf_;
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

void Assembler::read_prog()  //Reads users program and writes it in string 
{
	size_t filesize = file_len();
	prog_str_buf_  = (char*) calloc (filesize + 1, sizeof(char));
	if(!prog_str_buf_)
	{
		printf("Error! Cannot find memory to read file.\n");
		exit(1);
	}
	fread(prog_str_buf_, sizeof(char), filesize, asmFile_);
}

int Assembler::word_len() // returns length of a command
{
	int counter = 0;
	int i = 1;
	for (int i = 0; isalpha(prog_str_buf_[i]); i++)
		counter++;
	return counter;
}

int Assembler::arg_len() //returns argument length
{
	int counter = 0;
	int i       = 0;
	while ( isdigit(prog_str_buf_[i]) || (prog_str_buf_[i] == '.'))
	{
		i++;
		counter++;
	}
	return counter;
}

int Assembler::line_len()
{
	int counter = 0;
	for(int j = 0; (prog_str_buf_[j] != '\n'); j++) 
		counter++;
	return counter;
}

void Assembler::seek_eoline()
{
	while(*prog_str_buf_ != '\n')
		prog_str_buf_++;
}



/* Assembling part */



int Assembler::lbl_paster() //Finds label name returns its address and moves file string ptr
{
	size_t wlen = word_len();
	char sym = prog_str_buf_[wlen];
	prog_str_buf_[wlen] = '\0';
	for(int i = 0; i < strs_num_; i++)
	{
		if(!strcmp(labels_[i].name, prog_str_buf_))
		{
			prog_str_buf_[wlen] = sym;
			prog_str_buf_ += wlen;
			return labels_[i].addr;
		}
	}
	return -1;
}


#define ARGCASE(argSym, argTypeNum)		 		  \
case argSym :					 		  \
	fprintf(binFile_, "%c", argTypeNum);	 		  \
	prog_str_buf_++;					  \
	alen = arg_len();			 		  \
	sym = prog_str_buf_[alen];		 		  \
	prog_str_buf_[alen] = '\0';		 		  \
	sscanf(prog_str_buf_, "%lg", &argument);		  \
	fwrite(&argument, sizeof(char), sizeof(double), binFile_);\
	prog_str_buf_[alen] = sym;			 	  \
	prog_str_buf_ += alen;					  \
	break;

int Assembler::parse_arguments(int num) //divides arguments and writes them in file
{
	prog_str_buf_ += word_len();
	skip_space();
	int alen;
	char sym;
	double argument;
	for(int i = 0; i < num; i++)
	{
		if(*prog_str_buf_ == ',')
			prog_str_buf_++;
		skip_space();	
		if(isalpha(*prog_str_buf_))
		{
			int arg_addr = lbl_paster();
			if(arg_addr == -1)
			{
				printf("At line %i: Error! Unknown label name was taken.\n", cur_line_);
				exit(1);
			} else {
				fprintf(binFile_, "%c", arg_addr);
				continue;
			}
		}
		switch(*prog_str_buf_)
		{			
			ARGCASE('%', 1) 
			ARGCASE('$', 2) 	
		}
#undef ARGCASE
	}
	skip_comment();
	return 0;	
}


int Assembler::read_cmd_word(int* args_number, int* cmdnum) //Reads the command name and number of arguments
{
	char* cmd = (char*) calloc (word_len(), sizeof(char));
	sscanf(prog_str_buf_, "%s", cmd);
#define CPU_CMDS
#define CPU_CMD(cmdname, argnum)			       \
if (!strcmp(#cmdname, cmd))			   	       \
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

int Assembler::isword() // Writes command number in file and returns number of arguments
{
	int cmdnum;
	if(isalpha(*prog_str_buf_))
	{
		int argnum;
		int wlen = word_len();
		if(*(prog_str_buf_ + wlen - 1) == ':')
			return 0;		//skipping line
		read_cmd_word(&argnum, &cmdnum);
		fprintf(binFile_, "%c", cmdnum);
		return argnum;
	} else
		return -1;
}

int Assembler::parse_line()
{
	skip_comment();
	int args_num = isword();	
	if(args_num > 0)
		parse_arguments(args_num);
	skip_comment();
	return 1;
}
	
void Assembler::prog_parsing()
{
	prog_check_func();
	strs_num_     = prog_lines_count() - lbl_ptr_;
	if(!binFile_) 
	{
		printf("Error! Cannot create binary file.\n");
		exit(1);
	}
	cur_line_ = 0;
	while(cur_line_ < strs_num_) 
	{
		parse_line();
		seek_eoline();
		if(cur_line_ < strs_num_) 
		{
			prog_str_buf_++; 
			cur_line_++;
		} else {
			break;
		}
	}
	printf("Check up 'Assembled_file.b'\n");
}

int main(int argc, char* argv[]) 
{	
	Assembler badass(argv[1]);
	badass.prog_parsing();
}
