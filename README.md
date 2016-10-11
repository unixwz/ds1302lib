## DS1302lib
C library for work with ds1302 RTC chip.

The library it's two functions for read and write data from/to ds1302 chip.

## Read data from DS1302RTC
ds1302_get_time() - provides read data from chip.

This function takes one parameter - pointer to structure (a type ds1302time_t) for storage data. 

Function write data to struct, you can work with this values.

## Write data to DS1302RTC
ds1302_set_time() - provide write data to chip.

This function takes two parameters:
- value - value which will set to register;
- value_type - type of value, may be:
  - sec - seconds,
  - min - minutes,
  - hrs - hours,
  - day - date,
  - mnt - month,
  - wd - day of week,
  - yr - year.
  
## Examples
```c
int main()
{
    /* variable (struct) for exchange data 
     * with library */
	ds1302time_t ds1302time;
    /* setting time and date on ds1302 */
    ds1302_set_time(0, "sec");
	ds1302_set_time(57, "min");
	ds1302_set_time(20, "hrs");
	ds1302_set_time(11, "date");
	ds1302_set_time(10, "mnt");
	ds1302_set_time(16, "year");
	while (1) {
      /* reading data from ds1302 */
      ds1302_get_time(&ds1302time);
      /* ... your code ... */
      /* displaying values */
      draw_clock(ds1302time.hour, ds1302time.min, 0);
      /* ... your code ... */
	}
	return 0;
}
```
