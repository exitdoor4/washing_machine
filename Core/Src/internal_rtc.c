#include "main.h"
#include "i2c_lcd.h"
#include "string.h"
#include "stdlib.h"
#include "button.h"

void get_rtc(void);
void set_rtc(char *date_time);
unsigned char bin2des(unsigned char byte);
unsigned char dec2bin(unsigned char byte);



extern RTC_HandleTypeDef hrtc;
extern uint8_t lcd_display_mode_flag;

RTC_TimeTypeDef sTime = {0}; // time information;
RTC_DateTypeDef sDate = {0};
RTC_TimeTypeDef mTime = {0};


// 23년이 save된 binary format
// 7654 3210
// STM32의 RTC로부터 날짜 & 시각 정보를 읽어 오는 함수

unsigned char bin2dec(unsigned char byte)
{
	unsigned char high, low;
	low = byte & 0x0f; // 하위 4bit(low nibble)
	high = (byte >> 4) * 10; // 상위 4bit(high nibble)
	return high+low;
}

//decimal --> BCD ex)
unsigned char dec2bin(unsigned char byte)
{
	unsigned char high,low;
	high = (byte / 10) << 4;
	low = byte % 10;
	return high + low;

}

void get_rtc(void)
{
	static RTC_TimeTypeDef oldTime;
	char rtc_lcd_buff[40];

	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	if(oldTime.Seconds != sTime.Seconds)
	{
		// YYYY-MM-DD HH:mm:ss
		printf("%04d-%02d-%02d %02d:%02d:%02d\n",
			bin2dec(sDate.Year)+2000, bin2dec(sDate.Month), bin2dec(sDate.Date)
			,bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));

		if(lcd_display_mode_flag ==0)
		{
			sprintf(rtc_lcd_buff, "DATE: %04d-%02d-%02d ",
				bin2dec(sDate.Year)+2000, bin2dec(sDate.Month), bin2dec(sDate.Date));
			move_cursor(0, 0);
			lcd_string(rtc_lcd_buff);

			sprintf(rtc_lcd_buff, "TIME: %02d:%02d:%02d",
				bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
			move_cursor(1, 0);
			lcd_string(rtc_lcd_buff);
		}



	}

	oldTime.Seconds = sTime.Seconds;
}

//setrtc231016103800
void set_rtc(char *date_time)
{
	char yy[4],mm[4],dd[4]; // data
	char hh[4],min[4],ss[4]; // time


		strncpy(yy,date_time+6,2);
		strncpy(mm,date_time+8,2);
		strncpy(dd,date_time+10,2);
		strncpy(hh,date_time+12,2);
		strncpy(min,date_time+14,2);
		strncpy(ss,date_time+16,2);

		// ascii --> int ->>bcd

		sDate.Year=dec2bin(atoi(yy));
		sDate.Month=dec2bin(atoi(mm));
		sDate.Date=dec2bin(atoi(dd));
		sTime.Hours=dec2bin(atoi(hh));
		sTime.Minutes=dec2bin(atoi(min));
		sTime.Seconds=dec2bin(atoi(ss));

		HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}



void set_time_button_b1(void)
{
	static uint8_t hh = 0, min = 0, ss= 0;


	// 0,0 에 설정


	char rtc_lcd_buff[20];
	if(lcd_display_mode_flag ==3)
	{

		char lcd_buff[40];
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
		sprintf(lcd_buff, "DATE: %04d-%02d-%02d ",
		bin2dec(sDate.Year)+2000, bin2dec(sDate.Month), bin2dec(sDate.Date));
		move_cursor(0, 0);
		lcd_string(lcd_buff);

		sprintf(rtc_lcd_buff, "TIME: %02d:%02d:%02d",
		bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
		move_cursor(1, 0);
		lcd_string(rtc_lcd_buff);

		if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0)== BUTTON_PRESS)
		{
			hh++;

			if(hh>12)
			{
				hh = 1;
				mTime.Hours = dec2bin(hh);
			}
			else
			{
				mTime.Hours = dec2bin(hh);
			}

		}
		if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1)== BUTTON_PRESS)
		{
			min++;
			if(min>=60)
			{
				min = 0;
				hh++;
				if(hh>=12)
				{
					hh=1;
					mTime.Hours = dec2bin(hh);
				}
				else
				{
					mTime.Hours = dec2bin(hh);
				}
			}
			else
			{
				mTime.Minutes=dec2bin(min);
			}
		}

		}
		if(get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2)== BUTTON_PRESS)
		{
			ss++;

			if(ss>=60)
			{
				ss = 0;
				mTime.Seconds = dec2bin(ss);
				min++;
				if(min>=60)
				{
					min=0;
					hh++;
					if(hh>=12)
					{
						hh=0;
					}
					else
					{
						mTime.Hours = dec2bin(hh);
					}
				}
				else
				{
					mTime.Minutes=dec2bin(min);
				}
			}
			else
			{
				mTime.Seconds=dec2bin(ss);
			}

		}
		if(get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3)== BUTTON_PRESS)
		{

			//HAL_RTC_SetTime(&hrtc, &mTime, RTC_FORMAT_BCD);
			//hh = 0, min = 0, ss = 0;
			//lcd_display_mode_flag ==0;
			for(int i=0;i<5;i++)
			{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1);
			HAL_Delay(200);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
			HAL_Delay(500);
			}
		}
	}

