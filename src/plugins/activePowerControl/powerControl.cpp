//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "powerControl.h"
#include "../../utils/dbg.h"
#include <string>

const char* powerControl::searchTag = "\"total_power\":";


void powerControl::setup(settings_t* config)
{

    mConfig = config;

    lastPowerValue = 255;
}

void powerControl::tickPowerControlLoop_1s(void)
{

    static uint8_t called = 0;
    called ++;

    if(called%10 == 0)
    {
        DBGPRINT(F("tickPowerControlLoop_1s is called:  "));
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

            //send the request
            aClient->write("GET /status HTTP/1.1\r\n");
            aClient->write("Host: 192.168.178.158\r\n");
            aClient->write("\r\n");
        }

    }

    DBGPRINT(String(lastPowerValue));
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

}

void powerControl::client_onDisconnect(void* arg, AsyncClient* client)
{
    powerControl* self = static_cast<powerControl*>(arg);

    DBGPRINTLN(F("Disconnected"));

    size_t i;
    for (i = 0; i < self->shellycontent_index; i++)
    {
        /* Search inside long string for our value! */
        if (0 == memcmp((void*)&searchTag[0], (void*)&self->shellycontent[i], sizeof(searchTag)))
        {
            DBGPRINTLN(F("Checkpoint 1! "));
            /* 4 places because of watt*1000! */
            char strPower[5];
            memcpy((void*)(&self->shellycontent[i + sizeof(searchTag)]),(void*) &strPower[0], sizeof(strPower));
            strPower[4] = '\0'; // Add null-terminator

            DBGPRINTLN(F("Checkpoint 2! "));

            /* Perform some verification actions. */
            bool isValid = true;
            for (int j = 0; j < 4; j++)
            {
                if (!isdigit(strPower[j]))
                {
                    strPower[j] = '\0'; // Add null-terminator
                    if (j < 2)
                    {
                        /* Power value is tooo low. */
                        isValid = false;
                    }
                    break;
                }
            }

            DBGPRINTLN(F("Checkpoint 3! "));

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
    }
    self->shellycontent_index = 0;

    DBGPRINT(String(self->shellycontent) + '\n');
    DBGPRINT(String(self->lastPowerValue));

    self->connectionOK = false;
    self->aClient = NULL;
    delete client;
}

void powerControl::client_onData(void * arg, AsyncClient * c, void * data, size_t len)
{
    powerControl* self = static_cast<powerControl*>(arg);

    uint8_t * d = (uint8_t*)data;
    char ch;

    size_t i;
    for (i = self->shellycontent_index; i < len + self->shellycontent_index; i++)
    {
        /* Buffer all values in private array. */
        ch = (char)d[i];
        self->shellycontent[i] = ch;
        //DBGPRINT(String(ch));

    }

    /* Set array index to next value! */
    self->shellycontent_index = i+1;
    self->connectionOK = true;
}
