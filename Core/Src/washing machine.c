#include "washing machine.h"
#include "button.h"
#include "i2c_lcd.h"
#include "dcmotor.h"
#include "fnd4digit.h"
#include "ultrasonic.h"

extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);
extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN,uint8_t button_number);
extern void lcd_command(uint8_t command);
extern void end_bgm(void);
extern void FND4digit_off(void);
extern void make_trigger(void);

extern volatile int TIM10_10ms_counter;
extern volatile int t1ms_count;
extern volatile int TIM10_10ms_ultrasonic;

extern uint16_t FND_digit[4];

extern uint32_t FND_font[10];


uint32_t FND_1forward[4] =
{
		FND_a,FND_b|FND_c,FND_d,NULL
};
uint32_t FND_2forward[4] =
{
		FND_a,NULL,FND_d,FND_e|FND_f
};

extern uint16_t FND[4];    // FND에 쓰기 위한 값을 준비하는 변수




uint8_t end_flag = 0;
uint8_t stop_flag =0;
uint8_t start_flag = 0;
uint8_t forward_time =0;
uint8_t back_flag = 0;

volatile uint8_t door_flag = 0;

uint8_t water_flag = 0;
uint8_t water_high_num = 2;
uint16_t set_time = 90;
uint16_t remain_time = 30;
uint8_t step_flag =0; // 0은
uint8_t default_flag =1;



uint16_t print_Laundry;
uint16_t print_Rinse;
uint16_t print_Spin;

uint16_t save_Laundry =10;
uint16_t save_Rinse=10;
uint16_t save_Spin=10;

uint8_t forward_flag = 0;
uint8_t user_water_high_num = 2; // 이거 물높이도 해야함

uint8_t time_flag = 0;
uint8_t time_set_flag =0;

uint16_t Mode_flag = 0;
uint8_t mode_set_flag = 0;
uint8_t save_flag = 0;

extern void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
extern volatile int distance; // 상승엣지부터 하강엣지까지 펄스가 몇번 카운트 되었는지 그 횟수를 담아둘 변수
extern volatile int ic_cpt_finish_flag; // 초음파 거리 측정 완료 indicator 플래그변수
extern volatile uint8_t is_first_capture; // 상승엣지 때문에 콜백펑션에 들어온건지 하강엣지 때문에 콜백 펑션이 들어온건지 구분하기 위한 플래그변수(0: 상승엣지, 1: 하강엣지)


void washing_init(void);
void washing_process(void);
void default_chioce(void);
void print_menu(void);
void back_check(void);


void start_func(void);
void stop_time(void);
void go_time(void);
void start_moter(void);
void end_moter(void);

void water_high_func(void);
void water_high(void);
void water_middle(void);
void water_low(void);
void view_water(void);

void time_set_func(void);
void time_change(void);
void time_view(void);
void time_plus(void);
void time_minus(void);
void open_door(void);

void Mode_func(void);
void mode_view(void);
void Mode_end(void);
void User_Mode_func(void);

volatile int check_i; // fnd check
volatile int fnd_i=0;
volatile int fnd_j=3;
int value;

char lcd_buff[20];



void FND4digit(void)
{

	if(t1ms_count >= 2)
	{
		t1ms_count = 0;
		if(forward_flag)
		{
			FND[0] = FND_1forward[fnd_i];
			FND[1] = FND_2forward[fnd_i];
		}
		else
		{
			FND[0] = FND_1forward[fnd_j];
			FND[1] = FND_2forward[fnd_j];
		}

		switch(step_flag)
		{
		case 0:
			value = print_Laundry;
			break;
		case 1:
			value = print_Rinse;
			break;
		case 2:
			value = print_Spin;
			break;
		}

		FND[2] = FND_font[value % 10];
		FND[3] = FND_font[value / 10];

		//all_off function
		HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
		HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[check_i], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(FND_DATA_PORT, FND[check_i], GPIO_PIN_SET);
		check_i++;
		check_i %= 4;
	}
}

void ultrasonic_processing(void)
{

	char ultra_lcd_buff[20];
	int distance_lv; // 전역변수 값을 바로 쓰지 않고 지역변수에 복사하여 사용하는 것이 안전하기 때문에 별도의 지역변수를 선언함
	if (TIM10_10ms_ultrasonic >= 50) // 10ms가 100개면 1초
	{
		TIM10_10ms_ultrasonic = 0;
		make_trigger();
		if (ic_cpt_finish_flag) // 초음파 측정이 완료 되었다면..
		{
			ic_cpt_finish_flag = 0;
			distance_lv = distance; // 전역변수 값을 바로 쓰지 않고 지역변수에 복사하여 사용하는 것임
			distance_lv = distance_lv * 0.034 / 2;
			if(distance_lv >= 5)
			{
				door_flag =1;
			}
			else
			{
				door_flag =0;
			}
		}

	}

}
void washing_process(void)
{
	default_chioce();
	start_func();
	water_high_func();
	time_set_func();
	Mode_func();
	User_Mode_func();
	back_check();
	ultrasonic_processing();
}

void default_chioce(void)
{
	if(default_flag == 1)
	{
		print_menu();

		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
			{
				start_flag = 1;
				default_flag = 0;
			}
		if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
			{
				view_water();
				water_flag =1;
				default_flag = 0;
			}
		if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin, 2) == BUTTON_PRESS)
			{
				time_set_flag = 1;
				default_flag = 0;
			}
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
			{
				mode_set_flag= 1;
				default_flag = 0;
				lcd_command(CLEAR_DISPLAY);
			}
	}
}
void print_menu(void)
{
	move_cursor(0,0);
	lcd_string("b1:Start b2:High");
	move_cursor(1,0);
	lcd_string("b3:Time  b4:Mod");
}
void back_check(void)
{
	if(back_flag == 1)
	{

		if(mode_set_flag != 1)
		{
			move_cursor(0,0);
			lcd_string("apply complement");
		}

		start_flag =0;
		time_set_flag =0;
		water_flag =0;
		mode_set_flag=0;
		save_flag=0;
		Mode_flag=0;
		step_flag =0;


		move_cursor(1,0);
		lcd_string("| x| x| x| back|");
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			default_flag = 1;
			back_flag =0;
			lcd_command(CLEAR_DISPLAY);
		}
	}
}

void open_door(void)
{
	move_cursor(0,0);
	lcd_string("  open the door");
	move_cursor(1,0);
	lcd_string("                ");
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
}

 // start field
void start_func(void)
{
	if(start_flag == 1)
	{
		if(door_flag ==0)
		{

			if(stop_flag == 0)
			{
				FND4digit();
				go_time();
				start_moter();
			}
			else
			{
				if(door_flag ==0)
				{
					stop_time();
				}
			}
		}
		else if(door_flag==1)
		{
			open_door();
		}
		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
		{
			if(stop_flag == 0)
			{
				lcd_command(CLEAR_DISPLAY);
				stop_flag = 1;
			}
			else
			{
				lcd_command(CLEAR_DISPLAY);
				stop_flag = 0;
			}
		}
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			end_flag = 1;
			start_flag = 0;

			lcd_command(CLEAR_DISPLAY);
		}
	}
	else
	{
		if(end_flag == 1)
		{
			end_moter();
			end_bgm();
			FND4digit_off();
		}
	}
}
void stop_time(void)
{

	move_cursor(0,0);
	lcd_string("  stop");
	move_cursor(1,0);
	sprintf(lcd_buff,"remain : %04ds",remain_time);
	lcd_string(lcd_buff);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

}
void go_time(void)
{
	remain_time = print_Laundry + print_Rinse + print_Spin -1;

	if(TIM10_10ms_counter >=100)
	{

		TIM10_10ms_counter =0;

		fnd_i++;
		fnd_i %= 4;

		fnd_j--;
		if(fnd_j ==0)
		{
			fnd_j = 4;
		}



		move_cursor(0,0);
		sprintf(lcd_buff,"remain time:%03ds", remain_time);
		lcd_string(lcd_buff);
		remain_time--;
		forward_time++;
		if(forward_time == 3)
		{
			forward_time =0;
			if(forward_flag ==0)
			{
				forward_flag = 1;
			}
			else if(forward_flag == 1)
			{
				forward_flag = 0;
			}
		}

		switch(step_flag)
		{
		case 0:
			print_Laundry--;
			move_cursor(1,0);
			sprintf(lcd_buff,"Laundry : %04ds",print_Laundry);
			lcd_string(lcd_buff);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
			if(print_Laundry == 0)
			{
				step_flag = 1;
			}
			break;
		case 1:
			print_Rinse--;
			move_cursor(1,0);
			sprintf(lcd_buff,"Rinse   : %04ds",print_Rinse);
			lcd_string(lcd_buff);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 0);
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 1);

			if(print_Rinse == 0)
			{
				step_flag = 2;
			}
			break;
		case 2:
			print_Spin--;
			move_cursor(1,0);
			sprintf(lcd_buff,"Spin    : %04ds",print_Spin);
			lcd_string(lcd_buff);
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);
			if(print_Spin == 0)
			{
				start_flag =0;
				end_flag = 1;
				step_flag =0;
				print_Laundry = save_Laundry;
				print_Rinse = save_Rinse;
				print_Spin = save_Spin;
				lcd_command(CLEAR_DISPLAY);
			}
			break;

			}
		}
}
void start_moter(void)
{

		if(forward_flag ==0)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
		if(forward_flag == 1)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}

}
void end_moter(void)
{
	start_flag = 0;
	move_cursor(0,0);
	lcd_string("washing end");
	move_cursor(1,0);
	lcd_string("|x |x |x |home");


	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, 1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
	if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		lcd_command(CLEAR_DISPLAY);
		end_flag = 0;
		default_flag = 1;
		stop_flag = 0;

	}
}



void water_high_func(void)
{
	if(water_flag==1)
	{
printf("water_func excute\n");

		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
			{
				HAL_GPIO_WritePin(GPIOD,0xff,0);
				water_low();
			}
		if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
			{
				HAL_GPIO_WritePin(GPIOD,0xff,0);
				water_middle();
			}
		if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin, 2) == BUTTON_PRESS)
			{
				HAL_GPIO_WritePin(GPIOD,0xff,0);
				water_high();
			}
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
			{
				back_flag = 1;
			}
	}
}
void water_high(void)
{
	water_high_num = 3;

	for(int i = 0; i<6; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
	}

	lcd_command(CLEAR_DISPLAY);
	move_cursor(0,0);
	lcd_string("  mode : high");
	move_cursor(1,0);
	lcd_string("|1|2|3|apply|");
}
void water_middle(void)
{
	water_high_num = 2;

	for(int i = 0; i<4; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
	}

	lcd_command(CLEAR_DISPLAY);
	move_cursor(0,0);
	lcd_string("  mode : middle");
	move_cursor(1,0);
	lcd_string("|1|2|3|apply|");
}
void water_low(void)
{
	water_high_num = 1;


	for(int i = 0; i<2; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
	}

	lcd_command(CLEAR_DISPLAY);
	move_cursor(0,0);
	lcd_string("  mode : low");
	move_cursor(1,0);
	lcd_string("|1|2|3|apply|");
}
void view_water(void) // bt1 시작
{

	if(water_flag ==1)
	{
		for(int i = 0; i<4; i++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01<<i, 1);
			move_cursor(0,0);
			lcd_string("  mode : middle");
			move_cursor(1,0);
			lcd_string("|1|2|3|apply|");
		}

	}
}



void time_set_func(void)
{
	if(time_set_flag == 1)
	{
		time_view();
		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
			{
				lcd_command(CLEAR_DISPLAY);
				time_change();
			}
		if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
			{
				time_plus();
			}
		if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin, 2) == BUTTON_PRESS)
			{
				time_minus();
			}
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
			{
				back_flag = 1;
			}
	}
}
void time_change(void)
{
	time_flag ++;
	time_flag %= 3;
}
void time_view(void)
{
	time_set_flag = 1;

	if(time_flag == 0)
	{

		move_cursor(0,0);
		sprintf(lcd_buff,"Laundry time: %d",print_Laundry);
		lcd_string(lcd_buff);
		move_cursor(1,0);
		lcd_string("Rinse plus minus ");
	}
	if(time_flag == 1)
	{

		move_cursor(0,0);
		sprintf(lcd_buff,"Rinse time: %d  ",print_Rinse);
		lcd_string(lcd_buff);
		move_cursor(1,0);
		lcd_string("Spin plus minus  ");
	}
	if(time_flag == 2)
	{

		move_cursor(0,0);
		sprintf(lcd_buff,"Spin time: %d   ",print_Spin);
		lcd_string(lcd_buff);
		move_cursor(1,0);
		lcd_string("Laundry plus minus");
	}
}
void time_plus(void)
{
	switch(time_flag)
	{
	case 0:
		print_Laundry += 10;
		if(print_Laundry >=100)
		{
			print_Laundry =100;
		}
		break;
	case 1:
		print_Rinse += 10;
		if(print_Rinse >=100)
		{
			print_Rinse =100;
		}
		break;
	case 2:
		print_Spin += 10;
		if(print_Spin >=100)
		{
			print_Spin =100;
		}
		break;
	default:
		break;
	}
}
void time_minus(void)
{
	switch(time_flag)
	{
	case 0:
		print_Laundry -= 10;
		if(print_Laundry <=0)
		{
			print_Laundry =0;
		}
		break;
	case 1:
		print_Rinse -= 10;
		if(print_Rinse <= 0)
		{
			print_Rinse =0;
		}
		break;
	case 2:
		print_Spin -= 10;
		if(print_Spin <= 0)
		{
			print_Spin =0;
		}
		break;
	default:
		break;
	}
}



void Mode_end(void)
{
		lcd_command(CLEAR_DISPLAY);


		back_flag = 1;

		switch(Mode_flag)
		{
		case 1:
			move_cursor(0,0);
			lcd_string("apply speed mode");
			print_Laundry = 20;
			print_Rinse =20;
			print_Spin = 20;
			break;
		case 2:
			move_cursor(0,0);
			lcd_string("apply long mode");
			print_Laundry = 40;
			print_Rinse =40;
			print_Spin = 40;
			break;

		}
}
void mode_view(void)
{
	switch(Mode_flag)
		{
	case 0:
		move_cursor(0,0);
		lcd_string("select mode");
		break;
	case 1:
		move_cursor(0,0);
		lcd_string("select speed mod");
		break;
	case 2:
		move_cursor(0,0);
		lcd_string("select  long mod");
		break;
	case 3:
		move_cursor(0,0);
		lcd_string("select  user mod");
		break;
		}
	move_cursor(1,0);
	if(Mode_flag !=3)
	{
		lcd_string("speed|long|user");
	}
	else
	{
		if(save_flag == 0)
		{
			lcd_string("select|reset|x|x");
		}
		else if(save_flag == 1)
		{
			lcd_string("apply save mode    ");

		}
		else if(save_flag == 2)
		{
			lcd_string("reset save mode    ");
		}
	}
}
void Mode_func(void)
{
	if(mode_set_flag == 1 && Mode_flag != 3)
	{

		mode_view();

		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
			{
				Mode_flag = 1;
			}
		if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
			{
				Mode_flag = 2;
			}
		if(get_button(BUTTON2_GPIO_Port,BUTTON2_Pin, 2) == BUTTON_PRESS)
			{
				Mode_flag = 3;
			}
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
			{
				Mode_end();
			}
	}
}

void User_Mode_func(void)
{
	if(Mode_flag == 3)
	{
		mode_view();
		if(get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
			{
				lcd_command(CLEAR_DISPLAY);
				save_flag = 1;
				print_Laundry = save_Laundry;
				print_Rinse = save_Rinse;
				print_Spin = save_Spin;

			}
		if(get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
			{
				lcd_command(CLEAR_DISPLAY);
				save_flag = 2;
				save_Laundry = print_Laundry;
				save_Rinse = print_Rinse;
				save_Spin = print_Spin;
			}
		if(get_button(BUTTON3_GPIO_Port,BUTTON3_Pin, 3) == BUTTON_PRESS)
			{
				lcd_command(CLEAR_DISPLAY);
				back_flag = 1;
			}
	}
}



void washing_init(void)
{
	print_Laundry = save_Laundry;
	print_Rinse = save_Rinse;
	print_Spin = save_Spin;

}
