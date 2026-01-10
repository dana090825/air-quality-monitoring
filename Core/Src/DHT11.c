#include "DHT.h"

#define lineDown() HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_RESET)
#define lineUp() HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET)
#define getLine() (HAL_GPIO_ReadPin(DHT_Port, DHT_Pin) == GPIO_PIN_SET) //지금 걸려 있는 핀 라인을 읽어온다, set(1)인지 반별한다
#define Delay(d) HAL_Delay(d)

//DHT 데이터 핀을 출력 모드로 바꾸기
static void goToOutput(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0}; //구조체 값 0으로 세팅

	HAL_GPIO_WritePin(DHT_Port, DHT_Pin, GPIO_PIN_SET); //GPIO 출력 데이터 레지스터에 set(1)값을 써줌 (라인을 해제해 풀업이 올라가는 것)
	//why : DHT는 기본이 HIGH여야함 (아직 output이 아니라 먹진 않지만 미리 세팅)

	GPIO_InitStruct.Pin = DHT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //(OD-OpenDrain) MCU는 LOW만 HIGH는 풀업 (센서와 안전한 통신)
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
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(DHT_Port, &GPIO_InitStruct);
}

DHT_data DHT_getData(DHT_type t) {
	DHT_data data = {0.0f, 0.0f};

	// -> 이해하려면 데이터 시트 통신을 보자!!
	goToOutput();

	lineDown();
	Delay(18);

	lineUp();
	goToInput();
	//DHT11 응답과 데이터를 보내게 만드는 절차 (DHT11 datasheet)

	uint16_t timeout = 0; //얼마나 기다렸는지 카운트하기 위한 변수

	// datasheet의 20-40us 대기 대신 low가 올 때까지 기다리는 것
	while(getLine()) { //지금 라인이 set(1)일동안 (목적 : 센서가 응답을 시작해 low가 될때까지 기다림)
		timeout++; //while문 반복 횟수만큼 증가
		if (timeout > DHT_timeout) return data; //타임아웃이 설정 카운트를 넘기면 실패 처리
	}
	timeout = 0; //다시 사용할 수 있도록 초기화

	while(!getLine()) {
		timeout++;
		if(timeout > DHT_timeout) retunn data;
	}
	timeout = 0;

	while(getLine()) {
		timeout++;
		if (timeout > DHT_timeout) return data;
	}
	timeout = 0;
	//-> while은 데이터 시트 MCU Sends out Start Signal to DHT 보고 이해
}
