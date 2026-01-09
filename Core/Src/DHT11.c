#include "DHT.h"

#define Delay(d) HAL_Delay(d)

//DHT 데이터 핀을 출력 모드로 바꾸기
static void goToOutput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0}; //구조체 값 0으로 세팅

	HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET); //GPIO 출력 데이터 레지스터에 set(1)값을 써줌 (라인을 해제해 풀업이 올라가는 것)
	//why : DHT는 기본이 HIGH여야함 (아직 output이 아니라 먹진 않지만 미리 세팅)

	GPIO_InitStruct.Pin = DHT_Pin;
	GPIO_InitStruct.Mode = GPIO_Mode_OUTPUT_OD; //(OD-OpenDrain) MCU는 LOW만 HIGH는 풀업 (센서와 안전한 통신)
//		#if DHT_PullUP == 1 //외부 풀업 저항이 없어서 내부 풀업 사용
//	GPIO_InitStruct.Pull = GPIO_PULLUP; //MCU 내부에 약한 풀업
//		#else //외부 풀업을 이미 씀
	GPIO_InitStruct.Pull = GPIO_NOPULL; //외부 풀업 저항(더 안정적)
//		#endif
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //빠른 타이밍 기반 통신을 위함

	HAL_GPIO_Init(DHT_Port, &GPIO_InitStruct); //위 구조체를 바탕으로 세팅
}

static void goToInput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = DHT_Pin;
	GPIO_InitStruct.Mode = GPIO_Mode_INPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(DHT_Port, &GPIO_InitStruct);
}

DHT_data DHT_getData(DHT_type t) {
	DHT_data data = {0.0f, 0.0f};


}
