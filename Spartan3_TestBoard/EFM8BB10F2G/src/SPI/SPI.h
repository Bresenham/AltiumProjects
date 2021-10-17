/*
 * SPI.h
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_

void spi_transmit_receive_start(struct TRANSFER *t_r);
void spi_transmit_receive_handle_irq();
extern void spi_on_transmit_receive_finished_callback(struct TRANSFER *transfer);

#endif /* SPI_SPI_H_ */
