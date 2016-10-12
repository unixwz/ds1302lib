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
 
## Settings
For settings of library, you can edit header file (ds1302lib.h) which sets some parameters:
- WORKING_DDR - Address or constant of DDR register (DDRB, DDRC etc.)
- WORKING_PORT - Address or constant of PORT register (PORTB, PORTC etc.)
- WORKING_PIN - Address or constant of PORT register (PINB, PINC etc.)
- CLK_BIT - Bit number on WORKING_PORT for sending synch signal.
- CE_BIT - Bit number on WORKING_PORT for sending control signal.
- IO_BIT - Bit number on WORKING_PORT for sending and recieve data.
- TIME_FORMAT - Time format, 24 (const=0) or 12 (const=1).
- TIME_OF_DAY - Time of day, AM (const=0) or PM (const=1).

## Examples
```c
int main()
{
    /* variable (struct) for exchange data 
     * with chip */
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
