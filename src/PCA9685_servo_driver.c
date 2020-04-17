#include "PCA9685_servo_driver.h"

static uint8_t sensor_addr = 0x80;


/**************************************************************************/
/*! 
    @brief  Initial the I2C interface and hardware
*/
/**************************************************************************/
void PCA9685_init(uint8_t i2caddr){
  sensor_addr = i2caddr;
  pioInit();
  sccb_bus_init();
  PCA9685_reset();
  // set a default frequency
  PCA9685_setPWMFreq(1000);
}


/**************************************************************************/
/*! 
    @brief  Sends a reset command to the PCA9685 chip over I2C
*/
/**************************************************************************/
void PCA9685_reset(void) {
  wrSensorReg8_8(PCA9685_MODE1, 0x80);
  delay_ms(10);
}

/**************************************************************************/
/*! 
    @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
    @param  freq Floating point frequency that we will attempt to match
*/
/**************************************************************************/
void PCA9685_setPWMFreq(float freq) {
  freq *= 0.9;  // Correct for overshoot in the frequency setting.
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;
  uint8_t prescale = floor(prescaleval + 0.5);
  
  uint8_t oldmode;
  rdSensorReg8_8(PCA9685_MODE1, &oldmode);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  wrSensorReg8_8(PCA9685_MODE1, newmode); // go to sleep
  wrSensorReg8_8(PCA9685_PRESCALE, prescale); // set the prescaler
  wrSensorReg8_8(PCA9685_MODE1, oldmode);
  delay_ms(5);
  wrSensorReg8_8(PCA9685_MODE1, oldmode | 0xa0);  //  This sets the MODE1 register to turn on auto increment.
}

/**************************************************************************/
/*! 
    @brief  Sets the PWM output of one of the PCA9685 pins
    @param  num One of the PWM output pins, from 0 to 15
    @param  on At what point in the 4096-part cycle to turn the PWM output ON
    @param  off At what point in the 4096-part cycle to turn the PWM output OFF
*/
/**************************************************************************/
void PCA9685_setPWM(uint8_t num, uint16_t on, uint16_t off) {
  wrSensorReg8_8(LED0_ON_L+4*num, on);
  wrSensorReg8_8(LED0_ON_L+4*num + 1, on>>8);
  wrSensorReg8_8(LED0_ON_L+4*num + 2, off);
  wrSensorReg8_8(LED0_ON_L+4*num + 3, off>>8);
}

/**************************************************************************/
/*! 
    @brief  Helper to set pin PWM output. Sets pin without having to deal with on/off tick placement and properly handles a zero value as completely off and 4095 as completely on.  Optional invert parameter supports inverting the pulse for sinking to ground.
    @param  num One of the PWM output pins, from 0 to 15
    @param  val The number of ticks out of 4096 to be active, should be a value from 0 to 4095 inclusive.
    @param  invert If true, inverts the output, defaults to 'false'
*/
/**************************************************************************/
void PCA9685_setPin(uint8_t num, uint16_t val, _Bool invert)
{
  // Clamp value between 0 and 4095 inclusive.
  //val = min(val, (uint16_t)4095);
  (val < (uint16_t)4095) ? (val = val): (val = (uint16_t)4095);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      PCA9685_setPWM(num, 4096, 0);
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      PCA9685_setPWM(num, 0, 4096);
    }
    else {
      PCA9685_setPWM(num, 0, 4095-val);
    }
  }
  else {
    if (val == 4095) {
      // Special value for signal fully on.
      PCA9685_setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      PCA9685_setPWM(num, 0, 4096);
    }
    else {
      PCA9685_setPWM(num, 0, val);
    }
  }
}
/**************************************************************************/
/*! 
    @brief  Set the pulse width of servo.
    @param  PWM output channal.
    @param  Pulse width(seconds)
*/
/**************************************************************************/
void setServoPulse(uint8_t n, double pulse) {
   double pulselength;
   
   pulselength = 1000;   // 1,000 ms per second 
   pulselength /= 60;   // 60 Hz
   pulselength /= 4096;  
   pulse *= 1000; //ms
   pulse /= pulselength;
   PCA9685_setPWM(n, 0, pulse);
}
/**************************************************************************/
/*! 
    @brief  Set the rotation degree of the servo.
    @param  PWM output channel.
    @param  Degree.
*/
/**************************************************************************/
void setServoDegree(uint8_t n, uint8_t Degree){
    if(Degree >= 180){
      Degree = 180;
    }
    else if(Degree <= 0){
      Degree = 0;
    }
    double pulse = (Degree + 45) / (90.0 * 1000);
    setServoPulse(n, pulse);
}

/**************************************************************************/
/*! 
    @brief  Increase the degree of the specified channel.
    @param  PWM output channel.
    @param  The increased value.
*/
/**************************************************************************/
int ServoDegreeIncrease(uint8_t Channel, uint8_t Step){
    switch(Channel){
        case SERVO_UP_CH:
            if(ServoUpDegree >= SERVO_UP_MAX){
                ServoUpDegree = SERVO_UP_MAX;
                setServoDegree(Channel, ServoUpDegree);
            }
            else{
                ServoUpDegree += Step;
                setServoDegree(Channel, ServoUpDegree); 
            }
            break;
        case SERVO_DOWN_CH:
            if(ServoDownDegree >= SERVO_DOWN_MAX){
                ServoDownDegree = SERVO_DOWN_MAX;
                setServoDegree(Channel, ServoDownDegree); 
            }
            else{
                ServoDownDegree += Step;
                setServoDegree(Channel, ServoDownDegree); 
            }
            break;
        default:
            return 1;
            break;
    }
    delay_ms(STEP_DELAY);
    return 0;
}

/**************************************************************************/
/*! 
    @brief  Decrease the degree of the specified channel.
    @param  PWM output channel.
    @param  The decreased value.
*/
/**************************************************************************/
int ServoDegreeDecrease(uint8_t Channel, uint8_t Step){
    switch(Channel){
        case SERVO_UP_CH:
            if(ServoUpDegree <= SERVO_UP_MIN + Step){
                ServoUpDegree = SERVO_UP_MIN;
                setServoDegree(Channel, ServoUpDegree);
            }
            else{
                ServoUpDegree -= Step;
                setServoDegree(Channel, ServoUpDegree); 
            }
            break;
        case SERVO_DOWN_CH:
            if(ServoDownDegree <= SERVO_DOWN_MIN + Step){
                ServoDownDegree = SERVO_DOWN_MIN;
                setServoDegree(Channel, ServoDownDegree); 
            }
            else{
                ServoDownDegree -= Step;
                setServoDegree(Channel, ServoDownDegree); 
            }
            break;
        default:
            return 1;
            break;
    }
    delay_ms(STEP_DELAY);
    return 0;
}

/**************************************************************************/
/*! 
    @brief  Write 8bits data to sensor address.
    @param  Register address.
    @param  Write data.
*/
/**************************************************************************/
unsigned char wrSensorReg8_8(int regID, int regDat)
{
	delay_us(10);
	sccb_bus_start();                          
	if(sccb_bus_write_byte(sensor_addr) == 0)         
	{
		sccb_bus_stop();                        
		return 1;
	}
	delay_us(10);
	if(sccb_bus_write_byte(regID) == 0)
	{
		sccb_bus_stop();                              
		return 2;                                       
	}
	delay_us(10);
	if(sccb_bus_write_byte(regDat)==0)                    
	{
		sccb_bus_stop();                                 
		return 3;
	}
	sccb_bus_stop();                                    
	return 0;
}

/**************************************************************************/
/*! 
    @brief  Read 8bits data from sensor address.
    @param  Register address.
    @param  Read data.
*/
/**************************************************************************/
unsigned char rdSensorReg8_8(unsigned char regID, unsigned char* regDat)
{
	delay_us(10);
	
	sccb_bus_start();
	if(sccb_bus_write_byte(sensor_addr) == 0)                 
	{
		sccb_bus_stop();                                
		//goto start;
		return 1;                                        
	}
	delay_us(10);
	if(sccb_bus_write_byte(regID)==0)//ID
	{
		sccb_bus_stop();                                  
		//goto start;
		return 2;                                       
	}
	sccb_bus_stop();                                   
	delay_us(10);	
	sccb_bus_start();
	if(sccb_bus_write_byte(sensor_addr|0x01)==0)                    
	{
		sccb_bus_stop();                                   
		//goto start;
		return 3;                                          
	}
	delay_us(10);
	*regDat = sccb_bus_read_byte();                    
	sccb_bus_send_noack();                                
	sccb_bus_stop();                                      
	return 0;                
}


