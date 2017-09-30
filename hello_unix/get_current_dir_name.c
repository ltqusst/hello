#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	printf("Micro _GNU_SOURCE is %s\n",
#ifdef _GNU_SOURCE
	"Defined, there are no compile warnings"
#else
	"Undefined, you can see compilation warnings"
#endif
	);

	printf("get_current_dir_name()=%s\n", get_current_dir_name());
	exit(0);
}
