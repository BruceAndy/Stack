#include <stdio.h>
#include <stack_selfmade.h>

using namespace std;

class CPU
{
public:
	CPU();
	~CPU();
private:
	Stack stack(0);
	char* progMem;
	int   Registers;
}
