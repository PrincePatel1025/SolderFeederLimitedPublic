#include <Arduino.h>
#include "rotaryEncoder.h"
#include "systemData.h"
// #include <Wire.h>

RotaryEncoder::RotaryEncoder() {}

RotaryEncoder::RotaryEncoder(SystemData &data, SystemVar &var)
{
    this->rotaryInputA = data.rotaryInputA;
    this->rotaryInputB = data.rotaryInputB;
    this->btnInput = data.rotaryBtnInput;
    this->outStream = var.outstream;
}

RotaryEncoder::~RotaryEncoder() {}

void RotaryEncoder::begin()
{
    pinMode(rotaryInputA, INPUT);
    pinMode(rotaryInputB, INPUT);
    pinMode(btnInput, INPUT_PULLUP);
    aLastState = digitalRead(rotaryInputA);
}

void RotaryEncoder::getRotations(int &rotationVariable, int min, int max)
{

    if (rotationVariable > max || rotationVariable < min)
    {
        rotationVariable = min;
    }

    beforeUpdateVariable = rotationVariable;

    aState = digitalRead(rotaryInputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occurred
    if (aState != aLastState)
    {
        // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
        if ((digitalRead(rotaryInputB) != aState) && rotationVariable <= max)
        {

            if (rotationVariable != max)
            {
                ++rotationVariable;

                if (rotationVariable != (beforeUpdateVariable + 1))
                {
                    rotationVariable = (beforeUpdateVariable + 1);
                }
            }

            else if (rotationVariable == max)
            {
                rotationVariable = beforeUpdateVariable;
            }
        }
        else if (rotationVariable > min)
        {
            --rotationVariable;

            if (rotationVariable != (beforeUpdateVariable - 1))
            {
                rotationVariable = (beforeUpdateVariable - 1);
            }
        }

        aLastState = aState; // Updates the previous state of the outputA with the current state
    }
}

void RotaryEncoder::getRotationsRev(int &rotationVariable, int min, int max)
{

    if (rotationVariable > max || rotationVariable < min)
    {
        rotationVariable = min;
    }

    beforeUpdateVariable = rotationVariable;

    aState = digitalRead(rotaryInputA); // Reads the "current" state of the outputA
    // If the previous and the current state of the outputA are different, that means a Pulse has occurred
    if (aState != aLastState)
    {
        // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
        if ((digitalRead(rotaryInputB) != aState) && rotationVariable <= max)
        {

            if (rotationVariable > min)
            {
                --rotationVariable;

                if (rotationVariable != (beforeUpdateVariable - 1))
                {
                    rotationVariable = (beforeUpdateVariable - 1);
                }
            }

            else if (rotationVariable == max)
            {
                rotationVariable = beforeUpdateVariable;
            }
        }
        else if (rotationVariable < max)
        {
            ++rotationVariable;

            if (rotationVariable != (beforeUpdateVariable + 1))
            {
                rotationVariable = (beforeUpdateVariable + 1);
            }
        }

        aLastState = aState; // Updates the previous state of the outputA with the current state
    }
}

void RotaryEncoder::writeBtnClick1(bool &clickVariable)
{
    if (digitalRead(btnInput) == LOW)
    {
        clickVariable = 1;
    }

    else
    {
        clickVariable = 0;
    }
}

bool RotaryEncoder::returnBtnClick()
{
    bool output;

    if ((digitalRead(btnInput) == LOW) && (millis() - PreviousRotaryClick >= rotaryRefresh))
    {
        output = 1;
       // digitalWrite(10, HIGH);
        PreviousRotaryClick = millis();
    }

    else
    {
        output = 0;
        //digitalWrite(10, LOW);
    }

    return output;
}