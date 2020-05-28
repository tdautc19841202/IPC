#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define FILE_NAME "/dev/i2c-0"

//General Sensor Support Functions
//RAW SENSOR Reading 24bit format
#define SM_DEFAULT 0xA0 //0x0000
#define SM_USERSET 0xA1 //User Set
#define SM_AZSM_DEFAULT 0xA2 //0x0000/ ppppppppp
#define SM_AZSM_USERSET 0xA3 //User Set
#define TM_DEFAULT 0xA4 //0x0000
#define TM_USERSET 0xA5 //User Set
#define TM_AZTM_DEFAULT 0xA6 //0x0000
#define TM_AZTM_USERSET 0xA7 //User Set
#define SM_TM_AZTM_DEFAULT 0xAA
// Command List
#define Start_NOM 0xA8
#define Start_CM 0xA9
#define Measure 0xAA
#define MeasureCyclic 0xAB
#define Over2Measure 0xAC
#define Over4Measure 0xAD
#define Over8Measure 0xAE
#define Over16Measure 0xAF
#define SET_SM_Config1 0xB0
#define SET_SM_Config2 0xB1
#define STOP_CYC 0xBF
#define NOP 0xF0
// Not Assigned 8bit Registers
#define STATUS 0x00
#define SENSOR_HIGH 0x01
#define SENSOR_MIDDLE 0x02
#define SENSOR_LOW 0x03
#define TEMP_HIGH 0x04
#define TEMP_MIDDLE 0x05
#define TEMP_LOW 0x06
#define COMMAND 0x07
#define COMMAND_DAT1 0x08
#define COMMAND_DAT2 0x09
#define READ_REGISTER 0x0A
#define READ_MEMDAT1 0x0B
#define READ_MEMDAT2 0x0C

static unsigned char mcu_register[] =
{
0x00, // STATUS 0
0x01, // SENSOR_HIGH 1
0x02, // SENSOR_MiDDLE 2
0x03, // SENSOR_LOW 3
0x04, // TEMP_HIGH 4
0x05, // TEMP_MIDDLE 5
0x06, // TEMP_LOW 6
0x00, // COMMAND 7
0x00, // COMMAND_DAT1 8
0x00, // COMMAND_DAT2 9
0x00, // READ_REGISTER 10
0x00, // READ_MEMDAT1 11
0x00, // READ_MEMDAT2 12
0x00 // I2C_Error 13
};
static int i2c_write(int fd, int slave_addr, unsigned char *reg_addr, unsigned char reg_cnt)
{
    unsigned int data_size = 0;

	if (ioctl(fd, I2C_SLAVE_FORCE, slave_addr>>1) < 0) {
		perror("i2cSetAddress");
        return -1;
	}
    data_size = reg_cnt * sizeof(unsigned char);
    if (write(fd, reg_addr, data_size) != data_size)
    {
        perror("i2c_write");
        return -1;
    }
    return 0;
}
static int i2c_read(int fd, int slave_addr, unsigned char *reg_addr, unsigned char reg_cnt, unsigned char *value, unsigned char value_cnt)
{
    unsigned int data_size = 0;

	if (ioctl(fd, I2C_SLAVE_FORCE, slave_addr>>1) < 0) {
		perror("i2cSetAddress");
        return -1;
	}
    data_size = reg_cnt * sizeof(unsigned char);
    if (write(fd, reg_addr, data_size) != data_size)
    {
        perror("i2c_read 1");
        return -1;
    }
    data_size = value_cnt * sizeof(unsigned char);
    if (read(fd, value, data_size) != data_size)
    {
        perror("i2c_read 2");
        return -1;
    }

    return 0;
}


void Hex_to_BCD(unsigned long hex_input,unsigned char *BCD_output)
{
    BCD_output[0] = hex_input / (long)100000;
    hex_input = hex_input - (BCD_output[0]* (long)100000);
    BCD_output[1] = hex_input / (long)10000;
    hex_input = hex_input - (BCD_output[1]* (long)10000);
    BCD_output[2] = hex_input / (long)1000;
    hex_input = hex_input -(BCD_output[2] *(long) 1000);
    BCD_output[3] = hex_input / 100;
    hex_input = hex_input - (BCD_output[3] * 100);
    BCD_output[4] = hex_input / 10;
    hex_input = hex_input - (BCD_output[4] * 10);
    BCD_output[5] = hex_input / 1;
    return;
}

void calculate(unsigned char *symbol,unsigned long *temperature)    
 {      
	float TemperatureValue = 0.0;
	float SensorValue = 0.0;
	float Ambient = 0.0;
	float Delta_T = 0.0;	
	float Object;
	
	
//	mcu_register[TEMP_HIGH] = 0x8A;
//	mcu_register[TEMP_MIDDLE] = 0x09;
//	mcu_register[TEMP_LOW] = 0x40;
//	mcu_register[SENSOR_HIGH] = 0x7F;
//	mcu_register[SENSOR_MIDDLE] = 0xD6;
//	mcu_register[SENSOR_LOW] = 0xC0;
	  
	  
	
	//TemperatureValue = make32(mcu_register[TEMP_HIGH], mcu_register[TEMP_MIDDLE], mcu_register[TEMP_LOW]);
	
	TemperatureValue = (float)(mcu_register[TEMP_HIGH]*65536)+(float)(mcu_register[TEMP_MIDDLE]*256)+(float)mcu_register[TEMP_LOW];
		
	//SensorValue = make32(mcu_register[SENSOR_HIGH], mcu_register[SENSOR_MIDDLE], mcu_register[SENSOR_LOW]);
	
	SensorValue = (float)((float)mcu_register[SENSOR_HIGH]*(float)65536)+(float)((float)mcu_register[SENSOR_MIDDLE]*(float)256)+(float)mcu_register[SENSOR_LOW];
		
//	if (TemperatureValue > 8388607)//Change from 2’s complement
//	  TemperatureValue = 16777216 - TemperatureValue;
//	if (SensorValue > 8388607) //Change from 2’s complement
//	  SensorValue = 16777216 - SensorValue;
	
	Ambient = (TemperatureValue / (float)pow(2, 24))*(float)125.0 - (float)40.0;
	Delta_T = (float)1000*(SensorValue / (float)pow(2, 24) -(float) 0.5);
	Object = Ambient + Delta_T;  
	if(Object < 0)
	  *symbol = 0;
	else
	  *symbol = 1;
	*temperature = (unsigned long)(Object*(float)1000);
	

	return;      
}

int main(void)
{
    int fd = 0, i = 0;
    char bm_start_cmd[1] = {0xA9};
    char bm_stop_cmd[1] = {0xA8};
    char bm_measure_cmd[1] = {0xAA};
    char bm_measure_data[7] = {0, 0, 0, 0, 0, 0, 0};
    char bm_measure_idx[7] = {STATUS, SENSOR_HIGH, SENSOR_MIDDLE, SENSOR_LOW, TEMP_HIGH, TEMP_MIDDLE, TEMP_LOW};
    const char *bm_measure_info[7] = {"Status", "Sensor high", "Sensor middle", "Sensor low", "Temp high", "Temp middle", "Temp low"};
    unsigned char symbol = 0;
    unsigned long temperature = 0;
    unsigned char temp[6];
    
    memset(temp, 0, 6);
    fd = open(FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    i2c_write(fd, 0, (unsigned char *)bm_start_cmd, 1);
    printf("BM Start!\n");
    while (1)
    {
        i2c_read(fd, 0, (unsigned char *)bm_measure_cmd, 1, (unsigned char *)bm_measure_data, 7);
        for (i = 0; i < 7; i++)
        {
            printf("%s: = 0x%x\n", bm_measure_info[i], bm_measure_data[i]);
            mcu_register[(unsigned char)bm_measure_idx[i]] = (unsigned char)bm_measure_data[i];   
        }
        printf("BM Get data!\n");
        calculate(&symbol, &temperature);
        Hex_to_BCD(temperature, temp);
        printf("Temperature: (%c)%d%d%d.%d%d%d\n", symbol?('+'):('-'), temp[0] & 0xF, temp[1] & 0xF, temp[2] & 0xF, temp[3] & 0xF, temp[4] & 0xF, temp[5] & 0xF);
        getchar();
    }
    i2c_write(fd, 0, (unsigned char *)bm_stop_cmd, 1);
    close(fd);

    return 0;
}
