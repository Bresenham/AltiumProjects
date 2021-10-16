/*
 * W25Q32JV.c
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#include "W25Q32JV.h"

#include "../SPI/SPI.h"

const uint8_t CMD_RELEASE_PW_DOWN_ID[] = { 0x9F, 0x00, 0x00, 0x00 };

void w25q32jv_get_device_id(struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_transmit = CMD_RELEASE_PW_DOWN_ID;
  trans->data_len = sizeof(CMD_RELEASE_PW_DOWN_ID);

  spi_transmit_receive_start(trans);

}
