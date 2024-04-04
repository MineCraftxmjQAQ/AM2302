#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AM2302.h"

uint64_t data;//接收40位数据
int16_t temperature, humidity;//温度和湿度值
int main(void)
{
	OLED_Init();
	AM2302_Init();
	OLED_ShowString(1, 1, "Temp:");
	OLED_ShowString(2, 1, "Humi:");
	OLED_ShowString(2, 10, "%");
	Delay_s(2);//AM2302上电初始化延时
	while (1)
	{
		AM2302_Start();//发起通信
		data = AM2302_ReceiveByte();//接收数据
		
		temperature = (uint16_t)((data >> 8) & 0x0000ffff);
		humidity= (int16_t)((data >> 24) & 0x0000ffff);
		//OLED显示控制
		OLED_ShowSignedNum(1, 6, temperature / 10, 2);
		OLED_ShowString(1, 9, ".");
		OLED_ShowNum(1, 10, temperature % 10, 1);
		OLED_ShowNum(2, 6, humidity / 10, 2);
		OLED_ShowString(2, 8, ".");
		OLED_ShowNum(2, 9, humidity % 10, 1);
		OLED_ShowHexNum(3, 1,  (data & 0x000000ff00000000) >> 32, 2);
		OLED_ShowHexNum(3, 4,  (data & 0x00000000ff000000) >> 24, 2);
		OLED_ShowHexNum(3, 7,  (data & 0x0000000000ff0000) >> 16, 2);
		OLED_ShowHexNum(3, 10, (data & 0x000000000000ff00) >> 8 , 2);
		OLED_ShowHexNum(3, 13, (data & 0x00000000000000ff)		, 2);
		OLED_ShowString(4, 1, "                ");//清空第四行防止显示错误
		if
		(
			(
				(
					((data & 0x000000ff00000000) >> 32) +
					((data & 0x00000000ff000000) >> 24) +
					((data & 0x0000000000ff0000) >> 16) +
					((data & 0x000000000000ff00) >> 8)
				) & 0x00ff
			) == (data & 0x00000000000000ff)
		)//检查(湿度高八位+湿度低八位+温度高八位+温度低八位)的低八位是否等于校验位
		{
			OLED_ShowString(4, 1, "Check_Success");
		}
		else
		{
			OLED_ShowString(4, 1, "Check_Failed");
		}
		Delay_s(2);//数据接收间隔2秒
	}
}
