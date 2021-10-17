/*
 * W25Q32JV.h
 *
 *  Created on: 16 Oct 2021
 *      Author: test
 */

#ifndef W25Q32JV_W25Q32JV_H_
#define W25Q32JV_W25Q32JV_H_

#include "../Transfer.h"

void w25q32jv_get_device_id(struct TRANSFER *trans);
void w25q32jv_read_byte_from_addr(uint32_t addr, struct TRANSFER *trans);
void w25q32jv_sector_erase(uint32_t addr, struct TRANSFER *trans);

extern void w25q32jv_request_finished(struct TRANSFER *trans);

#endif /* W25Q32JV_W25Q32JV_H_ */
