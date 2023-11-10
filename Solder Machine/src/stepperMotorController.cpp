#include <Arduino.h>
#include <stepperMotorController.h>
#include "systemData.h"

StepperController::StepperController(void) {}
StepperController::~StepperController(void) {}

StepperController::StepperController(const SystemData data)
{
    dataOut = data.dataOut;
    dataLatch = data.dataLatch;
    dataClock = data.dataClock;

    stepM1 = data.stepMotor1;
    stepM2 = data.stepMotor2;
    stepM3 = data.stepMotor3;
}
void StepperController::begin(const SystemData data, HardwareSerial &serialData)
{
    outStream = &serialData;

    dataOut = data.dataOut;
    dataLatch = data.dataLatch;
    dataClock = data.dataClock;

    stepM1 = data.stepMotor1;
    stepM2 = data.stepMotor2;
    stepM3 = data.stepMotor3;

    pinMode(dataClock, OUTPUT);
    pinMode(dataLatch, OUTPUT);
    pinMode(dataOut, OUTPUT);

    pinMode(stepM1, OUTPUT);
    pinMode(stepM2, OUTPUT);
    pinMode(stepM3, OUTPUT);

    digitalWrite(stepM1, LOW);
    digitalWrite(stepM2, LOW);
    digitalWrite(stepM3, LOW);

    motorsOff();
}
void StepperController::test() {}

void StepperController::motorsOn()
{
    stepperM1En(true);
    stepperM2En(true);
    stepperM3En(true);
}
void StepperController::motorsOff()
{
    stepperM1En(false);
    stepperM2En(false);
    stepperM3En(false);
}

void StepperController::stepperM1En(const bool status)
{
    switch (status)
    {
    case true:
        enableM1 = false;
        break;

    case false:
        enableM1 = true;
        break;
    };
    shiftData();
}
void StepperController::stepperM2En(const bool status)
{
    switch (status)
    {
    case true:
        enableM2 = false;
        break;

    case false:
        enableM2 = true;
        break;
    };
    shiftData();
}
void StepperController::stepperM3En(const bool status)
{
    switch (status)
    {
    case true:
        enableM3 = false;
        break;

    case false:
        enableM3 = true;
        break;
    };
    shiftData();
}

void StepperController::stepperM1Dir(const bool rev)
{
    directionM1 = rev;
    shiftData();
}
void StepperController::stepperM2Dir(const bool rev)
{
    directionM2 = rev;
    shiftData();
}
void StepperController::stepperM3Dir(const bool rev)
{
    directionM3 = rev;
    shiftData();
}

void StepperController::extrude(const float distance, const int motor, const int speed)
{
    long steps = ((float)distance / (float)0.021);

    // outStream->print("motor: ");
    // outStream->print(motor);
    // outStream->print("  ");
    // outStream->print("steps: ");
    // outStream->print(steps);
    // outStream->print("  ");
    // outStream->print("steps: ");
    // outStream->println(((int)steps));

    for (int i = 0; i < (long)steps; i++)
    {
        digitalWrite(motor, HIGH);
        delayMicroseconds(speed);
        digitalWrite(motor, LOW);
        delayMicroseconds(speed);
    }
}
void StepperController::extrudeM1(const float distance)
{
    extrude(distance, stepM1, speedDelayM1);
}
void StepperController::extrudeM2(const float distance)
{
    extrude(distance, stepM2, speedDelayM2);
}
void StepperController::extrudeM3(const float distance)
{
    extrude(distance, stepM3, speedDelayM3);
}

void StepperController::extrudeM1E(const float distance)
{
    stepperM1En(true);
    extrude(distance, stepM1, speedDelayM1);
    stepperM1En(false);
}
void StepperController::extrudeM2E(const float distance)
{
    stepperM2En(true);
    extrude(distance, stepM2, speedDelayM2);
    stepperM2En(false);
}
void StepperController::extrudeM3E(const float distance)
{
    stepperM3En(true);
    extrude(distance, stepM3, speedDelayM3);
    stepperM3En(false);
}

void StepperController::extrudeM1DE(const float distance, const bool rev)
{
    stepperM1Dir(rev);
    stepperM1En(true);
    extrude(distance, stepM1, speedDelayM1);
    stepperM1En(false);
}
void StepperController::extrudeM2DE(const float distance, const bool rev)
{
    stepperM2Dir(rev);
    stepperM2En(true);
    extrude(distance, stepM2, speedDelayM2);
    stepperM2En(false);
}
void StepperController::extrudeM3DE(const float distance, const bool rev)
{
    stepperM3Dir(rev);
    stepperM3En(true);
    extrude(distance, stepM3, speedDelayM3);
    stepperM3En(false);
}

void StepperController::extrudeM1DE(const float distance, const bool rev, int speed)
{
    stepperM1Dir(rev);
    stepperM1En(true);
    extrude(distance, stepM1, speed);
    stepperM1En(false);
}
void StepperController::extrudeM2DE(const float distance, const bool rev, int speed)
{
    stepperM2Dir(rev);
    stepperM2En(true);
    extrude(distance, stepM2, speed);
    stepperM2En(false);
}
void StepperController::extrudeM3DE(const float distance, const bool rev, int speed)
{
    stepperM3Dir(rev);
    stepperM3En(true);
    extrude(distance, stepM3, speed);
    stepperM3En(false);
}

void StepperController::shiftData()
{
    long dataChip1 = (1 * enableM1) + (2 * directionM1) + (4 * enableM2) + (8 * directionM2) + (16 * enableM3) + (32 * directionM3);
    // long data = dataChip1 + dataChip2;

    // outStream->print(enableM1);
    // outStream->print("  ");
    // outStream->print(directionM1);
    // outStream->print("  ");
    // outStream->print(enableM2);
    // outStream->print("  ");
    // outStream->print(directionM2);
    // outStream->print("  ");
    // outStream->print(enableM3);
    // outStream->print("  ");
    // outStream->print(directionM3);
    // outStream->print("      :");
    // outStream->println(dataChip1);

    digitalWrite(dataLatch, LOW);
    shiftOutCos(dataOut, dataClock, MSBFIRST, (dataChip1));
    digitalWrite(dataLatch, HIGH);
}

void StepperController::setData(const long data)
{
    digitalWrite(dataLatch, LOW);
    shiftOutCos(dataOut, dataClock, MSBFIRST, data);
    digitalWrite(dataLatch, HIGH);
}

void StepperController::shiftOutCos(const uint8_t dataPin, const uint8_t clockPin, const uint8_t bitOrder, const long val)
{
    uint8_t i;
    long value = val;

    for (i = 0; i < 16; i++)
    {
        if (bitOrder == LSBFIRST)
        {
            digitalWrite(dataPin, value & 1);
            value >>= 1;
        }
        else
        {
            digitalWrite(dataPin, (value & 32768) != 0);
            value <<= 1;
        }

        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}