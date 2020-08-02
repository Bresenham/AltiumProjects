#ifndef DS18B20_H_
#define DS18B20_H_

#include <stdbool.h>

struct DS18B20 {
	bool (*startCommunication)(struct DS18B20*);
};

void initDS18B20(struct DS18B20 *self);

#endif /* DS18B20_H_ */