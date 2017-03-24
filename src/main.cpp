#include <iostream>
#include <malloc.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <assert.h>
#include "STACK_SELFMADE.H"
#include "ASM.H"

using namespace std;

class CPU
{
public:
	//CPU constructors
	CPU();
	explicit CPU(size_t capacity);
	~CPU();

	Stack  data_stack;
	int ax, bx, cx, dx;
private:
	int ip;
	int RAM[MAX_STR_LEN];
};

CPU::CPU():
    data_stack (0),
	ax (0),
	bx (0),
	cx (0),
	dx (0),
	ip (0)
	{}

CPU::CPU(size_t capacity):
	ax (0),
	bx (0),
	cx (0),
	dx (0),
	ip (0)
	{
		data_stack.Create(capacity);
	}

CPU::~CPU()
{
	printf("End of working\n");
	//s  free(data_stack.data_);
}

int main()
{
	int n;
	printf("Enter CPU stack_capacity:\n");
	scanf("%d", &n);
	CPU prots(n);

	Dump(prots.data_stack);

	return 0;
}
