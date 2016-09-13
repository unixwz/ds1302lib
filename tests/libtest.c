#include "../src/ds1302lib.h"

int main()
{
	ds1302_set_time(8, 'h', 0, 0);
	ds1302_set_date(8, 'd');
	return 0;
}
