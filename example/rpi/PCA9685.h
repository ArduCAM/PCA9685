#ifndef __PCA9685_H__
#define __PCA9685_H__
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
/*PCA9685 register address*/
#define I2C_ADDR           0x40  //IIC address 

#define MODE1              0x00
#define MODE2              0x01
#define SUBADR1            0x02
#define SUBADR2            0x03

#define PCA9685_SUBADR1    0x2
#define PCA9685_SUBADR2    0x3
#define PCA9685_SUBADR3    0x4

#define PCA9685_MODE1      0x0
#define PCA9685_PRESCALE   0xFE

#define LED0_ON_L          0x6
#define LED0_ON_H          0x7
#define LED0_OFF_L         0x8
#define LED0_OFF_H         0x9

#define ALLLED_ON_L        0xFA
#define ALLLED_ON_H        0xFB
#define ALLLED_OFF_L       0xFC
#define ALLLED_OFF_H       0xFD

#define SERVO_UP_MAX       145    
#define SERVO_UP_MIN       15     
#define SERVO_DOWN_MAX     180  
#define SERVO_DOWN_MIN     0        
#define STEP               1             
#define STEP_DELAY         2        
#define SERVO_UP_CH        0       
#define SERVO_DOWN_CH      1     

#define I2C_SLAVE_FORCE    0x0706

extern uint8_t ServoUpDegree;    
extern uint8_t ServoDownDegree;   
int i2c_init(char *i2c_dev, unsigned char i2c_addr);
int i2c_writeNbyte(unsigned char *buf, int len);
int i2c_readNbyte(unsigned char *buf, int len);
int i2c_readReg(uint8_t reg_addr, uint8_t *data);
int i2c_writeReg(uint8_t reg_addr, uint8_t data);
void PCA9685_setPWMFreq(float freq);
void PCA9685_setPWM(uint8_t num, uint16_t on, uint16_t off);
void setServoPulse(uint8_t n, double pulse);
void setServoDegree(uint8_t n, uint8_t Degree);
int get_key_board_from_termios();
int ServoDegreeIncrease(uint8_t Channel, uint8_t Step);
void processKeyboardEvent(void);
unsigned long  get_microsecond_timestamp();
void delay_us(int micros);
void delay_ms(int millis);
int ServoDegreeDecrease(uint8_t Channel, uint8_t Step);
#endif
