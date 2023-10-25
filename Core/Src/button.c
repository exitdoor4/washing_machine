#include "button.h"
#include "i2c_lcd.h"

void set_time_button_b1(void);
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN,uint8_t button_number);
char button_status[BUTTON_NUMBER]=
{
		BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE
};
uint8_t lcd_display_mode_flag =0;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef sTime;
void lcd_display_mode_select(void)
{
	if(get_button(GPIOC,GPIO_PIN_13,4)==BUTTON_PRESS)
		{
			lcd_command(CLEAR_DISPLAY);
			lcd_display_mode_flag++;
			lcd_display_mode_flag %= 4;
			printf("lcd_display : %d\n",lcd_display_mode_flag);
		}
}

// get_button(GPIO,PIN,BUTTON번호)
// 완전히 눌렀다 때면은 BUTTON_RELEASE(1)를 리턴한다.
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN,uint8_t button_number)
{
	unsigned char curr_state;

	curr_state = HAL_GPIO_ReadPin(GPIO, GPIO_PIN);//0,1

	if(curr_state == BUTTON_PRESS && button_status[button_number]== BUTTON_RELEASE)
	{
		HAL_Delay(80); //noise가 지나가기를 기다린다.
		button_status[button_number]=BUTTON_PRESS;
		return BUTTON_RELEASE; // 버튼이 눌려진 상태이나 아직은 noise상태로 인정
	}
	else if(curr_state == BUTTON_RELEASE && button_status[button_number]== BUTTON_PRESS)
	{	// 이전에 버튼이 눌려진 상태였고 지금은 버튼을 뗀 상태이면
		button_status[button_number] = BUTTON_RELEASE; // button_status table을 초기화하고
		HAL_Delay(30);
		return BUTTON_PRESS; // 버튼 1번을 눌렀다 뗀것으로 인정한다.
	}
	else
	return BUTTON_RELEASE;
}
