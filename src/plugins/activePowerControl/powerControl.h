//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#ifndef __POWERCONTROL_H__
#define __POWERCONTROL_H__

#include "../../config/settings.h"
#include <ESPAsyncTCP.h>
#include "appInterface.h"



template<class HMSYSTEM>
class powerControl{
    public:

    powerControl() { /* Not shure what to do here? */ }

    /* Implemented memeber functions. */
    void setup(HMSYSTEM *_sys, settings_t *config);
    void tickPowerControlLoop_1s(void);
    void runAsyncClient(void);

    static void set_ipMeasUnit(String s);

    static void client_onError(void *arg, AsyncClient *client, err_t error);
    static void client_onConnect(void *arg, AsyncClient *client);
    static void client_onDisconnect(void *arg, AsyncClient *client);
    static void client_onData(void * arg, AsyncClient * c, void * data, size_t len);

private:
    static String str_ipMeasUnit;

    settings_t *mConfig;
    AsyncClient * aClient = NULL;
    bool accessingServer;
    bool measUnitResponseReceived;
    HMSYSTEM *sys;

    enum{FILTER_SIZE = 6};
    int16_t lastPowerValue;
    int16_t actualPowerValue;
    int16_t controlledValueMeasurement[FILTER_SIZE]= {0,0,0,0,0,0};
    uint8_t controlledValueIndex;
    uint16_t maxInverter_P_out;
};


#endif