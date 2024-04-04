#ifndef __AM2302_H
#define __AM2302_H
void AM2302_W_SDA(uint8_t BitValue);
uint8_t AM2302_R_SDA(void);
void AM2302_Init(void);
void AM2302_Start(void);
uint64_t AM2302_ReceiveByte(void);

#endif
