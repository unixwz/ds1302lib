#include <stdio.h>
#include <time.h>

int main()
{
	time_t current_time;
	char* time_str;
	current_time = time(NULL);
	printf("%i\n", current_time);
    time_str = ctime(&current_time);	
	printf("%s", time_str);
	return 0;
}
