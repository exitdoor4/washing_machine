#include "ultrasonic.h"

extern void delay_us (unsigned long us);
extern volatile int TIM10_10ms_ultrasonic;
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);
extern uint8_t lcd_display_mode_flag;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
int ultrasonic_processing(void);
void make_trigger(void);

volatile int distance; // 상승엣지부터 하강엣지까지 펄스가 몇번 카운트 되었는지 그 횟수를 담아둘 변수
volatile int ic_cpt_finish_flag = 0; // 초음파 거리 측정 완료 indicator 플래그변수
volatile uint8_t is_first_capture = 0; // 상승엣지 때문에 콜백펑션에 들어온건지 하강엣지 때문에 콜백 펑션이 들어온건지 구분하기 위한 플래그변수(0: 상승엣지, 1: 하강엣지)


// 1. Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c에 가서 가져온 call-back function
// 2. 초음파 센서의 ECHO핀의 상승edge와 하강edge 발생 시 이 함수로 들어온다!!!
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (is_first_capture == 0) // 상승엣지 때문에 콜백 펑션에 들어온 경우
		{

			__HAL_TIM_SET_COUNTER(htim, 0); // 펄스를 셀 카운터를 초기화 하고 세기 시작하는 것이다.
			is_first_capture = 1; // 다음에 콜백 펑선이 불릴 때는 당연히 하강 엣지일 때 일 것이므로 플래그변수를 1로 셋팅해준다.
		}
		else if (is_first_capture == 1) // 하강 엣지 때문에 콜백 펑션에 들어온 경우
		{
			is_first_capture = 0; // 이제 다음 펄스를 또 카운트 하기 위해서 플래그 변수를 초기화 해준다.
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 상승엣지부터 하강엣지까지 펄스가 몇번 카운트 되었는지 그 값을 읽어온다.
			ic_cpt_finish_flag = 1; // 초음파 측정완료
		}
	}
}




void make_trigger(void)
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
	delay_us(2);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 1);
	delay_us(10);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
	// 위 5줄의 코드 라인을 통해 초음파 센서에서 요구하는 트리거 신호의 전기적 파형을 MCU가 날릴 수 있도록 구현했다.

}
