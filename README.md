## DS1302lib
C library for work with ds1302 RTC chip.

The library it's two functions for read and write data from/to ds1302 chip.

## Read data from DS1302RTC
ds1302_get_time() - provides read data from chip.
This function takes one parameter - pointer to structure (a type ds1302time_t) for storage data. Function write data to struct, you can work with this values.

## Write data to DS1302RTC
ds1302_set_time() - provide write data to chip.
This functio takes twi parameters:
	- value - value which will set to register;
	- value_type - type of value, may be:
	sec - seconds,
	min - minutes,
	hrs - hours,
	day - date,
	mnt - month,
	wd - day of week,
	yr - year.


