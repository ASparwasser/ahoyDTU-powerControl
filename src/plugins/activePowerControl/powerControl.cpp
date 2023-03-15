//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "powerControl.h"
#include "../../utils/dbg.h"
#include <string>

enum{SEARCH_TAG_LEN = 12};
static const char searchTag[SEARCH_TAG_LEN] = "total_power";


void powerControl::setup(settings_t* config)
{

    mConfig = config;

    lastPowerValue = 255;

    shellycontent_index = 0;
}

void powerControl::tickPowerControlLoop_1s(void)
{

    static uint8_t called = 0;
    called ++;

    if(called%10 == 0)
    {
        DBGPRINT(F("tickPowerControlLoop_1s is called:  \n"));
        DBGPRINT(String((unsigned long long)this));
        DBGPRINT(String(called));
        DBGPRINT(F("times"));

        AsyncClient * client = aClient;
        aClient = NULL;
        delete client;
        aClient = new AsyncClient();

        if(!aClient)//could not allocate client
            return;

        aClient->onError(&powerControl::client_onError, this);
        aClient->onConnect(&powerControl::client_onConnect, this);
        aClient->onDisconnect(&powerControl::client_onDisconnect, this);
        aClient->onData(&powerControl::client_onData, this);

        if(!aClient->connect("192.168.178.158", 80))
        {
            DBGPRINTLN(F("Connect Fail"));
            AsyncClient * client = aClient;
            aClient = NULL;
            delete client;
        }


        if(true == connectionOK)
        {
            if(!aClient->connect("192.168.178.158", 80))
            {
                DBGPRINTLN(F("Connect Fail"));
                AsyncClient * client = aClient;
                aClient = NULL;
                delete client;

                return;
            }
        }

    }

    // DBGPRINT(String(lastPowerValue));
}

void powerControl::client_onError(void* arg, AsyncClient* client, err_t error)
{
    powerControl* self = static_cast<powerControl*>(arg);

    self->connectionOK = false;
    DBGPRINT(F("Connect Error: "));
    DBGPRINT(String(error));
    DBGPRINT(F("\n"));

    self->aClient = NULL;
    delete client;
}

void powerControl::client_onConnect(void* arg, AsyncClient* client)
{
    powerControl* self = static_cast<powerControl*>(arg);

    self->connectionOK = true;
    DBGPRINTLN(F("Connected"));

    //send the request
    self->aClient->write("GET /status HTTP/1.1\r\n");
    self->aClient->write("Host: 192.168.178.158\r\n");
    self->aClient->write("\r\n");

}

void powerControl::client_onDisconnect(void* arg, AsyncClient* client)
{
    powerControl* self = static_cast<powerControl*>(arg);

    self->connectionOK = false;
    self->aClient = NULL;
}

void powerControl::client_onData(void * arg, AsyncClient * c, void * data, size_t len)
{
    powerControl* self = static_cast<powerControl*>(arg);

    DBGPRINTLN(F("\n OnData"));

    uint8_t * d = (uint8_t*)data;

    uint16_t i;
    enum{STRING_POWER_LEN = 5};
    char strPower[STRING_POWER_LEN];
    bool isValid = false;
    bool isMatch = false;
    for (i = 0; i < len; i++)
    {
        for(uint8_t j = 0; (char)d[i+j] == searchTag[j]; j++)
        {

            if(j==SEARCH_TAG_LEN-2)
            {
                // DBGPRINT(F("\n Match \n"));
                isMatch = true;

                for(uint8_t x=0; x < STRING_POWER_LEN; x++)
                {
                    char ch = (char)d[i+j+3+x];
                    DBGPRINT("X = " + String(ch));
                    DBGPRINT("\n");

                    if(true == isDigit(d[i+j+3+x]))
                    {
                        char ch = (char)d[i+j+3+x];
                        strPower[x] = ch;
                        isValid = true;
                    }
                    else
                    {
                        strPower[x] = '\0';
                        if (x < 2)
                        {
                            /* Power value is tooo low. */
                            isValid = false;
                        }
                        break;
                    }
                }
            }
        }
    }

    if(true == isMatch)
    {
        if (true == isValid )
        {
            /* Convert strPower to a uint16_t number. */
            self->lastPowerValue = std::strtoul(strPower, nullptr, 10);
        }
        else
        {
            /* Power value is tooo low. Set it to 0. */
            self->lastPowerValue = 0;
        }
    }
    else
    {
        /* Keep the value like it is. */
    }


    // DBGPRINTLN("String Power: " + String(strPower));
    // DBGPRINTLN("String number: " + String(self->lastPowerValue));
    self->connectionOK = true;
}
