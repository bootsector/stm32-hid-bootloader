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

#include "usb.h"
#include "hid.h"
#include "bitwise.h"

// HID Bootloader takes 3K. 1K of config flash follows it.
#define USER_PROGRAM 0x08001000

typedef void (*funct_ptr)(void);

int main() {
	uint32_t userProgramAddress = *(volatile uint32_t *)(USER_PROGRAM + 0x04);
	funct_ptr userProgram = (funct_ptr) userProgramAddress;

	// Turn GPIOA clock on
	bit_set(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

	// Set A8 as Input Mode with Pull-ups
	bit_clear(GPIOA->CRH, GPIO_CRH_MODE8);
	bit_clear(GPIOA->CRH, GPIO_CRH_CNF8_0);
	bit_set(GPIOA->CRH, GPIO_CRH_CNF8_1);
	bit_set(GPIOA->ODR, GPIO_ODR_ODR8);

	// Turn GPIOB clock on
	bit_set(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);

	// Set B2 as Input Mode Floating
	bit_clear(GPIOB->CRL, GPIO_CRL_MODE2);
	bit_set(GPIOB->CRL, GPIO_CRL_CNF2_0);
	bit_clear(GPIOB->CRL, GPIO_CRL_CNF2_1);

	// Wait 1uS so the pull-up settles...
	for(int i = 0; i < 72; i++) {
		asm volatile ("nop\n");
	}

	// If A8 is LOW or B2 is HIGH enter HID bootloader...
	if((!(GPIOA->IDR & GPIO_IDR_IDR8)) || (GPIOB->IDR & GPIO_IDR_IDR2)) {
		USB_Init(HIDUSB_EPHandler, HIDUSB_Reset);

		for(;;);
	}

	// Set A8 to input floating
	bit_clear(GPIOA->CRH, GPIO_CRH_MODE8);
	bit_set(GPIOA->CRH, GPIO_CRH_CNF8_0);
	bit_clear(GPIOA->CRH, GPIO_CRH_CNF8_1);
	bit_clear(GPIOA->ODR, GPIO_ODR_ODR8);

	// Turn GPIOA clock off
	bit_clear(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

	// Turn GPIOB clock off
	bit_clear(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);

	SCB->VTOR = USER_PROGRAM;

	asm volatile("msr msp, %0"::"g"(*(volatile u32 *) USER_PROGRAM));

	userProgram();

	for(;;);
}
