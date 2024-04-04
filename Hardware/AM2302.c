#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/*SDA线写入*/
void AM2302_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)BitValue);
}

/*SDA线读取*/
uint8_t AM2302_R_SDA(void)
{
	/*通信开始已度过高电平80ms数据准备发送信号
	AM2302的数据均以50ms低电平开头,'0'数据高电平时间26-28us,,'1'数据高电平时间70us
	下方死循环用于度过开头的低电平信号*/
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);
	Delay_us(40);//低电平结束后延时40us,如果当前为数据'0'则高电平时间结束SDA线仍为低电平,反之为高电平
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);//返回SDA线电平
}

/*引脚初始化*/
void AM2302_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*通信开始*/
void AM2302_Start(void)
{
	AM2302_W_SDA(1);//SDA线置高电平
	Delay_ms(1);//高电平维持1ms
	AM2302_W_SDA(0);//拉低SDA线至低电平产生下降沿
	Delay_ms(1);//低电平维持1ms,通知AM2302准备发送数据
	AM2302_W_SDA(1);//主机放手SDA线控制权交由AM2302
	Delay_us(60);//度过20ms高电平,延时完成后处于80ms低电平的中央位置
	
	/*当主从机通信不畅时以下循环可能会成为死循环,可加入容错机制*/
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);//度过AM2302发送的低电平80ms准备信号
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);//度过AM2302发送的高电平80ms数据准备发送信号
}

/*接收数据*/
uint64_t AM2302_ReceiveByte(void)
{
	uint64_t Byte = 0;
	for (uint8_t i = 0; i < 40; i ++)
	{
		if (AM2302_R_SDA() == 1)
		{
			Byte |= (0x8000000000 >> i);//高位先行
			//如果当前数据为'1'则需要等待高电平时间结束,用以对齐下一个数据
			while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
		}
	}
	return Byte;
}
