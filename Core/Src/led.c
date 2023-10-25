#include "led.h"
#include "button.h"


#define LED_ON_UP 0
#define LED_ON_DOWN 1



void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_up(void);
void led_on_down(void);
void keepon_up(void);
void keepon_down(void);
void flower_on(void);
void flower_off(void);
void button0_led_all_on_off_toggle(void);
void button0_toggle(void);
void move_led(void);
void Circular(void);


void (*fp[])(void) ={
	led_all_on,
	led_all_off,
	flower_on,
	flower_off,
	keepon_up,
	keepon_down,
	led_on_up,
	led_on_down
};
extern volatile int t1ms_count;

int button0_count=-1;
int i = 0; // move 함수 호출
int num = 0;
int num2 =0; // 함수들이 한번만 돌아가게 설정

void led_main(void)

{
	while(1)
	{
		button0_toggle();
//		if(t1ms_count >= 200)
//		{
//			t1ms_count=0;
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
//		}

//		HAL_GPIO_TogglePin(GPIOB, GPIO_Pin_0);
//		HAL_GPIO_TogglePin(GPIOB, GPIO_Pin_7);
//		HAL_GPIO_TogglePin(GPIOB, GPIO_Pin_14);
//		HAL_Delay(200);

//		button0_toggle();

//		flower_on();
//		flower_off();
		//phase#4
//		keepon_up();
//		led_all_off();
//		keepon_down();
//		led_all_off();

		//phase#3
#if 0

		switch(func_index)
		{
		case LED_ON_UP:
			led_on_up();
			break;
		case LED_ON_DOWN:
			led_on_down();
			break;
		default:
			break;
		}
#endif
		//phase#2
		//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_All);
		//HAL_Delay(500);
		//phase#1
//		led_all_on();
//		HAL_Delay(300);
//		led_all_off();
//		HAL_Delay(300);

		// led_array();

	}
}
void Circular(void)
{


	if(num2==0)
	{
		(*fp[num])();
		num2++;
	}

	if(t1ms_count >= 200)
	{
		t1ms_count =0;
		num++;
		num2 = 0;
	}
	if(num >= 8)
	{
		num=0;
	}
}

void move_led(void)
{

	if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
		i++;
	}
	else if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 0);
		i--;
	}
	else if(i < 0)
	{
		i =0;
	}
	else if(i>7)
	{
		i=7;
	}
}
void button0_toggle(void)
{

	while(1)
	{

		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
			{
				button0_count++;
				button0_count %= 8;

				if(num2==0) // 함수가 한번만 실행되게 하는 것
				{
					(*fp[button0_count])();
					num2 = 1;
				}

				printf("button0_count = %d\n",button0_count);
				num2 = 0;
			}

	}
}
void button0_led_all_on_off_toggle(void)
{
	 // static으로 선언을 하면 전역 변수 처럼 동작을 한다.
	//버튼을 한번 눌렀다 땐 상태라면
	if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %=2;
		if(button0_count)
		{
			led_all_on();
		}
		else
		{
			led_all_off();
		}
	}
}
// 이벤트가 발생되었는지 안되었는지 계속 돌면서 확인하는 것을 !! 폴링 !!
void flower_on(void)
{
	int i = 4;
	int j = 3;
	led_all_off();
		while(1)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
			HAL_GPIO_WritePin(GPIOD, 0x01<<j, 1);

			if(t1ms_count >= 200)
			{
				t1ms_count =0;
				i++;
				j--;
				if(j<-1)
					{
						break;
					}
			}
		}

}
void flower_off(void)
{
	int i = 0;
	int j = 7;

	while(j!=2)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x01<<j, 0);
		if(t1ms_count >= 200)
		{
			t1ms_count =0;
			i++;
			j--;

		}

	}
}
void keepon_up(void)
{
	int i=0;
	while(1)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
			if(t1ms_count >= 200)
			{
				t1ms_count =0;
				i++;
				if(i>8)
				{
					break;
				}

			}
		}
}
void keepon_down(void)
{
	led_all_off();

	int i=7;
	while(1)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
		if(t1ms_count >= 200)
		{
			t1ms_count =0;
			i--;
			if(i<0)
			{
				break;
			}

		}
	}
}
void led_on_up(void)
{
	int i=0;

	while(1)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
		if(t1ms_count >= 200)
		{
			t1ms_count =0;
			i++;
			if(i >= 8)
			{
				break;
			}
		}
	}

}

void led_on_down(void)
{
	int i=0;

	while(1)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80>>i, 1);
		if(t1ms_count >= 200)
		{
			t1ms_count =0;
			i++;
			if(i >= 8)
			{
				break;
			}
		}
	}

}

void led_all_on(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1); // 1 = set 0 = reset
	HAL_GPIO_WritePin(GPIOD,0xff,1);
}
void led_all_off(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);  // 0 = reset
	HAL_GPIO_WritePin(GPIOD,0xff,0);
}

