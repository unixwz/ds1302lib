/* Copyright © 2016 Dmitry Buryakov 
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * */
#include "ds1302lib.h"

/* addresses for write */
#define SEC_W_ADDRESS  0x80
#define MIN_W_ADDRESS  0x82
#define HRS_W_ADDRESS  0x84
#define DATE_W_ADDRESS 0x86
#define MNTH_W_ADDRESS 0x88
#define DAY_W_ADDRESS  0x8a
#define YEAR_W_ADDRESS 0x8c
/* addresses for read */
#define SEC_R_ADDRESS  0x81
#define MIN_R_ADDRESS  0x83
#define HRS_R_ADDRESS  0x85
#define DATE_R_ADDRESS 0x87
#define MNTH_R_ADDRESS 0x89
#define DAY_R_ADDRESS  0x8b
#define YEAR_R_ADDRESS 0x8d


/* this function transfers data (register address or value)
 * by 3-wire serial interface
 * parameters:
 * data - transsmission data
 * */
static void transfer_data(uint8_t data)
{
	for (int i = 0; i < 8; ++i) {
		if (data & (1 << i)) {
			WORKING_PORT |= (1 << IO_BIT);
		} else {
			WORKING_PORT &= ~(1 << IO_BIT);
		}
		WORKING_PORT |= (1 << CLK_BIT);
		_delay_us(1);
		WORKING_PORT &= ~(1 << CLK_BIT);
	}
}

/* this function set value in register by address specifies
 * in set_address function 
 *
 * parameters: 
 * data - transsmision data
 * */
static void set_value(uint8_t data)
{
	transfer_data(data);
	/* stop transmission & disable chip */
	WORKING_PORT &= ~(1 << CE_BIT);
	WORKING_PORT &= ~(1 << IO_BIT);
}

/* this function gets saved data from chip 
 * and return the resulting value */
static uint8_t get_value()
{
	uint8_t buffer = 0;
	/* set PIN as output for receive data */
	WORKING_DDR &= ~(1 << IO_BIT);
	for (int i = 0; i < 8; ++i) {
		if (PINB & (1 << IO_BIT))
			buffer |= (1 << i);
		else
			buffer &= ~(1 << i);
		WORKING_PORT |= (1 << CLK_BIT);
		_delay_us(1);
		WORKING_PORT &= ~(1 << CLK_BIT);
	}
	WORKING_PORT &= ~(1 << CE_BIT);
	WORKING_DDR |= (1 << IO_BIT);
	return buffer;
}

/* this function set register address for setting value
 * 
 * parameters:
 * address - register address to be set value,
 * read_enable - type of operation, may be:
 * 0 - write opeartion,
 * 1 - read operation
 * */
static void set_address(uint8_t address, uint8_t read_enable)
{
	WORKING_DDR |= (1 << CE_BIT) | (1 << CLK_BIT) | (1 << IO_BIT);
	WORKING_PORT |= (1 << CE_BIT); /* enable chip */
	_delay_us(1);
	if (!read_enable)
		/* init write operation */
		WORKING_PORT &= ~(1 << IO_BIT);
	else
		/* init read operation */
		WORKING_PORT |= (1 << IO_BIT);
	/* transfer register address */
	_delay_us(1);
	transfer_data(address);
	_delay_us(1); 
	/* specifies transfer or receive data on clock/calendar instead RAM */
	WORKING_PORT &= ~(1 << IO_BIT); 
	_delay_us(1); 	
	/* this bit must be a logic 1 for work chip */
	WORKING_PORT |= (1 << IO_BIT); 
	_delay_us(1); 
	/* set PIN to logic 0 */
	WORKING_PORT &= ~(1 << IO_BIT);
	_delay_us(1); 	
}

/* this function sets time to the register.
 *
 * parameters:
 * value - value which will set to register
 * value_type - type of value, may be:
 * s - seconds,
 * m - minutes,
 * h - hours.
 * t_format - format of time (12 if t_format = 1 otherwise 24),
 * t_day - time of days (PM if t_day = 1 otherwise AM).
 * */
void ds1302_set_time(uint8_t value, char value_type, uint8_t t_format,
					 uint8_t t_day)
{
	switch(value_type)
	{
		/* set 7 bit in 0 for enable chip */
		value &= ~(1 << 7);
		case 's':
			set_address(SEC_W_ADDRESS, 0);
			break;
		case 'm':
			set_address(MIN_W_ADDRESS, 0);
			break;
		case 'h':
			/* set time format 12 or 24 */
			if (t_format) {
				value |= (1 << 7);
				/* set time of days AM or PM
				 * will work only if uses 
				 * 12-hours time format */
				if (t_day)
					value |= (1 << 5);
				else
					value &= ~(1 << 5);
			} else {
				value &= ~(1 << 7);
			}
			set_address(HRS_W_ADDRESS, 0);
			break;
		default:
			break;
	}
	set_value(value);
}

/* this function sets date to the register
 *
 * parameters:
 * value - value which will set to register
 * value_type - type of value, may be:
 * d - date,
 * m - month,
 * w - day of week,
 * y - year
 * */
void ds1302_set_date(uint8_t value, char value_type)
{
	switch(value_type)
	{
		case 'd':
			set_address(DATE_W_ADDRESS, 0);
			break;
		default:
			break;
	}
	set_value(value);
}

/* this function get time from chip 
 * parameters:
 * struct_t - user structur for storage recieved data
 * */
void ds1302_get_time(ds1302time_t *struct_t)
{
	uint8_t buffer = 0;
	/* read seconds */
	set_address(HRS_R_ADDRESS, 1);
	buffer = get_value();
	struct_t->hour = buffer;
	_delay_ms(10);
	/* FIXME: Value comes with options 24/12 PM/AM etc... fix please 
	 * 82 = 10000010 where 0000010 - value and 1 - option */
	ds1302_set_time(struct_t->hour, 'm', 1, 0);
}
