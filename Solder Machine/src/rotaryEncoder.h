#ifndef rotaryencoder
#define rotaryencoder

#include <Arduino.h>
#include <systemData.h>
#include <Wire.h>

class RotaryEncoder
{
private:
    HardwareSerial *outStream;
    int aState;
    int aLastState;
    int rotaryInputA;
    int rotaryInputB;
    int btnInput;
    int beforeUpdateVariable;
    int afterUpdateVariable;

    unsigned long PreviousRotaryClick = 0UL;
    unsigned long rotaryRefresh = 500UL;

protected:
public:
    RotaryEncoder();
    RotaryEncoder(SystemData &data, SystemVar &var);
    ~RotaryEncoder();

    void begin();
    void getRotations(int &rotationVariable, int min, int max);
    void getRotationsRev(int &rotationVariable, int min, int max);
    void writeBtnClick1(bool &clickVariable);
    bool returnBtnClick();
};

#endif