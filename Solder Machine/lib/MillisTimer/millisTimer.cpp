#include <Arduino.h>
#include <millisTimer.h>

bool MillisTimer::waitUntil(unsigned long waitTime)
{
    if (timeSet == 0)
    {
        timeSet = 1;
        startTime = millis();
        waitDone = 0;
    }

    if (timeSet == 1)
    {
        if (millis() - startTime >= waitTime)
        {
            waitDone = 1;
            timeSet = 0;
            startTime = millis();
        }

        else
        {
            waitDone = 0;
        }
    }

    return waitDone;
}

void MillisTimer::resetTimer()
{
    startTime = millis();
}
