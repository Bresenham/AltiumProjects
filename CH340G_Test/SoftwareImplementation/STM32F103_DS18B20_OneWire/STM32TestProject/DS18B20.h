#ifndef DS18B20_H_
#define DS18B20_H_

#include <stdint.h>
#include <stdbool.h>

struct DS18B20 {
	uint64_t (*readDeviceID)();
};

void initDS18B20(struct DS18B20 *self);

#endif /* DS18B20_H_ */