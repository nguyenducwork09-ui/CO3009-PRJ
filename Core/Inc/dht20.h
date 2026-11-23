#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdint.h>

uint8_t dht20_init(void);

uint8_t dht20_read(float *temperature, float *humidity);

#endif /* INC_DHT20_H_ */
