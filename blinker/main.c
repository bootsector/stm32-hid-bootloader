/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stm32f10x.h>

#include "bitwise.h"
#include "led.h"

void delay_us(uint32_t us) {
	/*
	 *  SysTick->LOAD can hold values from 1 and 0x00FFFFFF. That means it can
	 *  count up to 16777215 cycles. Assuming a 72Mhz clock, that number of
	 *  cycles represents 233016 uS.
	 *
	 *  Given the above, passing a number bigger than 233016 (uS) to this
	 *  function will actually trigger a faster delay.
	 *
	 *  I've chosen to not check the parameter and leave that to the programmer
	 *  so we don't add more overhead cycles to the beginning of this function.
	 */

	// Discount an overhead of around 18 cycles
	SysTick->LOAD = ((SystemCoreClock / 1000000UL) * us) - 18 - 1;

	SysTick->VAL = 0;

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

	bit_clear(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

void delay_ms(uint32_t ms) {
	while(ms--) {
		delay_us(1000);
	}
}

void disable_usb_pullup() {
	// PA_12 output mode: General purpose output open drain (b01)
	bit_set(GPIOA->CRH, GPIO_CRH_CNF12_0);
	bit_clear(GPIOA->CRH, GPIO_CRH_CNF12_1);

	// Set PA_12 to output
	bit_set(GPIOA->CRH, GPIO_CRH_MODE12);// PA_12 set as: Output mode, max speed 50 MHz.

	// Sinks A12 to GND
	GPIOA->BRR = GPIO_BRR_BR12;
}

int main() {

	// Disable USB pull-up on Blue Pill boards so USB host won't try to enumerate USB
	disable_usb_pullup();

	led_init();

	for(;;) {
		led_on();

		delay_ms(500);

		led_off();

		delay_ms(500);
	}
}
