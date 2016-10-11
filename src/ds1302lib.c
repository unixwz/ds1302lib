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

/* length of str */
uint8_t len(char *str)
{
	uint8_t i = 0;
	for (i = 0; str[i] != '\0'; ++i)
		++i;
	return i;
}

/* compares two string */
uint8_t str_cmp(char *str, char *pattern)
{
	for (int i = 0; (str[i] != '\n' && pattern[i] != '\n') && (str[i] == pattern[i]); ++i)
		if (i == len(str))
			return 1;
	return 0;
}

/* this function convert BCD code 
 * to DEC code */
uint8_t bcd_to_dec(uint8_t value)
{
    uint8_t dec_num = 0;
    dec_num = ((value & 0xf0 ) >> 4) * 10 + (value & 0x0f);
    return dec_num;
}

/* this function convert DEC code 
 * to BCD code */
uint8_t dec_to_bcd(uint8_t value)
{
    uint8_t bcd_num = 0;
    bcd_num = ((value / 10) * 16) + (value % 10);
    return bcd_num;
}

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
	/* specifies transfer or receive data on clock/calendar 
	 * instead RAM */
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
 * sec - seconds,
 * min - minutes,
 * hrs - hours,
 * date - your date,
 * mnt - month,
 * wd - day of week,
 * year - your year.
 * */
void ds1302_set_time(uint8_t value, char *value_type)
{
	if (value_type && value) {
		if (str_cmp(value_type, "sec"))
			set_address(SEC_W_ADDRESS, 0);
		else if (str_cmp(value_type, "min"))
			set_address(MIN_W_ADDRESS, 0);
		else if (str_cmp(value_type, "hrs")) {
			if (TIME_FORMAT) {
				value |= (1 << 7);
				/* set time of days AM or PM
				 * will work only if uses 
				 * 12-hours time format */
				if (TIME_OF_DAY)
					value |= (1 << 5);
				else
					value &= ~(1 << 5);
			} else {
				value &= ~(1 << 7);
			} 
			set_address(HRS_W_ADDRESS, 0);
		} 
		else if (str_cmp(value_type, "date"))
			set_address(DATE_W_ADDRESS, 0);
		else if (str_cmp(value_type, "mnt"))
			set_address(MNTH_W_ADDRESS, 0);
		else if (str_cmp(value_type, "day"))
			set_address(DAY_W_ADDRESS, 0);
		else if (str_cmp(value_type, "year"))
			set_address(YEAR_W_ADDRESS, 0);
		set_value(dec_to_bcd(value));
	}
}

/* this function get time from chip 
 * parameters:
 * struct_t - user structur for storage recieved data
 * */
void ds1302_get_time(ds1302time_t *struct_t)
{
    uint8_t buffer = 0x00;
    
    /* read sec */
    set_address(SEC_R_ADDRESS, 1);
    buffer = get_value();
    /* set 7 bit in 1 which set for enable chip */
    buffer &= ~(1 << 7);
    struct_t->sec = bcd_to_dec(buffer);
    buffer = 0x00;
    
    /* read min */
    set_address(MIN_R_ADDRESS, 1);
    buffer = get_value();
    struct_t->min = bcd_to_dec(buffer);
    buffer = 0x00;
    
    /* read hours */
    set_address(HRS_R_ADDRESS, 1);
    buffer = get_value();
    /* if 7-bit on buffer = 1
     * then used 24 format of times
     * clean this bit for correct value of hours */
    if (buffer & (1 << 7))
        buffer &= ~(1 << 7);
    struct_t->hour = bcd_to_dec(buffer);
    buffer = 0x00;

	/* read date */
    set_address(DATE_R_ADDRESS, 1);
    buffer = get_value();
    struct_t->date = bcd_to_dec(buffer);
    buffer = 0x00;

	/* read month */
    set_address(MNTH_R_ADDRESS, 1);
    buffer = get_value();
    struct_t->month = bcd_to_dec(buffer);
    buffer = 0x00;
	
	/* read day of week */
    set_address(DAY_R_ADDRESS, 1);
    buffer = get_value();
    struct_t->day = bcd_to_dec(buffer);
    buffer = 0x00;
	
	/* read year */
    set_address(YEAR_R_ADDRESS, 1);
    buffer = get_value();
    struct_t->year = bcd_to_dec(buffer);
    buffer = 0x00;
}
