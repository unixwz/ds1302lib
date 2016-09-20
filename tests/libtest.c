#include "../src/ds1302lib.h"

int main()
{
	ds1302time_t ds1302time;
	// ds1302_set_time(0x09, 'h', 1, 0);
	// ds1302_set_date(8, 'd');
	_delay_us(5);
	ds1302_get_time(&ds1302time);
	return 0;
}
