#ifndef THEMILLISTIMER
#define THEMILLISTIMER

#include <Arduino.h>
class MillisTimer
{
private:
    unsigned long startTime = 0UL;
    bool timeSet = 0;
    bool waitDone = 0;

public:
    bool waitUntil(unsigned long waitTime);
    void resetTimer();
};

#endif
