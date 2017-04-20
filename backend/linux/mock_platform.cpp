#include "platform.hpp"

namespace platform
{
    TimePoint currentTime;

    void setTime(TimePoint t)
    {
        currentTime = t;
    }

    TimePoint now(void)
    {
        return currentTime;
    }

    void pinMode(Pin p, Mode m) {}

    void digitalWrite(DigitalPin p, State s) {}

    void analogWrite(AnalogPin p, unsigned int value) {}
}
