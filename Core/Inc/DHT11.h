#ifndef DHT_H_
#define DHT_H_


#include "main.h"

typedef struct {
	float hum;
	float temp;
} DHT_data;

#define DHT_Port GPIOA
#define DHT_Pin GPIO_PIN_1
#define DHT_PullUp 0
#define DHT_timeout 10000

DHT_data DHT_getData(DHT_type t);

#endif
