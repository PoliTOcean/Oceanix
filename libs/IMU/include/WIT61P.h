#ifndef __WIT_61_P
#define __WIT_61_P

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"
#include "wit_c_sdk.h"
#include <stdio.h>

#define TIMEOUT	3
#define RETRY	3

#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80

static void AutoScanSensor(void);
static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum);
static void Delayms(uint16_t ucMs);
static int i2c_read(u8 addr, u8 reg, u8 *data, u32 len);
static int i2c_write(u8 addr, u8 reg, u8 *data, u32 len);
int WT61P_begin(char* dev, uint8_t addr);
int WT61P_read_angle();
int WT61P_read_acc();
int WT61P_read_gyro();
int WT61P_read_all();
float WT61P_get_roll();
float WT61P_get_pitch();
float WT61P_get_yaw();
float WT61P_get_AX();
float WT61P_get_AY();
float WT61P_get_AZ();
float WT61P_get_GX();
float WT61P_get_GY();
float WT61P_get_GZ();
float WT61P_get_temp();


#ifdef __cplusplus
}
#endif

#endif /* __WIT_C_SDK_H */
