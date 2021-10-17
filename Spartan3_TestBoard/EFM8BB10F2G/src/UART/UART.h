/*
 * UART.h
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#include "../Transfer.h"

void uart_transmit_start(struct TRANSFER *trans);
void uart_transmit_handle_irq();
extern void uart_on_transmit_finished_callback(struct TRANSFER *trans);

void uart_receive_start(struct TRANSFER *recv);
void uart_receive_handle_irq(uint8_t dat);
extern void uart_on_receive_finished_callback(struct TRANSFER *recv);


#endif /* UART_H_ */
