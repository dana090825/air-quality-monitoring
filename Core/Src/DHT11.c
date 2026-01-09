#include "DHT.h"

#define Delay(d) HAL_Delay(d)

static void goToOutput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = DHT_Pin;
	GPIO_InitStruct.Mode = GPIO_Mode_OUTPUT_OD;
		#if DHT_PullUP == 1
	GPIO_InitStruct.Pull = GPIO_PULLUP;
		#else
	GPIO_InitStruct.Pull = GPIO_NOPULL;
		#endif
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(DHT_Port, &GPIO_InitStruct);

}

DHT_data DHT_getData(DHT_type t) {
	DHT_data data = {0.0f, 0.0f};


}
