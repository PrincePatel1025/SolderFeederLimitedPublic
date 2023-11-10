#ifndef SYSTEMDATA_
#define SYSTEMDATA_

#include <Arduino.h>
struct SystemData
{
    // const int dataOut = 11;
    // const int dataLatch = 10;
    // const int dataClock = 9;
    // const int stepMotor1 = 3;
    // const int stepMotor2 = 4;
    // const int stepMotor3 = 5;

    const int dataOut = 2;
    const int dataLatch = 10;
    const int dataClock = 9;
    const int stepMotor1 = 4;
    const int stepMotor2 = 5;
    const int stepMotor3 = 6;

    const int btn1_2 = A3;
    const int btn3_4 = A0;
    const int btn5_6 = A7;

    const int rotaryInputA = 11;
    const int rotaryInputB = 12;
    const int rotaryBtnInput = 7;
};

struct SystemVar
{
    HardwareSerial *outstream;
    const int slowSpeedDelay = 200;
    const int midSpeedDelay = 73;
    const int fastSpeedDelay = 55;

    const int speedDelayM1 = 65;
    const int speedDelayM2 = 65;
    const int speedDelayM3 = 65;
};

#endif
