/*
 * W25Q32JV.c
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#include "W25Q32JV.h"

#include "../SPI/SPI.h"

static void fill_address(uint8_t *mem, uint32_t addr) {

  mem[0] = ( addr >> 16 ) & 0xFF;
  mem[1] = ( addr >> 8 ) & 0xFF;
  mem[2] = addr  & 0xFF;
}

void spi_on_transmit_receive_finished_callback(struct TRANSFER *trans) {

  w25q32jv_request_finished(trans);
}

void w25q32jv_write_enable(struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 1;
  trans->data_trans[0] = 0x06;

}

void w25q32jv_get_device_id(struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 4;
  trans->data_trans[0] = 0x9F;
  trans->type = W25Q32JV_DEVICE_ID;
}

void w25q32jv_sector_erase(uint32_t addr, struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 4;
  trans->data_trans[0] = 0x20;
  fill_address(&trans->data_trans[1], addr);
  trans->type = W25Q32JV_SECTOR_ERASE;
}

void w25q32jv_read_byte_from_addr(uint32_t addr, struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 5;
  trans->data_trans[0] = 0x03;
  fill_address(&trans->data_trans[1], addr);
  trans->data_trans[4] = 0x00;
  trans->type = W25Q32JV_READ_FROM_ADDR;
}

void w25q32jv_write_byte_to_addr(uint32_t addr, uint8_t dat, struct TRANSFER *trans) {

  trans->data_idx = 0;
  trans->data_len = 5;
  trans->data_trans[0] = 0x02;
  fill_address(&trans->data_trans[1], addr);
  trans->data_trans[4] = dat;
  trans->type = W25Q32JV_WRITE_TO_ADDR;
}

void w25q32jv_execute_transfer(struct TRANSFER *trans) {
  spi_transmit_receive_start(trans);
}
