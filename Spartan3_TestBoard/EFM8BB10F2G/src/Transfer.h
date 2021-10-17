/*
 * Transfer.h
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#ifndef TRANSFER_H_
#define TRANSFER_H_

#include <stdint.h>

enum TRANSFER_TYPE {
  W25Q32JV_SECTOR_ERASE,
  W25Q32JV_DEVICE_ID,
  W25Q32JV_READ_FROM_ADDR,
  W25Q32JV_WRITE_TO_ADDR
};

struct TRANSFER {
  uint8_t *data_transmit;
  uint8_t *data_recv;
  uint8_t data_len;
  uint8_t data_idx;
  enum TRANSFER_TYPE type;
};

#endif /* TRANSFER_H_ */
