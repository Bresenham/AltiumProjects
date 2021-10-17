/*
 * W25Q32JV.c
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#include "W25Q32JV.h"

#include "../SPI/SPI.h"

const uint8_t CMD_RELEASE_PW_DOWN_ID[] = { 0x9F, 0x00, 0x00, 0x00 };

static void fill_address(uint8_t *mem, uint32_t addr) {

  mem[1] = ( addr >> 16 ) & 0xFF;
  mem[2] = ( addr >> 8 ) & 0xFF;
  mem[3] = addr  & 0xFF;
}

void spi_on_transmit_receive_finished_callback(struct TRANSFER *trans) {

  w25q32jv_request_finished(trans);
}

void w25q32jv_get_device_id(struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_transmit = CMD_RELEASE_PW_DOWN_ID;
  trans->data_len = sizeof(CMD_RELEASE_PW_DOWN_ID);
  trans->type = W25Q32JV_DEVICE_ID;

  spi_transmit_receive_start(trans);
}

void w25q32jv_read_byte_from_addr(uint32_t addr, struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 5;
  trans->data_transmit[0] = 0x03;
  fill_address(&trans->data_transmit[1], addr);
  trans->data_transmit[4] = 0x00;
  trans->type = W25Q32JV_READ_FROM_ADDR;

  spi_transmit_receive_start(trans);
}

void w25q32jv_sector_erase(uint32_t addr, struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 4;
  trans->data_transmit[0] = 0x20;
  fill_address(&trans->data_transmit[1], addr);
  trans->type = W25Q32JV_SECTOR_ERASE;

  spi_transmit_receive_start(trans);
}
