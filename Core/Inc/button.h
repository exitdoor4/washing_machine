#include "main.h" // 무조건 main.h 참고해야함 왜하냐 gpio 핸들링을 하기 위해서

// 버튼을 누른 상태 , 버튼을 땐 상태, 버튼 갯수 3개 정도를 디파인한다.
#define BUTTON_RELEASE 1 // 버튼을 땐 상태 1 pull-up / normal state = high
#define BUTTON_PRESS 0 // 버튼을 누른 상태 0 ==> active-low 라는 상태라고 한다.
#define BUTTON_NUMBER 5 // 버튼의 갯수를 의미한다.

int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN,uint8_t button_number);
