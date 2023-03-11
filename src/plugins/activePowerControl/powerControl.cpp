//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "powerControl.h"
#include "../../utils/dbg.h"


void powerControl::setup(settings_t* config)
{

    mConfig = config;

}

void powerControl::tickPowerControlLoop_1s(void)
{

    static uint8_t called = 0;
    called ++;
    DBGPRINT(F("tickPowerControlLoop_1s is called:  "));
    DBGPRINT(String(called));
    DBGPRINT(F("times"));
}
