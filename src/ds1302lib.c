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

/* this function transfers data (register address or value)
 * by 3-wire serial interface
 * parameters:
 * data - transsmission data,
 * value_type - type of transsmit value, may be:
 * -1 - not defined,
 * 0 - time,
 * 1 - date
 * */
static void transfer_data(uint8_t data, uint8_t value_type)
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
 * value_type - type of transsmit value, may be:
 * -1 - not defined,
 * 0 - time,
 * 1 - date
 * */
static void set_value(uint8_t data, uint8_t value_type)
{
	transfer_data(data, value_type);
	/* stop transmission & disable chip */
	WORKING_PORT &= ~(1 << CE_BIT);
}

/* this function set register address for setting value
 * 
 * parameters:
 * address - register address to be set value,
 * read_enable - type of operation, may be:
 * 0 - write opeartion,
 * 1 - read operation
 * */
static void set_address(uint8_t address, char read_enable)
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
	transfer_data(address, -1); 
	/* specifies transfer or receive data on clock/calendar instead RAM */
	WORKING_PORT &= ~(1 << IO_BIT); 	
	/* this bit must be a logic 1 for work chip */
	WORKING_PORT |= (1 << IO_BIT); 
	/* set PIN to logic 0 */
	WORKING_PORT &= ~(1 << IO_BIT);	
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
void ds1302_set_time(uint8_t value, char value_type, uint8_t t_format, uint8_t t_day)
{
	switch(value_type)
	{
		case 's':
			set_address(0x80, 0);
			break;
		case 'm':
			set_address(0x82, 0);
			break;
		default:
			break;
	}
	set_value(value, 0);
}

/* this function sets date to the register
 *
 * parameters:
 * value - value which will set to register
 * value_type - type of value, may be:
 * d - date,
 * m - month
 * w - day of week
 * y - year
 * */
void ds1302_set_date(uint8_t value, char value_type)
{
}

/* this function get time from chip */
void ds1302_get_time(ds1302time_t *struct_t)
{
	struct_t->min = 8;
	struct_t->hour = 5;
}
