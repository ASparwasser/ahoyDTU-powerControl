//-----------------------------------------------------------------------------
// 2022 Ahoy, https://www.mikrocontroller.net/topic/525778
// Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//-----------------------------------------------------------------------------

#include "powerControl.h"
#include "../../utils/dbg.h"
#include <string>

AsyncClient * aClient = NULL;

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
    }
}



void powerControl::runAsyncClient(){
    DBGPRINT(F("runAsyncClient called \n"));
  if(aClient)//client already exists
    return;

  DBGPRINT(F("runAsyncClient stage 1 \n"));
  aClient = new AsyncClient();
  if(!aClient)//could not allocate client
    return;

  DBGPRINT(F("runAsyncClient stage 2 \n"));

  aClient->onError([](void * arg, AsyncClient * client, int error){
    DBGPRINT(F("Connect Error"));
    aClient = NULL;
    delete client;
  }, NULL);

  aClient->onConnect([](void * arg, AsyncClient * client){
    DBGPRINT(F("Connected"));
    aClient->onError(NULL, NULL);

    client->onDisconnect([](void * arg, AsyncClient * c){
      DBGPRINT(F("Disconnected"));
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

    }, NULL);

    //send the request
    client->write("GET / HTTP/1.0\r\nHost: 192.168.178.158/status\r\n\r\n");
  }, NULL);

  if(!aClient->connect("192.168.178.158/status", 80)){
    DBGPRINT(F("Connect Fail"));
    AsyncClient * client = aClient;
    aClient = NULL;
    delete client;
  }
}