#include <Arduino.h>
#include "manager.h"
#include "LCD_I2C.h"
#include "systemData.h"
#include "millisTimer.h"
#include "rotaryEncoder.h"
#include "stepperMotorController.h"

Manager::Manager(void)
{
    Controller = StepperController(data); //! UNCOMMENT AFTER REMOVING SERIAL
    encorder = RotaryEncoder(data, var);
    lcd = LCDI2C(0x27, 20, 4);

    btn1_2 = data.btn1_2;
    btn3_4 = data.btn3_4;
    btn5_6 = data.btn5_6;
}

Manager::~Manager(void) {}

void Manager::begin(HardwareSerial &serialData)
{
    lcd.init();
    lcd.backlight();

    outstream = &serialData;
    var.outstream = &serialData;
    Controller.begin(data, serialData);
    encorder.begin();

    pinMode(btn1_2, INPUT);
    pinMode(btn3_4, INPUT);
    pinMode(btn5_6, INPUT);
}

void Manager::getBtnInputs()
{
    int b1 = analogRead(btn1_2);
    int b2 = analogRead(btn3_4);
    int b3 = analogRead(btn5_6);

    if (b1 >= 712 && b1 <= 835)
    {
        Controller.extrudeM1DE(m1ExtrudeDistance, false);
    }

    else if (b1 >= 835)
    {
        Controller.extrudeM1DE(m1ExtrudeDistance, true);
        delay(10);
    }

    if (b2 >= 712 && b2 <= 835)
    {
        Controller.extrudeM2DE(m2ExtrudeDistance, false);
    }

    else if (b2 >= 835)
    {
        Controller.extrudeM2DE(m2ExtrudeDistance, true);
        delay(10);
    }

    if (b3 >= 712 && b3 <= 835)
    {
        Controller.extrudeM3DE(m3ExtrudeDistance, false);
    }

    else if (b3 >= 835)
    {
        Controller.extrudeM3DE(m3ExtrudeDistance, true);
        delay(10);
    }
}

void Manager::display(void)
{

    switch (menustage)
    {
    case -2:
        welcomePage();
        break;
    case -1:
        statusPage();
        break;
    case 0:
        homePage();
        break;

    case 10:
        motor1Page();
        break;
    case 100:
        extrudeDistancePage(1);
        break;
    case 101:
        speedPage(1);
        break;
    case 102:
        modepage(1);
        break;
    case 103:
        extrudeDelayPage(1);
        break;

    case 11:
        motor2Page();
        break;
    case 110:
        extrudeDistancePage(2);
        break;
    case 111:
        speedPage(2);
        break;
    case 112:
        modepage(2);
        break;
    case 113:
        extrudeDelayPage(2);
        break;

    case 12:
        motor3Page();
        break;
    case 120:
        extrudeDistancePage(3);
        break;
    case 121:
        speedPage(3);
        break;
    case 122:
        modepage(3);
        break;
    case 123:
        extrudeDelayPage(3);
        break;

    case 13:
        // settingsPage();
        break;

    default:
        break;
    }
}
void Manager::display2(void)
{
}

void Manager::welcomePage()
{
    lcd.centerStage("Solder Dispenser", 1);
    lcd.centerStage("Dispenser V1", 2);
    lcd.rightShift("by Prince", 4);
    menustage = -1;

    delay(1000);
    lcd.clear();
}
void Manager::statusPage()
{
    if (initialPrint == 1)
    {

        lcd.leftShift("D", 2);
        lcd.leftShift("S", 3);
        lcd.leftShift("M", 4);

        lcd.leftShift("M1", 2, 3, 1);
        lcd.leftShift(m1ExtrudeDistance, 3, 3, 2);
        lcd.leftShift(intSpeedToString(m1Speed), 1, 3, 3);
        lcd.leftShift(intModeToString(m1Mode), 2, 3, 4);

        lcd.centerStage("M2", 10, 12, 1);
        lcd.leftShift(m2ExtrudeDistance, 3, 10, 2);
        lcd.leftShift(intSpeedToString(m2Speed), 1, 10, 3);
        lcd.leftShift(intModeToString(m2Mode), 2, 10, 4);

        lcd.rightShift("M3", 2, 18, 1);
        lcd.leftShift(m3ExtrudeDistance, 3, 17, 2);
        lcd.leftShift(intSpeedToString(m3Speed), 1, 17, 3);
        lcd.leftShift(intModeToString(m3Mode), 2, 17, 4);

        initialPrint = 0;
    }

    if (encorder.returnBtnClick() == 1)
    {
        menustage = 0;
        initialPrint = 1;
        lcd.clear();
    }
}
void Manager::homePage()
{
    encorder.getRotationsRev(homePageLocation, 1, 4);

    if (initialPrint == 1)
    {
        lcd.clear();
        lcd.leftShift("Motor 1", 1);
        lcd.leftShift("Motor 2", 2);
        lcd.leftShift("Motor 3", 3);
        lcd.leftShift("Settings", 4);
        initialPrint = 0;
    }

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {
        lcd.rightShift("  ", PreviousHomePageLocation);
        lcd.rightShift("<-", homePageLocation);
        PreviousHomePageLocation = homePageLocation;

        screenRefresh.resetTimer();
    }

    if (encorder.returnBtnClick() == 1)
    {
        menustage = 9 + homePageLocation;
        initialPrint = 1;

        homePageLocation = 1;
        PreviousHomePageLocation = 0;
        lcd.clear();
    }

    if (screenStatusPageRefresh.waitUntil(5000) == 1)
    {
        initialPrint = 1;
        menustage = -1;
        lcd.clear();
        screenStatusPageRefresh.resetTimer();
    }
}
void Manager::motor1Page()
{
    encorder.getRotationsRev(homePageLocation, 1, 5);

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {

        if (initialPrint == 1)
        {
            if (homePageLocation <= 4)
            {
                lcd.clear();
                lcd.leftShift("Distance", 1);
                lcd.leftShift("Speed", 2);
                lcd.leftShift("Mode", 3);
                lcd.leftShift("Delay", 4);
                initialPrint = 0;
                initialPrint2 = 1;
            }
        }
        if (initialPrint2 == 1)
        {
            if (homePageLocation > 4)
            {
                lcd.clear();
                lcd.leftShift("Back", 1);
                initialPrint = 1;
                initialPrint2 = 0;
            }
        }

        if (PreviousHomePageLocation != homePageLocation)
        {
            lcd.rightShift("  ", 1);
            lcd.rightShift("  ", 2);
            lcd.rightShift("  ", 3);
            lcd.rightShift("  ", 4);
            lcd.rightShift("<-", homePageLocation % 4);
            PreviousHomePageLocation = homePageLocation;
        }
        screenRefresh.resetTimer();
    }

    if (encorder.returnBtnClick() == 1)
    {
        if (homePageLocation != 5)
        {
            menustage = 99 + homePageLocation;
        }
        else
        {
            menustage = 0;
        }

        initialPrint = 1;
        homePageLocation = 0;
        lcd.clear();
    }
}
void Manager::motor2Page()
{
    encorder.getRotationsRev(homePageLocation, 1, 5);

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {

        if (initialPrint == 1)
        {
            if (homePageLocation <= 4)
            {
                lcd.clear();
                lcd.leftShift("Distance", 1);
                lcd.leftShift("Speed", 2);
                lcd.leftShift("Mode", 3);
                lcd.leftShift("Delay", 4);
                initialPrint = 0;
                initialPrint2 = 1;
            }
        }
        if (initialPrint2 == 1)
        {
            if (homePageLocation > 4)
            {
                lcd.clear();
                lcd.leftShift("Back", 1);
                initialPrint = 1;
                initialPrint2 = 0;
            }
        }

        if (PreviousHomePageLocation != homePageLocation)
        {
            lcd.rightShift("  ", 1);
            lcd.rightShift("  ", 2);
            lcd.rightShift("  ", 3);
            lcd.rightShift("  ", 4);
            lcd.rightShift("<-", homePageLocation % 4);
            PreviousHomePageLocation = homePageLocation;
        }
        screenRefresh.resetTimer();
    }

    if (encorder.returnBtnClick() == 1)
    {
        if (homePageLocation != 5)
        {
            menustage = 109 + homePageLocation;
        }
        else
        {
            menustage = 0;
        }

        initialPrint = 1;
        homePageLocation = 0;
        lcd.clear();
    }
}
void Manager::motor3Page()
{
    encorder.getRotationsRev(homePageLocation, 1, 5);

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {

        if (initialPrint == 1)
        {
            if (homePageLocation <= 4)
            {
                lcd.clear();
                lcd.leftShift("Distance", 1);
                lcd.leftShift("Speed", 2);
                lcd.leftShift("Mode", 3);
                lcd.leftShift("Delay", 4);
                initialPrint = 0;
                initialPrint2 = 1;
            }
        }
        if (initialPrint2 == 1)
        {
            if (homePageLocation > 4)
            {
                lcd.clear();
                lcd.leftShift("Back", 1);
                initialPrint = 1;
                initialPrint2 = 0;
            }
        }

        if (PreviousHomePageLocation != homePageLocation)
        {
            lcd.rightShift("  ", 1);
            lcd.rightShift("  ", 2);
            lcd.rightShift("  ", 3);
            lcd.rightShift("  ", 4);
            lcd.rightShift("<-", homePageLocation % 4);
            PreviousHomePageLocation = homePageLocation;
        }
        screenRefresh.resetTimer();
    }

    if (encorder.returnBtnClick() == 1)
    {
        if (homePageLocation != 5)
        {
            menustage = 119 + homePageLocation;
        }
        else
        {
            menustage = 0;
        }

        initialPrint = 1;
        homePageLocation = 0;
        lcd.clear();
    }
}

void Manager::speedPage(int motorNumber)
{
    encorder.getRotationsRev(speedLocation, -1, 1);

    if (initialPrint == 1)
    {
        lcd.centerStage("Speed M" + (String)motorNumber, 1);

        switch (motorNumber)
        {
        case 1:
            speedLocation = m1Speed;
            break;
        case 2:
            speedLocation = m2Speed;
            break;
        case 3:
            speedLocation = m3Speed;
            break;

        default:
            break;
        }
        initialPrint = 0;
    }

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {
        lcd.centerStage(intSpeedToString(speedLocation), 3);
    }

    if (encorder.returnBtnClick() == 1)
    {
        switch (motorNumber)
        {
        case 1:
            m1Speed = speedLocation;
            break;
        case 2:
            m2Speed = speedLocation;
            break;
        case 3:
            m3Speed = speedLocation;
            break;

        default:
            break;
        }

        menustage /= 10;

        screenStatusPageRefresh.resetTimer();
        homePageLocation = PreviousHomePageLocation;
        PreviousHomePageLocation = 0;
        initialPrint = 1;
        lcd.clear();
    }
}
void Manager::modepage(int motorNumber)
{
    encorder.getRotationsRev(modeLocation, -1, 1);

    if (initialPrint == 1)
    {
        lcd.centerStage("Mode M" + (String)motorNumber, 1);

        switch (motorNumber)
        {
        case 1:
            modeLocation = m1Mode;
            break;
        case 2:
            modeLocation = m2Mode;
            break;
        case 3:
            modeLocation = m3Mode;
            break;

        default:
            break;
        }
        initialPrint = 0;
    }

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {
        lcd.centerStage(intModeToString(modeLocation), 3);
    }

    if (encorder.returnBtnClick() == 1)
    {
        switch (motorNumber)
        {
        case 1:
            m1Mode = modeLocation;
            break;
        case 2:
            m2Mode = modeLocation;
            break;
        case 3:
            m3Mode = modeLocation;
            break;

        default:
            break;
        }

        menustage /= 10;

        screenStatusPageRefresh.resetTimer();
        homePageLocation = PreviousHomePageLocation;
        PreviousHomePageLocation = 0;
        initialPrint = 1;
        lcd.clear();
    }
}
void Manager::extrudeDistancePage(int motorNumber)
{
    encorder.getRotationsRev(distanceLoacation, 0, 100);

    if (initialPrint == 1)
    {
        lcd.centerStage("Distance M" + (String)motorNumber, 1);

        switch (motorNumber)
        {
        case 1:
            distanceLoacation = m1ExtrudeDistance;
            break;
        case 2:
            distanceLoacation = m2ExtrudeDistance;
            break;
        case 3:
            distanceLoacation = m3ExtrudeDistance;
            break;

        default:
            break;
        }
        initialPrint = 0;
    }

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {
        lcd.centerStage(distanceLoacation, 3);
        screenRefresh.resetTimer();
    }

    if (encorder.returnBtnClick() == 1)
    {
        switch (motorNumber)
        {
        case 1:
            m1ExtrudeDistance = distanceLoacation;
            break;
        case 2:
            m2ExtrudeDistance = distanceLoacation;
            break;
        case 3:
            m3ExtrudeDistance = distanceLoacation;
            break;

        default:
            break;
        }

        menustage /= 10;

        screenStatusPageRefresh.resetTimer();
        homePageLocation = PreviousHomePageLocation;
        PreviousHomePageLocation = 0;
        initialPrint = 1;
        lcd.clear();
    }
}
void Manager::extrudeDelayPage(const int motorNumber)
{
    encorder.getRotationsRev(delayLocation, 1, 50);

    if (initialPrint == 1)
    {
        outstream->print(motorNumber);
        outstream->print("  ");

        lcd.centerStage("Delay M" + (String)motorNumber, 1);

        switch (motorNumber)
        {
        case 1:
            delayLocation = m1Delay;
            break;
        case 2:
            delayLocation = m2Delay;
            break;
        case 3:
            delayLocation = m3Delay;
            break;

        default:
            break;
        }
        initialPrint = 0;

        outstream->print(m1Delay);
        outstream->print("  ");
        outstream->print(m2Delay);
        outstream->print("  ");
        outstream->println(m3Delay);
    }

    if (screenRefresh.waitUntil(refreshDelay) == 1)
    {
        lcd.centerStage(delayLocation, 3);
        screenRefresh.resetTimer();
    }

    if (encorder.returnBtnClick() == 1)
    {
        switch (motorNumber)
        {
        case 1:
            m1Delay = delayLocation;
            break;
        case 2:
            m2Delay = delayLocation;
            break;
        case 3:
            m3Delay = delayLocation;
            break;

        default:
            break;
        }

        outstream->print(motorNumber);
        outstream->print("  ");
        outstream->print(m1Delay);
        outstream->print("  ");
        outstream->print(m2Delay);
        outstream->print("  ");
        outstream->println(m3Delay);

        menustage /= 10;

        screenStatusPageRefresh.resetTimer();
        homePageLocation = PreviousHomePageLocation;
        PreviousHomePageLocation = 0;
        initialPrint = 1;
        lcd.clear();
    }
}

String Manager::intSpeedToString(const int speed)
{
    String dataOut;

    switch (speed)
    {
    case -1:
        dataOut = "S";
        break;
    case 0:
        dataOut = "N";
        break;
    case 1:
        dataOut = "F";
        break;

    default:
        dataOut = "E";
        break;
    }

    return dataOut;
}
String Manager::intModeToString(const int mode)
{
    String dataOut;

    switch (mode)
    {
    case -1:
        dataOut = "AE";
        break;
    case 0:
        dataOut = "BE";
        break;
    case 1:
        dataOut = "CE";
        break;

    default:
        dataOut = "EE";
        break;
    }

    return dataOut;
}

void Manager::extrudeM1()
{
    int a1 = analogRead(btn1_2);

    if (m1Mode == -1)
    {
        if (a1 >= 712 && a1 <= 835)
        {
            startM1 = 0;
        }

        else if (a1 > 835)
        {
            startM1 = 1;
        }

        if (startM1 == 1 && motorAutoModeDelay1.waitUntil(m1Delay * 1000) == 1)
        {
            Controller.extrudeM1DE(m1ExtrudeDistance, false);
            motorAutoModeDelay1.resetTimer();
        }
    }

    else if (m1Mode == 0 && motorBtnDelay1.waitUntil(100) == 1)
    {
        if (a1 >= 712 && a1 <= 835)
        {
            Controller.extrudeM1DE(m1ExtrudeDistance, false);
            motorBtnDelay1.resetTimer();
        }

        else if (a1 > 835)
        {
            Controller.extrudeM1DE(m1ExtrudeDistance, true);
            motorBtnDelay1.resetTimer();
        }
    }

    else if (m1Mode == 1)
    {
        if (a1 >= 712 && a1 <= 835)
        {
            Controller.extrudeM1DE(10, false);
        }

        else if (a1 > 835)
        {
            Controller.extrudeM1DE(10, true);
        }
    }
}
void Manager::extrudeM2()
{
    int b1 = analogRead(btn3_4);

    if (m2Mode == -1)
    {
        if (b1 >= 712 && b1 <= 835)
        {
            startM2 = 0;
        }

        else if (b1 > 835)
        {
            startM2 = 1;
        }

        if (startM2 == 1 && motorAutoModeDelay2.waitUntil(m2Delay * 1000) == 1)
        {
            Controller.extrudeM2DE(m2ExtrudeDistance, false);
            motorAutoModeDelay2.resetTimer();
        }
    }

    else if (m2Mode == 0 && motorBtnDelay2.waitUntil(100) == 1)
    {
        if (b1 >= 712 && b1 <= 835)
        {
            Controller.extrudeM2DE(m2ExtrudeDistance, false);
            motorBtnDelay2.resetTimer();
        }

        else if (b1 > 835)
        {
            Controller.extrudeM2DE(m2ExtrudeDistance, true);
            motorBtnDelay2.resetTimer();
        }
    }

    else if (m2Mode == 1)
    {
        if (b1 >= 712 && b1 <= 835)
        {
            Controller.extrudeM2DE(10, false);
        }

        else if (b1 > 835)
        {
            Controller.extrudeM2DE(10, true);
        }
    }
}
void Manager::extrudeM3()
{

    int b1 = analogRead(btn5_6);

    if (m3Mode == -1)
    {
        if (b1 >= 712 && b1 <= 835)
        {
            startM3 = 0;
        }

        else if (b1 > 835)
        {
            startM3 = 1;
        }

        if (startM3 == 1 && motorAutoModeDelay3.waitUntil(m3Delay * 1000) == 1)
        {
            Controller.extrudeM3DE(m3ExtrudeDistance, false);
            motorAutoModeDelay3.resetTimer();
        }
    }

    else if (m3Mode == 0 && motorBtnDelay3.waitUntil(100) == 1)
    {
        if (b1 >= 712 && b1 <= 835)
        {
            Controller.extrudeM3DE(m3ExtrudeDistance, false);
            motorBtnDelay3.resetTimer();
        }

        else if (b1 > 835)
        {
            Controller.extrudeM3DE(m3ExtrudeDistance, true);
            motorBtnDelay3.resetTimer();
        }
    }

    else if (m3Mode == 1)
    {
        if (b1 >= 712 && b1 <= 835)
        {
            Controller.extrudeM3DE(10, false);
        }

        else if (b1 > 835)
        {
            Controller.extrudeM3DE(10, true);
        }
    }
}
void Manager::extrudeAllMotors()
{
    extrudeM1();
    extrudeM2();
    extrudeM3();
}
/*void ext(){

    // encorder.getRotationsRev(homePageLocation, 1, 5);

    // if ((PreviousHomePageLocation == 4 && homePageLocation == 5) || (PreviousHomePageLocation == 5 && homePageLocation == 4) || (initialPrint == 1))
    // {
    //     if (homePageLocation == 5)
    //     {
    //         lcd.clear();
    //         lcd.leftShift("Back", 1);
    //         initialPrint = 0;
    //     }

    //     else if (homePageLocation != 5)
    //     {
    //         lcd.clear();
    //         lcd.leftShift("Distance", 1);
    //         lcd.leftShift("Speed", 2);
    //         lcd.leftShift("Mode", 3);
    //         lcd.leftShift("Delay", 4);
    //         initialPrint = 0;
    //     }
    // }

    // if (screenRefresh.waitUntil(refreshDelay) == 1)
    // {
    //     if (homePageLocation == 5)
    //     {
    //         lcd.rightShift("  ", 1);
    //         lcd.rightShift("  ", 2);
    //         lcd.rightShift("  ", 3);
    //         lcd.rightShift("  ", 4);
    //         lcd.rightShift("<-", homePageLocation % 4);
    //         PreviousHomePageLocation = homePageLocation;
    //     }

    //     else
    //     {
    //         lcd.rightShift("  ", 1);
    //         lcd.rightShift("  ", 2);
    //         lcd.rightShift("  ", 3);
    //         lcd.rightShift("  ", 4);
    //         lcd.rightShift("<-", homePageLocation);
    //         PreviousHomePageLocation = homePageLocation;
    //     }
    //     screenRefresh.resetTimer();
    // }

    // if (encorder.returnBtnClick() == 1)
    // {
    //     if (homePageLocation != 5)
    //     {
    //         menustage = 119 + homePageLocation;
    //     }
    //     else
    //     {
    //         menustage = 0;
    //     }

    //     initialPrint = 1;
    //     homePageLocation = 0;
    //     lcd.clear();
    // }
}
*/