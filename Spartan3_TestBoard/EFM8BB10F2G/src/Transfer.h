/*
 * Transfer.h
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#ifndef TRANSFER_H_
#define TRANSFER_H_

#include <stdint.h>

struct TRANSFER {
  uint8_t *data_transmit;
  uint8_t *data_recv;
  uint8_t data_len;
  uint8_t data_idx;
};

#endif /* TRANSFER_H_ */
