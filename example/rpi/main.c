#include <PCA9685.h>

int main(int argc,char **argv)
{
    char i2c_dev[32];
    snprintf(i2c_dev, sizeof(i2c_dev), "/dev/i2c-1");
    i2c_init(i2c_dev, I2C_ADDR);
    i2c_writeReg(MODE1, 0x80);
    usleep(10000);
    PCA9685_setPWMFreq(1000);  //Set the default frequency
    PCA9685_setPWMFreq(60);
    setServoDegree(SERVO_UP_CH,ServoUpDegree);
    setServoDegree(SERVO_DOWN_CH,ServoDownDegree );
    processKeyboardEvent();
    while(1)
    {
        
    }
    return 0;
}
