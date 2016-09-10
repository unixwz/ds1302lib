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

/* this function transfers data 
 *(register address or value)
 * by 3-wire serial interface
 * params:
 * - dw_enable - if dw_enable = 0 then enable write operation of time
 *               if dw_enable = 1 then enable write operation of date
 */
static void transfer_data(uint8_t data, uint8_t *dw_enable)
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

/* this function set value in register
 * by address specifies in set_address function 
 * params: 
 * - data - transsmision data
 * */
static void set_value(uint8_t data)
{
	transfer_data(data, 0);
	/* stop transmission & disable chip */
	WORKING_PORT &= ~(1 << CE_BIT);
}

/* this function set register address 
 * in chip that is accessed 
 * param: 
 * - re (read enable) if re=0 enable write operation,
 *                    if re=1 enable read operation.
 */
static void set_address(uint8_t address, uint8_t re)
{
	WORKING_DDR |= (1 << CE_BIT) | (1 << CLK_BIT) | (1 << IO_BIT);
	WORKING_PORT |= (1 << CE_BIT); /* enable chip */
	_delay_us(1);
	if (!re)
		/* init write operation */
		WORKING_PORT &= ~(1 << IO_BIT);
	else
		/* init read operation */
		WORKING_PORT |= (1 << IO_BIT);
	/* transfer register address */
	transfer_data(address, 0); 
	/* specifies transfer or receive data on clock/calendar instead RAM */
	WORKING_PORT &= ~(1 << IO_BIT); 	
	/* this bit must be a logic 1 for work chip */
	WORKING_PORT |= (1 << IO_BIT); 
	/* set PIN to logic 0 */
	WORKING_PORT &= ~(1 << IO_BIT);	
}

/* this function sets: hours, minutes, seconds */
void ds1302_set_time(ds1302time_t data)
{
	set_address(0x80, 0);
	set_value(data.sec);
	set_address(0x84, 0);
	set_value(data.hour);
}

int main()
{
	ds1302time_t ds1302time;
	ds1302time.sec = 0;
	ds1302time.hour = 22;
	ds1302_set_time(ds1302time);
	return 0;
}
