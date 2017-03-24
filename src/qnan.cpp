#include <stdio.h>
#include <limits>

int main()
{
	double naN = (std::numeric_limits<double>::quiet_NaN());
	printf("0.0/0.0 = %lg\n", naN);
	if(naN != naN)
		printf("AHUET\n");
	return 0;
}
