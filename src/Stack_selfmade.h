//This is a selfmade lib and stack backup at the same time

#ifndef _CLASS_STACK_INCLUDED_
#define _CLASS_STACK_INCLUDED_

#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <assert.h>

#define Govnocode
#define MAX_STR_LEN 100

//Poisons
#define PSN_DBL 3.14159265
enum {PSN_INT = 0xDEADFACE,};

//Error codes
enum STKERRS
{
	STKERR_OK,
	STKERR_OVERFLOW,
	STKERR_EMPTY,
	STKERR_NULLDATAPTR,
	SKERR_CANARY_CALLS,
};       

typedef double StkData;

class Stack
{
    public:
        //Initializes fields of the 'Stack' class
        Stack();
        explicit Stack(size_t capacity);
        ~Stack();

	int	 Create (size_t capacity);
        int      Push   (double value   );
        StkData  Pop    (int* errcode   );
        StkData  Peek   (int* errcode   );
        int      getSize();
        int      getCapacity();
        int      Empty  ();
        int      Print  ();
        int      Add    (int* errcode);
        int      Mul    (int* errcode);
        int      Dump_  (const char* stackName);
        StkData* getDataPtr();
        void     ReadExpression();
        int      Sub    (int* errcode);
        int      Div    (int* errcode);
        char*    ReadExpression (int length, char* expression);
        void     CountExpression(int length, char* express, int* errcode);

    private:
        int          IsOk();
        double       getCheckSum();
        int          Extend();
	size_t 	     err_mask;
        unsigned int canary1_;
        double*      data_canary_left_;
        double*      data_canary_right_;
        StkData*     data_;
        size_t       counter_;
        size_t       capacity_;
        size_t       extender_;
        double       check_sum_;
        unsigned int canary2_;
};

Stack::Stack():
	err_mask          (0),
    	canary1_          (0),
	data_canary_left_ (0),
	data_canary_right_(0),
	data_             (0),
	counter_          (0),
	capacity_         (0),
	extender_         (0),
	check_sum_        (0),
	canary2_          (0)
    {}


#define INIT_IN_CTOR_AND_CREATOR

Stack::Stack(size_t capacity):
    	canary1_  (PSN_INT),
    	counter_  (0),
    	capacity_ (capacity),
    	canary2_  (PSN_INT)
    	{
        	data_ = (double*) malloc ((capacity + 2) * sizeof(double)); // Finding memory for requested stack and two canaries
	        data_canary_left_  = data_;
	        data_canary_right_ = data_ + capacity_ + 1;                 // Initializing right data canary
	        *data_canary_left_ = *data_canary_right_ = PSN_DBL;
	        data_++;
	        check_sum_         = getCheckSum();
	}


int Stack::Create(size_t capacity)
{
	printf("SPEAKING FROM CREATE\n");
	if (!data_ || !capacity_ || !check_sum_)
	{
		printf("Stack created but not Initialized\nInitializing...\n");
		canary1_  = canary2_ = PSN_INT;
    		counter_  = 0;
    		capacity_ = capacity;
	       	data_ = (double*) malloc ((capacity + 2) * sizeof(double)); // Finding memory for requested stack and two canaries
	        data_canary_left_  = data_;
	        data_canary_right_ = data_ + capacity_ + 1;                 // Initializing right data canary
	        *data_canary_left_ = *data_canary_right_ = PSN_DBL;
	        data_++;
	        check_sum_         = getCheckSum();
	        return 0;
	}
	printf ("Stack was intialized earlier\n");
	return -1;

}

Stack::~Stack()
{
	 free(data_);
}

double* Stack::getDataPtr()
{
   	 IsOk();
   	 return data_;
}

#define POPEEK_FUNC(funcName, errString, index)     \
double Stack::funcName(int* errcode)                \
{                                                   \
	IsOk();                                     \
	if ( !getSize() )                           \
	{                                           \
		printf(errString);                  \
		*errcode = STKERR_EMPTY;            \
		return -1;                          \
    	}                                           \
    	*errcode           = STKERR_OK;             \
    	double popped_data = data_[index];          \
    	check_sum_         = getCheckSum();         \
    	IsOk();                                     \
    	return popped_data;                         \
}
POPEEK_FUNC(Pop,  "There's nothing to pop\n",  --counter_  )
POPEEK_FUNC(Peek, "There's nothing to peek\n", counter_ - 1)

#undef POPEEK_FUNC

#define MATH_FUN(namefunc, errString, act)    \
int Stack::namefunc(int* errcode)             \
{                                             \
    IsOk();                                   \
    if (counter_ <= 1)                        \
    {                                         \
        printf(errString);                    \
        *errcode = STKERR_EMPTY;              \
        return -1;	                      \
    }                                         \
    *errcode = STKERR_OK;                     \
    Push(Pop(errcode) act Pop(errcode));      \
    check_sum_ = getCheckSum();               \
    return IsOk();                            \
}

MATH_FUN(Add, "Cannot perform addition.\n",       +)
MATH_FUN(Mul, "Cannot perform multiplication.\n", *)
MATH_FUN(Sub, "Cannot perform subtraction.\n",    -)
MATH_FUN(Div, "Cannot perform division.\n",       /)

#undef MATH_FUN


int Stack::Push(double value)
{
	IsOk();
	if (counter_ + 1 >= capacity_)
    	{
       		printf("Stack overflow. Calling for 'Extend'\n");
        	Extend();
    	}
    	data_[counter_++] = value;
    	check_sum_        = getCheckSum();
    	return IsOk();
}

int Stack::Print()
{
	IsOk();
	printf("Stack_data: |");
	for (unsigned int i = 0; i < capacity_; i++)
		printf(" %lg |", data_[i]);
	printf("\n");
	return IsOk();
}

int Stack::getSize()
{
	IsOk();
	return counter_;
}

int Stack::getCapacity()
{
	IsOk();
	return capacity_;
}

int Stack::Empty ()
{
	IsOk();
	for (unsigned int i = 0; i < counter_; data_[i++] = 0);
	counter_   = 0;
	check_sum_ = getCheckSum();
	return IsOk();
}

int Stack::Dump_(const char stackName[MAX_STR_LEN])
{
	printf("================================================\n");
	IsOk  ();
	printf("Stack %s is %s\n", stackName, (!IsOk())?"OK":"!OK");
	printf("\t{\n");
	printf("\tcanary1_ is           %i\n", 	 canary1_          );
	printf("\tdata_canary_left_ is  %p\n",   data_canary_left_ );
	printf("\tdata_canary_right_ is %p\n",   data_canary_right_);
	printf("\tcounter_ is           %lu\n",  counter_          );
	printf("\tcapacity_ is          %lu\n",  capacity_         );
	printf("\tdata_ is              %p\n",	 data_             );
	printf("\tcheck_sum_ is         %lg\n",  check_sum_        );
	printf("\tcanary2_ is           %i\n\n", canary2_          );
	printf("\tStack content:\n");
	for (size_t i = 0; i < capacity_; i++)
		printf("\t\t%c [%lu]: %lg\n", (i < counter_)?42:32, i, (i < counter_)?data_[i]:0);

	printf("\t}\n");
	printf("================================================\n");
	IsOk();
	return 1;
}

#define Dump(stackname)                      \
{                                            \
    assert(strlen(#stackname) < MAX_STR_LEN);\
    stackname.Dump_(#stackname);             \
}

int Stack::Extend()
{
	IsOk();
	capacity_ *= 2;
	data_ = (double*) realloc ( --data_ , (capacity_ + 2) * sizeof (*data_) ); // copying stack with canaries
	if (data_ == NULL)
	{
		printf("Error finding memory.\n");
		return -1;
	}
	data_canary_left_  = data_;
	data_canary_right_ = data_ + capacity_ + 1; // Initializing right data canary
	*data_canary_left_ = *data_canary_right_ = PSN_DBL;
	data_++;
	check_sum_         = getCheckSum();
	return IsOk();
}

double Stack::getCheckSum()
{
	double sum =
	capacity_  	 +
	(size_t)data_ 	 +
	counter_   	 +
	canary1_  	 +
	canary2_  	 +
	(size_t)data_canary_left_  +
	(size_t)data_canary_right_ +
	*data_canary_left_         +
	*data_canary_right_;

	for (unsigned int i = 0; i < counter_; sum += data_[i++]);
	return sum;
}

int Stack::IsOk()
{
	if (canary1_ != PSN_INT || canary2_ != PSN_INT)
	{
		printf("Structure canary calls.\n");
	}

	if (*data_canary_left_ != PSN_DBL)
	{
		printf("data_canary_left_ in stack calls.\n");
	}
	if (*data_canary_right_ != PSN_DBL)
	{
		printf("data_canary_right_ in stack calls.\n");
	}
	if (getCheckSum() != check_sum_)
	{
		printf("Check_sum_ is wrong.\n");
		exit(1);
	}
	return STKERR_OK;
}

char* Stack::ReadExpression(int length, char* expression)
{
	scanf("%s", expression);
	return expression;
}

void Stack::CountExpression(int length, char* express, int* errcode)
{
	int i = 0;
	while (i < length)
	{
		if(express[i] >= '0' && express[i] <= '9')
		    	Push(express[i] - '0');

		switch(express[i])
		{
#define OPERCASE(funcname, actsym)\
case actsym:                      \
funcname(errcode);          	  \
break;
			OPERCASE(Add, '+')
			OPERCASE(Sub, '-')
			OPERCASE(Mul, '*')
			OPERCASE(Div, '/')
#undef OPERCASE
		}
		i++;
	}
}

#endif

