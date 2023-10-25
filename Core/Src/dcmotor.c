#if 0

#include "button.h"
#include "dcmotor.h"

extern volatile int t1ms_count;
extern TIM_HandleTypeDef htim4;
extern void led_all_off(void);
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);
extern volatile int TIM10_10ms_counter;
extern void i2c_lcd_init(void);
void dcmotor_pwm_control(void);
void dcmoter_init(void);
void print_lcd(void);
void press_BlueB1(void);
void press_button0(void);
void press_button1(void);
void press_button2(void);
void press_button3(void);

// BUTTON0 : start/stop : LED1 (toggle)
// BUTTON1 : SPEED-UP : LED2 (toggle)
// BUTTON2 : SPEED-DOWN : LED3 (toggle)

uint8_t pwm_start_flag=0; // dcmotor start/stop indicator flag
uint16_t CCR_UP_Value = 0; // PWM UP control
uint16_t CCR_DOWN_Value = 0; // PWM DOWN control
uint8_t auto_flag =0; // default = manual 기능을 선택했을 때 1을 온한다.
uint8_t led_count = 4;
GPIO_PinState bitstatus; // blue button 사용
uint8_t swit_flag=0; // 정방향 디폴트 0'
char lcd_buff[20];


void dcmotor_pwm_control(void)
{


	swit_led(swit_flag,pwm_start_flag);
	print_lcd();
//	press_BlueB1(); // switch auto/manual
	press_button0(); // start - end
	press_button1(); // auto = +10sec , manual = speed up
	press_button2(); // auto = +20sec , manual = speed down
	press_button3(); // auto = -10sec , manual = switch dir

}

void swit_led(uint8_t swit_flag,uint8_t pwm_start_flag)
{
	if(pwm_start_flag ==1)
	{
		if(swit_flag==0)
		{
			if(t1ms_count>=50)
				{
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
					t1ms_count = 0;
				}
		}
		else if(swit_flag==1)
		{
			if(t1ms_count>=100)
				{
					HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
					t1ms_count = 0;
				}
		}
	}
}

void print_lcd(void)
{
	if(auto_flag == 1)
	{
		move_cursor(0,1);
		sprintf(lcd_buff,"s:%4ds r:%4ds",set_time,remain_time);
		lcd_string(lcd_buff);
		move_cursor(1,0);
		if(pwm_start_flag==1)
		{
			lcd_string("X +10 +20 -10");
		}
		else
		{
			lcd_string("O +10 +20 -10");
		}
		if(TIM10_10ms_counter >= 100)
		{
			if(pwm_start_flag==1) // 시작된 상태일때만 동작
			{
				if(remain_time>=1)
				{
					remain_time--;
				}
				TIM10_10ms_counter =0;
			}
		}
		if(remain_time<=0)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			remain_time = 0;
		}
		else
		{
			if(pwm_start_flag==1)
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			}
		}
	}
}

void dcmoter_init(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,1);
}

//void press_BlueB1(void)
//{
//	if(get_button(GPIOC,GPIO_PIN_13, 4) == BUTTON_PRESS)
//		{
//			if(auto_flag == 0)
//			{
//				auto_flag = 1;
//				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,0);
//				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,1);
//				led_all_off();
//
//			}
//			else
//			{
//				auto_flag = 0;
//				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,1);
//				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,0);
//				i2c_lcd_init();
//				remain_time = 30;
//				set_time = 30;
//				if(pwm_start_flag==1)
//				{
//					for(int i =0;i<4;i++)
//					{
//						HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
//					}
//				}
//			}
//		}
//}

void press_button0(void)
{
	if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
		{

			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

			if(swit_flag==0)
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			}
			if(auto_flag == 0)//manual
			{
				if(!pwm_start_flag)
				{
					pwm_start_flag =1;
					HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,80-1);

					for(int i =0;i<4;i++)
					{
						HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
					}

				}
				else
				{
					pwm_start_flag=0;
					HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
					led_all_off();
					led_count = 4;
				}
			}
			else //auto
			{
				if(!pwm_start_flag)
				{
					pwm_start_flag =1;
					HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,80-1);

				}
				else
				{
					pwm_start_flag=0;
					HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
				}
			}
		}
}
void press_button1(void)
{
	if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			if(auto_flag == 0)
			{
				CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4,TIM_CHANNEL_1);
				CCR_UP_Value += 5;
					HAL_GPIO_WritePin(GPIOD, 0x01<<led_count, 1);
					led_count++;
				if(CCR_UP_Value > 100)
				{
					CCR_UP_Value= 100;
				}
				__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,CCR_UP_Value);
			}
			else
			{
				remain_time +=10;
				set_time +=10;

			}

		}
}
void press_button2(void)
{
	if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			if(auto_flag == 0)
			{
				CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4,TIM_CHANNEL_1);
				CCR_DOWN_Value -= 5;

					led_count--;
					HAL_GPIO_WritePin(GPIOD, 0x01<<led_count, 0);


				if(CCR_DOWN_Value < 0) CCR_DOWN_Value= 0;
				__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,CCR_DOWN_Value);
			}
			else
			{
				remain_time +=20;
				set_time +=20;
			}
		}
}

void press_button3(void)
{
	if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			if(auto_flag==0)
			{
				if(swit_flag ==0)
				{
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
					swit_flag=1;

					}
				else if(swit_flag ==1)
				{

					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
					swit_flag =0;

				}
			}
			else
			{
				if(remain_time <=10)
				{
					remain_time = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
				}
				else
				{
					remain_time -=10;
				}
				set_time -=10;
			}
		}
}
#endif
