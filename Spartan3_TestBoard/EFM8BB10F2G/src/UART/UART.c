/*
 * UART.c
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#include <stdint.h>

#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include "UART.h"

volatile struct TRANSFER *receive = NULL;
volatile struct TRANSFER *transmit = NULL;

SI_INTERRUPT (UART_ISR, UART0_IRQn) {

  /* Transmit completed interrupt */
  if( SCON0 & SCON0_TI__BMASK ) {

      SCON0 &= ~SCON0_TI__BMASK;

      uart_transmit_handle_irq();
  }
}

void uart_transmit_start(struct TRANSFER *trans) {

  transmit = trans;
  SBUF0 = transmit->data_trans[transmit->data_idx++];
}

void uart_transmit_handle_irq() {

  if( transmit != NULL ) {

      if( transmit->data_idx < transmit->data_len ) {
          SBUF0 = transmit->data_trans[transmit->data_idx++];
      } else {
          uart_on_transmit_finished_callback(transmit);
      }

  }
}

void uart_receive_start(struct TRANSFER *recv) {
  receive = recv;
}

void uart_receive_handle_irq(uint8_t dat) {

  if( receive != NULL ) {

      if( receive->data_idx < receive->data_len ) {
          receive->data_recv[receive->data_idx++] = dat;
      } else {
          uart_on_receive_finished_callback(receive);
          receive = NULL;
      }
  }

}

