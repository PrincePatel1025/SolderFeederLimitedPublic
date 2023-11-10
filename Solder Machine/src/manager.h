#ifndef MANAGER
#define MANAGER

#include <Arduino.h>
#include "LCD_I2C.h"
#include "systemData.h"
#include "millisTimer.h"
#include "rotaryEncoder.h"
#include "stepperMotorController.h"

class Manager
{
private:
    LCDI2C lcd;
    SystemVar var;
    SystemData data;
    RotaryEncoder encorder;
    MillisTimer motorBtnDelay1;
    MillisTimer motorBtnDelay2;
    MillisTimer motorBtnDelay3;
    MillisTimer screenRefresh;
    HardwareSerial *outstream;
    StepperController Controller;
    MillisTimer screenStatusPageRefresh;
    MillisTimer motorAutoModeDelay1;
    MillisTimer motorAutoModeDelay2;
    MillisTimer motorAutoModeDelay3;

    int btn1_2;
    int btn3_4;
    int btn5_6;

    //! display Variables
    int x = 0;
    int menustage = -2;
    int initialPrint = 1;
    int initialPrint2 = 0;
    int refreshDelay = 200;
    int refreshDelay2 = 10000;
    bool statusPageRefresh = 1;

    int homePageLocation = 1;
    int PreviousHomePageLocation = 0;

    int speedLocation = 0;
    int distanceLoacation = 0;
    int modeLocation = 0;
    int delayLocation = 0;

    //! Motor VAriables
    int m1ExtrudeDistance = 10;
    int m2ExtrudeDistance = 10;
    int m3ExtrudeDistance = 10;

    int m1Speed = 0;
    int m2Speed = 0;
    int m3Speed = 0;

    int m1Mode = 0;
    int m2Mode = 0;
    int m3Mode = 0;

    unsigned long m1Delay = 1;
    unsigned long m2Delay = 1;
    unsigned long m3Delay = 1;

    bool startM1 = 0;
    bool startM2 = 0;
    bool startM3 = 0;

    bool changeAllowed = 1;
    int a1 = 0;

protected:
    unsigned long previousMillis = 0;

public:
    Manager(void);
    ~Manager(void);

    void begin(HardwareSerial &serialData);
    void getBtnInputs();

    //! Display Stuff Below
    void display();
    void display2();

    void welcomePage();
    void statusPage();
    void homePage();

    void motor1Page();
    void motor2Page();
    void motor3Page();

    void modepage(int motorNumber);
    void speedPage(int motorNumber);
    void extrudeDelayPage(const int motorNumber);
    void extrudeDistancePage(int motorNumber);

    String intSpeedToString(const int speed);
    String intModeToString(const int mode);

    void extrudeAllMotors();
    void extrudeM1();
    void extrudeM2();
    void extrudeM3();
};

#endif