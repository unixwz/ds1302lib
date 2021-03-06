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
 
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>

/* defenition port & bits for 
 * data transfer with ds1302 
 */
#define WORKING_DDR  DDRB
#define WORKING_PORT PORTB
#define WORKING_PIN  PINB
#define CLK_BIT 6  /* sync signal */
#define CE_BIT  0  /* counter enable */
#define IO_BIT  7  /* input-output */

#define TIME_FORMAT 0 /* 24 = 0; 12 = 1 */
#define TIME_OF_DAY 0 /* AM = 0; PM = 1 */


typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t date;
	uint8_t month;
	uint8_t day;
	uint8_t year;
} ds1302time_t;

void ds1302_set_time(uint8_t value, char *value_type);
void ds1302_get_time(ds1302time_t *struct_t);

