//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#ifndef __POWERCONTROL_H__
#define __POWERCONTROL_H__

#include "../../config/settings.h"
#include <ESPAsyncTCP.h>

class powerControl{
    public:

    powerControl() { /* Not shure what to do here? */ }

    /* Implemented memeber functions. */
    void setup(settings_t *config);
    void tickPowerControlLoop_1s(void);

    private:

    settings_t *mConfig;

};

#endif