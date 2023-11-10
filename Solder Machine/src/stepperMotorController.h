#ifndef STEPPERMOTORCONTROLLER_
#define STEPPERMOTORCONTROLLER_
#include <Arduino.h>
#include "systemData.h"

class StepperController
{
private:
    HardwareSerial *outStream;
    int dataOut = 0;
    int dataLatch = 0;
    int dataClock = 0;

    int stepM1 = 0;
    int stepM2 = 0;
    int stepM3 = 0;

    unsigned int speedDelayM1 = 75;
    unsigned int speedDelayM2 = 75;
    unsigned int speedDelayM3 = 75;

    unsigned int preShiftData = 0;

    bool enableM1 = 0; //      1
    bool enableM2 = 0; //      4
    bool enableM3 = 0; //      16

    bool directionM1 = 0; //      2
    bool directionM2 = 0; //      8
    bool directionM3 = 0; //      32

protected:
    void extrude(const float distance, const int motor, const int speed);
    void shiftData();

public:
    void setData(const long data);

    StepperController(void);
    StepperController(const SystemData data);

    ~StepperController(void);

    void shiftOutCos(const uint8_t dataPin, const uint8_t clockPin, const uint8_t bitOrder, const long val);
    void begin(const SystemData data, HardwareSerial &serialData);
    void test(void);

    void stepperM1En(const bool status);
    void stepperM2En(const bool status);
    void stepperM3En(const bool status);

    void stepperM1Dir(const bool rev);
    void stepperM2Dir(const bool rev);
    void stepperM3Dir(const bool rev);

    void extrudeM1(const float distance);
    void extrudeM2(const float distance);
    void extrudeM3(const float distance);

    void extrudeM1E(const float distance);
    void extrudeM2E(const float distance);
    void extrudeM3E(const float distance);

    void extrudeM1DE(const float distance, const bool rev);
    void extrudeM2DE(const float distance, const bool rev);
    void extrudeM3DE(const float distance, const bool rev);

    void extrudeM1DE(const float distance, const bool rev, int Speed);
    void extrudeM2DE(const float distance, const bool rev, int Speed);
    void extrudeM3DE(const float distance, const bool rev, int Speed);

    void motorsOn();
    void motorsOff();
};

#endif