/*
 * SPI.c
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#include <SI_EFM8BB1_Defs.h>
#include <SI_EFM8BB1_Register_Enums.h>

#include "../Transfer.h"

#include "SPI.h"

volatile struct TRANSFER *trans_recv;

void spi_cs_down() {

  SPI0CN0 &= ~SPI0CN0_NSSMD__FMASK;
  SPI0CN0 |= SPI0CN0_NSSMD__4_WIRE_MASTER_NSS_LOW;

}

void spi_cs_up() {

  SPI0CN0 &= ~SPI0CN0_NSSMD__FMASK;
  SPI0CN0 |= SPI0CN0_NSSMD__4_WIRE_MASTER_NSS_HIGH;

}


void spi_transmit_receive_start(struct TRANSFER *t_r) {

  spi_cs_down();
  trans_recv = t_r;
  SPI0DAT = trans_recv->data_transmit[trans_recv->data_idx];

}

void spi_transmit_receive_handle_irq(){

  if( trans_recv != NULL ) {

      if( trans_recv->data_idx < trans_recv->data_len ) {

          trans_recv->data_recv[trans_recv->data_idx] = SPI0DAT;

          trans_recv->data_idx += 1;

          SPI0DAT = trans_recv->data_transmit[trans_recv->data_idx];

      } else {

          spi_cs_up();
          spi_on_transmit_receive_finished_callback(trans_recv);
          trans_recv = NULL;

      }
  }

}
