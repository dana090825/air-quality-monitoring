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

DHT_data DHT_getData() {
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
		if(timeout > DHT_timeout) return data;
	}
	timeout = 0;

	while(getLine()) {
		timeout++;
		if (timeout > DHT_timeout) return data;
	}
	timeout = 0;
	//-> while은 데이터 시트 MCU Sends out Start Signal to DHT 보고 이해

	//DHT11의 5바이트(40비트)를 읽어 채우고 체크섬이 맞으면 data에 넣고 반환
	uint8_t rawData[5] = {0, 0, 0, 0, 0}; //datasheet Communication Process: Serial Interface 참고
	// 0바이트 : 습도 정수, 1바이트 : 습도 소수(보통 0), 2바이트 : 온도 정수, 3바이트 : 온도 소수(보통 0), 4바이트 : 체크섬(하위 8비트)

	for(uint8_t a = 0; a < 5; a++) {
		for(uint8_t b = 7; b != 255; b--) { //!= 255 를 사용한 이유 : 음수가 없어 언더플로우(... 1->0->255 ...)
		// 다른 방식도 있지만 파형 그림에 맞추고 비트 시프트 계산이 단순해서 위 방식 사용
		// -> DHT의 데이터 수신 방식 : high가 얼마나 유지 되었는지 (전압X)
			uint8_t hT = 0, lT = 0; // highTime, longTime (시간이 아닌 횟수임!)

			// datasheet의 DHT Responses to MCU 참고(두 파형 그림 high 길이 비교)
			while(!getLine()) lT++;
			while(getLine()) hT++;

			//hT이 lT보다 더 오래 유지되면 1
			if(hT > lT) rawData[a] |= (1<<b); //rawData의 a번째 바이트의 b번째 비트를 or연산자로 1로 변경
		}
	}

	//체크섬 (앞 4바이트 데이터를 다 더한 후 가장 아래 8비트만 잘라서 보낸 값)
	if ((uint8_t)(rawData[0] + rawData[1] + rawData[2] + rawData[3]) == rawData[4]) {
		data.hum = (float)rawData[0];
		data.temp = (float)rawData[2];
	}

	return data;
}
