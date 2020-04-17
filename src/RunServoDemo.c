#include "PCA9685_servo_driver.h"
#include <termios.h>
#include <pthread.h>
#include <signal.h>

uint8_t ServoUpDegree = 90;
uint8_t ServoDownDegree = 90;
pthread_t manualFocusPthreadID;

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

int main(int argc, char *argv[]){
    PCA9685_init(I2C_ADDR);
    PCA9685_setPWMFreq(60);  // Analog servos run at ~60 Hz updates
    setServoDegree(SERVO_UP_CH, ServoUpDegree);
    setServoDegree(SERVO_DOWN_CH, ServoDownDegree);
    delay_ms(1000);
    processKeyboardEvent();
}





