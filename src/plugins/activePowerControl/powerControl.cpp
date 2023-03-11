//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "powerControl.h"
#include "../../utils/dbg.h"
#include <string>

AsyncClient * aClient = NULL;
bool connectionOK = false;

void powerControl::setup(settings_t* config)
{

    mConfig = config;

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

        runAsyncClient();
        if(true == connectionOK)
        {
            //send the request
            //aClient->write("GET / HTTP/1.1\r\nHost: http://192.168.178.158/status\r\n\r\n");
            aClient->write("GET /status HTTP/1.1\r\n");
            aClient->write("Host: 192.168.178.158\r\n");
            aClient->write("\r\n");
        }

    }
}



void powerControl::runAsyncClient(){
    DBGPRINTLN(F("runAsyncClient called \n"));
  if(aClient)//client already exists
    return;

  DBGPRINTLN(F("runAsyncClient stage 1 \n"));
  aClient = new AsyncClient();
  if(!aClient)//could not allocate client
    return;

  DBGPRINTLN(F("runAsyncClient stage 2 \n"));

  aClient->onError([](void * arg, AsyncClient * client, err_t error){
    connectionOK = false;
    DBGPRINT(F("Connect Error: "));
    DBGPRINT(String(error));
    DBGPRINT(F("\n"));

    aClient = NULL;
    delete client;
  }, NULL);

  aClient->onConnect([](void * arg, AsyncClient * client){
    connectionOK = true;
    DBGPRINTLN(F("Connected"));
    aClient->onError(NULL, NULL);

    client->onDisconnect([](void * arg, AsyncClient * c){
      DBGPRINTLN(F("Disconnected"));
      connectionOK = false;
      aClient = NULL;
      delete c;
    }, NULL);

    client->onData([](void * arg, AsyncClient * c, void * data, size_t len){
      DBGPRINT(F("\r\n len: "));
      DBGPRINT(String(len));
      DBGPRINT(F("Data: \n"));


      uint8_t * d = (uint8_t*)data;
      char ch;

      for(size_t i=0; i<len;i++)
      {
        ch = (char)d[i];
        DBGPRINT(String(ch));
      }

      connectionOK = true;
      //c->close();
    }, NULL);

  }, NULL);

  if(!aClient->connect("192.168.178.158", 80))
  {
    DBGPRINTLN(F("Connect Fail"));
    AsyncClient * client = aClient;
    aClient = NULL;
    delete client;
  }
}