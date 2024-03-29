#include <PCA9685.h>

static int fd = 0;
uint8_t ServoUpDegree = 90;    //Servo Up Angle
uint8_t ServoDownDegree =90;   //Servo Down Angle
/*   
// functions：i2c_init
// parameters：i2c_dev：i2c device file path，i2c_addr：i2c address
// return：0 success，-1 failed
*/
int i2c_init(char *i2c_dev, unsigned char i2c_addr)
{
    int res =0;
    fd = open(i2c_dev, O_RDWR);  
    if (fd < 0) 
    {
        printf("Failed to open the i2c bus\n");
        return -1;
    }
    res = ioctl(fd, I2C_TENBIT, 0);        //7-bit mode
    res = ioctl(fd, I2C_SLAVE, i2c_addr);  //Sets the slave device address
    return res;
}
/*  
// functions：i2c reading data
// parameters：buf：Data buffer，len：length
// return: The length of the actual read   
*/
int i2c_readNbyte(unsigned char *buf, int len)
{
	int res = 0;
	res = read(fd, buf,len);
	return res;
}
/*  
// functions：i2c writing data
// parameters：buf：Data buffer，len：length
// return: The length of the actual write    
*/
int i2c_writeNbyte(unsigned char *buf, int len)
{
	int res = 0;
	res = write(fd, buf,len);
	return res;
}
/*  
// functions：read at specified address
// parameters：reg_addr： register address，buf：Data buffer
// return: The length of the actual read    
*/
int i2c_readReg(uint8_t reg_addr, uint8_t *data)
{
	int res = 0;
	write(fd, &reg_addr, 1);
	res = read(fd, data,1);
}
/*  
// functions：write at specified address
// parameters：reg_addr： register address，buf：Data buffer
// return: The length of the actual write   
*/
int i2c_writeReg(uint8_t reg_addr, uint8_t data)
{
	int res = 0,i;
	uint8_t *buff = 0;
	buff = (uint8_t *)malloc(2);
	buff[0] = reg_addr;
    buff[1] = data;
    res = write(fd, buff, 2);
	free(buff);
	return res;
}
/*  
// functions：set frequency
// parameters：freq：value
// return：no
*/
void PCA9685_setPWMFreq(float freq)
{
  freq *= 0.8449;                        // Correct the overshoot of the frequency setting
  float prescaleval = 25000000;       // 25MHz
  prescaleval /= 4096;                // 4096 PWM cycles
  prescaleval /= freq;                // Calculate the frequency division value
  prescaleval -= 1;
  float prescale = (float)(prescaleval + 0.5);
  uint8_t oldmode;
  i2c_readReg(PCA9685_MODE1, &oldmode);
  uint8_t newmode = (oldmode&0x7F) | 0x10; 
  i2c_writeReg(PCA9685_MODE1, newmode);              // sleep
  i2c_writeReg(PCA9685_PRESCALE, (uint8_t)prescale); // Set predivider
  i2c_writeReg(PCA9685_MODE1, oldmode);
  usleep(5000);
  i2c_writeReg(PCA9685_MODE1, oldmode | 0xa0);       // Set the MODE1 register to turn on autoincrement
}
/*  
// functions：Set PWM value
// parameters：num：channel number，on：duty cycle high bytes，off： duty cycle low bytes
// return：no   
*/
void PCA9685_setPWM(uint8_t num, uint16_t on, uint16_t off)
{
  i2c_writeReg(LED0_ON_L+4*num, on);
  i2c_writeReg(LED0_ON_L+4*num + 1, (on>>8));
  i2c_writeReg(LED0_ON_L+4*num + 2, off);
  i2c_writeReg(LED0_ON_L+4*num + 3, off>>8);
}
/*  
// functions：Set servo pulse
// parameters：num：channel number，pulse：pulse value
// return：no 
*/
void setServoPulse(uint8_t n, double pulse)
{
   double pulselength;
   pulselength = 1000;   // 1,000 ms per second 
   pulselength /= 60;    // 60 Hz
   pulselength /= 4096;  
   pulse *= 1000;        //ms
   pulse /= pulselength;
   PCA9685_setPWM(n, 0, pulse);
}
/*  
// functions：Set the Angle of rotation of the servo
// parameters：n：channel number，Degree：angle value
// return：no
*/
void setServoDegree(uint8_t n, uint8_t Degree)
{
    if(Degree >= 180)
    {
      Degree = 180;
    }
    else if(Degree <= 0)
    {
      Degree = 0;
    }
    double pulse = (Degree + 45) / (90.0 * 1000);
    setServoPulse(n, pulse);
}
/*  
// functions：getting key values
// parameters：no
// return：  key values  
*/
int get_key_board_from_termios()
{
    int key_value;
    struct termios new_config;
    struct termios old_config;

    tcgetattr(0, &old_config);
    new_config = old_config;
    new_config.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new_config);
    key_value = getchar();
    tcsetattr(0, TCSANOW, &old_config);
    return key_value;
}
/*  
// functions：Increases the degree of the specified channel
// parameters：channel：通道号，step：步数
// retrun：   key values    
*/
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
    usleep(STEP_DELAY * 1000);
    return 0;
}
/*  
// functions：Increases the degree of the specified channel
// parameters：channel：通道号，step：步数
// retrun：   key values    
*/
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
    usleep(STEP_DELAY * 1000);
    return 0;
}
/*  
// functions：keys control the steering gear rotation
// parameters：no
// retrun：no   
*/
void processKeyboardEvent(void){
    int keyVal = 0;
    while(1){
        usleep(100);
        tcflush(0, TCIOFLUSH);
        keyVal= get_key_board_from_termios();
        if(keyVal == 27){
            keyVal= get_key_board_from_termios();
            if(keyVal == 91){
              keyVal= get_key_board_from_termios();
               switch (keyVal)
                {
                    case 65/* up */:   
                        ServoDegreeIncrease(SERVO_UP_CH, STEP);
                        break;
                    case 66/* down */:
                        ServoDegreeDecrease(SERVO_UP_CH, STEP);
                        break;
                    case 67/* right */: 
                        ServoDegreeIncrease(SERVO_DOWN_CH, STEP);
                      break;
                    case 68/* left */: 
                        ServoDegreeDecrease(SERVO_DOWN_CH, STEP);
                        break;   
                    default :
                      break;
                }
            }
          }
      }
}
