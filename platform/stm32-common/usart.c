/* Copyright (c) 2014 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include INC_PLAT(usart.h)
#include INC_PLAT(gpio.h)

#include INC_PLAT(usart.c)

/* Calculates the value for the USART_BRR */
/* TODO: Need more precise algorithm */
static int16_t usart_baud(uint32_t base, uint32_t baud)
{
	uint16_t mantissa;
	uint16_t fraction;

	/* USART1 and USART6 are on APB2 whose frequency is 84MHz,
	 * while USART2, USART3, UART4, and UART5 are on APB1 whose
	 * frequency is 42 MHz (max).
	 */
	if (base == USART1_BASE) {
		mantissa = (84000000) / (16 *  baud);
		fraction = (84000000 / baud) % 16;
	} else {
		mantissa = (42000000) / (16 *  baud);
		fraction = (42000000 / baud) % 16;
	}

	return (mantissa << 4) | fraction;
}

void usart_config_interrupt(struct usart_dev *usart, uint16_t it,
                            uint8_t state)
{
	uint32_t it_reg, it_bit;

	if (it == USART_IT_CTS && (usart->u_num == 4 || usart->u_num == 5))
		return;

	it_reg = usart->base + USART_IT_ENB_REG_OFFSET(it);
	it_bit = (0x1 << USART_IT_POSITION(it));

	if (state)
		*(volatile uint32_t *) it_reg |= it_bit;
	else
		*(volatile uint32_t *) it_reg &= ~it_bit;
}

int usart_interrupt_status(struct usart_dev *usart, uint16_t it)
{
	uint32_t it_reg, it_bit;

	if (it == USART_IT_CTS && (usart->u_num == 4 || usart->u_num == 5))
		return 0;

	it_reg = usart->base + USART_IT_ENB_REG_OFFSET(it);
	it_bit = (0x1 << USART_IT_POSITION(it));

	return (*(volatile uint32_t *) it_reg & it_bit);
}

int usart_status(struct usart_dev *usart, uint16_t st)
{
	return (*USART_SR(usart->base) & st);
}

uint8_t usart_getc(struct usart_dev *usart)
{
	return (*USART_DR(usart->base) & 0xff);
}

void usart_putc(struct usart_dev *usart, uint8_t c)
{
	*USART_DR(usart->base) = c;
}

void usart_init(struct usart_dev *usart)
{
	*(usart->rcc_apbenr) |= usart->rcc_reset;
	gpio_config(&usart->tx);
	gpio_config(&usart->rx);

	*USART_CR1(usart->base) |= USART_CR1_UE;

	/* FIXME: Hardcode 8-bit */
	*USART_CR1(usart->base) &= ~(USART_CR1_M9);

	/* FIXME: Hardcode 1 stop bit */
	*USART_CR2(usart->base) &= ~(3 << 12);

	/* Set baud rate */
	*USART_BRR(usart->base) = usart_baud(usart->base, usart->baud);

	/* Enable reciever and transmitter */
	*USART_CR1(usart->base) |= (USART_CR1_RE | USART_CR1_TE);
}
