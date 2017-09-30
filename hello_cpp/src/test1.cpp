#include <stdio.h>
#include <vector>

int main(int argc, char * argv[])
{
	std::vector<int> val={1,2,3,4,5};
	//int val[10]{};//{1,2,3,4,5};
	printf("sizeof(val)=%lu, val=%d,%d,%d,...\n", sizeof(val),val[0],val[1],val[2]);
}

